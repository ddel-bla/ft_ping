#include "ft_ping.h"
#include "icmp.h"

// Variables globales
t_ping g_ping;
volatile bool g_ping_loop_running = true;

/**
 * main - Función principal del programa ft_ping
 * @param argc: Número de argumentos
 * @param argv: Array de strings con los argumentos
 * @return: Código de salida (0 en caso de éxito)
 *
 * Esta función inicializa la configuración, procesa los argumentos,
 * resuelve el hostname, crea el socket y ejecuta el bucle de ping.
 */
int main(int argc, char **argv)
{
    
    // Inicializar estructura ping
    memset(&g_ping, 0, sizeof(t_ping));
    g_ping.ttl = PING_DEFAULT_TTL;
    g_ping.timeout = PING_DEFAULT_TIMEOUT;
    g_ping.pid = getpid() & 0xFFFF; // Usar los 16 bits menos significativos del PID

    // Parsear argumentos
    parse_args(argc, argv, &g_ping);

    // Resolver hostname a IP
    if (resolve_hostname(&g_ping) < 0) {
        return E_EXIT_ERR_HOST;
    }

    // Crear socket
    if (create_socket(&g_ping) < 0) {
        if (g_ping.ip_str) {
            free(g_ping.ip_str);
        }
        return E_EXIT_ERR_HOST;
    }

    // Configurar manejador de señal para SIGINT (Ctrl+C)
    signal(SIGINT, handle_signal);

    // Inicializar estadísticas
    init_stats(&g_ping.stats);

    // Mostrar mensaje inicial
    printf("PING %s (%s) %ld(%ld) bytes of data.\n", 
           g_ping.hostname, g_ping.ip_str, 
           PING_PACKET_SIZE - sizeof(struct icmphdr),
           PING_PACKET_SIZE + sizeof(struct iphdr));

    // Ejecutar bucle de ping
    ping_loop(&g_ping);

    // Cerrar socket y liberar recursos
    if (g_ping.sockfd > 0) {
        close(g_ping.sockfd);
    }
    if (g_ping.ip_str) {
        free(g_ping.ip_str);
    }

    return (g_ping.stats.packets_received > 0) ? E_EXIT_OK : E_EXIT_ERR_HOST;
}