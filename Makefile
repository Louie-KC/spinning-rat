CC = clang
FLAGS = -Wall -pedantic
INC = -I./include -I./include/stb include/glad/glad.c src/matrix.c src/vector.c src/camera.c src/util.c src/scene-object.c src/shader.c
LOC_LIB = -lglfw -lassimp
FRAMEWORKS = -framework Cocoa -framework OpenGL
SRC = src/main.c

all:
	${CC} ${SRC} ${INC} ${FRAMEWORKS} ${LOC_LIB} -g

clean:
	rm -r ./a.out
	rm -r *.dSYM