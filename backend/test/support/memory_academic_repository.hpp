#pragma once

#include "application/AcademicRepository.hpp"
#include <algorithm>

namespace bcc323::test {
    struct MemoryRepository : AcademicRepository {

        std::map<std::string, AcademicProfile> profiles;

        std::map<std::string, std::vector<std::string>> history;
        bool failHistoryWrite = false;

        CourseRecord catalog{"course", "Computação", false, {
            {"A", "Introdução", 60, 1, {}, true},
            {"B", "Estruturas", 90, 2, {"A"}, true},
            {"C", "A validar", 30, 2, {}, false},
        }, {}};

        std::optional<AcademicProfile> profile(const RequestContext& c) override {
            if (!profiles.count(c.uid)) {
                return std::nullopt;
            }
            return profiles.at(c.uid);
        }

        void saveProfile(const RequestContext& c, const AcademicProfile& p, bool) override {
            profiles[c.uid] = p;
        }

        std::vector<CourseRecord> courses(const RequestContext&) override {
            return {catalog};
        }

        CourseRecord course(const RequestContext&, const std::string& id) override {
            if (id != catalog.id) throw AppError(404, "course_not_found", "Curso ausente");
            return catalog;
        }

        std::vector<std::string> completed(const RequestContext& c) override {
            return history[c.uid];
        }

        void setCompleted(const RequestContext& c, const std::string& id, bool done) override {
            if (failHistoryWrite) throw AppError(503, "storage_unavailable", "Banco indisponível");
            auto& ids = history[c.uid];
            ids.erase(std::remove(ids.begin(), ids.end(), id), ids.end());
            if (done) ids.push_back(id);
        }
    };
}
