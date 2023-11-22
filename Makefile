CC = clang
FLAGS = -Wall -pedantic
INC = -I./include include/glad/glad.c src/matrix.c src/camera.c src/util.c
LOC_LIB = -lglfw -lassimp
FRAMEWORKS = -framework Cocoa -framework OpenGL
SRC = src/main.c

all:
	${CC} ${SRC} ${INC} ${FRAMEWORKS} ${LOC_LIB}

clean:
	rm -r ./a.out
	rm -r *.dSYM