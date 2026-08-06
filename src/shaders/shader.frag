#version 450

layout(set = 1, binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;


void main(){
	vec4 tex = vec4(texture(texSampler, fragTexCoord));

	if(tex.a < 0.5)
		discard;

	outColor = tex;
	//outColor = vec4(fragTexCoord, 0.0f, 1.0f);
}
