#include "ft_ping.h"
#include "icmp.h"

/**
 * receive_ping - Recibe y procesa un paquete ICMP Echo Reply
 * @param ping: Estructura con la configuración
 * @return: 0 en caso de éxito, -1 en caso de error o timeout
 *
 * Esta función espera la recepción de un paquete ICMP Echo Reply
 * en respuesta a un paquete enviado previamente, con un timeout
 * configurable. Si recibe el paquete, procesa la información y
 * muestra los resultados.
 */
int receive_ping(t_ping *ping)
{
    char buffer[RECV_BUFFER_SIZE];
    struct sockaddr_in from;
    socklen_t fromlen = sizeof(from);
    ssize_t bytes_received;
    struct timeval tv_end;
    struct timeval timeout;
    
    // Establecer timeout para recvfrom
    timeout.tv_sec = ping->timeout;
    timeout.tv_usec = 0;
    
    if (setsockopt(ping->sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        if (ping->verbose) {
            perror("ft_ping: setsockopt SO_RCVTIMEO");
        }
        return -1;
    }
    
    // Recibir paquete
    bytes_received = recvfrom(ping->sockfd, buffer, sizeof(buffer), 0,
                             (struct sockaddr *)&from, &fromlen);
    
    if (bytes_received < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // Timeout, no se recibió respuesta
            if (ping->verbose) {
                printf("Request timeout for icmp_seq=%d\n", ping->seq - 1);
            }
            return -1;
        } else {
            if (ping->verbose) {
                perror("ft_ping: recvfrom");
            }
            return -1;
        }
    }
    
    // Obtener tiempo actual para calcular RTT
    gettimeofday(&tv_end, NULL);
    
    // Procesar paquete recibido
    return process_icmp_packet(buffer, bytes_received, ping, &tv_end);
}