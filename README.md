# ft_ping

## Descripción
`ft_ping` es una reimplementación del comando `ping` de Unix, desarrollado como parte del curriculum de 42 School. Este proyecto recrea la funcionalidad básica del comando `ping` utilizando sockets raw y el protocolo ICMP (Internet Control Message Protocol) para evaluar la conectividad de red con otros hosts.

## Funcionalidades

### Características Principales
- Envío de paquetes ICMP Echo Request a hosts de destino
- Medición del tiempo de ida y vuelta (RTT - Round Trip Time)
- Soporte para nombres de host y direcciones IPv4
- Estadísticas de pérdida de paquetes, tiempos mínimo/máximo/promedio
- Modo verboso para información detallada de depuración
- Gestión correcta de señales para mostrar estadísticas al interrumpir

### Opciones Implementadas
- `-v, --verbose`: Activa el modo verboso con información adicional de depuración
- `-?, --help`: Muestra la ayuda y las opciones disponibles

## Requisitos Técnicos
- Sistema operativo: Linux (Debian >= 7.0, kernel > 3.14)
- Privilegios de root (necesarios para los sockets raw)
- Compilador GCC

## Compilación e Instalación
Clona el repositorio y compila el proyecto:

```bash
git clone <url-del-repositorio> ft_ping
cd ft_ping
make