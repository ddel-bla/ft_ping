#include "ft_ping.h"

/**
 * print_help - Muestra la ayuda del programa
 * 
 * Esta función imprime información de uso y las opciones disponibles
 * del comando ft_ping, similar al comando ping estándar de Unix.
 */
void print_help(void)
{
    printf("Usage: ft_ping [OPTION...] HOST ...\n");
    printf("Send ICMP ECHO_REQUEST packets to network hosts.\n\n");
    printf("  -?, --help                 give this help list\n");
    printf("  -v, --verbose              verbose output\n");
    exit(EXIT_SUCCESS);
}

/**
 * parse_args - Analiza los argumentos de línea de comandos
 * @param argc: Número de argumentos
 * @param argv: Array de strings con los argumentos
 * @param ping: Estructura donde almacenar la configuración resultante
 * 
 * Esta función parsea los argumentos proporcionados al programa,
 * configurando las opciones en la estructura ping y verificando
 * que los argumentos sean válidos.
 */
void parse_args(int argc, char **argv, t_ping *ping)
{
    int i;

    // Verificar que haya al menos un argumento
    if (argc < 2) {
        error_handler(ERROR_FATAL, "args", "missing host operand", true);
    }

    // Parsear opciones
    for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            // Opciones cortas
            if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
                ping->verbose = true;
            }
            else if (strcmp(argv[i], "-?") == 0 || strcmp(argv[i], "--help") == 0) {
                print_help();
            }
            else {
                fprintf(stderr, "ft_ping: invalid option -- '%s'\n", argv[i]);
                fprintf(stderr, "Try 'ft_ping --help' for more information.\n");
                exit(EXIT_FAILURE);
            }
        }
        else {
            // El primer argumento que no es una opción es el hostname
            ping->hostname = argv[i];
            break;
        }
    }

    // Verificar si se proporcionó un hostname
    if (ping->hostname == NULL) {
        error_handler(ERROR_FATAL, "args", "missing host operand", true);
    }
    
    // Verificar si hay demasiados hosts (solo se permite uno)
    for (i++; i < argc; i++) {
        if (argv[i][0] != '-') {
            error_handler(ERROR_FATAL, "args", "only one host is allowed", true);
        }
    }
    
    // Verificar que el usuario tenga permisos de root (necesarios para sockets raw)
    if (getuid() != 0) {
        error_handler(ERROR_FATAL, "permissions", "need to be run as root", true);
    }
}