#include "utils/DateUtils.h"
#include <sstream>
#include <iomanip>
#include <ctime>

// Convertir time_point a string
std::string DateUtils::timePointToString(const std::chrono::system_clock::time_point& tp,
                                         const std::string& format) {
    std::time_t time = std::chrono::system_clock::to_time_t(tp);
    std::tm* tm = std::localtime(&time);
    
    std::stringstream ss;
    ss << std::put_time(tm, format.c_str());
    return ss.str();
}

// Convertir string a time_point (implementación simplificada)
std::chrono::system_clock::time_point DateUtils::stringToTimePoint(const std::string& str,
                                                                    const std::string& format) {
    std::tm tm = {};
    std::istringstream ss(str);
    ss >> std::get_time(&tm, format.c_str());
    
    std::time_t time = std::mktime(&tm);
    return std::chrono::system_clock::from_time_t(time);
}

// Formatos específicos
std::string DateUtils::toDateString(const std::chrono::system_clock::time_point& tp) {
    return timePointToString(tp, "%Y-%m-%d");
}

std::string DateUtils::toDateTimeString(const std::chrono::system_clock::time_point& tp) {
    return timePointToString(tp, "%Y-%m-%d %H:%M:%S");
}

std::string DateUtils::toReadableString(const std::chrono::system_clock::time_point& tp) {
    std::time_t time = std::chrono::system_clock::to_time_t(tp);
    std::tm* tm = std::localtime(&time);
    
    const char* meses[] = {
        "Enero", "Febrero", "Marzo", "Abril", "Mayo", "Junio",
        "Julio", "Agosto", "Septiembre", "Octubre", "Noviembre", "Diciembre"
    };
    
    std::stringstream ss;
    ss << tm->tm_mday << " de " << meses[tm->tm_mon] << " de " << (1900 + tm->tm_year);
    return ss.str();
}

// Operaciones con fechas
int DateUtils::daysBetween(const std::chrono::system_clock::time_point& start,
                          const std::chrono::system_clock::time_point& end) {
    auto diff = std::chrono::duration_cast<std::chrono::hours>(end - start);
    return static_cast<int>(diff.count() / 24);
}

std::chrono::system_clock::time_point DateUtils::addDays(
    const std::chrono::system_clock::time_point& tp, int days) {
    return tp + std::chrono::hours(days * 24);
}

std::chrono::system_clock::time_point DateUtils::addHours(
    const std::chrono::system_clock::time_point& tp, int hours) {
    return tp + std::chrono::hours(hours);
}

// Comparaciones
bool DateUtils::isToday(const std::chrono::system_clock::time_point& tp) {
    auto now = std::chrono::system_clock::now();
    return toDateString(tp) == toDateString(now);
}

bool DateUtils::isTomorrow(const std::chrono::system_clock::time_point& tp) {
    auto tomorrow = addDays(std::chrono::system_clock::now(), 1);
    return toDateString(tp) == toDateString(tomorrow);
}

bool DateUtils::isThisWeek(const std::chrono::system_clock::time_point& tp) {
    auto now = std::chrono::system_clock::now();
    int days = daysBetween(now, tp);
    return days >= 0 && days <= 7;
}

// Fecha actual
std::chrono::system_clock::time_point DateUtils::now() {
    return std::chrono::system_clock::now();
}

// Crear fecha desde componentes
std::chrono::system_clock::time_point DateUtils::createDate(
    int year, int month, int day, int hour, int minute, int second) {
    std::tm tm = {};
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    tm.tm_hour = hour;
    tm.tm_min = minute;
    tm.tm_sec = second;
    
    std::time_t time = std::mktime(&tm);
    return std::chrono::system_clock::from_time_t(time);
}

