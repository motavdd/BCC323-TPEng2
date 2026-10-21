#include "application/AcademicService.hpp"
#include <cmath>
#include <iostream>
#include <functional>
#include <algorithm>

using namespace bcc323;
namespace {
    void check(bool value, const char* message) { 
        if (!value) throw std::runtime_error(message); 
    }

    struct MemoryRepository : AcademicRepository {

        std::map<std::string, AcademicProfile> profiles;

        std::map<std::string, std::vector<std::string>> history;

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
            if (id != catalog.id) throw AppError(404, "course_not_found", "Curso ausente"); return catalog;
        }

        std::vector<std::string> completed(const RequestContext& c) override { 
            return history[c.uid]; 
        }

        void setCompleted(const RequestContext& c, const std::string& id, bool done) override {
            auto& ids = history[c.uid]; ids.erase(std::remove(ids.begin(), ids.end(), id), ids.end()); if (done) ids.push_back(id);
        }
    };

    void expectError(int status, const std::function<void()>& action) {
        try { 
            action(); 
        } catch (const AppError& e) { 
            check(e.status == status, "status incorreto"); return; 
        }
        throw std::runtime_error("Era esperado erro de aplicação");
    }
}
int main() {
    try {
        MemoryRepository repository; AcademicService service(repository);
        RequestContext first{"first", ""}, second{"second", ""};
        check(!service.profile(first), "perfil ausente");
        expectError(409, [&] { service.summary(first); });
        expectError(400, [&] { service.saveProfile(first, {"", "Computação", "", 0}); });
        service.saveProfile(first, {"Aluno", "Computação", "", 1});
        service.saveProfile(second, {"Outro", "Computação", "", 2});
        check(service.profile(first)->courseId == "course", "curso persistente");
        check(service.summary(first).subjects.at("B").state == "blocked", "requisito inicial");
        auto result = service.setCompleted(first, "A", true);
        check(result.completedHours == 60 && std::abs(result.progress - 100.0 / 3) < 0.001, "progresso do domínio");
        check(result.subjects.at("B").state == "available", "requisito liberado");
        check(result.subjects.at("C").state == "unknown", "requisito desconhecido");
        check(service.summary(second).completedCount == 0, "isolamento por UID");
        check(service.setCompleted(first, "A", true).completedCount == 1, "PUT idempotente");
        service.setCompleted(first, "B", true);
        result = service.setCompleted(first, "A", false);
        check(result.completedCount == 1 && result.subjects.at("B").state == "completed", "preservar demais conclusões");
        result = service.setCompleted(first, "B", false);
        check(result.completedHours == 0 && result.subjects.at("B").state == "blocked", "remoção atualiza elegibilidade");
        expectError(404, [&] { service.setCompleted(first, "missing", true); });
        repository.catalog.subjects[0].prerequisites = {"B"};
        expectError(503, [&] { service.summary(first); });
        check(repository.history[first.uid].empty(), "falha não grava histórico");
        std::cout << "PASS: perfil, domínio, progresso, elegibilidade, isolamento e persistência\n";
        return 0;
    } catch (const std::exception& e) { 
        std::cerr << e.what() << '\n'; return 1; 
    }
}
