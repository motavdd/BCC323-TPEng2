#pragma once
#include "application/AcademicRepository.hpp"
#include <crow/json.h>
namespace bcc323 {
    crow::json::wvalue toJson(const AcademicProfile&);
    crow::json::wvalue toJson(const CourseRecord&);
    crow::json::wvalue toJson(const AcademicSummary&);
    AcademicProfile profileFromJson(const crow::json::rvalue&);
}
