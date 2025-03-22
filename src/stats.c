#include "ft_ping.h"
#include <math.h>

void init_stats(t_ping_stats *stats)
{
    stats->packets_sent = 0;
    stats->packets_received = 0;
    stats->min_time = 0.0;
    stats->max_time = 0.0;
    stats->total_time = 0.0;
    stats->sum_squares = 0.0;
}

void update_stats(t_ping_stats *stats, double rtt)
{
    // Actualizar tiempo mínimo
    if (stats->min_time == 0.0 || rtt < stats->min_time)
        stats->min_time = rtt;

    // Actualizar tiempo máximo
    if (rtt > stats->max_time)
        stats->max_time = rtt;

    // Actualizar suma total para calcular promedio
    stats->total_time += rtt;

    // Actualizar suma de cuadrados para calcular desviación estándar
    stats->sum_squares += rtt * rtt;
}

void print_stats(t_ping *ping)
{
    t_ping_stats *stats = &ping->stats;
    double avg, mdev;
    int loss_percent;

    // Calcular estadísticas
    if (stats->packets_received > 0)
    {
        avg = stats->total_time / stats->packets_received;
        mdev = sqrt((stats->sum_squares / stats->packets_received) - (avg * avg));
    }
    else
    {
        avg = 0;
        mdev = 0;
    }

    // Calcular porcentaje de pérdida
    if (stats->packets_sent > 0)
        loss_percent = 100 - ((stats->packets_received * 100) / stats->packets_sent);
    else
        loss_percent = 0;

    // Imprimir estadísticas
    printf("\n--- %s ping statistics ---\n", ping->hostname);
    printf("%d packets transmitted, %d received, %d%% packet loss, time %.0f ms\n",
           stats->packets_sent, stats->packets_received, loss_percent, stats->total_time);
    
    if (stats->packets_received > 0)
    {
        printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n",
               stats->min_time, avg, stats->max_time, mdev);
    }
}
