#pragma once
#include "application/AcademicRepository.hpp"
#include <crow.h>
namespace bcc323 { 
    RequestContext authenticate(const crow::request&);
}
