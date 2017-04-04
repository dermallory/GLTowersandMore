#include <glm\gtc\epsilon.hpp>
#include "mesh.h"
#include "..\include\mesh.h"

#ifdef __cplusplus
extern "C" {
	char* load_blend(const char* filename);
	void free_blend(void* data);
}
#endif

Mesh::Mesh()
:
m_vertices(),
m_indices(),
m_filename(),
m_vbo(0),
m_ibo(0),
m_isLoaded(false)
{
	
}

Mesh::~Mesh()
{
	// RAII!
	glDeleteBuffers(1, &m_ibo);
	glDeleteBuffers(1, &m_vbo);
}

bool Mesh::loadMesh(const char* filename)
{	
	if (m_isLoaded)
	{
		fprintf(stderr, "Error: A mesh has already been loaded using this structure. Cannot load: %s\n", filename);
		return false;
	}

	// get the raw blender data
	ModelData* data = reinterpret_cast<ModelData*>(load_blend(filename));
	if (!data)
	{
		return m_isLoaded = false;
	}

	// now need to convert from using indices for each individual attribute,
	// to using only one set of indices (opengl requirement)
	int positions = data->vertexCount;
	int normals = data->normalCount;
	int tex_coords = data->texCoordCount;

	// check what attributes that are going to be used
	MeshAttributeFlags attrib = 0;
	attrib |= positions ? ATTRIB_POS : 0;
	attrib |= normals ? ATTRIB_NORM : 0;
	attrib |= tex_coords ? ATTRIB_TEX : 0;

	if (!(attrib & ATTRIB_POS))
	{
		fprintf(stderr, "Cannot load mesh. It has no vertices!\n");
		free_blend(data);
		return m_isLoaded = false;
	}

	int blendIndexStride = ((attrib & ATTRIB_POS) ? 1 : 0) 
		+ ((attrib & ATTRIB_NORM) ? 1 : 0) + ((attrib & ATTRIB_TEX) ? 1 : 0);
	unsigned int* indices = data->indices;
	int indexCount = data->indexCount/blendIndexStride;

	m_vertices.reserve(positions*2);
	m_indices.reserve(indexCount);

	int i = 0;

	// foreach new "vertex", check that there are no duplicates before
	// inserting into the array. otherwise, simply add to the index array.
	for (auto index = &indices[0]; i<indexCount; index+=blendIndexStride, ++i)
	{
		bool isNewVertex = true;

		MeshVertex vertex;
		// reformat position and normal, so that y = z and z = -y due to blender's coordinate system vs opengl's
		vertex.position.x = (attrib & ATTRIB_POS) ? *(data->vertices+(3*(*index))) : 0;
		vertex.position.y = ((attrib & ATTRIB_POS) ? *(data->vertices+(3*(*index))+1) : 0);
		vertex.position.z = (attrib & ATTRIB_POS) ? *(data->vertices+(3*(*index))+2) : 0;
		vertex.texCoord.s = (attrib & ATTRIB_TEX) ? *(data->texCoords+(2*(*(index+(attrib&ATTRIB_POS))))) : -1;
		vertex.texCoord.t = (attrib & ATTRIB_TEX) ? *(data->texCoords+(2*(*(index+(attrib&ATTRIB_POS))))+1) : -1;
		vertex.normal.x = (attrib & ATTRIB_NORM) ? *(data->normals+(3*(*(index+(attrib&ATTRIB_POS)+(attrib&ATTRIB_TEX))))) : 0;
		vertex.normal.y = ((attrib & ATTRIB_NORM) ? *(data->normals+(3*(*(index+(attrib&ATTRIB_POS)+(attrib&ATTRIB_TEX))))+1) : 0);
		vertex.normal.z = (attrib & ATTRIB_NORM) ? *(data->normals+(3*(*(index+(attrib&ATTRIB_POS)+(attrib&ATTRIB_TEX))))+2) : 0;

		if (m_vertices.size() > 0)
		{
			int j = 0; // current index in this vertex iteration

			// compare each vertex to already added vertices
			for each (auto v in m_vertices)
			{
				bool posIsEqual = glm::epsilonEqual<glm::float32>(v.position.x, vertex.position.x, glm::epsilon<glm::float32>()) &&
					glm::epsilonEqual<glm::float32>(v.position.y, vertex.position.y, glm::epsilon<glm::float32>()) &&
					glm::epsilonEqual<glm::float32>(v.position.z, vertex.position.z, glm::epsilon<glm::float32>());

				bool normIsEqual = glm::epsilonEqual<glm::float32>(v.normal.x, vertex.normal.x, glm::epsilon<glm::float32>()) &&
					glm::epsilonEqual<glm::float32>(v.normal.y, vertex.normal.y, glm::epsilon<glm::float32>()) &&
					glm::epsilonEqual<glm::float32>(v.normal.z, vertex.normal.z, glm::epsilon<glm::float32>());
				
				bool texIsEqual = glm::epsilonEqual<glm::float32>(v.texCoord.s, vertex.texCoord.t, glm::epsilon<glm::float32>()) &&
					glm::epsilonEqual<glm::float32>(v.texCoord.s, vertex.texCoord.t, glm::epsilon<glm::float32>());

				if (posIsEqual && normIsEqual && texIsEqual)
				{
					m_indices.push_back(j);
					isNewVertex = false;

					break;
				}

				++j;
			}
		}	
		
		// is a new vertex, therefore a new index to keep track of as well
		if (isNewVertex)
		{
			m_vertices.push_back(vertex);

			// start from 0 instead of 1...
			m_indices.push_back(m_vertices.size()-1);
		}
	}
	
	// cache the vertex attribute flags
	m_attributes = attrib;
	m_filename = filename;

	// don't need the raw blender data in memory anymore, so clear it
	free_blend(data);

	// create vertex and index buffers, and send the data over to the 'server'
	glCreateBuffers(1, &m_vbo);
	glNamedBufferStorage(m_vbo, sizeof(MeshVertex)*m_vertices.size(), &m_vertices.front(), NULL);

	glCreateBuffers(1, &m_ibo);
	glNamedBufferStorage(m_ibo, sizeof(unsigned int)*m_indices.size(), &m_indices.front(), NULL);

	return m_isLoaded = true;
}

int Mesh::vertexCount() const
{
	return m_vertices.size();
}

int Mesh::indicesCount() const
{
	return m_indices.size();
}

Mesh::MeshVertex& Mesh::vertices()
{
	return m_vertices.front();
}

unsigned int& Mesh::indices()
{
	return m_indices.front();
}

std::string Mesh::filename() const
{
	return m_filename;
}

GLuint Mesh::vertexBuffer() const
{
	return m_vbo;
}

GLuint Mesh::indexBuffer() const
{
	return m_ibo;
}

int Mesh::stride() const
{
	return sizeof(MeshVertex);
}

Mesh::MeshAttributeFlags Mesh::flags() const
{
	return m_attributes;
}

bool Mesh::isLoaded() const
{
	return m_isLoaded;
}

// TODO:
std::string Mesh::vertexToString(const MeshVertex& vertex)
{
	return std::string();
}
