OBJ = mplayer
OBJS += main.c
OBJS += mediaServer.c
OBJS += menu.c
CC = gcc
INCLUDE= -I../include

$(OBJ):$(OBJS)
	$(CC) $^ -o $@ $(INCLUDE)
.PHONY:
clean:
	rm $(OBJ)
