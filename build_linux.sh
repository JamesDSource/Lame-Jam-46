emcc -o game.html libraylib.a -Ivendor/raylib/src/ src/main.c -Wall -L -Isrc/ -s USE_GLFW=3 --shell-file vendor/raylib/src/minshell.html -DPLATFORM_WEB
