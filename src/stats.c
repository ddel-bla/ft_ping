#include "ft_ping.h"
#include <math.h>

/**
 * init_stats - Inicializa la estructura de estadísticas
 * @param stats: Puntero a la estructura de estadísticas
 *
 * Esta función inicializa los valores de la estructura de estadísticas
 * para comenzar a recopilar datos.
 */
void init_stats(t_ping_stats *stats)
{
    stats->packets_sent = 0;
    stats->packets_received = 0;
    stats->min_time = 0.0;
    stats->max_time = 0.0;
    stats->total_time = 0.0;
    stats->sum_squares = 0.0;
}

/**
 * update_stats - Actualiza las estadísticas con un nuevo RTT
 * @param stats: Puntero a la estructura de estadísticas
 * @param rtt: Tiempo de ida y vuelta en milisegundos
 *
 * Esta función actualiza los valores de estadísticas con un nuevo
 * tiempo de ida y vuelta (RTT), calculando mínimos, máximos, etc.
 */
void update_stats(t_ping_stats *stats, double rtt)
{
    // Actualizar contador de paquetes recibidos
    stats->packets_received++;
    
    // Actualizar tiempo mínimo (si es el primero o menor que el mínimo actual)
    if (stats->min_time == 0.0 || rtt < stats->min_time) {
        stats->min_time = rtt;
    }
    
    // Actualizar tiempo máximo (si es mayor que el máximo actual)
    if (rtt > stats->max_time) {
        stats->max_time = rtt;
    }
    
    // Actualizar suma total para calcular promedio
    stats->total_time += rtt;
    
    // Actualizar suma de cuadrados para calcular desviación estándar
    stats->sum_squares += rtt * rtt;
}

/**
 * print_stats - Imprime las estadísticas de ping
 * @param ping: Estructura principal con la configuración y estadísticas
 *
 * Esta función calcula e imprime las estadísticas finales del ping,
 * incluyendo porcentaje de pérdida, RTT mínimo/promedio/máximo y
 * desviación estándar.
 */
void print_stats(t_ping *ping)
{
    t_ping_stats *stats = &ping->stats;
    double avg = 0.0, mdev = 0.0;
    int loss_percent = 0;
    double total_time_ms = 0.0;
    
    // Calcular estadísticas solo si se recibieron paquetes
    if (stats->packets_received > 0) {
        // Tiempo promedio
        avg = stats->total_time / stats->packets_received;
        
        // Desviación estándar
        // stddev = sqrt((sum(x²)/n) - (sum(x)/n)²)
        mdev = sqrt((stats->sum_squares / stats->packets_received) - 
                    (avg * avg));
    }
    
    // Calcular porcentaje de pérdida de paquetes
    if (stats->packets_sent > 0) {
        loss_percent = 100 - ((stats->packets_received * 100) / stats->packets_sent);
    }
    
    // Calcular tiempo total aproximado (en ms)
    total_time_ms = (stats->packets_sent > 0) ? 
                    (stats->packets_sent - 1) * 1000.0 : 0.0;
    
    // Imprimir estadísticas
    printf("\n--- %s ping statistics ---\n", ping->hostname);
    printf("%d packets transmitted, %d received, %d%% packet loss, time %.0fms\n",
           stats->packets_sent, stats->packets_received, loss_percent, total_time_ms);
    
    // Imprimir estadísticas de RTT solo si se recibieron paquetes
    if (stats->packets_received > 0) {
        printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n",
               stats->min_time, avg, stats->max_time, mdev);
    }
}