#version 450

layout(binding = 0) readonly uniform FrameUB{
	mat4 view;
	mat4 proj;
}camera;

struct BillboardY {
	vec4 position;
	vec4 dimensions;
	uint materialID;
};

layout(binding = 1) readonly buffer BillboardYBuffer{
	BillboardY billboards[];
};

layout(push_constant) uniform PushConstants{
	uint objectID;
}pc;


layout(location = 0) out vec2 fragTexCoord;

void main(){
	BillboardY billboard = billboards[pc.objectID];

	vec2 corners[4] = vec2[](
		vec2(-0.5, 0.0),
		vec2( 0.5, 0.0),
		vec2(-0.5, 1.0),
		vec2( 0.5, 1.0)
	);

	vec2 texCoords[4] = vec2[](
		vec2(0.0, 1.0),
		vec2(1.0, 1.0),
		vec2(0.0, 0.0),
		vec2(1.0, 0.0)
	);

	vec3 right = normalize(vec3(camera.view[0][0], 0.0, camera.view[2][0]));

	vec3 worldPosition = billboard.position.xyz + right * corners[gl_VertexIndex].x * billboard.dimensions.x +
			     vec3(0.0, 1.0, 0.0) * corners[gl_VertexIndex].y * billboard.dimensions.y;

	gl_Position = camera.proj * camera.view * vec4(worldPosition, 1.0);
	fragTexCoord =  texCoords[gl_VertexIndex];
}
