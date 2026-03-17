CXX = g++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98

SRCS = main.cpp server/Server.cpp Server/Commands.cpp Client/Client.cpp Channel/Channel.cpp


OBJS = $(SRCS:.cpp=.o)

# Nom de l'exécutable
NAME = irc_server

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re