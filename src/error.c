#include "ft_ping.h"

/**
 * error_handler - Maneja errores de forma unificada
 * @param level: Nivel de error (ERROR_FATAL, ERROR_WARNING)
 * @param source: Fuente del error (función o componente)
 * @param message: Mensaje de error
 * @param should_exit: Indica si el programa debe terminar tras el error
 * 
 * Esta función proporciona un manejo de errores consistente, permitiendo
 * tanto mensajes personalizados como el uso de errno para errores de sistema.
 * Además, puede terminar el programa si el error es fatal.
 */
void error_handler(int level, const char *source, const char *message, bool should_exit)
{
    // Prefijo según el nivel de error
    const char *prefix = (level == ERROR_FATAL) ? "Error" : "Warning";
    
    // Imprimir fuente del error si se proporciona
    if (source) {
        fprintf(stderr, "ft_ping: %s in %s: %s\n", prefix, source, message);
    } else {
        fprintf(stderr, "ft_ping: %s: %s\n", prefix, message);
    }
    
    // Si el mensaje es NULL y errno está establecido, mostrar el error del sistema
    if (message == NULL && errno != 0) {
        fprintf(stderr, "ft_ping: %s: %s\n", prefix, strerror(errno));
    }
    
    // Si es un error fatal, limpiar recursos y salir
    if (should_exit) {
        // Mostrar estadísticas si hay disponibles
        if (g_ping.stats.packets_sent > 0) {
            print_stats(&g_ping);
        }
        
        // Limpiar recursos
        if (g_ping.sockfd > 0) {
            close(g_ping.sockfd);
        }
        if (g_ping.ip_str) {
            free(g_ping.ip_str);
        }
        
        exit(EXIT_FAILURE);
    }
}