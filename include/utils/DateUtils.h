#ifndef DATE_UTILS_H
#define DATE_UTILS_H

#include <string>
#include <chrono>

using namespace std;

/**
 * @brief Utilidades para manejo de fechas
 */
class DateUtils
{
public:
    // Convertir entre string y time_point
    static string timePointToString(const chrono::system_clock::time_point &tp,
                                         const string &format = "%Y-%m-%d %H:%M:%S");
    static chrono::system_clock::time_point stringToTimePoint(const string &str,
                                                                   const string &format = "%Y-%m-%d %H:%M:%S");

    // Formatos específicos
    static string toDateString(const chrono::system_clock::time_point &tp);     // YYYY-MM-DD
    static string toDateTimeString(const chrono::system_clock::time_point &tp); // YYYY-MM-DD HH:MM:SS
    static string toReadableString(const chrono::system_clock::time_point &tp); // "DD de Mes de YYYY"

    // Operaciones con fechas
    static int daysBetween(const chrono::system_clock::time_point &start,
                           const chrono::system_clock::time_point &end);
    static chrono::system_clock::time_point addDays(const chrono::system_clock::time_point &tp,
                                                         int days);
    static chrono::system_clock::time_point addHours(const chrono::system_clock::time_point &tp,
                                                          int hours);

    // Comparaciones
    static bool isToday(const chrono::system_clock::time_point &tp);
    static bool isTomorrow(const chrono::system_clock::time_point &tp);
    static bool isThisWeek(const chrono::system_clock::time_point &tp);

    // Fecha actual
    static chrono::system_clock::time_point now();

    // Crear fecha desde componentes
    static chrono::system_clock::time_point createDate(int year, int month, int day,
                                                            int hour = 0, int minute = 0, int second = 0);
};

#endif // DATE_UTILS_H
