#include <chrono>
#include <cstdint>

#include "stb_vulf.cpp"
#include "physics.cpp"


Vulf vulf;

int main(){
	uint32_t labModelID = vulf.loadModel("models/maptest.obj");

	vulf.init();

	vulf.FOV = 60;

	
	uint32_t b = vulf.createBillboardY("Material.002", {1.0, 1.0}, {-1.0, 1.0, -1.0});
	uint32_t a = vulf.createObject(labModelID, Transphorm{{0, 0, 0}, {0, 0, 0, 0}, {1, 1, 1}});

	uint32_t labID = parseShapeFromAFile("collisions/walls1.txt", {0.0f, -4.0f}, pi/4, {1, 1});
	wallsDisabled[labID] = false;

	FloorCollider floor1 = parseFloorFromAFile("collisions/floor1.txt", {0, 0, 0});
	floor1.enableFloors();

	float time = 0;
	float previousTime;
	while (vulf.shouldRun()){
		glfwPollEvents();

		static auto startTime = std::chrono::high_resolution_clock::now();
		auto currentTime = std::chrono::high_resolution_clock::now();
		previousTime = time;
		time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
		float deltaT = time - previousTime;

		processPlayerMovement(deltaT, vulf);

		vulf.drawFrame();
	}

	vulf.cleanup();

	return 0;
}
