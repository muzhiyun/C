OBJ = mplayer
OBJS += main.c
OBJS += mediaServer.c
OBJS += menu.c
OBJS += doub_list_for_fileList.c
CC = gcc
INCLUDE= -I../include

$(OBJ):$(OBJS)
	$(CC) $^ -o $@ $(INCLUDE)
.PHONY:
clean:
	rm $(OBJ)
