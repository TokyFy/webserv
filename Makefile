NAME = server

FLAGS = -Wall -Wextra -Werror -std=c++98

SRC = server.cpp

OBJ = $(SRC:.cpp=.o)

COMPILER = c++

HEADER = -I.

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

.PHONY: clean fclean re
