CC = clang
FLAGS = -Wall -pedantic
INC = -I./include src/glad.c
LOC_LIB = -lglfw
FRAMEWORKS = -framework Cocoa -framework OpenGL
SRC = src/main.c

all:
	${CC} ${SRC} ${INC} ${FRAMEWORKS} ${LOC_LIB}
