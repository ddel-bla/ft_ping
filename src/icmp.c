#include "ft_ping.h"
#include "icmp.h"

/**
 * calculate_checksum - Calcula el checksum para un paquete ICMP
 * @param addr: Puntero al inicio de los datos
 * @param len: Longitud de los datos en bytes
 * @return: Valor del checksum calculado
 *
 * Esta función implementa el algoritmo estándar de checksum usado
 * en protocolos de Internet como ICMP, IP, TCP y UDP.
 */
uint16_t calculate_checksum(uint16_t *addr, int len)
{
    int nleft = len;
    uint32_t sum = 0;
    uint16_t *w = addr;
    uint16_t answer = 0;

    // Sumar palabras de 16 bits
    while (nleft > 1) {
        sum += *w++;
        nleft -= 2;
    }

    // Manejar byte impar si existe
    if (nleft == 1) {
        *(unsigned char *)(&answer) = *(unsigned char *)w;
        sum += answer;
    }

    // Añadir los acarreos
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    
    // Retornar complemento a uno
    answer = ~sum;
    return answer;
}

/**
 * create_icmp_packet - Crea un paquete ICMP Echo Request
 * @param packet: Buffer donde se almacenará el paquete
 * @param ping: Estructura con información de configuración
 * @return: 0 en caso de éxito, -1 en caso de error
 *
 * Esta función inicializa un paquete ICMP Echo Request con
 * los parámetros adecuados y calcula su checksum.
 */
int create_icmp_packet(t_icmp_packet *packet, t_ping *ping)
{
    struct timeval *timestamp;
    
    // Limpiar el buffer del paquete
    memset(packet, 0, sizeof(t_icmp_packet));
    
    // Configurar cabecera ICMP para Echo Request
    packet->header.type = ICMP_ECHO;
    packet->header.code = 0;
    packet->header.un.echo.id = htons(ping->pid);
    packet->header.un.echo.sequence = htons(ping->seq++);
    
    // Incorporar timestamp en los datos para medir RTT
    timestamp = (struct timeval *)packet->data;
    if (gettimeofday(timestamp, NULL) == -1) {
        perror("ft_ping: gettimeofday");
        return -1;
    }
    
    // Rellenar el resto de datos con un patrón para debugging
    for (size_t i = sizeof(struct timeval); 
         i < sizeof(packet->data); i++) {
        packet->data[i] = (char)('0' + (i % 10));
    }
    
    // Calcular y establecer el checksum
    packet->header.checksum = 0; // Debe ser 0 antes de calcular
    packet->header.checksum = calculate_checksum((uint16_t *)packet, sizeof(t_icmp_packet));
    
    return 0;
}

/**
 * process_icmp_packet - Procesa un paquete ICMP recibido
 * @param buffer: Buffer con el paquete recibido
 * @param bytes: Número de bytes recibidos
 * @param ping: Estructura con información de configuración
 * @param tv_recv: Tiempo de recepción del paquete
 * @return: 1 si es un paquete válido, 0 si debe ignorarse, -1 en caso de error
 *
 * Esta función analiza un paquete ICMP recibido, verifica si es una respuesta
 * a uno de nuestros paquetes, y extrae la información relevante.
 */
int process_icmp_packet(char *buffer, ssize_t bytes, t_ping *ping, struct timeval *tv_recv)
{
    struct iphdr *ip_hdr;
    struct icmphdr *icmp_hdr;
    struct timeval *tv_send;
    double rtt;
    char src_ip[INET_ADDRSTRLEN];
    
    // Extraer cabeceras
    ip_hdr = (struct iphdr *)buffer;
    icmp_hdr = (struct icmphdr *)(buffer + sizeof(struct iphdr));
    
    // Convertir IP origen a string
    inet_ntop(AF_INET, &(ip_hdr->saddr), src_ip, INET_ADDRSTRLEN);
    
    // Verificar si es una respuesta Echo Reply a nuestro paquete
    if (icmp_hdr->type == ICMP_ECHOREPLY) {
        // Verificar que el ID coincida con nuestro PID
        if (ntohs(icmp_hdr->un.echo.id) != ping->pid) {
            if (ping->verbose) {
                printf("Received ICMP Echo Reply, but ID doesn't match (got %d, expected %d)\n",
                      ntohs(icmp_hdr->un.echo.id), ping->pid);
            }
            return 0; // No es para nosotros
        }
        
        // Extraer timestamp de envío
        tv_send = (struct timeval *)(buffer + sizeof(struct iphdr) + sizeof(struct icmphdr));
        
        // Calcular RTT en milisegundos
        rtt = get_time_ms(tv_send, tv_recv);
        
        // Actualizar estadísticas
        update_stats(&ping->stats, rtt);
        
        // Mostrar información del paquete
        printf("%ld bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n",
              bytes - sizeof(struct iphdr), src_ip,
              ntohs(icmp_hdr->un.echo.sequence), ip_hdr->ttl, rtt);
              
        return 1;
    }
    // Manejar mensajes de error ICMP
    else if (icmp_hdr->type == ICMP_DEST_UNREACH ||
             icmp_hdr->type == ICMP_TIME_EXCEEDED ||
             icmp_hdr->type == ICMP_PARAMETERPROB) {
        
        // En los mensajes de error, nuestro paquete original está dentro del mensaje
        struct iphdr *orig_ip = (struct iphdr *)(buffer + sizeof(struct iphdr) + sizeof(struct icmphdr));
        struct icmphdr *orig_icmp = (struct icmphdr *)(buffer + sizeof(struct iphdr) + 
                                                     sizeof(struct icmphdr) + sizeof(struct iphdr));
        
        // Verificar que el paquete original sea nuestro
        if (orig_ip->protocol == IPPROTO_ICMP && 
            ntohs(orig_icmp->un.echo.id) == ping->pid) {
            
            printf("From %s icmp_seq=%d ", src_ip, ntohs(orig_icmp->un.echo.sequence));
            
            // Identificar tipo de error
            switch (icmp_hdr->type) {
                case ICMP_DEST_UNREACH:
                    printf("Destination Unreachable");
                    if (icmp_hdr->code == ICMP_HOST_UNREACH)
                        printf(" (Host Unreachable)");
                    else if (icmp_hdr->code == ICMP_NET_UNREACH)
                        printf(" (Network Unreachable)");
                    break;
                    
                case ICMP_TIME_EXCEEDED:
                    printf("Time to live exceeded");
                    break;
                    
                case ICMP_PARAMETERPROB:
                    printf("Parameter problem");
                    break;
            }
            printf("\n");
            
            // Información verbose
            if (ping->verbose) {
                printf("  ICMP type=%d, code=%d\n", icmp_hdr->type, icmp_hdr->code);
            }
            
            return 1;
        }
    }
    
    // Otros tipos de ICMP, mostrar solo en modo verbose
    if (ping->verbose) {
        printf("Received ICMP type=%d, code=%d from %s\n", 
              icmp_hdr->type, icmp_hdr->code, src_ip);
    }
    
    return 0;
}