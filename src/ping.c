#include "ft_ping.h"
#include "icmp.h"

/**
 * handle_signal - Manejador de señales para SIGINT
 * @param sig: Número de señal recibida
 *
 * Esta función maneja la señal SIGINT (Ctrl+C) para terminar el programa
 * de forma adecuada, mostrando las estadísticas finales.
 */
void handle_signal(int sig)
{
    if (sig == SIGINT) {
        g_ping_loop_running = false;
    }
}

/**
 * send_ping - Envía un paquete ICMP Echo Request
 * @param ping: Estructura con la configuración
 * @return: 0 en caso de éxito, -1 en caso de error
 *
 * Esta función crea y envía un paquete ICMP Echo Request al host especificado.
 */
int send_ping(t_ping *ping)
{
    t_icmp_packet packet;
    ssize_t bytes_sent;

    // Crear paquete ICMP
    if (create_icmp_packet(&packet, ping) < 0) {
        return -1;
    }

    // Enviar paquete
    bytes_sent = sendto(ping->sockfd, &packet, sizeof(packet), 0,
                      (struct sockaddr *)&ping->dest_addr, sizeof(ping->dest_addr));

    if (bytes_sent < 0) {
        if (errno == EACCES) {
            fprintf(stderr, "ft_ping: socket access error. Are you trying to ping broadcast?\n");
        } else {
            perror("ft_ping: sendto");
        }
        return -1;
    }

    // Incrementar contador de paquetes enviados
    ping->stats.packets_sent++;

    return 0;
}

/**
 * ping_loop - Implementa el bucle principal de envío/recepción
 * @param ping: Estructura con la configuración
 * @return: 0 en caso de éxito, -1 en caso de error
 *
 * Esta función implementa el bucle principal de envío de paquetes ICMP Echo Request
 * y recepción de respuestas, gestionando los tiempos y las estadísticas.
 */
int ping_loop(t_ping *ping)
{
    char recv_buffer[RECV_BUFFER_SIZE];
    struct timeval tv_start, tv_end, tv_timeout;
    fd_set read_fds;
    int max_fd, ret;
    struct sockaddr_in from;
    socklen_t from_len = sizeof(from);
    ssize_t bytes_received;
    
    // Iniciar bucle de ping
    while (g_ping_loop_running) {
        // 1. Crear y enviar paquete ICMP Echo Request
        if (send_ping(ping) < 0) {
            return -1;
        }
        
        gettimeofday(&tv_start, NULL);
        
        // 2. Esperar respuestas con timeout
        tv_timeout.tv_sec = 1;  // Esperar 1 segundo
        tv_timeout.tv_usec = 0;
        
        // Usar select para esperar respuesta con timeout
        while (tv_timeout.tv_sec > 0 || tv_timeout.tv_usec > 0) {
            // Inicializar set de descriptores
            FD_ZERO(&read_fds);
            FD_SET(ping->sockfd, &read_fds);
            max_fd = ping->sockfd + 1;
            
            // Esperar datos disponibles o timeout
            ret = select(max_fd, &read_fds, NULL, NULL, &tv_timeout);
            
            // Error en select
            if (ret < 0) {
                if (errno == EINTR) {
                    // Señal recibida, verificar si debemos continuar
                    if (!g_ping_loop_running) {
                        break;
                    }
                    continue;
                }
                perror("ft_ping: select");
                return -1;
            }
            
            // Timeout, continuar con siguiente ping
            if (ret == 0) {
                if (ping->verbose) {
                    printf("Request timeout for icmp_seq=%d\n", ping->seq - 1);
                }
                break;
            }
            
            // Datos disponibles, recibir paquete
            if (FD_ISSET(ping->sockfd, &read_fds)) {
                bytes_received = recvfrom(ping->sockfd, recv_buffer, RECV_BUFFER_SIZE, 0,
                                        (struct sockaddr *)&from, &from_len);
                
                if (bytes_received < 0) {
                    if (errno == EINTR) {
                        continue;  // Interrumpido por señal
                    }
                    perror("ft_ping: recvfrom");
                    return -1;
                }
                
                // Obtener tiempo de recepción
                gettimeofday(&tv_end, NULL);
                
                // Procesar paquete recibido
                process_icmp_packet(recv_buffer, bytes_received, ping, &tv_end);
            }
        }
        
        // 3. Esperar al siguiente ciclo (1 segundo entre pings)
        if (g_ping_loop_running) {
            sleep(1);
        }
    }
    
    // Imprimir estadísticas finales
    print_stats(ping);
    
    return 0;
}