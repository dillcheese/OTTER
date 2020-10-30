#pragma once
#include "MeshFactory.h"


class OBJLoader
{
public:
	static VertexArrayObject::sptr LoadFile(const std::string& file);

protected:
	OBJLoader() = default;
	~OBJLoader() = default;

};

