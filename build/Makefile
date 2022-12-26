
CC 	 = g++
CFLAGS 	 = -g
LIBS     = 
SRCS     = ../src/main.cpp ../imgui/imgui.cpp ../imgui/imgui_draw.cpp  \
       	   ../imgui/imgui_widgets.cpp ../imgui/imgui_tables.cpp \
		   ../imgui/imgui_demo.cpp
OBJS     = $(SRCS:.c=.o)
MAIN     = chip8

all:    $(MAIN)

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS)  -o $(MAIN) $(OBJS) $(LIBS)

clean:
	$(RM) *.o $(MAIN)
