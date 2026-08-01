#include <chrono>

#include "extern/glm/glm/ext/quaternion_geometric.hpp"
#include "extern/glm/glm/geometric.hpp"
#include "stb_vulf.cpp"


Vulf vulf;

float playerHeight = 1.8;
float movementSpeed = 10;
float turnSpeedX = 2.0;
float trunSpeedY = 1.0;
float floorY = 0;
float g = 10;

float playerRadius = 0.11f;

glm::vec3 playerPosition = glm::vec3(0, 1, 0);
glm::vec3 playerDirection = glm::vec3(0, 0, 0);
glm::vec3 playerVelocity = glm::vec3(0, 0, 0);


//moving from A to B right side os solid
struct CollisionShape{
	std::vector<glm::vec2> vertices;
};

std::vector<CollisionShape> walls;

struct CollisonFloor{
	//TODO
};

std::vector<CollisonFloor> floors;


void processPlayerMovement(float deltaT);
bool intersectionPoint2D(glm::vec2 A1, glm::vec2 B1, glm::vec2 A2, glm::vec2 B2, glm::vec2& collision);


int main(){
	uint32_t labModelID = vulf.loadModel("models/maptest.obj");

	//box
	/*
	std::vector<glm::vec2> shape = {glm::vec2(-2,-2),
					glm::vec2(-2, 2),
					glm::vec2( 2, 2),
					glm::vec2( 2, -2)
					};
	*/

	//inside out box
	/*
	std::vector<glm::vec2> shape = {glm::vec2(-2,-2),
					glm::vec2( 2,-2),
					glm::vec2( 2, 2),
					glm::vec2(-2, 2)
					};
	*/

	//trinagle
	std::vector<glm::vec2> shape = {glm::vec2( 0,2),
					glm::vec2(-2,-2),
					glm::vec2( 2,-2),
					};
	

	walls.push_back(CollisionShape{shape});

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

		processPlayerMovement(deltaT);

		vulf.drawFrame();
	}

	vulf.cleanup();

	return 0;
}


void processPlayerMovement(float deltaT){
	playerVelocity += glm::vec3(0, -1 * g, 0) * deltaT;

	if(playerPosition.y <= floorY){
		playerPosition.y = floorY;
		playerVelocity.y = 0;
		if(glfwGetKey(vulf.window, GLFW_KEY_LEFT_SHIFT)){
			playerVelocity.y = 5;
		}
	}

	playerVelocity.x = 0;
	playerVelocity.z = 0;
	float front = 0, right = 0;
	if(glfwGetKey(vulf.window, GLFW_KEY_W)) front += 1;
	if(glfwGetKey(vulf.window, GLFW_KEY_S)) front -= 1;
	if(glfwGetKey(vulf.window, GLFW_KEY_D)) right += 1;
	if(glfwGetKey(vulf.window, GLFW_KEY_A)) right -= 1;
	glm::vec2 horizontalVelocity = glm::vec2(sin(playerDirection.x) * front, cos(playerDirection.x) * front);
	horizontalVelocity += glm::vec2(-cos(playerDirection.x) * right, sin(playerDirection.x) * right);
	if(front != 0 || right != 0) horizontalVelocity = glm::normalize(horizontalVelocity);

	glm::vec2 paddingVector = horizontalVelocity * deltaT * movementSpeed;
	glm::vec2 playerPosition2D = {playerPosition.x, playerPosition.z};
	for(auto shape : walls){
		for(int i = 0; i < shape.vertices.size(); i++){
			int j;
			if(i == 0){
				j = shape.vertices.size() - 1;
			} else {
				j = i - 1;
			}

			if(glm::length(shape.vertices[i] - playerPosition2D - paddingVector) < playerRadius){
				glm::vec2 dirrection = glm::normalize(playerPosition2D + paddingVector - shape.vertices[i]);
				paddingVector = shape.vertices[i] + dirrection * playerRadius - playerPosition2D;
				continue;
			}

			glm::vec2 wallDirection = glm::normalize(shape.vertices[j] - shape.vertices[i]);
			glm::vec2 wallNormal = {-wallDirection.y, wallDirection.x};
			if(glm::dot(glm::normalize(paddingVector), wallNormal) > 0.05f) continue;

			glm::vec2 collision;
			if(!intersectionPoint2D(playerPosition2D,
			   playerPosition2D + paddingVector,
			   shape.vertices[i] + wallNormal * playerRadius,
			   shape.vertices[j] + wallNormal * playerRadius,
			   collision)) continue;

			glm::vec2 toCollision = collision - playerPosition2D;
			glm::vec2 diference = paddingVector - toCollision;
			paddingVector = wallDirection * glm::dot(wallDirection, diference) + toCollision;
		}
	}

	float upR = 0, rightR = 0;
	if(glfwGetKey(vulf.window, GLFW_KEY_K)) upR += 1;
	if(glfwGetKey(vulf.window, GLFW_KEY_J)) upR -= 1;
	if(glfwGetKey(vulf.window, GLFW_KEY_L)) rightR += 1;
	if(glfwGetKey(vulf.window, GLFW_KEY_H)) rightR -= 1;
	glm::vec3 rotation = glm::vec3(-rightR * turnSpeedX, upR * trunSpeedY, 0);
	playerDirection += rotation * deltaT;
	if(playerDirection.y >  1.5) playerDirection.y =  1.5;
	if(playerDirection.y < -1.5) playerDirection.y = -1.5;

	playerPosition += playerVelocity * deltaT;
	playerPosition.x += paddingVector.x;
	playerPosition.z += paddingVector.y;
	vulf.cameraPosition = playerPosition;
	vulf.cameraPosition.y += playerHeight;
	vulf.cameraDirection = playerDirection;
}

bool intersectionPoint2D(glm::vec2 A1, glm::vec2 B1, glm::vec2 A2, glm::vec2 B2, glm::vec2& collision){

	float denominator = (A1.x - B1.x) * (A2.y - B2.y) - (A1.y - B1.y) * (A2.x - B2.x);
	if(std::abs(denominator) < 0.001f) return false;

	float k1 = A1.x * B1.y - A1.y * B1.x;
	float k2 = A2.x * B2.y - A2.y * B2.x;

	collision.x = (k1 * (A2.x - B2.x) - k2 * (A1.x - B1.x)) / denominator;
	collision.y = (k1 * (A2.y - B2.y) - k2 * (A1.y - B1.y)) / denominator;
	
	float delta = 0.001;
	if( ((collision.x + delta >= A1.x && collision.x - delta <= B1.x)  ||
	     (collision.x - delta <= A1.x && collision.x + delta >= B1.x)) &&
	    ((collision.y - delta <= A1.y && collision.y + delta >= B1.y)  ||
	     (collision.y + delta >= A1.y && collision.y - delta <= B1.y)) &&
	    ((collision.x + delta >= A2.x && collision.x - delta <= B2.x)  ||
	     (collision.x - delta <= A2.x && collision.x + delta >= B2.x)) &&
	    ((collision.y + delta >= A2.y && collision.y - delta <= B2.y)  ||
	     (collision.y - delta <= A2.y && collision.y + delta >= B2.y))) return true;

	return false;
}

