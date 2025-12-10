#include "utils/DateUtils.h"
#include <sstream>
#include <iomanip>
#include <ctime>

using namespace std;

// Convertir time_point a string
string DateUtils::timePointToString(const chrono::system_clock::time_point& tp,
                                         const string& format) {
    time_t time = chrono::system_clock::to_time_t(tp);
    tm* tm = localtime(&time);
    
    stringstream ss;
    ss << put_time(tm, format.c_str());
    return ss.str();
}

// Convertir string a time_point (implementación simplificada)
chrono::system_clock::time_point DateUtils::stringToTimePoint(const string& str,
                                                                    const string& format) {
    tm tm = {};
    istringstream ss(str);
    ss >> get_time(&tm, format.c_str());
    
    time_t time = mktime(&tm);
    return chrono::system_clock::from_time_t(time);
}

// Formatos específicos
string DateUtils::toDateString(const chrono::system_clock::time_point& tp) {
    return timePointToString(tp, "%Y-%m-%d");
}

string DateUtils::toDateTimeString(const chrono::system_clock::time_point& tp) {
    return timePointToString(tp, "%Y-%m-%d %H:%M:%S");
}

string DateUtils::toReadableString(const chrono::system_clock::time_point& tp) {
    time_t time = chrono::system_clock::to_time_t(tp);
    tm* tm = localtime(&time);
    
    const char* meses[] = {
        "Enero", "Febrero", "Marzo", "Abril", "Mayo", "Junio",
        "Julio", "Agosto", "Septiembre", "Octubre", "Noviembre", "Diciembre"
    };
    
    stringstream ss;
    ss << tm->tm_mday << " de " << meses[tm->tm_mon] << " de " << (1900 + tm->tm_year);
    return ss.str();
}

// Operaciones con fechas
int DateUtils::daysBetween(const chrono::system_clock::time_point& start,
                          const chrono::system_clock::time_point& end) {
    auto diff = chrono::duration_cast<chrono::hours>(end - start);
    return static_cast<int>(diff.count() / 24);
}

chrono::system_clock::time_point DateUtils::addDays(
    const chrono::system_clock::time_point& tp, int days) {
    return tp + chrono::hours(days * 24);
}

chrono::system_clock::time_point DateUtils::addHours(
    const chrono::system_clock::time_point& tp, int hours) {
    return tp + chrono::hours(hours);
}

// Comparaciones
bool DateUtils::isToday(const chrono::system_clock::time_point& tp) {
    auto now = chrono::system_clock::now();
    return toDateString(tp) == toDateString(now);
}

bool DateUtils::isTomorrow(const chrono::system_clock::time_point& tp) {
    auto tomorrow = addDays(chrono::system_clock::now(), 1);
    return toDateString(tp) == toDateString(tomorrow);
}

bool DateUtils::isThisWeek(const chrono::system_clock::time_point& tp) {
    auto now = chrono::system_clock::now();
    int days = daysBetween(now, tp);
    return days >= 0 && days <= 7;
}

// Fecha actual
chrono::system_clock::time_point DateUtils::now() {
    return chrono::system_clock::now();
}

// Crear fecha desde componentes
chrono::system_clock::time_point DateUtils::createDate(
    int year, int month, int day, int hour, int minute, int second) {
    tm tm = {};
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    tm.tm_hour = hour;
    tm.tm_min = minute;
    tm.tm_sec = second;
    
    time_t time = mktime(&tm);
    return chrono::system_clock::from_time_t(time);
}

