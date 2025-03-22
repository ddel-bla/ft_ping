#include "ft_ping.h"

void print_help(void)
{
    printf("Usage: ft_ping [OPTION...] HOST ...\n");
    printf("Send ICMP ECHO_REQUEST packets to network hosts.\n\n");
    printf("  -?, --help                 give this help list\n");
    printf("  -v, --verbose              verbose output\n");
    exit(EXIT_SUCCESS);
}

void parse_args(int argc, char **argv, t_ping *ping)
{
    int i;

    if (argc < 2)
    {
        fprintf(stderr, "ft_ping: missing host operand\n");
        fprintf(stderr, "Try 'ft_ping --help' for more information.\n");
        exit(EXIT_FAILURE);
    }

    // Parsear opciones
    for (i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0)
                ping->verbose = true;
            else if (strcmp(argv[i], "-?") == 0 || strcmp(argv[i], "--help") == 0)
                print_help();
            else
            {
                fprintf(stderr, "ft_ping: invalid option -- '%s'\n", argv[i]);
                fprintf(stderr, "Try 'ft_ping --help' for more information.\n");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            // El primer argumento que no es una opción es el hostname
            ping->hostname = argv[i];
            break;
        }
    }

    // Verificar si se proporcionó un hostname
    if (ping->hostname == NULL)
    {
        fprintf(stderr, "ft_ping: missing host operand\n");
        fprintf(stderr, "Try 'ft_ping --help' for more information.\n");
        exit(EXIT_FAILURE);
    }
}
