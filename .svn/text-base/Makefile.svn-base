CC = gcc
FLAGS = -Wall -g
LIBS = -lpthread -lncurses
TARGET = cdcpp
OBJ = main.o hub.o lock2key.o search.o download.o ui.o

all: $(TARGET)
$(TARGET): $(OBJ)
	$(CC) $(LIBS) $(OBJ) -o $@
%.o : %.c
	$(CC) $(FLAGS) -c $<
clean:
	rm -f $(OBJ) $(TARGET)
debug:
	gdb $(TARGET)
valgrind:
	valgrind --tool=memcheck --leak-check=full --show-reachable=yes ./$(TARGET)
install:
	cp $(TARGET) /usr/bin/
