#include "ft_ping.h"

// Variable global para manejar la señal SIGINT
t_ping g_ping;

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
    if (resolve_hostname(&g_ping) < 0)
        return (EXIT_FAILURE);

    // Crear socket
    if (create_socket(&g_ping) < 0)
        return (EXIT_FAILURE);

    // Configurar manejador de señal para SIGINT (Ctrl+C)
    signal(SIGINT, handle_interrupt);

    // Inicializar estadísticas
    init_stats(&g_ping.stats);

    // Mostrar mensaje inicial
    printf("PING %s (%s) %ld(%ld) bytes of data.\n", 
           g_ping.hostname, g_ping.ip_str, 
           PING_PACKET_SIZE - sizeof(struct icmphdr),
           PING_PACKET_SIZE + sizeof(struct iphdr));

    // Ejecutar ping
    do_ping(&g_ping);

    return (EXIT_SUCCESS);
}