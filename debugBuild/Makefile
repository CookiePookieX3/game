game: ../src/main.cpp
	g++ -std=c++17 -O2 ../src/main.cpp -o game -lglfw -lvulkan -I../src/stb/

test: game
	./game

clean:
	rm -f game
