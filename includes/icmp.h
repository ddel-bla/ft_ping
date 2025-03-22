#ifndef ICMP_H
# define ICMP_H

# include "ft_ping.h"

// Estructura para formar un paquete ICMP Echo Request
typedef struct  s_icmp_packet {
    struct icmphdr header;
    char data[PING_PACKET_SIZE - sizeof(struct icmphdr)];
}               t_icmp_packet;

// Estructura para un paquete ICMP Echo Reply (incluye header IP)
typedef struct  s_icmp_echo_reply {
    struct iphdr    ip_header;
    struct icmphdr  icmp_header;
    char            data[PING_PACKET_SIZE - sizeof(struct icmphdr)];
}               t_icmp_echo_reply;

#endif