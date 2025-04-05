#include "ft_ping.h"

/**
 * resolve_hostname - Resuelve un nombre de host a dirección IP
 * @param ping: Estructura principal con la configuración
 * @return: 0 en caso de éxito, -1 en caso de error
 *
 * Esta función implementa una resolución DNS robusta, manejando tanto
 * direcciones IP literales como nombres de host que requieren resolución DNS.
 * Almacena la dirección resuelta y la representación en string para uso posterior.
 */
int resolve_hostname(t_ping *ping)
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    char ip_str[INET6_ADDRSTRLEN];
    int ret;

    // Inicializar hints para getaddrinfo
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;     // Forzar IPv4
    hints.ai_socktype = SOCK_RAW;  // Socket raw
    hints.ai_flags = AI_ADDRCONFIG; // Solo direcciones configuradas en el host

    // Intentar resolver hostname
    ret = getaddrinfo(ping->hostname, NULL, &hints, &result);
    if (ret != 0) {
        fprintf(stderr, "ft_ping: %s: %s\n", ping->hostname, gai_strerror(ret));
        return -1;
    }

    // Buscar una dirección IPv4 válida
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        if (rp->ai_family == AF_INET) {
            // Guardar la dirección de destino
            memcpy(&ping->dest_addr, rp->ai_addr, sizeof(struct sockaddr_in));
            
            // Convertir dirección a string para mostrar
            if (inet_ntop(AF_INET, &(((struct sockaddr_in*)rp->ai_addr)->sin_addr),
                         ip_str, INET_ADDRSTRLEN) == NULL) {
                perror("ft_ping: inet_ntop");
                freeaddrinfo(result);
                return -1;
            }
            
            // Guardar string de IP
            ping->ip_str = strdup(ip_str);
            if (ping->ip_str == NULL) {
                perror("ft_ping: strdup");
                freeaddrinfo(result);
                return -1;
            }
            
            // Liberar memoria y retornar éxito
            freeaddrinfo(result);
            return 0;
        }
    }

    // Si llegamos aquí, no se encontró una dirección válida
    fprintf(stderr, "ft_ping: could not resolve %s to a valid IPv4 address\n", ping->hostname);
    freeaddrinfo(result);
    return -1;
}