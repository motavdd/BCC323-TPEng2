#pragma once
#include "application/AcademicRepository.hpp"
#include <crow/json.h>

namespace bcc323 {
    class FirestoreRepository : public AcademicRepository {

        std::string base_, documents_, documentName_;
        crow::json::wvalue request(const RequestContext&, const std::string& method, const std::string& url, const crow::json::wvalue* body = nullptr, bool allowMissing = false);
        void write(const RequestContext&, const std::string& path, const crow::json::wvalue& fields, const std::string& timestampField, bool createOnly = false);
        std::vector<crow::json::wvalue> list(const RequestContext&, const std::string& path);

    public:

        FirestoreRepository();

        std::optional<AcademicProfile> profile(const RequestContext&) override;

        void saveProfile(const RequestContext&, const AcademicProfile&, bool create) override;

        std::vector<CourseRecord> courses(const RequestContext&) override;

        CourseRecord course(const RequestContext&, const std::string&) override;

        std::vector<std::string> completed(const RequestContext&) override;

        void setCompleted(const RequestContext&, const std::string&, bool) override;

    };
}
