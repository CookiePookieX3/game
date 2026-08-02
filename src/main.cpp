#include <chrono>
#include <cstdint>
#include <fstream>

#include "stb_vulf.cpp"


Vulf vulf;

float playerHeight = 1.8;
float movementSpeed = 10;
float turnSpeedX = 2.0;
float trunSpeedY = 1.0;
float g = 10;
float playerRadius = 0.2f;
float pi = 3.141;

//direction > 0 => turning left
glm::vec3 playerPosition = glm::vec3(0, 1, 0);
glm::vec3 playerDirection = glm::vec3(0, 0, 0);
glm::vec3 playerVelocity = glm::vec3(0, 0, 0);


//moving from A to B right side os solid
struct CollisionShape{
	std::vector<glm::vec2> vertices;
};

std::vector<CollisionShape> walls;
std::vector<bool> wallsDisabled;
std::vector<uint32_t> wallsDeleted;
uint32_t parseShapeFromAFile(std::string fileName, glm::vec2 offset);
void deleteShape(uint32_t shapeID);


struct TriangleFloor{
	glm::vec3 A;
	glm::vec3 B;
	glm::vec3 C;

	glm::vec3 normal;
	float d;

	void preCalculate();
};


//at point A, on certain height generates platform by putting vector pointing to direction and countourclockwise to it perpendicular vector with lengths of demensions x and y corespondidly
struct RectangleFloor{
	float height;
	float direction;
	glm::vec2 dementions;
	glm::vec2 A;

};


//code assumes that boundary.y > .x
struct AxisFloor{
	float height;
	glm::vec2 boundaryX;
	glm::vec2 boundaryY;
};

std::vector<TriangleFloor> triangleFloors;
std::vector<RectangleFloor> rectangleFloors;
std::vector<AxisFloor> axisFloors;
std::vector<bool> triangleFloorsDisabled;
std::vector<bool> rectangleFloorsDisabled;
std::vector<bool> axisFloorsDisabled;
bool trinagleFloorArea(int floorIndex);
float triangleFloorY(int floorIndex);
bool rectangleFloorArea(int floorIndex);
bool axisFloorArea(int floorIndex);


void processPlayerMovement(float deltaT);
bool intersectionPoint2D(glm::vec2 A1, glm::vec2 B1, glm::vec2 A2, glm::vec2 B2, glm::vec2& collision);


int main(){
	uint32_t labModelID = vulf.loadModel("models/maptest.obj");

	vulf.init();

	vulf.FOV = 60;

	uint32_t a = vulf.createObject(labModelID, Transphorm{{0, 0, 0}, {0, 0, 0, 0}, {1, 1, 1}});
	axisFloors.push_back(AxisFloor{0.0f, glm::vec2(-20.0f, 20.0f), glm::vec2(-20.0f, 20.0f)});
	axisFloorsDisabled.push_back(false);
	uint32_t labID = parseShapeFromAFile("collisions/lab.txt", glm::vec2(0.0f, 0.0f));
	wallsDisabled[labID] = false;

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

	float upR = 0, rightR = 0;
	if(glfwGetKey(vulf.window, GLFW_KEY_K)) upR += 1;
	if(glfwGetKey(vulf.window, GLFW_KEY_J)) upR -= 1;
	if(glfwGetKey(vulf.window, GLFW_KEY_L)) rightR += 1;
	if(glfwGetKey(vulf.window, GLFW_KEY_H)) rightR -= 1;
	glm::vec3 rotation = glm::vec3(-rightR * turnSpeedX, upR * trunSpeedY, 0);
	playerDirection += rotation * deltaT;
	if(playerDirection.y >  1.5) playerDirection.y =  1.5;
	if(playerDirection.y < -1.5) playerDirection.y = -1.5;



	playerVelocity += glm::vec3(0, -1 * g, 0) * deltaT;
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
	for(int s = 0; s < walls.size(); s++){
		if(wallsDisabled[s]) continue;
		CollisionShape shape = walls[s];
		for(int i = 0; i < shape.vertices.size(); i++){
			int j;
			if(i == 0) j = shape.vertices.size() - 1;
			else       j = i - 1;

			if(glm::length(shape.vertices[i] - playerPosition2D - paddingVector) < playerRadius){
				glm::vec2 dirrection = glm::normalize(playerPosition2D + paddingVector - shape.vertices[i]);
				paddingVector = shape.vertices[i] + dirrection * playerRadius * 1.1f - playerPosition2D;
			}

			glm::vec2 wallDirection = glm::normalize(shape.vertices[j] - shape.vertices[i]);
			glm::vec2 wallNormal = {-wallDirection.y, wallDirection.x};
			if(glm::dot(glm::normalize(paddingVector), wallNormal) >= 0.0f) continue;

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

	playerPosition.x += paddingVector.x;
	playerPosition.z += paddingVector.y;

	float floorY = -10;
	for(int i = 0; i < triangleFloors.size(); i++){
		if(triangleFloorsDisabled[i]) continue;
		if(trinagleFloorArea(i)){
			floorY = std::max(floorY, triangleFloorY(i));
		}
	}

	for(int i = 0; i < rectangleFloors.size(); i++){
		if(rectangleFloorsDisabled[i]) continue;
		if(rectangleFloorArea(i)){
			floorY = std::max(floorY, rectangleFloors[i].height);
		}
		continue;
	}

	for(int i = 0; i < axisFloors.size(); i++){
		if(axisFloorsDisabled[i]) continue;
		if(axisFloorArea(i)){
			floorY = std::max(floorY, axisFloors[i].height);
		}
		continue;
	}

	if(playerPosition.y <= floorY){
		playerPosition.y = floorY;
		playerVelocity.y = 0;
		if(glfwGetKey(vulf.window, GLFW_KEY_LEFT_SHIFT)){
			playerVelocity.y = 5;
		}
	}

	playerPosition += playerVelocity * deltaT;
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


void TriangleFloor::preCalculate(){
	glm::vec3 vec1 = B - A;
	glm::vec3 vec2 = C - A;

	normal = glm::cross(vec1, vec2);
	d = glm::dot(normal, A);
}

float sign3D(glm::vec3& A, glm::vec3& B, glm::vec3& C){
	return (A.x - C.x) * (B.z - C.z) -
	       (B.x - C.x) * (A.z - C.z);
}

float sign2D(glm::vec3& A, glm::vec2& B, glm::vec2& C){
	return (A.x - C.x) * (B.y - C.y) -
	       (B.x - C.x) * (A.z - C.y);
}

bool trinagleFloorArea(int floorIndex){
	TriangleFloor floor = triangleFloors[floorIndex];
	float d1 = sign3D(playerPosition, floor.A, floor.B);
	float d2 = sign3D(playerPosition, floor.B, floor.C);
	float d3 = sign3D(playerPosition, floor.C, floor.A);

	bool hasNeg = (d1 < 0) || (d2 < 0) || (d3 < 0);
	bool hasPos = (d1 > 0) || (d2 > 0) || (d3 > 0);

	return !(hasNeg && hasPos);
}

float triangleFloorY(int floorIndex){
	TriangleFloor triangle = triangleFloors[floorIndex];

	return (triangle.d - triangle.normal.x * playerPosition.x -
		triangle.normal.z * playerPosition.z) / triangle.normal.y;
}


bool rectangleFloorArea(int floorIndex){
	RectangleFloor floor = rectangleFloors[floorIndex];
	glm::vec2 vec1 = glm::vec2(sin(floor.direction), cos(floor.direction)) * floor.dementions.x;
	glm::vec2 vec2 = glm::vec2(cos(floor.direction), -sin(floor.direction)) * floor.dementions.y;

	glm::vec2 B = floor.A + vec1;
	glm::vec2 C = B + vec2;
	glm::vec2 D = floor.A + vec2;

	float d1 = sign2D(playerPosition, floor.A, B);
	float d2 = sign2D(playerPosition, B, C);
	float d3 = sign2D(playerPosition, C, D);
	float d4 = sign2D(playerPosition, D, floor.A);

	bool hasNeg = (d1 < 0) || (d2 < 0) || (d3 < 0) || (d4 < 0);
	bool hasPos = (d1 > 0) || (d2 > 0) || (d3 > 0) || (d4 > 0);

	return !(hasNeg && hasPos);
}


bool axisFloorArea(int floorIndex){
	AxisFloor floor = axisFloors[floorIndex];
	if((playerPosition.x < floor.boundaryX.y) &&
	   (playerPosition.x > floor.boundaryX.x) &&
	   (playerPosition.z < floor.boundaryY.y) &&
	   (playerPosition.z > floor.boundaryY.x)) return true;

	return false;
}


uint32_t parseShapeFromAFile(std::string fileName, glm::vec2 offset){
	std::ifstream file(fileName);

	if(!file.is_open()){
		std::cout << "Failed to open " << fileName << '\n';
		return UINT32_MAX;
	}

	uint32_t length;
	file >> length;
	std::vector<glm::vec2> data;
	for(int i = 0; i < length; i++){
		float x, y;

		if(!(file >> x >> y)){
			std::cout << "Error reding " << fileName << '\n';
		}

		data.push_back(glm::vec2(x,y) + offset);
	}

	CollisionShape shape{data};
	uint32_t ID;
	if(wallsDeleted.size() > 0){
		ID = wallsDeleted.back();
		wallsDeleted.pop_back();
		walls[ID] = shape;
		wallsDisabled[ID] = true;
	} else {
		ID = walls.size();
		walls.push_back(shape);
		wallsDisabled.push_back(true);
	}

	return ID;
}

void deleteShape(uint32_t shapeID){

}
