#include "ft_ping.h"
#include "icmp.h"

// Enviar un paquete ICMP Echo Request
int send_ping(t_ping *ping)
{
    t_icmp_packet packet;
    ssize_t bytes_sent;

    // Inicializar cabecera ICMP
    init_icmp_header(&packet, ping);

    // Enviar paquete
    bytes_sent = sendto(ping->sockfd, &packet, PING_PACKET_SIZE, 0,
                       (struct sockaddr *)&ping->dest_addr, sizeof(ping->dest_addr));

    if (bytes_sent < 0)
    {
        if (ping->verbose)
            perror("ft_ping: sendto");
        return (-1);
    }

    // Incrementar contador de paquetes enviados
    ping->stats.packets_sent++;

    return (0);
}

// Recibir y procesar un paquete ICMP Echo Reply
int receive_ping(t_ping *ping, struct timeval *tv_start)
{
    char buffer[PING_PACKET_SIZE + sizeof(struct iphdr)];
    struct sockaddr_in from;
    socklen_t fromlen = sizeof(from);
    ssize_t bytes_received;
    struct timeval tv_end;
    double rtt;
    t_icmp_echo_reply *reply;
    struct iphdr *ip_header;

    // Establecer timeout para recvfrom
    struct timeval timeout;
    timeout.tv_sec = ping->timeout;
    timeout.tv_usec = 0;
    if (setsockopt(ping->sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
    {
        if (ping->verbose)
            perror("ft_ping: setsockopt SO_RCVTIMEO");
        return (-1);
    }

    // Recibir paquete
    bytes_received = recvfrom(ping->sockfd, buffer, sizeof(buffer), 0,
                             (struct sockaddr *)&from, &fromlen);

    if (bytes_received < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            if (ping->verbose)
                printf("Request timeout for icmp_seq=%d\n", ping->seq - 1);
            return (-1);
        }
        else
        {
            if (ping->verbose)
                perror("ft_ping: recvfrom");
            return (-1);
        }
    }

    // Obtener tiempo actual para calcular RTT
    gettimeofday(&tv_end, NULL);
    rtt = get_time_ms(tv_start, &tv_end);

    // Procesar cabeceras IP e ICMP
    ip_header = (struct iphdr *)buffer;
    reply = (t_icmp_echo_reply *)buffer;

    // Verificar que sea una respuesta ICMP Echo Reply y corresponda a nuestro PID
    if (reply->icmp_header.type != ICMP_ECHOREPLY ||
        ntohs(reply->icmp_header.un.echo.id) != ping->pid)
    {
        if (ping->verbose)
            printf("Received ICMP packet with type %d, code %d\n",
                  reply->icmp_header.type, reply->icmp_header.code);
        return (-1);
    }

    // Incrementar contador de paquetes recibidos y actualizar estadísticas
    ping->stats.packets_received++;
    update_stats(&ping->stats, rtt);

    // Mostrar información del paquete
    printf("%ld bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n",
          bytes_received - sizeof(struct iphdr), inet_ntoa(from.sin_addr),
          ntohs(reply->icmp_header.un.echo.sequence), ip_header->ttl, rtt);

    return (0);
}
