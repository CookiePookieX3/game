#include <iostream>

#define STB_VULF_IMPLEMENTATION
#include <stb_vulf.hpp>


int main(){
	Vulf vulf;

	vulf.init();
	vulf.mainLoop();
	vulf.cleanup();

	std::cout << "Hello, vulf!\n";

	return 0;
}
