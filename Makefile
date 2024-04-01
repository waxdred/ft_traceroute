################################################################################
##                               Présentation                                 ##
################################################################################

COLOR_NORM		=	\033[0m
COLOR_RED		=	\033[31m
COLOR_PURPLE	=	\033[35m

################################################################################
##                               SRCS                                         ##
################################################################################

MK = Makefile

# Headers

DIR_HDS				=	includes

RELATIVE_HDS		=	ft_traceroute.h 

# Code

DIR_GLOBAL						=	srcs
SRC_GLOBAL						=	ft_traceroute.c   \
												mem.c							\
												init.c						\
												tool.c						\
												signal.c						\
												trace.c						\
												parsing.c						\
												main.c

################################################################################
##                       Compilation Environnement                            ##
################################################################################

NAME	=	ft_traceroute
CC		=	cc
CFLAGS	=	-Wall -Werror -Wextra -O3 -g3
# -g3 -ggdb -fsanitize=address

# Library

LIBS = -Llibft -lft

# libft

DIR_LIBFT			= 	libft
LIBFT_NAME			=	$(DIR_LIBFT)/libft.a

# Sources

DIRS_SRC			=	$(DIR_GLOBAL)
						

DIR_OBJ				=	obj

SRCS		=	$(addprefix $(DIR_GLOBAL)/, $(SRC_GLOBAL))					\

HDS			=	$(addprefix $(DIR_HDS)/, $(RELATIVE_HDS))
OBJS		=	$(addprefix $(DIR_OBJ)/, $(SRCS:.c=.o))

DEPENDS		=	$(HDS) $(MK) $(LIBFT_NAME)

################################################################################
##                                 Règles                                     ##
################################################################################

all		:	$(NAME)

define src2obj

$(DIR_OBJ)/$(1)/%.o:	$(1)/%.c $(2)
	@mkdir -p $(DIR_OBJ)/$(1)
	@printf "\r\033[K\t[✅]\t$(COLOR_PURPLE)$$< --> $$@\$(COLOR_NORM)"
	@$(CC) $(CFLAGS) -c -o $$@ $$< $(INC_INC)
endef

$(foreach dir,$(DIRS_SRC),$(eval $(call src2obj,$(dir), $(DEPENDS))))

$(NAME)	: $(DEPENDS) $(OBJS)
	@printf "\n[✅]\tCompilation of $(COLOR_PURPLE)$(NAME)\$(COLOR_NORM)\n"
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJS) $(LIBS)

$(LIBFT_NAME):
	@printf "\n\tCompilation of $(COLOR_PURPLE)$(LIBFT_NAME)\$(COLOR_NORM)\n"
	@make --quiet -C $(DIR_LIBFT)

clean:
	@make --quiet -C $(DIR_LIBFT) clean
	@printf "[✅]\tDelete $(COLOR_RED)object of $(DIR_GLOBAL)$(COLOR_NORM) of $(NAME)\n"
	@rm -rf $(DIR_OBJ)

fclean: clean
	@make --quiet -C $(DIR_LIBFT) fclean
	@printf "[✅]\tDelete $(COLOR_RED)all binary on $(DIR_LIBFT)$(COLOR_NORM)\n"
	@rm -rf $(NAME)

send: fclean 
	@scp -r ../ft_ping vm:/home/jmilhas/

re:	fclean all
