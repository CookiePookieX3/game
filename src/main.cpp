#include <iostream>
#include <chrono>
#include <string>

#include "stb_vulf.cpp"
#include "stb_vulf.hpp"

int main(){
	Vulf vulf;

	//uint32_t cubeModelID = vulf.loadModel("models/cube.obj");
	uint32_t labModelID = vulf.loadModel("models/maptest.obj");

	vulf.init();

	//uint32_t object1 = vulf.createObject(labModelID, Transphorm{glm::vec3(0,0,0), glm::quat(0,0,0,0), glm::vec3(0.075,0.075,0.075)});
	uint32_t object1 = vulf.createObject(labModelID, Transphorm{glm::vec3(0,0,0), glm::quat(0,0,0,0), glm::vec3(1,1,1)});

	//uint32_t object2 = vulf.createObject(labModelID, Transphorm{glm::vec3(0,1,0), glm::quat(0,0,0,0),  glm::vec3(0.075,0.075,0.075)});

	std::cout << "Hello, Vulf!\n";

	vulf.cameraPosition = glm::vec3(2, 1, 0);
	vulf.cameraDirection = glm::vec3(-1.57, 0, 0);
	vulf.FOV = 100;

	float time = 0;
	float perviousTime;

	while (vulf.shouldRun()){
		static auto startTime = std::chrono::high_resolution_clock::now();
		auto currentTime = std::chrono::high_resolution_clock::now();
		perviousTime = time;
		time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
		float deltaT = time - perviousTime;

		//vulf.cameraDirection = glm::vec3(time, -0.5, 0);

		vulf.cameraPosition += glm::vec3(0, 0, -deltaT);

		//vulf.setObjectTrasphorm(object1, Transphorm{glm::vec3(0,0,1 + 0.2*sin(time)),
			//glm::quat(0,0,0,0), glm::vec3(0.075, 0.075, 0.075)});

		glfwPollEvents();
		vulf.drawFrame();
	}

	vulf.cleanup();

	return 0;
}
