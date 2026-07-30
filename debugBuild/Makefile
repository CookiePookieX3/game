game: ../src/main.cpp ../src/stb_vulf.hpp ../src/shaders/shader.vert ../src/shaders/shader.frag
	g++ -c -std=c++17 -O2 ../src/main.cpp -o main.o
	g++ main.o -lglfw -lvulkan -L../src/static_lib -lTP -o game
	glslc ../src/shaders/shader.vert -o shaders/vert.spr
	glslc ../src/shaders/shader.frag -o shaders/frag.spr
	rm main.o

test: game
	./game

clean:
	rm -f game

dwragets:
	g++ -std=c++17 -O2 ../src/main.cpp -o game -lglfw -lvulkan -I../src/stb
