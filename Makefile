OBJS=main.o renderer.o window.o
CC=clang
CFLAGS+=-g -Wall -Wextra -Werror -std=c99
LFLAGS+=-lglew -lglfw3 -framework opengl

opengl: $(OBJS)
	$(CC) $(OBJS) $(LFLAGS) -o opengl

# pull in dependency info for *existing* .o files
-include $(OBJS:.o=.d)

# compile and generate dependency info
%.o: %.c
	$(CC) -c $(CFLAGS) $*.c -o $*.o
	@$(CC) -MM $(CFLAGS) $*.c > $*.d

clean:
	@rm *.o *.d opengl
