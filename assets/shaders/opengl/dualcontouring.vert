#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform DualContouringUniformBufferObject {
  mat4 model;
  mat4 view;
  mat4 proj;
  vec3 camera;
} dcubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec3 inBitTangent;

layout(location = 0) out vec3 outNormal;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 FragPos;

void main() {
  gl_Position = dcubo.proj * dcubo.view * dcubo.model * vec4(inPosition, 1.0);
  mat3 normalMatrix = transpose(inverse(mat3(dcubo.view * dcubo.model)));
  outNormal = inNormal; 
  fragTexCoord = inTexCoord;

  FragPos = vec3(dcubo.model * vec4(inPosition, 1.0));
}
