#pragma once

#include <gl\glew.h>
#include <glm\glm.hpp>

#include <string>
#include <vector>

// attribute flags
#define ATTRIB_POS 0x1
#define ATTRIB_NORM 0x2
#define ATTRIB_TEX 0x4

class Mesh
{	
	// used to load blender data
	// internally.
	struct BlendData
	{
		unsigned int vertexCount;
		unsigned int texCoordCount;
		unsigned int normalCount;	
		unsigned int indexCount;
		unsigned int smooth;		 // is the texture smooth (0 for no, 1 for yes)

		float* vertices;
		float* normals;
		float* texCoords;			 // less precission error (and cheaper) to pass as short...?
		unsigned int* indices;
	} typedef ModelData;
	
public:	
	typedef GLubyte MeshAttributeFlags;

	struct MeshVertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoord;
	};

public:
	Mesh();
	Mesh(const Mesh&) = delete;
	~Mesh();

	bool loadMesh(const char* filename);

	MeshVertex& vertices();
	unsigned int& indices();

	GLuint vertexBuffer() const;
	GLuint indexBuffer() const;

	MeshAttributeFlags flags() const;
	std::string filename() const;
	int vertexCount() const;
	int indicesCount() const;
	int stride() const;

	bool isLoaded() const;

public:
	static std::string vertexToString(const MeshVertex& vertex);

private:
	bool m_isLoaded;

	std::vector<MeshVertex> m_vertices;
	std::vector<unsigned int> m_indices;
	std::string m_filename;

	MeshAttributeFlags m_attributes;

	// gl stuff
	GLuint m_vbo;
	GLuint m_ibo;
};
