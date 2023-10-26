CC = clang
FLAGS = -Wall -pedantic
INC = -I/opt/homebrew/Cellar/glfw/3.3.8/lib
LOC_LIB = -lglfw
FRAMEWORKS = -framework Cocoa -framework OpenGL
SRC = src/main.c

all:
	${CC} ${FLAGS} ${LOC_LIB} ${FRAMEWORKS} ${SRC}
