#ifndef FT_PING_H
# define FT_PING_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <signal.h>
# include <netdb.h>
# include <sys/time.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
# include <arpa/inet.h>
# include <errno.h>
# include <stdint.h>
# include <stdbool.h>
# include <math.h>

/* Definiciones y constantes */
# define PING_PACKET_SIZE 64
# define PING_DEFAULT_TTL 64
# define PING_DEFAULT_TIMEOUT 1
# define RECV_BUFFER_SIZE 1024

/* Códigos de error */
# define ERROR_FATAL 0
# define ERROR_WARNING 1
# define E_EXIT_OK 0
# define E_EXIT_ERR_HOST 1
# define E_EXIT_ERR_ARGS 64

/* Estructura para almacenar las estadísticas de ping */
typedef struct s_ping_stats {
    int         packets_sent;
    int         packets_received;
    double      min_time;
    double      max_time;
    double      total_time;
    double      sum_squares;
} t_ping_stats;

/* Estructura principal para la configuración y estado de ft_ping */
typedef struct s_ping {
    char        *hostname;             // Nombre de host o IP proporcionado
    char        *ip_str;               // Dirección IP como string
    struct sockaddr_in dest_addr;      // Dirección de destino
    int         sockfd;                // Descriptor del socket
    int         ttl;                   // Time To Live
    int         timeout;               // Timeout en segundos
    uint16_t    pid;                   // PID usado para identificar paquetes
    uint16_t    seq;                   // Número de secuencia del paquete
    bool        verbose;               // Modo verbose (-v)
    t_ping_stats stats;                // Estadísticas
} t_ping;

/* Declaraciones de funciones */

// Gestión de errores
void    error_handler(int level, const char *source, const char *message, bool should_exit);

// Funciones de argumentos/opciones
void    parse_args(int argc, char **argv, t_ping *ping);
void    print_help(void);

// Funciones de DNS
int     resolve_hostname(t_ping *ping);

// Funciones ICMP
uint16_t calculate_checksum(uint16_t *addr, int len);
int     process_icmp_packet(char *buffer, ssize_t bytes, t_ping *ping, struct timeval *tv_recv);

// Funciones de socket
int     create_socket(t_ping *ping);

// Funciones de ping
int     send_ping(t_ping *ping);
int     receive_ping(t_ping *ping);
int     ping_loop(t_ping *ping);

// Funciones de estadísticas
void    init_stats(t_ping_stats *stats);
void    update_stats(t_ping_stats *stats, double rtt);
void    print_stats(t_ping *ping);

// Funciones de utilidad
double  get_time_ms(struct timeval *tv_start, struct timeval *tv_end);
void    handle_signal(int sig);

// Variables globales
extern t_ping g_ping;
extern volatile bool g_ping_loop_running;

#endif /* FT_PING_H */