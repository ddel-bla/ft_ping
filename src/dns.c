#include "ft_ping.h"

int resolve_hostname(t_ping *ping)
{
    struct addrinfo hints;
    struct addrinfo *result;
    char *ip;
    int ret;

    // Inicializar hints
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;     // Solo IPv4
    hints.ai_socktype = SOCK_RAW;  // Socket raw

    // Resolver hostname
    ret = getaddrinfo(ping->hostname, NULL, &hints, &result);
    if (ret != 0)
    {
        fprintf(stderr, "ft_ping: %s: %s\n", ping->hostname, gai_strerror(ret));
        return (-1);
    }

    // Guardar la dirección IP
    ping->dest_addr = *(struct sockaddr_in *)result->ai_addr;

    // Convertir la dirección IP a string
    ip = inet_ntoa(ping->dest_addr.sin_addr);
    ping->ip_str = strdup(ip);
    if (ping->ip_str == NULL)
    {
        fprintf(stderr, "ft_ping: memory allocation failed\n");
        freeaddrinfo(result);
        return (-1);
    }

    freeaddrinfo(result);
    return (0);
}
