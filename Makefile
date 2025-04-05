# Nombre del ejecutable
NAME = ft_ping

# Compilador y flags
CC = gcc
CFLAGS = -Wall -Wextra -Werror

# Directorios
SRCDIR = src
INCDIR = includes
OBJDIR = obj

# Archivos fuente
SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))

# Headers
INCS = -I$(INCDIR)

# Librerías matemáticas
LIBS = -lm

# Reglas
all: $(NAME)

$(NAME): $(OBJDIR) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME) $(LIBS)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re