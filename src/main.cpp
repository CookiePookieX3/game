#include <chrono>

#include "stb_vulf.cpp"


Vulf vulf;

float playerHeight = 1.8;
float movementSpeed = 10;
float turnSpeedX = 2.0;
float trunSpeedY = 1.0;
float floorY = 0;
float g = 10;

glm::vec3 playerPosition = glm::vec3(0, 1, 0);
glm::vec3 playerDirection = glm::vec3(0, 0, 0);
glm::vec3 playerSpeed = glm::vec3(0, 0, 0);


void processPlayerMovement(float deltaT);

int main(){
	uint32_t labModelID = vulf.loadModel("models/maptest.obj");

	vulf.init();

	vulf.FOV = 60;

	vulf.cameraDirection = glm::vec3(0, 0, 0);
	vulf.cameraPosition = glm::vec3(0, 0, 0);

	uint32_t a = vulf.createObject(labModelID, Transphorm{{0, 0, 0}, {0, 0, 0, 0}, {1, 1, 1}});

	float time = 0;
	float previousTime;
	while (vulf.shouldRun()){
		glfwPollEvents();

		static auto startTime = std::chrono::high_resolution_clock::now();
		auto currentTime = std::chrono::high_resolution_clock::now();
		previousTime = time;
		time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
		float deltaT = time - previousTime;

		playerSpeed += glm::vec3(0, -1 * g * deltaT, 0);
		playerPosition += playerSpeed * deltaT;

		if(playerPosition.y <= floorY){
			playerPosition.y = floorY;
			playerSpeed.y = 0;
			if(glfwGetKey(vulf.window, GLFW_KEY_LEFT_SHIFT)){
				playerSpeed.y = 5;
			}
		}

		processPlayerMovement(deltaT);

		vulf.drawFrame();
	}

	vulf.cleanup();

	return 0;
}


void processPlayerMovement(float deltaT){
		float front = 0, right = 0;
		if(glfwGetKey(vulf.window, GLFW_KEY_W)) front += 1;
		if(glfwGetKey(vulf.window, GLFW_KEY_S)) front -= 1;
		if(glfwGetKey(vulf.window, GLFW_KEY_D)) right += 1;
		if(glfwGetKey(vulf.window, GLFW_KEY_A)) right -= 1;
		glm::vec3 movement = glm::vec3(cos(playerDirection.x) * front, 0, sin(playerDirection.x) * front);
		movement += glm::vec3(-sin(playerDirection.x) * right, 0, cos(playerDirection.x) * right);
		if(front != 0 || right != 0) movement = glm::normalize(movement);
		movement *= movementSpeed;
		playerPosition += movement * deltaT;

		float upR = 0, rightR = 0;
		if(glfwGetKey(vulf.window, GLFW_KEY_K)) upR += 1;
		if(glfwGetKey(vulf.window, GLFW_KEY_J)) upR -= 1;
		if(glfwGetKey(vulf.window, GLFW_KEY_L)) rightR += 1;
		if(glfwGetKey(vulf.window, GLFW_KEY_H)) rightR -= 1;
		glm::vec3 rotation = glm::vec3(rightR * turnSpeedX, upR * trunSpeedY, 0);
		rotation *= deltaT;
		playerDirection += rotation;
		if(playerDirection.y >  1.5) playerDirection.y =  1.5;
		if(playerDirection.y < -1.5) playerDirection.y = -1.5;

		vulf.cameraPosition = playerPosition;
		vulf.cameraPosition.y += playerHeight;
		vulf.cameraDirection = playerDirection;
}
