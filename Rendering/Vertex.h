#pragma once
#include <glm\ext\vector_float3.hpp>
#include <glm\ext\vector_float2.hpp>

struct Vertex {
	glm::vec3 Position;
	glm::vec2 UV;
	glm::vec3 Normal;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
};

struct Vertex2d {
	glm::vec2 Position;
	glm::vec2 UV;
};