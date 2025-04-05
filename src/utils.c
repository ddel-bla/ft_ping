#include "ft_ping.h"

/**
 * get_time_ms - Calcula la diferencia de tiempo en milisegundos
 * @param tv_start: Tiempo de inicio
 * @param tv_end: Tiempo de fin
 * @return: Diferencia en milisegundos
 *
 * Esta función calcula la diferencia de tiempo entre dos structs timeval
 * y devuelve el resultado en milisegundos con precisión de 3 decimales.
 */
double get_time_ms(struct timeval *tv_start, struct timeval *tv_end)
{
    double start, end;
    
    // Convertir tiempos a milisegundos (con componente de microsegundos)
    start = (double)tv_start->tv_sec * 1000.0 + (double)tv_start->tv_usec / 1000.0;
    end = (double)tv_end->tv_sec * 1000.0 + (double)tv_end->tv_usec / 1000.0;
    
    // Retornar diferencia
    return (end - start);
}