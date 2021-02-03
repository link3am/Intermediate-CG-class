#pragma once
//#include "MeshFactory.h"
#include "Graphics/VertexArrayObject.h"
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include "MeshBuilder.h"
#include "VertexTypes.h"
class ObjLoader
{
public:
	static VertexArrayObject::sptr LoadFromFile(const std::string& filename, const glm::vec4& inColor = glm::vec4(1.0f));

protected:
	ObjLoader() = default;
	~ObjLoader() = default;
};