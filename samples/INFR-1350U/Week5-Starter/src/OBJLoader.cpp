#include "OBJLoader.h"

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>


std::vector< unsigned int > vertexIndices, texIndices, normalIndices;
std::vector< glm::vec3 > temp_vertices;
std::vector< glm::vec2 > temp_texs;
std::vector< glm::vec3 > temp_normals;

std::vector< glm::vec3 > vertex;
std::vector< glm::vec2 > texture;
std::vector< glm::vec3 > normal;
GLint temp = 0;


VertexArrayObject::sptr OBJLoader::LoadFile(const std::string& filename)
{
	// Open our file in binary mode
	std::ifstream file (filename);
	//file.open(filename, std::ios::binary);
	//FILE* file = fopen(filename, "r");

	// If our file fails to open, we will throw an error
	if (!file) {
		throw std::runtime_error("Failed to open file");
	}

	MeshBuilder<VertexPosNormTexCol> mesh;
	std::string line;

	while (!file.eof()) {
		std::getline(file, line);

		if (line[0] == '#')
		{
			// Comment, nothing done for comments 
		}

		else if (line.substr(0, 2) == "v ") // We can do equality check this way since the left side is a string and not a char*
		{
			//std::cout << " 22 222 TESTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT " << std::endl;
			std::istringstream ss = std::istringstream(line.substr(1));
			glm::vec3 pos;
			ss >> pos.x >> pos.y >> pos.z;
			//std::cout << " Pos X: " << pos.x << " Pos Y: " << pos.y << " Pos Z: " << pos.z << std::endl;

			temp_vertices.push_back(glm::vec3(pos.x, pos.y, pos.z));
		}

		else if (line.substr(0, 2) == "vt")
		{
			std::istringstream ss = std::istringstream(line.substr(2));
			glm::vec2 tex;
			ss >> tex.x >> tex.y;
			//std::cout << " Tex X: " << tex.x << " Tex Y: " << tex.y << std::endl;

			temp_texs.push_back(glm::vec2(tex.x, tex.y));
		}

		else if (line.substr(0, 2) == "vn")
		{
			std::istringstream ss = std::istringstream(line.substr(2));
			glm::vec3 norm;
			ss >> norm.x >> norm.y >> norm.z;
			//std::cout << " Norma X: " << norm.x << " Norma Y: " << norm.y << " Norma Z: " << norm.z << std::endl;

			temp_normals.push_back(glm::vec3(norm.x, norm.y, norm.z));
		}

		else if (line.substr(0, 2) == "f ")
		{
			std::istringstream ss = std::istringstream(line.substr(1));
			
			int counter = 0;
			while (ss >> temp) {
				if (counter==0)
					vertexIndices.push_back(temp);
				else if (counter == 1)
					texIndices.push_back(temp);
				else if (counter == 2)
					normalIndices.push_back(temp);

				//std::cout << " NUM: " << temp << std::endl;

				if (ss.peek() == '/') {
					++counter;
					ss.ignore(1, '/');}

				else if (ss.peek() == ' ') {
					++counter;
					ss.ignore(1, ' ');
				}
				if (counter > 2) {
					counter = 0;
				}

			}
			
			/*unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];

			ss >> vertexIndex[0] >> uvIndex[0] >> normalIndex[0];
			ss >> vertexIndex[1] >> uvIndex[1] >> normalIndex[1];
			ss >> vertexIndex[2] >> uvIndex[2] >> normalIndex[2];

			std::cout << " VERTEX: " << vertexIndex[0] << " TEXTURE: " << uvIndex[0] << " NORMAL: " << normalIndex[0] << std::endl;
			std::cout << " VERTEX1: " << vertexIndex[1] << " TEXTURE1: " << uvIndex[1] << " NORMAL1: " << normalIndex[1] << std::endl;
			std::cout << " VERTEX2: " << vertexIndex[2] << " TEXTURE2: " << uvIndex[2] << " NORMAL2: " << normalIndex[2] << std::endl;

			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			texIndices.push_back(uvIndex[0]);
			texIndices.push_back(uvIndex[1]);
			texIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);*/
		}

		else {		}

	}
	//std::cout << " NUM of  vertices:  " << vertex.size() << std::endl;

	std::cout << " NUM of  vertices:  " << vertexIndices.size() << std::endl;

	for (unsigned int i = 0; i < vertexIndices.size(); i++) {
		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int texIndex = texIndices[i];
		unsigned int normalIndex = normalIndices[i];

		// Get the attributes thanks to the index
		glm::vec3 vertexs = temp_vertices[vertexIndex - 1];
		glm::vec2 texs = temp_texs[texIndex - 1];
		glm::vec3 normals = temp_normals[normalIndex - 1];

		// Put the attributes in buffers
		vertex.push_back(vertexs);
		texture.push_back(texs);
		normal.push_back(normals);

		mesh.AddIndex(i);
		mesh.AddVertex(vertexs, normals, texs, glm::vec4 (0.35f));
	
	}

	return mesh.Bake();
}