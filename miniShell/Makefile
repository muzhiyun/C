OBJ = miniShell

OBJS += src/cat.o
OBJS += src/cd.o
OBJS += src/cp.o
OBJS += src/link.o
OBJS += src/ll.o
OBJS += src/ls.o
OBJS += src/main.o
OBJS += src/mkdir.o
OBJS += src/mv.o
OBJS += src/rm.o
OBJS += src/top.o
OBJS += src/touch.o

CC = gcc
INCLUDE= -I ./include

$(OBJ):$(OBJS)
	$(CC) $^ -o $@
%.o:%.c
	$(CC) -c $< -o $@ $(INCLUDE)
.PHONY:
clean:
	rm $(OBJ)
	rm $(OBJS)
install:
	sudo cp $(OBJ) /bin
	sudo cp ./README /bin
uninstall:
	sudo rm /bin/$(OBJ)
	sudo rm /bin/README
