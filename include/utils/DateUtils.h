#ifndef DATE_UTILS_H
#define DATE_UTILS_H

#include <string>
#include <chrono>

/**
 * @brief Utilidades para manejo de fechas
 */
class DateUtils
{
public:
    // Convertir entre string y time_point
    static std::string timePointToString(const std::chrono::system_clock::time_point &tp,
                                         const std::string &format = "%Y-%m-%d %H:%M:%S");
    static std::chrono::system_clock::time_point stringToTimePoint(const std::string &str,
                                                                   const std::string &format = "%Y-%m-%d %H:%M:%S");

    // Formatos específicos
    static std::string toDateString(const std::chrono::system_clock::time_point &tp);     // YYYY-MM-DD
    static std::string toDateTimeString(const std::chrono::system_clock::time_point &tp); // YYYY-MM-DD HH:MM:SS
    static std::string toReadableString(const std::chrono::system_clock::time_point &tp); // "DD de Mes de YYYY"

    // Operaciones con fechas
    static int daysBetween(const std::chrono::system_clock::time_point &start,
                           const std::chrono::system_clock::time_point &end);
    static std::chrono::system_clock::time_point addDays(const std::chrono::system_clock::time_point &tp,
                                                         int days);
    static std::chrono::system_clock::time_point addHours(const std::chrono::system_clock::time_point &tp,
                                                          int hours);

    // Comparaciones
    static bool isToday(const std::chrono::system_clock::time_point &tp);
    static bool isTomorrow(const std::chrono::system_clock::time_point &tp);
    static bool isThisWeek(const std::chrono::system_clock::time_point &tp);

    // Fecha actual
    static std::chrono::system_clock::time_point now();

    // Crear fecha desde componentes
    static std::chrono::system_clock::time_point createDate(int year, int month, int day,
                                                            int hour = 0, int minute = 0, int second = 0);
};

#endif // DATE_UTILS_H
