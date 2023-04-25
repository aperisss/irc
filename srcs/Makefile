NAME = ircserv

CC = c++
RM = rm -rf
FLAGS = -Wextra -Wall -Werror -std=c++98

SRC_FILES = Server.cpp ClientIRC.cpp ChannelIRC.cpp ChannelManager.cpp CommandManager.cpp main.cpp
			
OBJS = $(SRC_FILES:.cpp=.o)
OBJS_F	= .objFiles/

$(NAME): 

					@mkdir -p $(OBJS_F)
					@$(CC) $(FLAGS) -c $(addprefix $(SRC_DIR), $(SRC_FILES))
					@mv $(OBJS) $(OBJS_F)
					@$(CC) $(FLAGS) $(addprefix $(OBJS_F), $(OBJS)) -o $(NAME)
					@echo -ne '\033[32m                            (0%)\r'
					@sleep 0.2
					@echo -ne '#####                      (33%)\r'
					@sleep 0.2
					@echo -ne '#############              (66%)\r'
					@sleep 0.2
					@echo -ne '#######################   (100%)\r'
					@echo -ne '\n'
					@echo "\033[33mC'est compilé !"

all:				$(NAME)

clean:
					@${RM} $(OBJS_F)
					@${RM}  ${OBJS}
					@echo -ne '\033[36m#######################   (100%)\r'
					@sleep 0.2
					@echo -ne '#############              (66%)\r'
					@sleep 0.2
					@echo -ne '#####                      (33%)\r'
					@sleep 0.2
					@echo -ne '                            (0%)\r'
					@echo -ne '\n'
					@echo "\033[33mC'est clean !"

fclean: 	clean
					@${RM} $(NAME)
					@echo -ne '\033[34m#######################   (100%)\r'
					@sleep 0.2
					@echo -ne '#############              (66%)\r'
					@sleep 0.2
					@echo -ne '#####                      (33%)\r'
					@sleep 0.2
					@echo -ne '                            (0%)\r'
					@echo -ne '\n'
					@echo "\033[33mC'est fclean !"

re:			fclean all