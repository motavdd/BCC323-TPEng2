#include "application/AcademicService.hpp"
#include <algorithm>
#include <cctype>
#include <chrono>
#include <ctime>
#include <regex>
#include <tuple>

namespace bcc323 {
namespace {
    std::string campusDate() {
        const auto time = std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now() - std::chrono::hours(3));
        std::tm local{};
#ifdef _WIN32
        gmtime_s(&local, &time);
#else
        gmtime_r(&time, &local);
#endif
        char date[11];
        std::strftime(date, sizeof date, "%Y-%m-%d", &local);
        return date;
    }

    bool validDate(const std::string& date) {
        static const std::regex pattern("[0-9]{4}-[0-9]{2}-[0-9]{2}");
        if (!std::regex_match(date, pattern)) return false;
        const auto year = std::stoi(date.substr(0, 4));
        const auto month = std::stoi(date.substr(5, 2));
        const auto day = std::stoi(date.substr(8, 2));
        if (year < 1 || month < 1 || month > 12 || day < 1) return false;
        const int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        const bool leap = year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
        return day <= days[month - 1] + (month == 2 && leap);
    }

    bool validText(const std::string& text) {
        return text.size() <= 100 && std::any_of(text.begin(), text.end(),
            [](unsigned char c) { return !std::isspace(c) && !std::iscntrl(c); })
            && std::none_of(text.begin(), text.end(), [](unsigned char c) { return std::iscntrl(c); });
    }

    template<class Item>
    auto findRecord(std::vector<Item>& records, const std::string& id) {
        return std::find_if(records.begin(), records.end(), [&](const auto& item) { return item.id == id; });
    }

    std::vector<AcademicTerm> orderedTerms(std::vector<AcademicTerm> terms) {
        std::sort(terms.begin(), terms.end(), [](const auto& a, const auto& b) { return a.start < b.start; });
        std::map<std::string, bool> seen;
        std::string previousEnd;
        for (const auto& term : terms) {
            if (term.id.empty() || !seen.emplace(term.id, true).second
                || !validDate(term.start) || !validDate(term.end)
                || term.end < term.start || term.start <= previousEnd) {
                throw AppError(503, "invalid_academic_calendar", "O calendário acadêmico precisa de revisão.");
            }
            previousEnd = term.end;
        }
        return terms;
    }

    CourseRecord applySchedule(CourseRecord course, const AcademicSchedule& schedule,
                               const std::string& term, const std::string& date) {
        for (const auto& change : schedule.changes) {
            if (change.effectiveDate > date) break;
            if (change.subject) {
                const auto item = findRecord(course.subjects, change.subject->id);
                if (item != course.subjects.end()) *item = *change.subject;
            } else if (change.offering && change.term == term) {
                const auto item = findRecord(course.offerings, change.offering->id);
                if (item != course.offerings.end()) *item = *change.offering;
            }
        }
        return course;
    }

    void validateOffering(const OfferingRecord& offering) {
        static const std::regex day("Seg|Ter|Qua|Qui|Sex|Sáb");
        static const std::regex time("([01][0-9]|2[0-3]):[0-5][0-9]");
        if ((!offering.professor.empty() && !validText(offering.professor))
            || offering.times.empty() || offering.times.size() > 24) {
            throw AppError(400, "invalid_offering", "Informe dados válidos para a turma.");
        }
        for (const auto& slot : offering.times) {
            if (!std::regex_match(slot.day, day) || !std::regex_match(slot.start, time)
                || !std::regex_match(slot.end, time) || slot.start >= slot.end
                || (!slot.room.empty() && !validText(slot.room))) {
                throw AppError(400, "invalid_offering", "Informe dia, horário e sala válidos.");
            }
        }
    }
}

CourseRecord AcademicService::resolveCourse(const RequestContext& context, CourseRecord course, const std::string& term) {
    const auto terms = orderedTerms(repository_.academicTerms(context));
    const auto today = today_.empty() ? campusDate() : today_;
    auto selected = std::find_if(terms.begin(), terms.end(), [&](const auto& item) {
        if (term == "next") return today < item.start;
        return term.empty() ? item.start <= today && today <= item.end : item.id == term;
    });
    if (selected == terms.end() && !term.empty() && term != "next") {
        throw AppError(404, "term_not_found", "Semestre não cadastrado.");
    }
    if (selected == terms.end() && term == "next") {
        selected = std::find_if(terms.begin(), terms.end(), [&](const auto& item) {
            return item.start <= today && today <= item.end;
        });
    }
    const auto date = selected == terms.end() ? today : std::clamp(today, selected->start, selected->end);
    const auto selectedId = selected == terms.end() ? std::string{} : selected->id;
    const auto schedule = repository_.schedule(context, course.id);
    return applySchedule(std::move(course), schedule, selectedId, date);
}

CourseRecord AcademicService::courseAt(const RequestContext& context, const std::string& course, const std::string& term) {
    return resolveCourse(context, repository_.course(context, course), term);
}

ScheduledChange AcademicService::scheduleChange(const RequestContext& context, const std::string& courseId,
                                                ScheduledChange change) {
    if (!repository_.coordinates(context, courseId)) {
        throw AppError(403, "coordinator_required", "Apenas o coordenador deste curso pode fazer alterações.");
    }
    const auto terms = orderedTerms(repository_.academicTerms(context));
    const auto term = std::find_if(terms.begin(), terms.end(), [&](const auto& item) { return item.id == change.term; });
    if (term == terms.end()) throw AppError(404, "term_not_found", "Semestre não cadastrado.");
    const auto today = today_.empty() ? campusDate() : today_;
    if (change.subject.has_value() == change.offering.has_value()) {
        throw AppError(400, "invalid_change", "Informe uma alteração de disciplina ou de turma.");
    }
    auto base = repository_.course(context, courseId);
    auto schedule = repository_.schedule(context, courseId);
    if (change.subject) {
        if (term->start <= today || (!change.effectiveDate.empty() && change.effectiveDate != term->start)) {
            throw AppError(400, "invalid_effective_date", "A matriz só muda no início de um semestre futuro.");
        }
        change.effectiveDate = term->start;
        const auto& subject = *change.subject;
        if (findRecord(base.subjects, subject.id) == base.subjects.end()) {
            throw AppError(404, "subject_not_found", "Disciplina fora da matriz.");
        }
        if (!validText(subject.name) || subject.hours < 0 || subject.hours > 10000
            || subject.period < 1 || subject.period > 20) {
            throw AppError(400, "invalid_subject", "Informe nome, carga horária e período válidos.");
        }
    } else {
        if (change.effectiveDate.empty()) change.effectiveDate = std::max(today, term->start);
        if (!validDate(change.effectiveDate) || change.effectiveDate < today
            || change.effectiveDate < term->start || change.effectiveDate > term->end) {
            throw AppError(400, "invalid_effective_date", "Data de vigência fora do semestre ou no passado.");
        }
        auto& offering = *change.offering;
        auto scheduled = applySchedule(base, schedule, term->id, change.effectiveDate);
        const auto item = findRecord(scheduled.offerings, offering.id);
        if (item == scheduled.offerings.end()) throw AppError(404, "offering_not_found", "Turma não cadastrada.");
        offering.subjectId = item->subjectId;
        offering.group = item->group;
        if (offering.professor.empty()) offering.professor = item->professor;
        if (offering.times.empty()) offering.times = item->times;
        validateOffering(offering);
    }
    auto& changes = schedule.changes;
    const auto key = [](const ScheduledChange& item) {
        return std::make_tuple(item.effectiveDate, item.term, item.subject.has_value(),
            item.subject ? item.subject->id : item.offering->id);
    };
    const auto existing = std::find_if(changes.begin(), changes.end(), [&](const auto& item) { return key(item) == key(change); });
    if (existing == changes.end()) {
        changes.push_back(change);
    } else {
        *existing = change;
    }
    std::sort(changes.begin(), changes.end(), [&](const auto& a, const auto& b) { return key(a) < key(b); });
    if (change.subject) {
        for (const auto& future : terms) {
            if (future.start < change.effectiveDate) continue;
            const auto candidate = applySchedule(base, schedule, future.id, future.start);
            try {
                evaluate(candidate, {"", candidate.name, candidate.id, 1}, {});
            } catch (const AppError& error) {
                if (error.code != "invalid_curriculum") throw;
                throw AppError(400, "invalid_curriculum", "A alteração deixa pré-requisitos inválidos na matriz.");
            }
        }
    }
    repository_.saveSchedule(context, courseId, schedule);
    return change;
}
}
