#include "ft_ping.h"
#include "icmp.h"

// Cálculo de checksum para ICMP
uint16_t calculate_checksum(uint16_t *addr, int len)
{
    int nleft = len;
    uint32_t sum = 0;
    uint16_t *w = addr;
    uint16_t answer = 0;

    // Sumar palabras de 16 bits
    while (nleft > 1)
    {
        sum += *w++;
        nleft -= 2;
    }

    // Si queda un byte, sumarlo también
    if (nleft == 1)
    {
        *(unsigned char *)(&answer) = *(unsigned char *)w;
        sum += answer;
    }

    // Sumar acarreos
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);

    // Complemento a 1
    answer = ~sum;
    return (answer);
}

// Inicializar cabecera ICMP para Echo Request
void init_icmp_header(void *packet, t_ping *ping)
{
    t_icmp_packet *icmp_packet = (t_icmp_packet *)packet;

    // Inicializar la cabecera ICMP
    memset(packet, 0, PING_PACKET_SIZE);
    icmp_packet->header.type = ICMP_ECHO;
    icmp_packet->header.code = 0;
    icmp_packet->header.checksum = 0;
    icmp_packet->header.un.echo.id = htons(ping->pid);
    icmp_packet->header.un.echo.sequence = htons(ping->seq++);

    // Generar datos para el paquete (patrón simple)
    for (size_t i = 0; i < (PING_PACKET_SIZE - sizeof(struct icmphdr)); i++)
        icmp_packet->data[i] = 'a' + (i % 26);

    // Calcular y establecer checksum
    icmp_packet->header.checksum = calculate_checksum((uint16_t *)packet, PING_PACKET_SIZE);
}