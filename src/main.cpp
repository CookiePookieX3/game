#include <chrono>
#include <cstdint>

#include "stb_vulf.cpp"
#include "physics.cpp"


Vulf vulf;

int main(){
	uint32_t labModelID = vulf.loadModel("models/maptest.obj");

	vulf.setConfig("engineConfig.toml");
	vulf.init();

	vulf.FOV = 60;

	
	uint32_t r = vulf.createBillboardY("Material.003", {3.0, 4.0}, {-3.0, 0.0, -3.0});
	uint32_t u = vulf.createBillboardY("Material.003", {3.0, 4.0}, {-3.0, 4.0, -3.0});
	uint32_t y = vulf.createBillboardY("Material.005", {2.5, 4.0}, {2.0, 0.0, 2.0});
	uint32_t t = vulf.createBillboardY("Material.004", {3.0, 6.0}, {2.0, 0.0, -2.0});
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
		
		//std::cout << 1 / deltaT << '\n';

		processPlayerMovement(deltaT, vulf);

		vulf.drawFrame();
	}

	vulf.cleanup();

	return 0;
}
