NAME = ircserv

SRCS = main.cpp \
       server/Server.cpp \
       client/Client.cpp \
       channel/Channel.cpp \
       utils/utils.cpp

OBJS = $(SRCS:.cpp=.o)
CXX_STANDARD = c++98
CXX = c++
CXXFLAGS = -g -Wall -Wextra -Werror -std=$(CXX_STANDARD)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) 

fclean: clean
	rm -rf $(NAME) .vscode

re: fclean all

.PHONY: all clean fclean re