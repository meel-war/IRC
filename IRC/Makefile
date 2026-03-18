CXX = g++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -g

SRCS = main.cpp server/Server.cpp server/Commands.cpp client/Client.cpp channel/Channel.cpp


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