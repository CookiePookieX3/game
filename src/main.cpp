#include <iostream>
#include <chrono>

#include "stb_vulf.cpp"
#include "stb_vulf.hpp"

int main(){
	Vulf vulf;

	//uint32_t cubeModelID = vulf.loadModel("models/cube.obj");
	uint32_t labModelID = vulf.loadModel("models/maptest.obj");

	vulf.init();

	uint32_t object1 = vulf.createObject(labModelID, Transphorm{glm::vec3(0,0,1), glm::quat(0,0,0,0), glm::vec3(0.075,0.075,0.075)});

	uint32_t object2 = vulf.createObject(labModelID, Transphorm{glm::vec3(0,1,0), glm::quat(0,0,0,0),  glm::vec3(0.075,0.075,0.075)});

	std::cout << "Hello, Vulf!\n";

	vulf.cameraPos = glm::vec3(2, 0, 0);
	vulf.cameraDir = glm::vec3(1, 0, -0.5);

	while (vulf.shouldRun()){
		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		vulf.cameraDir = glm::vec3(cos(time/3.0f), sin(time/3.0f), -0.5);
		vulf.cameraPos = glm::vec3(-2*cos(time/3.0f), -2*sin(time/3.0f), 1);

		vulf.editObject(object1, Transphorm{glm::vec3(0,0,1 + 0.2*sin(time)), glm::quat(0,0,0,0), glm::vec3(0.075, 0.075, 0.075)});

		glfwPollEvents();
		vulf.drawFrame();
	}

	vulf.cleanup();

	return 0;
}
