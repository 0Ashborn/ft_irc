CXX = c++
RM = rm -f
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -I includes/

SRCS = src/Channel.cpp src/CommandHandler.cpp src/main.cpp src/Server.cpp src/User.cpp 
# OBJS = $(SRCS:.cpp=.o)
# DEPS = $(OBJS:.o=.d)
OBJ_DIR = obj

OBJS = $(patsubst src/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
DEPS = $(OBJS:.o=.d)

NAME = ircserv

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
 
# %.o: %.cpp
# 	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@
$(OBJ_DIR)/%.o: src/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

clean:
	$(RM) $(OBJS) $(DEPS)
 
fclean: clean
	$(RM) $(NAME)
 
re: fclean all

-include $(DEPS)

.PHONY: all clean fclean re