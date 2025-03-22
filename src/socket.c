#include "ft_ping.h"

int create_socket(t_ping *ping)
{
    // Crear socket raw para ICMP
    ping->sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (ping->sockfd < 0)
    {
        perror("ft_ping: socket");
        return (-1);
    }

    // Configurar TTL
    if (setsockopt(ping->sockfd, IPPROTO_IP, IP_TTL, &ping->ttl, sizeof(ping->ttl)) < 0)
    {
        perror("ft_ping: setsockopt TTL");
        close(ping->sockfd);
        return (-1);
    }

    return (0);
}
