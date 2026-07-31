// NOLINTBEGIN(misc-definitions-in-headers)
#pragma once

#include <vulkan/vulkan_core.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

//#define STB_IMAGE_IMPLEMENTATION
#include <extern/stb_image.h>

//#define TINYOBJLOADER_IMPLEMENTATION
#include <extern/tiny_obj_loader.h>

#define FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#include <extern/glm/glm/ext/matrix_transform.hpp>
#include <extern/glm/glm/trigonometric.hpp>
#include <extern/glm/glm/ext/matrix_clip_space.hpp>
#include <extern/glm/glm/gtc/quaternion.hpp>

#include <stdexcept>
#include <vector>
#include <cstring>
#include <optional>
#include <limits>
#include <algorithm>
#include <array>
#include <fstream>
#include <cstdint>
#include <set>
#include <iostream>


static const int MAX_FRAMES_IN_FLIGHT = 2;
static const size_t MAX_OBJECTS = 1000;


static VkVertexInputBindingDescription getBindingDescription();
static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();


struct QueueFamilyIndicies {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete();
};

struct SwapchainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

struct FrameUBO{
	glm::mat4 view;
	glm::mat4 proj;
};

struct ObjectUBO{
	glm::mat4 model;
};

struct PushConstans{
	uint32_t objectID;
};

struct Vertex{
	glm::vec3 pos;
	glm::vec2 texCoord;
};

struct Mesh{
	uint32_t firstVertex;
	uint32_t firstIndex;
	uint32_t indiciesCount;
};

struct Material{
	std::string materialName;
	uint32_t textureID;
};

struct Texture{
	std::string texturePath;
	VkImage image;
	VkDeviceMemory memory;
	VkImageView imageView;
	VkDescriptorSet descriptorSet;
};

struct SubMesh{
	Mesh mesh;
	uint32_t materialID;
};

struct Model{
	std::vector<SubMesh> subMeshes;
};

struct Transphorm{
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;

	glm::mat4 modelMatrix() const;
};

struct RenderObject{
	uint32_t modelID;
	Transphorm transphorm;
};

static const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifndef NDEBUG
#include <iostream>
#include <map>


static const std::vector<const char*> vulkanValidationLayers = {
	"VK_LAYER_KHRONOS_validation"
};


static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
						    VkDebugUtilsMessageTypeFlagsEXT messageType, 
						    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
						    void* pUserData);
#endif

#ifdef STB_VULF_IMPLEMENTATION
static VkVertexInputBindingDescription getBindingDescription(){
	VkVertexInputBindingDescription bindingDescription;
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(Vertex);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return bindingDescription;
}

static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions(){
	std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(Vertex, pos);

	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(Vertex, texCoord);

	return attributeDescriptions;
}

bool QueueFamilyIndicies::isComplete(){
	return graphicsFamily.has_value() && presentFamily.has_value();
}

glm::mat4 Transphorm::modelMatrix() const {
	return glm::translate(glm::mat4(1.0f), position) *
	       glm::mat4_cast(rotation) *
	       glm::scale(glm::mat4(1.0f), scale);
}
#ifndef NDEBUG
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
						    VkDebugUtilsMessageTypeFlagsEXT messageType, 
						    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
						    void* pUserData) {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}
#endif
#endif


class Vulf{
public:
	glm::vec3 cameraPosition;
	glm::vec3 cameraDirection;
	float     FOV;
	GLFWwindow*      window;


	uint32_t loadModel(std::string fileName);
	void init();
	bool shouldRun();
	void drawFrame();
	void cleanup();

	void acceptScreenName(std::string name);
	uint32_t createObject(uint32_t modelID, Transphorm objectTrasnphorm);
	void setObjectTrasphorm(uint32_t objectID, Transphorm objectTransphorm);
	void editObgjectTransphorm(uint32_t objectID, Transphorm objectTransphorm);
	void deleteObject(uint32_t objectID);

private:
	VkInstance       instance;
	VkSurfaceKHR     surface;
	VkQueue          graphicsQueue;
	VkQueue          presentQueue;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice         device/* = VK_NULL_HANDLE*/; //TODO figure out how does this work

	VkSwapchainKHR           swapchain;
	std::vector<VkImage>     swapchainImages;
	std::vector<VkImageView> swapchainImageViews;
	VkFormat                 swapchainImageFormat;
	VkExtent2D               swapchainExtent;

	VkRenderPass          renderPass;
	VkDescriptorSetLayout frameDescriptorSetLayout;
	VkDescriptorSetLayout textureDescriptorSetLayout;
	VkPipeline            graphicsPipeline;
	VkPipelineLayout      graphicsPipelineLayout;
	VkCommandPool         commandPool;

	VkImage                    depthImage;
	VkImageView                depthImageView;
	VkDeviceMemory             depthImageMemory;
	std::vector<VkFramebuffer> swapchainFramebuffers;
	VkSampler                  textureSampler;

	VkBuffer                    vertexBuffer;
	VkDeviceMemory              vertexBufferMemory;
	VkBuffer                    indexBuffer;
	VkDeviceMemory              indexBufferMemory;
	std::vector<VkBuffer>       frameUBs;
	std::vector<VkDeviceMemory> frameUBsMemory;
	std::vector<void*>          frameUBsMemoryMapped;
	std::vector<VkBuffer>       objectUBs;
	std::vector<VkDeviceMemory> objectUBsMemory;
	std::vector<void*>          objectUBsMemoryMapped;
	std::vector<ObjectUBO>      objectUBOs;

	VkDescriptorPool             descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;
	std::vector<VkCommandBuffer> commandBuffers;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence>     inFlightFences;

	std::vector<Vertex>       vertices;
	std::vector<uint32_t>     indices;
	std::vector<Model>        models;
	std::vector<Material>     materials;
	std::vector<Texture>      textures;
	std::vector<RenderObject> renderObjects;
	std::vector<uint32_t>     freeIDs;
	std::vector<ObjectUBO>    renderTransphorms;

	bool framebufferResized = false;
	uint32_t currentFrame = 0;


	uint32_t createMaterial(const tinyobj::material_t& objMat);
	void loadTexture(Texture& texture);
	
	void initWindow();
	void createInstance();
	void createSurface();
	void pickPhysicalDevice();
	void createLogicalDevice();
	void createSwapchain();
	void createImageViews();
	void createRenderPass();
	void createFrameDescriptorSetLayout();
	void createTextureDescriptorSetLayout();
	void createGraphicsPipeline();
	void createCommandPool();
	void createDepthResources();
	void createFramebuffers();
	void createTextureSampler();
	void createVertexBuffer();
	void createIndexBuffer();
        void createFrameUBs();
	void createObjectUBs();
        void createDescriptorPool();
        void createFrameDescriptorSets();
        void createCommandBuffers();
        void createSyncObjects();
	void loadTextures();

	bool isDeviceSuitable(VkPhysicalDevice device);
	std::vector<const char*> getRequiredExtensions();
	bool checkDeviceExtansionSupport(VkPhysicalDevice device);
	QueueFamilyIndicies findQueueFamilies(VkPhysicalDevice device);
	static void frameBufferResizeCallback(GLFWwindow* window, int width, int height);

	SwapchainSupportDetails querySwapchainSupport(VkPhysicalDevice device);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	void recreateSwapchain();
	void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
		 	 VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image,
		 	 VkDeviceMemory& imageMemory);
	void createImageView(VkImage image, VkImageView& imageView, VkFormat format, VkImageAspectFlagBits aspectFlags);
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	VkFormat findDepthFormat();
	VkFormat findSupportedFormat(const std::vector<VkFormat>& cnadidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	bool hasStencilComponent(VkFormat format);

	static std::vector<char> readFile(const std::string& filename);
	void createTextureDescriptor(Texture& texture);
	VkShaderModule createShaderModule(const std::vector<char>& code);

	VkCommandBuffer beginSigleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);

	void createBuffer(VkDeviceSize size, VkBufferUsageFlags flags, VkMemoryPropertyFlags properties, VkBuffer& buffer,
			  VkDeviceMemory& bufferMemory);
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
	void updateFrameUBO(uint32_t currentFrame);
	void updateStorageBuffers(uint32_t currentFrame);

	void cleanupSwapchain();

#ifndef NDEBUG
	VkDebugUtilsMessengerEXT debugMessenger;

	void setupDebugMessenger();
	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
					      const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	bool checkValidationLayerSupport();
	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
					   const VkAllocationCallbacks* pAllocator);
#endif
};



#ifdef STB_VULF_IMPLEMENTATION

void Vulf::frameBufferResizeCallback(GLFWwindow* window, int width, int height){
	auto vulf = reinterpret_cast<Vulf*>(glfwGetWindowUserPointer(window));
	vulf->framebufferResized = true;
}

uint32_t Vulf::loadModel(std::string fileName){
	Model model;

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> objectMaterials;
	std::string warn;
	std::string err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &objectMaterials, &warn, &err, fileName.c_str())) {
		throw std::runtime_error(err);
	}

	std::vector<uint32_t> materialMap(objectMaterials.size());

	for (size_t i = 0; i < objectMaterials.size(); i++) {
		materialMap[i] = createMaterial(objectMaterials[i]);
	}

	for (const auto& shape : shapes) {
		Mesh mesh;
		mesh.firstIndex = indices.size();
		mesh.firstVertex = vertices.size();

		uint32_t currentIndex = 0;
		size_t indexOffset = 0;

		for(size_t face = 0; face < shape.mesh.num_face_vertices.size(); face++){
			int vertexCount = shape.mesh.num_face_vertices[face];
			for(int v = 0; v < vertexCount; v++){
				Vertex vertex{};
				auto index = shape.mesh.indices[indexOffset + v];

				vertex.pos = {
					attrib.vertices[3*index.vertex_index+0],
					attrib.vertices[3*index.vertex_index+1],
					attrib.vertices[3*index.vertex_index+2]
				};

				vertex.texCoord = {
					attrib.texcoords[2*index.texcoord_index+0],
					attrib.texcoords[2*index.texcoord_index+1]
				};


				vertices.push_back(vertex);
				indices.push_back(currentIndex++);
			}
			indexOffset += vertexCount;
		}

		int shapeMaterial = shape.mesh.material_ids[0];
		mesh.indiciesCount = indices.size() - mesh.firstIndex;
		model.subMeshes.push_back({mesh, materialMap[shapeMaterial]});
	}
	models.push_back(model);
	return models.size()-1;
}

uint32_t Vulf::createMaterial(const tinyobj::material_t& objMat){
	std::string name = objMat.name;

	for(uint32_t i = 0; i < materials.size(); i++){
		if(materials[i].materialName == name){
			return i;
		}
	}

	Material newMaterial;
	newMaterial.materialName = name;
	newMaterial.textureID = -1;

	std::string texturePath = objMat.diffuse_texname;
	for(uint32_t i = 0; i < textures.size(); i++){
		if(textures[i].texturePath == texturePath){
			newMaterial.textureID = i;
		}
	}

	if(newMaterial.textureID == -1){
		Texture newTexture;
		newTexture.texturePath = texturePath;
		textures.push_back(newTexture);
		newMaterial.textureID = static_cast<uint32_t>(textures.size() - 1);
	}

	materials.push_back(newMaterial);
	return static_cast<uint32_t>(materials.size() - 1);
}

void Vulf::loadTextures(){
	for(auto& texture : textures){
		loadTexture(texture);
	}
}

void Vulf::loadTexture(Texture& texture){
	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load(texture.texturePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	VkDeviceSize imageSize = texWidth * texHeight * 4;

	if(!pixels){
		throw std::runtime_error("Falied to load texture image\n");
	}

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		     stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	vkUnmapMemory(device, stagingBufferMemory);

	stbi_image_free(pixels);

	createImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
		    VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, texture.image, texture.memory);

	transitionImageLayout(texture.image, VK_FORMAT_R8G8B8A8_SRGB,
			      VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	copyBufferToImage(stagingBuffer, texture.image,
			  static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));

	transitionImageLayout(texture.image, VK_FORMAT_R8G8B8A8_SRGB,
			      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);

	createImageView(texture.image, texture.imageView, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);

	createTextureDescriptor(texture);
}

void Vulf::createTextureDescriptor(Texture& texture){
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &textureDescriptorSetLayout;

	if(vkAllocateDescriptorSets(device, &allocInfo, &texture.descriptorSet) != VK_SUCCESS){
		throw std::runtime_error("Failed to allocate descriptor set\n");
	}

	VkDescriptorImageInfo imageInfo{};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = texture.imageView;
	imageInfo.sampler = textureSampler;

	VkWriteDescriptorSet descriptorWrite{};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = texture.descriptorSet;
	descriptorWrite.dstBinding = 0;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pImageInfo = &imageInfo;

	vkUpdateDescriptorSets(device ,1, &descriptorWrite, 0, nullptr);
}

uint32_t Vulf::createObject(uint32_t modelID, Transphorm objectTrasnphorm){
	RenderObject object{modelID, objectTrasnphorm};
	ObjectUBO ubo = {objectTrasnphorm.modelMatrix()};

	uint32_t ID;
	if(freeIDs.size() == 0){
		ID = renderObjects.size();
		renderObjects.push_back(object);
		objectUBOs.push_back(ubo);
	} else {
		ID = freeIDs[0];
		renderObjects[ID] = object;
		objectUBOs[ID] = ubo;
	}

	return ID;
}

void Vulf::setObjectTrasphorm(uint32_t objectID, Transphorm objectTransphorm){ renderObjects[objectID].transphorm = objectTransphorm;

	ObjectUBO ubo{renderObjects[objectID].transphorm.modelMatrix()};
	objectUBOs[objectID] = ubo;
}

void Vulf::editObgjectTransphorm(uint32_t objectID, Transphorm objectTransphorm){
	renderObjects[objectID].transphorm.position += objectTransphorm.position;
	renderObjects[objectID].transphorm.rotation += objectTransphorm.rotation;
	renderObjects[objectID].transphorm.scale += objectTransphorm.scale;

	ObjectUBO ubo{renderObjects[objectID].transphorm.modelMatrix()};
	objectUBOs[objectID] = ubo;
}

void Vulf::deleteObject(uint32_t objectID){
	freeIDs.push_back(objectID);
}

void Vulf::updateStorageBuffers(uint32_t currentFrame){
	memcpy(objectUBsMemoryMapped[currentFrame], objectUBOs.data(), sizeof(ObjectUBO)*objectUBOs.size());
}

void Vulf::updateFrameUBO(uint32_t currentImage){
	FrameUBO ubo;

	glm::vec3 forward;
	forward.x = cos(cameraDirection.x) * cos(cameraDirection.y);
	forward.y = sin(cameraDirection.y);
	forward.z = sin(cameraDirection.x) * cos(cameraDirection.y);

	forward = glm::normalize(forward);
	glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
	glm::vec3 up = glm::cross(right, forward);

        ubo.view = glm::lookAt(cameraPosition, cameraPosition + forward, up);
        ubo.proj = glm::perspective(glm::radians(FOV), swapchainExtent.width / (float) swapchainExtent.height, 0.1f, 50.0f);
        ubo.proj[1][1] *= -1;

        memcpy(frameUBsMemoryMapped[currentImage], &ubo, sizeof(ubo));
}

void Vulf::init(){
	initWindow();
	createInstance();
#ifndef NDEBUG
	setupDebugMessenger();
#endif
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();

	createSwapchain();
	createImageViews();

	createRenderPass();
	createFrameDescriptorSetLayout();
	createTextureDescriptorSetLayout();
	createGraphicsPipeline();
	createCommandPool();

	createDepthResources();
	createFramebuffers();
	createTextureSampler();

	createVertexBuffer();
	createIndexBuffer();
	createFrameUBs();
	createObjectUBs();

	createDescriptorPool();
	createFrameDescriptorSets();
	createCommandBuffers();
	createSyncObjects();

	loadTextures();
}

bool Vulf::shouldRun(){
	return !glfwWindowShouldClose(window);
}

void Vulf::initWindow(){
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	window = glfwCreateWindow(800, 600, "Vulf graphic engine", nullptr, nullptr);
        glfwSetWindowUserPointer(window, this); //TODO
        glfwSetFramebufferSizeCallback(window, frameBufferResizeCallback);
}

void Vulf::acceptScreenName(std::string name){
	glfwSetWindowTitle(window, name.c_str());
}

void Vulf::createInstance(){
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	//TODO change name
	appInfo.pApplicationName = "Renderer";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "Vulf";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo instanceInfo{};
	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pApplicationInfo = &appInfo;
#ifdef NDEBUG
	instanceInfo.enabledLayerCount = 0;
#else
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if(!checkValidationLayerSupport()){
		throw std::runtime_error("Validation layers requeted, but not available\n");
	}

	instanceInfo.enabledLayerCount = static_cast<uint32_t>(vulkanValidationLayers.size());
	instanceInfo.ppEnabledLayerNames = vulkanValidationLayers.data();

	populateDebugMessengerCreateInfo(debugCreateInfo);
	instanceInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
#endif

	auto extensions = getRequiredExtensions();
	instanceInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	instanceInfo.ppEnabledExtensionNames = extensions.data();

	if(vkCreateInstance(&instanceInfo, nullptr, &instance) != VK_SUCCESS){
		throw std::runtime_error("Failed to create vulkan instance\n");
	}
}

std::vector<const char*> Vulf::getRequiredExtensions(){
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#ifndef NDEBUG
	extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

	return extensions;
}

void Vulf::createSurface(){
	if(glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS){
		throw std::runtime_error("Failed to create window surface\n");
	}
}
	
//TODO chage divice pick method
void Vulf::pickPhysicalDevice(){
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if(deviceCount == 0){
		throw std::runtime_error("Failed to find GPUs with Vulkan support\n");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	for(const auto& device : devices){
		if(isDeviceSuitable(device)){
			physicalDevice = device;
			break;
		}
	}

	if(physicalDevice == VK_NULL_HANDLE){
		throw std::runtime_error("Failed to find suitable GPU\n");
	}
}

bool Vulf::isDeviceSuitable(VkPhysicalDevice device){
	QueueFamilyIndicies indices = findQueueFamilies(device);

	bool extensionsSupported = checkDeviceExtansionSupport(device);

	bool swapchainAdequate = false;
	if(extensionsSupported){
		SwapchainSupportDetails swapchainSupport = querySwapchainSupport(device);
		swapchainAdequate = (!swapchainSupport.formats.empty()) && (!swapchainSupport.presentModes.empty());
	}

	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

	return indices.isComplete() && 
		extensionsSupported && 
		swapchainAdequate /*&&
		supportedFeatures.samplerAnisotropy*/;
}

bool Vulf::checkDeviceExtansionSupport(VkPhysicalDevice device){
	uint32_t extensionsCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionsCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionsCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionsCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for(const auto& extension : availableExtensions){
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

QueueFamilyIndicies Vulf::findQueueFamilies(VkPhysicalDevice device){
	QueueFamilyIndicies indices{};

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for(const auto& queueFamily : queueFamilies){
		if(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT){
			indices.graphicsFamily = i;
		}

		//TODO
		//could be optimised to try to pick th same queue family
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
		if(presentSupport){
			indices.presentFamily = i;
		}

		if(indices.isComplete()){
			break;
		}

		i++;
	}

	return indices;
}

SwapchainSupportDetails Vulf::querySwapchainSupport(VkPhysicalDevice device){
	SwapchainSupportDetails details{};

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

	if(formatCount != 0){
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}


	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

	if(presentModeCount != 0){
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

void Vulf::createLogicalDevice(){
	QueueFamilyIndicies indices = findQueueFamilies(physicalDevice);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(),
						  indices.presentFamily.value()};

	float queuePriority = 1.0f;
	for(uint32_t queueFamily : uniqueQueueFamilies){
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};
	deviceFeatures.samplerAnisotropy = VK_FALSE;

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();

	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

#ifndef NDEBUG
	createInfo.enabledLayerCount = static_cast<uint32_t>(vulkanValidationLayers.size()); createInfo.ppEnabledLayerNames = vulkanValidationLayers.data();
#else
	createInfo.enabledLayerCount = 0;
#endif

	if(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS){
		throw std::runtime_error("Failed to create logical device\n");
	}

	vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}


void Vulf::createSwapchain(){
	SwapchainSupportDetails swapchainSupport = querySwapchainSupport(physicalDevice);
	
	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapchainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapchainSupport.presentModes);
	VkExtent2D extent = chooseSwapExtent(swapchainSupport.capabilities);

	uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;

	if(swapchainSupport.capabilities.maxImageCount > 0 &&
	   imageCount > swapchainSupport.capabilities.maxImageCount){
		imageCount = swapchainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndicies indices = findQueueFamilies(physicalDevice);
	uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(),
					 indices.presentFamily.value()};

	if(indices.graphicsFamily != indices.presentFamily){
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	} else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}

	createInfo.preTransform = swapchainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if(vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapchain) != VK_SUCCESS){
		throw std::runtime_error("Failed to create swapchain\n");
	}

	vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
	swapchainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(device, swapchain, &imageCount, swapchainImages.data());

	swapchainImageFormat = surfaceFormat.format;
	swapchainExtent = extent;
}

VkSurfaceFormatKHR Vulf::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats){
	for(const auto& availableFormat : availableFormats){
		if(availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
		   availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR){
			return availableFormat;
		}
	}

	return availableFormats[0];
}

VkPresentModeKHR Vulf::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes){
	for(const auto& availablePresentMode : availablePresentModes){
		if(availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR){
			return VK_PRESENT_MODE_MAILBOX_KHR;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Vulf::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities){
	if(capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()){
		return capabilities.currentExtent;
	} else {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
	static_cast<uint32_t>(height)
    };

    actualExtent.width = std::clamp(actualExtent.width,
				    capabilities.minImageExtent.width, 
				    capabilities.maxImageExtent.width);

    actualExtent.height = std::clamp(actualExtent.height,
				     capabilities.minImageExtent.height,
				     capabilities.maxImageExtent.height);

    return actualExtent;
}
}

void Vulf::recreateSwapchain(){
	int width = 0, height = 0;
	glfwGetFramebufferSize(window, &width, &height);
	while(width == 0 || height == 0){
		glfwGetFramebufferSize(window, &width, &height);
		glfwWaitEvents();
	}
	
	vkDeviceWaitIdle(device);

	cleanupSwapchain();

	createSwapchain();
	createImageViews();
	createDepthResources();
	createFramebuffers();
}

void Vulf::createImageViews(){
	swapchainImageViews.resize(swapchainImages.size());

	for(size_t i = 0; i < swapchainImages.size(); i++){
		createImageView(swapchainImages[i], swapchainImageViews[i],
				swapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
	}
}

void Vulf::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
		 VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image,
		 VkDeviceMemory& imageMemory){
	VkImageCreateInfo imageCreateInfo {};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.extent.width = width;
	imageCreateInfo.extent.height = height;
	imageCreateInfo.extent.depth = 1;
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.format = format;
	imageCreateInfo.tiling = tiling;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageCreateInfo.usage = usage;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.flags = 0;

	if(vkCreateImage(device, &imageCreateInfo, nullptr, &image) != VK_SUCCESS){
		throw std::runtime_error("Failed to create image\n");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device, image, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, 
						   properties);

	if(vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS){
		throw std::runtime_error("Failed to allocate image memory\n");
	}

	vkBindImageMemory(device, image, imageMemory, 0);
}

uint32_t Vulf::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties){
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice,  &memProperties);

	for(uint32_t i = 0; i < memProperties.memoryTypeCount; i++){
		if((typeFilter & (1 << i)) && ((memProperties.memoryTypes[i].propertyFlags & properties) == properties)){
			return i;
		}
	}

	throw std::runtime_error("Failed to allocate a buffer\n");
}

void Vulf::createImageView(VkImage image, VkImageView& imageView,
		     VkFormat format, VkImageAspectFlagBits aspectFlags){
	VkImageViewCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.image = image;
	createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	createInfo.format = format;
	createInfo.subresourceRange.aspectMask = aspectFlags;
	createInfo.subresourceRange.baseMipLevel = 0;
	createInfo.subresourceRange.levelCount = 1;
	createInfo.subresourceRange.baseArrayLayer = 0;
	createInfo.subresourceRange.layerCount = 1;

	if(vkCreateImageView(device, &createInfo, nullptr, &imageView) != VK_SUCCESS){
		throw std::runtime_error("Failed to create image view\n");
	}
}

void Vulf::createRenderPass(){
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = swapchainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentDescription depthAttachment{};
	depthAttachment.format = findDepthFormat();
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;

	std::array<VkAttachmentDescription, 2> attachments = {colorAttachment,
							      depthAttachment};

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

	VkSubpassDependency dependecy{};
	dependecy.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependecy.dstSubpass = 0;
	dependecy.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
				 VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

	dependecy.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	dependecy.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
				 VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

	dependecy.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
				  VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;


	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependecy;

	if(vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS){
		throw std::runtime_error("Failed to create render pass\n");
	}
}

VkFormat Vulf::findDepthFormat(){
	return findSupportedFormat({VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
				    VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

VkFormat Vulf::findSupportedFormat(const std::vector<VkFormat>& cnadidates, VkImageTiling tiling,
			     VkFormatFeatureFlags features){
	for(VkFormat format : cnadidates){
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);
		if(tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features){
			return format;
		}

		if(tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features){
			return format;
		}
	}

	throw std::runtime_error("Failed to find supported format\n");
}

void Vulf::createFrameDescriptorSetLayout(){
	VkDescriptorSetLayoutBinding frameBinding{};
	frameBinding.binding = 0;
	frameBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	frameBinding.descriptorCount = 1;
	frameBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutBinding objectBinding{};
	objectBinding.binding = 1;
	objectBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	objectBinding.descriptorCount = 1;
	objectBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	std::array<VkDescriptorSetLayoutBinding, 2> bindings = {frameBinding, objectBinding};
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &frameDescriptorSetLayout) != VK_SUCCESS){
		throw std::runtime_error("Failed to create descriptor set layout\n");
	}
}

void Vulf::createTextureDescriptorSetLayout(){
	VkDescriptorSetLayoutBinding textureBinding{};
	textureBinding.binding = 0;
	textureBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	textureBinding.descriptorCount = 1;
	textureBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	std::array<VkDescriptorSetLayoutBinding, 1> bindings = {textureBinding};
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &textureDescriptorSetLayout) != VK_SUCCESS){
		throw std::runtime_error("Failed to create descriptor set layout\n");
	}
}

void Vulf::createGraphicsPipeline(){
	auto vertShaderCode = readFile("shaders/vert.spr");
	auto fragShaderCode = readFile("shaders/frag.spr");

	VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
	VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageCreateInfo{};
	vertShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageCreateInfo.module = vertShaderModule;
	vertShaderStageCreateInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageCreateInfo{};
	fragShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageCreateInfo.module = fragShaderModule;
	fragShaderStageCreateInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageCreateInfo, 
							  fragShaderStageCreateInfo};

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};

	auto bindingDescription = getBindingDescription();
	auto attributeDescriptions = getAttributeDescriptions();

	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float) swapchainExtent.width;
	viewport.height = (float) swapchainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent = swapchainExtent;

	//TODO should i keed dynamic states?
	std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	//TODO cull mode
	rasterizer.cullMode = VK_CULL_MODE_NONE;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f;
	rasterizer.depthBiasClamp = 0.0f;
	rasterizer.depthBiasSlopeFactor = 0.0f;

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f;
	multisampling.pSampleMask = nullptr;
	multisampling.alphaToCoverageEnable = VK_FALSE;
	multisampling.alphaToOneEnable = VK_FALSE;

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
					      VK_COLOR_COMPONENT_G_BIT |
					      VK_COLOR_COMPONENT_B_BIT |
					      VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;

	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.stencilTestEnable = VK_FALSE;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	std::array<VkDescriptorSetLayout, 2> descriptorSetLayouts = {
		frameDescriptorSetLayout,
		textureDescriptorSetLayout
	};

	pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
	pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();

	std::array<VkPushConstantRange, 1> pushConstantRanges{};
	pushConstantRanges[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	pushConstantRanges[0].size = sizeof(PushConstans);
	pushConstantRanges[0].offset = 0;

	pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
	pipelineLayoutInfo.pPushConstantRanges = pushConstantRanges.data();

	if(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &graphicsPipelineLayout) != VK_SUCCESS){
		throw std::runtime_error("Failed to create pipeline layout\n");
	}

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = graphicsPipelineLayout;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex = -1;

	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline\n");
	}

	vkDestroyShaderModule(device, vertShaderModule, nullptr);
	vkDestroyShaderModule(device, fragShaderModule, nullptr);
}

std::vector<char> Vulf::readFile(const std::string& filename){
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if(!file.is_open()){
		throw std::runtime_error("Failed to open a file\n");
	}

	size_t fileSize = (size_t) file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();
	return buffer;
}

VkShaderModule Vulf::createShaderModule(const std::vector<char>& code){
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS){
		throw std::runtime_error("Failed to create shader module\n");
	}

	return shaderModule;
}

void Vulf::createCommandPool(){
	QueueFamilyIndicies queueFamilyIndices = findQueueFamilies(physicalDevice);

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

	if(vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS){
		throw std::runtime_error("Failed to create command pool\n");
	}
}

void Vulf::createDepthResources(){
	VkFormat depthFormat = findDepthFormat();

	createImage(swapchainExtent.width, swapchainExtent.height, depthFormat,
		    VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);

	createImageView(depthImage, depthImageView, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

	transitionImageLayout(depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED,
			      VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

}

void Vulf::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout){
	VkCommandBuffer commandBuffer = beginSigleTimeCommands();

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	if(newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL){
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

		if(hasStencilComponent(format)){
			barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
	} else {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if(oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
		newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL){
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	} else if(oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
		newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	} else if(oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
		newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL){
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
					VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	} else {
		throw std::runtime_error("Unsupported image layout trnsition\n");
	}

	vkCmdPipelineBarrier(commandBuffer,
			     sourceStage, destinationStage,
			     0,
			     0, nullptr,
			     0, nullptr,
			     1, &barrier);

	endSingleTimeCommands(commandBuffer);
}

VkCommandBuffer Vulf::beginSigleTimeCommands(){
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

void Vulf::endSingleTimeCommands(VkCommandBuffer commandBuffer){
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submotInfo{};
	submotInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submotInfo.commandBufferCount = 1;
	submotInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(graphicsQueue, 1, &submotInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicsQueue);

	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

bool Vulf::hasStencilComponent(VkFormat format){
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
	       format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void Vulf::createFramebuffers(){
	swapchainFramebuffers.resize(swapchainImageViews.size());

	for(size_t i = 0; i < swapchainImageViews.size(); i++){
		std::array<VkImageView, 2> attachments = {
			swapchainImageViews[i],
			depthImageView
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = swapchainExtent.width;
		framebufferInfo.height = swapchainExtent.height;
		framebufferInfo.layers = 1;

		if(vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapchainFramebuffers[i]) != VK_SUCCESS){
			throw std::runtime_error("Failed to create framebuffer\n");
		}
	}
}

void Vulf::createBuffer(VkDeviceSize size, VkBufferUsageFlags flags, 
		  VkMemoryPropertyFlags properties, VkBuffer& buffer, 
		  VkDeviceMemory& bufferMemory){
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = flags;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if(vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS){
		throw std::runtime_error("Failed to create vertex buffer\n");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	if(vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS){
		throw std::runtime_error("Failed to allocate buffer memory\n");
	}

	vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

void Vulf::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size){
	VkCommandBuffer commandBuffer = beginSigleTimeCommands();

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	endSingleTimeCommands(commandBuffer);
}

void Vulf::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height){
	VkCommandBuffer commandBuffer = beginSigleTimeCommands();

	VkBufferImageCopy region{};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = {0, 0, 0};
	region.imageExtent = {width, height, 1};

	vkCmdCopyBufferToImage(commandBuffer, buffer, image, 
			       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	endSingleTimeCommands(commandBuffer);
}

void Vulf::createTextureSampler(){
        VkSamplerCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        createInfo.magFilter = VK_FILTER_NEAREST;
        createInfo.minFilter = VK_FILTER_LINEAR;
        createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(physicalDevice, &properties);

        createInfo.anisotropyEnable = VK_FALSE;
        createInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

        createInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
        createInfo.unnormalizedCoordinates = VK_FALSE;
        createInfo.compareEnable = VK_FALSE;
        createInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        createInfo.mipLodBias = 0.0f;
        createInfo.minLod = 0.0f;
        createInfo.maxLod = 0.0f;

        if(vkCreateSampler(device, &createInfo, nullptr, &textureSampler) != VK_SUCCESS){
            throw std::runtime_error("Failed to create sampler object\n");
        }
}

void Vulf::createVertexBuffer(){
	VkDeviceSize size = sizeof(Vertex) * vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	createBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
		     stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(device, stagingBufferMemory, 0, size, 0, &data);
	memcpy(data, vertices.data(), (size_t) size);
	//DO WE HAVE tO UNMAP???????????????????
	//TODO

	createBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

	copyBuffer(stagingBuffer, vertexBuffer, size);

	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);
	}

void Vulf::createIndexBuffer(){
	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
		     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
		     stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), (size_t) bufferSize);
	vkUnmapMemory(device, stagingBufferMemory);

	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

	copyBuffer(stagingBuffer, indexBuffer, bufferSize);

	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);
	}

void Vulf::createFrameUBs(){
	VkDeviceSize bufferSize = sizeof(FrameUBO);

	frameUBs.resize(MAX_FRAMES_IN_FLIGHT);
	frameUBsMemory.resize(MAX_FRAMES_IN_FLIGHT);
	frameUBsMemoryMapped.resize(MAX_FRAMES_IN_FLIGHT);

	for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){
		createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
			     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
			     frameUBs[i], frameUBsMemory[i]);

		vkMapMemory(device, frameUBsMemory[i], 0, bufferSize, 0, &frameUBsMemoryMapped[i]);
	}
}

void Vulf::createObjectUBs(){
	VkDeviceSize bufferSize = MAX_OBJECTS * sizeof(ObjectUBO);

	objectUBs.resize(MAX_FRAMES_IN_FLIGHT);
	objectUBsMemory.resize(MAX_FRAMES_IN_FLIGHT);
	objectUBsMemoryMapped.resize(MAX_FRAMES_IN_FLIGHT);

	for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){
		createBuffer(bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, //<- maybe should use uniform bit
			     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
			     objectUBs[i], objectUBsMemory[i]);

		vkMapMemory(device, objectUBsMemory[i], 0, bufferSize, 0, &objectUBsMemoryMapped[i]);
	}
}

void Vulf::createDescriptorPool(){
	std::array<VkDescriptorPoolSize, 3> poolSizes{};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	poolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	poolSizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[2].descriptorCount = 10;

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	//poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	poolInfo.maxSets = 14;

	if(vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS){
		throw std::runtime_error("Failed to create descriptor pool\n");
	}
}

void Vulf::createFrameDescriptorSets(){
	std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, frameDescriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	allocInfo.pSetLayouts = layouts.data();

	descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
	if(vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) != VK_SUCCESS){
		throw std::runtime_error("Failed to allocate descriptor sets\n");
	}

	for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){
		VkDescriptorBufferInfo frameUBInfo{};
		frameUBInfo.buffer = frameUBs[i];
		frameUBInfo.offset = 0;
		frameUBInfo.range = sizeof(FrameUBO);

		VkDescriptorBufferInfo objectUBInfo{};
		objectUBInfo.buffer = objectUBs[i];
		objectUBInfo.offset = 0;
		objectUBInfo.range = MAX_OBJECTS * sizeof(ObjectUBO);

		std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = descriptorSets[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &frameUBInfo;
		descriptorWrites[0].pImageInfo = nullptr;
		descriptorWrites[0].pTexelBufferView = nullptr;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = descriptorSets[i];
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pBufferInfo = &objectUBInfo;
		descriptorWrites[1].pImageInfo = nullptr;
		descriptorWrites[1].pTexelBufferView = nullptr;
		vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()),
				       descriptorWrites.data(), 0, nullptr);
	}
}

void Vulf::cleanupSwapchain(){
	vkDestroyImageView(device, depthImageView, nullptr);
	vkDestroyImage(device, depthImage, nullptr);
	vkFreeMemory(device, depthImageMemory, nullptr);

	for(auto framebuffer : swapchainFramebuffers){
		vkDestroyFramebuffer(device, framebuffer, nullptr);
	}

	for(auto imageView : swapchainImageViews){
		vkDestroyImageView(device, imageView, nullptr);
	}

	vkDestroySwapchainKHR(device, swapchain, nullptr);
}

void Vulf::createCommandBuffers(){
	commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();

	if(vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS){
		throw std::runtime_error("Failed to allocate command buffers\n");
	}
}

void Vulf::createSyncObjects(){
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(swapchainImages.size());
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){
		if(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
		   vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS){
			throw std::runtime_error("Failed to create sync obects\n");
		}
	}
	for(size_t i = 0; i < swapchainImages.size(); i++){
		if(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS){
			throw std::runtime_error("Failed to create sync obects\n");
		}
	}
}

void Vulf::drawFrame(){
	vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, imageAvailableSemaphores[currentFrame],
					 	VK_NULL_HANDLE, &imageIndex);

	if(result == VK_ERROR_OUT_OF_DATE_KHR){
		recreateSwapchain();
		return;
	} else if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR){
		throw std::runtime_error("Failed to acquire swapchain image\n");
	}

	vkResetFences(device, 1, &inFlightFences[currentFrame]);

	updateFrameUBO(currentFrame);

	vkResetCommandBuffer(commandBuffers[currentFrame], 0);
	recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

	VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[imageIndex]};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if(vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS){
		throw std::runtime_error("Failed to submit draw command buffer\n");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapchains[] = {swapchain};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapchains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;

	result = vkQueuePresentKHR(presentQueue, &presentInfo);

	if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized){
		framebufferResized = false;
		recreateSwapchain();
	} else if(result != VK_SUCCESS){
		throw std::runtime_error("Failed to present swapchain image\n");
	}

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

    void Vulf::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex){
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if(vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS){
            throw std::runtime_error("Failed to begin recording command buffer\n");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = swapchainFramebuffers[imageIndex];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapchainExtent;

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {{0.3f, 0.5f, 0.5f, 1.0f}};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swapchainExtent.width);
        viewport.height = static_cast<float>(swapchainExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = swapchainExtent;
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	updateFrameUBO(currentFrame);
	updateStorageBuffers(currentFrame);

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelineLayout, 0, 1, 
				&descriptorSets[currentFrame], 0, nullptr);

        VkBuffer vertexBuffers[] = {vertexBuffer};
        VkDeviceSize offsets[] = {0};

	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

	for(uint32_t i = 0; i < renderObjects.size(); i++){

		bool skip = false;
		for(uint32_t ID : freeIDs){
			if(i == ID){
				skip = true;
			}
		}

		if(skip) continue;

		PushConstans pc {i};

		for(const SubMesh& subMesh : models[renderObjects[i].modelID].subMeshes){
			vkCmdPushConstants(commandBuffer, graphicsPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0,
					   sizeof(PushConstans), &pc);
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelineLayout, 
						1, 1, &textures[materials[subMesh.materialID].textureID].descriptorSet, 0, nullptr);

			Mesh mesh = subMesh.mesh;
			vkCmdDrawIndexed(commandBuffer, mesh.indiciesCount, 1, mesh.firstIndex, mesh.firstVertex, 0);
		}
	}

        vkCmdEndRenderPass(commandBuffer);

        if(vkEndCommandBuffer(commandBuffer) != VK_SUCCESS){
            throw std::runtime_error("Failed to end command buffer\n");
        }
    }

void Vulf::cleanup(){

	vkDeviceWaitIdle(device);
	cleanupSwapchain();

	vkDestroyDescriptorPool(device, descriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(device, frameDescriptorSetLayout, nullptr);
	vkDestroyDescriptorSetLayout(device, textureDescriptorSetLayout, nullptr);
	vkDestroySampler(device, textureSampler, nullptr);

	for(auto texture : textures){
		vkDestroyImage(device, texture.image, nullptr);
		vkDestroyImageView(device, texture.imageView, nullptr);
		vkFreeMemory(device, texture.memory, nullptr);
	}

	for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){
		vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(device, inFlightFences[i], nullptr);

		vkDestroyBuffer(device, frameUBs[i], nullptr);
		vkFreeMemory(device, frameUBsMemory[i], nullptr);

		vkDestroyBuffer(device, objectUBs[i], nullptr);
		vkFreeMemory(device, objectUBsMemory[i], nullptr);
	}

	for(size_t i = 0; i < swapchainImages.size(); i++){
		vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
	}

	vkDestroyCommandPool(device, commandPool, nullptr);

	vkDestroyBuffer(device, vertexBuffer, nullptr);
	vkFreeMemory(device, vertexBufferMemory, nullptr);
	vkDestroyBuffer(device, indexBuffer, nullptr);
	vkFreeMemory(device, indexBufferMemory, nullptr);

	vkDestroyPipeline(device, graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(device, graphicsPipelineLayout, nullptr);
	vkDestroyRenderPass(device, renderPass, nullptr);

	vkDestroyDevice(device, nullptr);
	vkDestroySurfaceKHR(instance, surface, nullptr);

#ifndef NDEBUG
	DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
#endif

	vkDestroyInstance(instance, nullptr);

	glfwDestroyWindow(window);
	glfwTerminate();
}

#ifndef NDEBUG

void Vulf::setupDebugMessenger(){
	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	populateDebugMessengerCreateInfo(createInfo);

	if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
		throw std::runtime_error("failed to set up debug messenger!");
	}
}

VkResult Vulf::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
					    const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger){
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	} else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

//VAL SEVERETY
void Vulf::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo){
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity= //VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
				    //VK_DEBUG_UTIlS_MESSAGE_SEVERITY_INFO_BIT_EXT |
				    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
				    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
				 VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
				 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
}

bool Vulf::checkValidationLayerSupport() {
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : vulkanValidationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
			layerFound = true;
			break;
		}
	}

		if (!layerFound) {
			return false;
		}
	}

	return true;
}

void Vulf::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
				   const VkAllocationCallbacks* pAllocator){
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
        	func(instance, debugMessenger, pAllocator);
	}
}
#endif
#endif
// NOLINTEND(misc-definitions-in-headers)
