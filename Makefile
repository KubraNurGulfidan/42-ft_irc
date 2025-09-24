NAME = ircserv

SRCS = 	main.cpp channel/Channel.cpp client/Client.cpp server/Server.cpp \
		commands/commandHandle.cpp commands/Invite.cpp commands/Join.cpp \
		commands/Kick.cpp commands/List.cpp commands/Mode.cpp commands/Nick.cpp \
		commands/Part.cpp commands/Pass.cpp commands/Privmsg.cpp \
		commands/Quit.cpp commands/Topic.cpp commands/User.cpp commands/Who.cpp


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