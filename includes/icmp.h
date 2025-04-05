#ifndef ICMP_H
# define ICMP_H

# include "ft_ping.h"

/**
 * Estructura para formar un paquete ICMP Echo Request
 * Contiene la cabecera ICMP seguida de los datos de la carga útil
 */
typedef struct s_icmp_packet {
    struct icmphdr header;              // Cabecera ICMP
    char data[PING_PACKET_SIZE - sizeof(struct icmphdr)]; // Datos (incluye timestamp)
} t_icmp_packet;

/**
 * Estructura para un paquete ICMP Echo Reply recibido
 * Incluye la cabecera IP, seguida de la cabecera ICMP y los datos
 */
typedef struct s_icmp_echo_reply {
    struct iphdr    ip_header;          // Cabecera IP
    struct icmphdr  icmp_header;        // Cabecera ICMP
    char            data[PING_PACKET_SIZE - sizeof(struct icmphdr)]; // Datos
} t_icmp_echo_reply;

/* Declaraciones de funciones para el manejo de ICMP */
int create_icmp_packet(t_icmp_packet *packet, t_ping *ping);

#endif /* ICMP_H */