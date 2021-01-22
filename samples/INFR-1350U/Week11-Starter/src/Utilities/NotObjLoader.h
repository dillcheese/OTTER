<<<<<<< HEAD
#pragma once
#include "MeshFactory.h"

class NotObjLoader
{
public:
	static VertexArrayObject::sptr LoadFromFile(const std::string& filename);

protected:
	NotObjLoader() = default;
	~NotObjLoader() = default;
=======
#pragma once
#include "MeshFactory.h"

class NotObjLoader
{
public:
	static VertexArrayObject::sptr LoadFromFile(const std::string& filename);

protected:
	NotObjLoader() = default;
	~NotObjLoader() = default;
>>>>>>> master
};