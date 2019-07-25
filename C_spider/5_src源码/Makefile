CXX = gcc
CXXFLAGS = -Wall -Werror -Wextra -pedantic -std=c++17 -g -fsanitize=address
LDFLAGS =  -lm


OBJ = main.c cJSON.c
EXEC = spider

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LBLIBS) $(LDFLAGS)

clean:
	rm -rf $(EXEC)