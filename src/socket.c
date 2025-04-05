#include "ft_ping.h"

/**
 * create_socket - Crea un socket raw para ICMP
 * @param ping: Estructura con la configuración
 * @return: 0 en caso de éxito, -1 en caso de error
 *
 * Esta función crea un socket raw para el protocolo ICMP y configura
 * el TTL según la configuración proporcionada.
 */
int create_socket(t_ping *ping)
{
    int on = 1;
    
    // Crear socket raw para ICMP
    ping->sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (ping->sockfd < 0) {
        perror("ft_ping: socket");
        return -1;
    }

    // Configurar TTL
    if (setsockopt(ping->sockfd, IPPROTO_IP, IP_TTL, &ping->ttl, sizeof(ping->ttl)) < 0) {
        perror("ft_ping: setsockopt TTL");
        close(ping->sockfd);
        return -1;
    }
    
    // Habilitar recepción de cabeceras IP
    if (setsockopt(ping->sockfd, IPPROTO_IP, IP_RECVTTL, &on, sizeof(on)) < 0) {
        if (ping->verbose) {
            perror("ft_ping: setsockopt IP_RECVTTL");
        }
        // No es un error fatal, continuamos
    }
    
    // Opción para recibir errores ICMP
    if (setsockopt(ping->sockfd, SOL_IP, IP_RECVERR, &on, sizeof(on)) < 0) {
        if (ping->verbose) {
            perror("ft_ping: setsockopt IP_RECVERR");
        }
        // No es un error fatal, continuamos
    }

    return 0;
}