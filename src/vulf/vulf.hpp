#pragma once

#include <iostream>
#include <stdexcept>
#include <vector>
#include <cstring>
#include <map>
#include <optional>
#include <set>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan_core.h>


#ifndef NDEBUG //TODO
	const bool enableVulkanValidationLayers = true;

const std::vector<const char*> vulkanValidationLayers = {
	"VK_LAYER_KHRONOS_validation"
};
#endif

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};


struct QueueFamilyIndicies {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete(){
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct SwapchainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

class Vulf {
public:
	void init(){
		initWindow();
		createInstance();
		createSurface();
		pickPhysicalDevice();
		createLogicalDevice();
#ifndef NDEBUG
		setupDebugMessenger();
#endif
	}

private:
	GLFWwindow*      window;
	VkInstance       instance;
	VkSurfaceKHR     surface;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice         device/* = VK_NULL_HANDLE*/; //TODO figure out how does this work
	VkQueue          graphicsQueue;
	VkQueue          presentQueue;


	void initWindow(){
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		window = glfwCreateWindow(800, 600, "game", nullptr, nullptr);
	}

	void createInstance(){
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Renderer";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
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

	std::vector<const char*> getRequiredExtensions() {
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#ifndef NDEBUG
		if (enableVulkanValidationLayers) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}
#endif

		return extensions;
	}

	void createSurface(){
		if(glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS){
			throw std::runtime_error("Failed to create window surface\n");
		}
	}
	
	//TODO refactor ts
	void pickPhysicalDevice(){
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

	bool isDeviceSuitable(VkPhysicalDevice device){
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
			swapchainAdequate &&
			supportedFeatures.samplerAnisotropy;
	}

	bool checkDeviceExtansionSupport(VkPhysicalDevice device){
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

	QueueFamilyIndicies findQueueFamilies(VkPhysicalDevice device){
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

	SwapchainSupportDetails querySwapchainSupport(VkPhysicalDevice device){
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

	void createLogicalDevice(){
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
		createInfo.enabledLayerCount = static_cast<uint32_t>(vulkanValidationLayers.size());
		createInfo.ppEnabledLayerNames = vulkanValidationLayers.data();
#else
		createInfo.enabledLayerCount = 0;
#endif

		if(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS){
			throw std::runtime_error("Failed to create logical device\n");
		}

		/*
		if(device == VK_NULL_HANDLE){
			throw std::runtime_error("Failed to create logical device\n");
		}
		*/

		vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
		vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
	}

	void cleanup(){
#ifndef NDEBUG
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
#endif

		vkDestroyInstance(instance, nullptr);

		glfwDestroyWindow(window);
		glfwTerminate();
	}

#ifndef NDEBUG
	VkDebugUtilsMessengerEXT debugMessenger;

	void setupDebugMessenger(){
		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		populateDebugMessengerCreateInfo(createInfo);

		if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
			throw std::runtime_error("failed to set up debug messenger!");
		}
	}

	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
				      const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger){
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr) {
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		} else {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	//VAL SEVERETY
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo){
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = //VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
					     VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
					     VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
					 VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
					 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
	}

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
							    VkDebugUtilsMessageTypeFlagsEXT messageType, 
							    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
							    void* pUserData) {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }

	bool checkValidationLayerSupport() {
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

	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
					const VkAllocationCallbacks* pAllocator){
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
        	func(instance, debugMessenger, pAllocator);
	}
	}
#endif
};
