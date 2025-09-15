NAME = ircserv

SRCS = 	main.cpp channel/Channel.cpp client/Client.cpp commands/commandHandle.cpp server/Server.cpp

OBJS = $(SRCS:.cpp=.o)
CXX = c++
CXXFLAGS = -g -Wall -Wextra -Werror -std=c++98

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

clean:
	rm -f $(OBJS) 

fclean: clean
	rm -rf $(NAME) .vscode

re: fclean all

.PHONY: all clean fclean re