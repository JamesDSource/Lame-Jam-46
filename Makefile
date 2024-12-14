CC=emcc
CFLAGS=-Wall -s USE_GLFW=3 -DPLATFORM_WEB
OUT=game.html
INCLUDE=-Isrc/ -Ivendor/raylib
HTML=minshell.html
SRC=$(wildcard src/*.c)
LIBS=-Llib -lraylib

all:
	$(CC) -o $(OUT) $(INCLUDE) $(SRC) $(LIBS) --shell-file $(HTML) $(CFLAGS)
