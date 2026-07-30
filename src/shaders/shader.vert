#version 450

layout(binding = 0) readonly uniform FrameUB{
	mat4 view;
	mat4 proj;
}camera;

layout(binding = 1) readonly buffer ObjectUBO{
	mat4 models[];
}objects;

layout(push_constant) uniform PushConstants{
	uint objectID;
}pc;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec2 fragTexCoord;

void main(){
	gl_Position = camera.proj * camera.view * objects.models[pc.objectID] * vec4(inPosition, 1.0);
	fragTexCoord = inTexCoord;
}
