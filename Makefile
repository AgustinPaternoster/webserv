NAME = webserv
CC = c++
CFLAGS = -Wall -Werror -Wextra -std=c++98 -MMD -MP -g
SRC_PATH = ./src/
SRC_FILES = main.cpp socket.cpp Config.cpp socket_utils.cpp client_requests.cpp HTTPHeaders.cpp HTTPRequest.cpp HTTPRequestParser.cpp\
				HTTPResponse.cpp HTTPStatusCode.cpp ResponseBuilder.cpp
SRCS = $(addprefix $(SRC_PATH), $(SRC_FILES))

OBJ_PATH = ./objects/
OBJS = $(SRCS:$(SRC_PATH)%.cpp=$(OBJ_PATH)%.o)
DEPS = $(OBJS:.o=.d)

INC_PATH = ./include
INC = -I $(INC_PATH)

all : $(OBJ_PATH) $(NAME)

$(OBJ_PATH):
	mkdir -p $(OBJ_PATH)

$(OBJ_PATH)%.o : $(SRC_PATH)%.cpp
		$(CC) $(CFLAGS) -c $< -o $@ $(INC)

$(NAME) : $(OBJS)
		$(CC) $(CFLAGS)  $(OBJS) -o $@ $(INC)

clean :
	rm -fr $(OBJ_PATH)
	rm webserv

fclean : clean


re: fclean all
-include $(DEPS)
