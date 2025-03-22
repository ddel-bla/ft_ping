#include "ft_ping.h"

void do_ping(t_ping *ping)
{
    struct timeval tv_start;

    while (1)
    {
        // Registrar tiempo de envío
        gettimeofday(&tv_start, NULL);

        // Enviar paquete ICMP Echo Request
        if (send_ping(ping) == 0)
        {
            // Recibir respuesta
            receive_ping(ping, &tv_start);
        }

        // Esperar 1 segundo entre pings
        sleep(1);
    }
}
