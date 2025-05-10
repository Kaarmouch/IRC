NAME = ircserv

CC		= c++
FLAGS	= -Wall -Wextra -Werror -std=c++98
RM		= rm -rf

OBJDIR	= .objFiles

FILES	= main Server Client Channel utils

SRC		= $(FILES:=.cpp)
OBJ		= $(addprefix $(OBJDIR)/, $(FILES:=.o))
HEADER	= Channel.hpp Client.hpp Server.hpp utils.hpp

GREEN	= \e[92m
YELLOW	= \e[93m
GRAY	= \e[33md
RESET	= \e[0m
CURSIVE	= \e[3m

.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(OBJ) $(HEADER)
	@$(CC) $(OBJ) -o $(NAME)
	@printf "______________________________________\n\n$(RESET)"
	@printf "$(GREEN)-> IRC SERVER\n$(RESET)"
	@printf "$(GREEN)-> Projet : ft_irc\n$(RESET)"
	@printf "______________________________________\n\n$(RESET)"
	@printf "$(YELLOW)-> Compilation de ircserv...$(RESET)\n"
	@printf "$(GREEN)-> Exécutable prêt !$(RESET)\n\n"
	@printf "$(GREEN)./$(NAME) <port> <password>\n\n$(RESET)"
	@printf "______________________________________\n\n$(RESET)"	

$(OBJDIR)/%.o: %.cpp $(HEADER)
	@mkdir -p $(dir $@)
	@$(CC) $(FLAGS) -c $< -o $@

clean:
	@$(RM) $(OBJDIR)
	@printf "$(YELLOW)-> Fichiers objets supprimés.$(RESET)\n"

fclean: clean
	@$(RM) $(NAME)
	@printf "$(YELLOW)-> Exécutable supprimé.$(RESET)\n"

re: fclean all
