game: ../src/main.cpp ../src/stb_vulf.hpp ../src/shaders/shader.vert ../src/shaders/shader.frag
	g++ -std=c++17 -O3 ../src/main.cpp -o ./game -I../src -lvulkan -lglfw -L../src/externCompiled -lextern
	glslc ../src/shaders/shader.vert -o shaders/vert.spr
	glslc ../src/shaders/shader.frag -o shaders/frag.spr

test: game
	./game

clean:
	rm -f game
