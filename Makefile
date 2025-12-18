NAME = webserv

FLAGS = -Wall -Wextra -Werror -std=c++98 -g
SRC = webserv.cpp HttpServer.cpp HttpClient.cpp HttpAgent.cpp Pool.cpp utils.cpp config.cpp 

OBJ = $(SRC:.cpp=.o)
COMPILER = c++
HEADER = -I. -Ihandler

all : $(NAME)

$(NAME) : $(OBJ)
	$(COMPILER) $(FLAGS) $(HEADER) $(OBJ) -o $(NAME)

%.o : %.cpp
	$(COMPILER) $(FLAGS) $(HEADER) -c $< -o $@

clean:
	$(RM) -f $(OBJ)

fclean: clean
	$(RM) -f $(NAME)

re: fclean $(NAME)

c: 
	$(COMPILER) $(FLAGS) HttpServer.cpp HttpAgent.cpp config.cpp -o webserv

.PHONY: clean fclean re
