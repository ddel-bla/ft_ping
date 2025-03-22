#include "ft_ping.h"

// Calcular diferencia de tiempo en milisegundos
double get_time_ms(struct timeval *tv_start, struct timeval *tv_end)
{
    return ((tv_end->tv_sec - tv_start->tv_sec) * 1000.0 +
            (tv_end->tv_usec - tv_start->tv_usec) / 1000.0);
}

// Manejador de señal SIGINT (Ctrl+C)
void handle_interrupt(int sig)
{
    (void)sig;
    
    // Imprimir estadísticas y cerrar el socket
    print_stats(&g_ping);
    
    if (g_ping.sockfd > 0)
        close(g_ping.sockfd);
    
    // Liberar memoria asignada
    if (g_ping.ip_str)
        free(g_ping.ip_str);
    
    exit(EXIT_SUCCESS);
}
