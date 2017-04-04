#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "blendload.h"

// forward declarations
void* _alloc_blend();
char _get_symbol(char* symbol);

// each index contains the vertex, normal, and texture coordinate
struct ModelData
{
	unsigned int vertexCount;
	unsigned int texCoordCount;
	unsigned int normalCount;	
	unsigned int indexCount;
	unsigned int smooth;		 // is the texture smooth (0 for no, 1 for yes)
	
	float* vertices;
	float* normals;
	float* texCoords;
	unsigned int* indices;
} typedef ModelData;

/*
// do not need to free the indices because we are using them
// as a return value. It is left on the responsibility of the
// receiver.
// returns null if not successful.
*/
void* load_blend(const char* filename)
{
	FILE* file;
	ModelData* data = _alloc_blend();
	unsigned int maxVertex = BUF_SZ;
	unsigned int maxTex = BUF_SZ;
	unsigned int maxNormal = BUF_SZ;
	unsigned int maxIndex = BUF_SZ;
	int attributes = 0;				// amount of vertex attributes per index

	if (!data)
	{
		perror("modload.c: Could not allocate space for the model load.");
		return NULL;
	}

	// make sure file name is of appropriate length
	unsigned int fileNameCount = 0;
	for (const char* n = filename; *n; ++n)
	{
		if (fileNameCount > FILENAME_MAX)
		{
			perror("modload.c: Filename is too long");
			free(data);
			return NULL;
		}
		
		++fileNameCount;
	}
	
	file = fopen(filename, "r");

	// check whether file was opened successfully
	if (!file)
	{
		perror("modload.c: Error loading file");
		return NULL;
	}

	char lineBuffer[BUF_SZ];
	int currentLine = 0;
	int hasVertices=0, hasTexCoords=0, hasNormals=0;
	while (TRUE)
	{
		++currentLine;
		char* lineResult = fgets(lineBuffer, BUF_SZ, file);
		
		// if end of file, break out of loop
		if (!lineResult)
		{
			break;
		}

		char lineSymbol[3] = {'\0'};
		lineSymbol[0] = lineBuffer[0];
		lineSymbol[1] = lineBuffer[1];

		// check the symbol for validity and support
		char symbol = _get_symbol(lineSymbol);
		if (!symbol)
		{
			printf("modload.c: Invalid symbol: line(%d)\n", currentLine);
			continue;
		}

		// check the line type
		if (symbol == COMMENT ||
			symbol == NAME)
		{
			continue;
		}

		// parse and record vertex data to return
		if (symbol == VERTEX)
		{
			// extract the vertex coordinates
			float x = extract_float_from_str(&lineResult);
			while (isdigit(*lineResult) || ispunct(*lineResult))
			{
				++lineResult;
			}

			float y = extract_float_from_str(&lineResult);
			while (isdigit(*lineResult) || ispunct(*lineResult))
			{
				++lineResult;
			}

			float z = extract_float_from_str(&lineResult);

			int index = data->vertexCount*3;
			data->vertices[index++] = x;
			data->vertices[index++] = y;
			data->vertices[index] = z;

			++data->vertexCount;

			if (!hasVertices)
			{
				++attributes;
				hasVertices = TRUE;
			}	
			
			// reallocate the normal data since it has
			// grown too large
			if (data->vertexCount >= maxVertex)
			{
				printf("reallocating vertices.\n");
				maxTex += BUF_SZ;
				void* resultVertices = realloc(data->normals, maxVertex*(sizeof(unsigned int)*2));	

				if (!resultVertices)
				{
					free_blend(data);
					return NULL;
				}

				data->vertices = resultVertices;
			}		
		}
		else if (symbol == TEX_COORD)
		{
			// extract the texture coordinates
			float t = extract_float_from_str(&lineResult);
			while (isdigit(*lineResult) || ispunct(*lineResult))
			{
				++lineResult;
			}

			float c = extract_float_from_str(&lineResult);

			int index = data->texCoordCount*2;
			data->texCoords[index++] = t;
			data->texCoords[index] = c;

			++data->texCoordCount;

			if (!hasTexCoords)
			{
				++attributes;
				hasTexCoords = TRUE;
			}

			// reallocate the tex-coord data since it has
			// grown too large
			if (data->texCoordCount >= maxTex)
			{
				printf("reallocating tex.\n");
				maxTex += BUF_SZ;
				void* resultTexCoords = realloc(data->normals, maxTex*(sizeof(unsigned int)*2));	

				if (!resultTexCoords)
				{
					free_blend(data);
					return NULL;
				}

				data->texCoords = resultTexCoords;
			}
		}
		else if (symbol == NORMAL)
		{
			// extract the normals
			float x = extract_float_from_str(&lineResult);
			while (isdigit(*lineResult) || ispunct(*lineResult))
			{
				++lineResult;
			}

			float y = extract_float_from_str(&lineResult);
			while (isdigit(*lineResult) || ispunct(*lineResult))
			{
				++lineResult;
			}

			float z = extract_float_from_str(&lineResult);

			int index = data->normalCount*3;
			data->normals[index++] = x;
			data->normals[index++] = y;
			data->normals[index] = z;
			
			++data->normalCount;

			if (!hasNormals)
			{
				++attributes;
				hasNormals = TRUE;
			}

			// reallocate the normal data since it has
			// grown too large
			if (data->normalCount >= maxNormal)
			{
				printf("reallocating norm.\n");
				maxNormal += BUF_SZ;
				void* resultNormals = realloc(data->normals, maxNormal*(sizeof(unsigned int)*3));	

				if (!resultNormals)
				{
					free_blend(data);
					return NULL;
				}

				data->normals = resultNormals;
			}
		}
		else if (symbol == SMOOTH)
		{
			if (hasTexCoords)
			{
				data->smooth = TRUE;
			}
		}
		else if (symbol == FACE)
		{
			// we have no attributes... so calculating indices seems futile
			if (attributes <= 0)
			{
				perror("blendload.c: There are no attributes to index.");
				free_blend(data);
				return NULL;
			}
			
			// extract indices 
			// v1/vt1/vn3, ... v99/vt99/vn99 ..., etc.
			char* linePtr = lineBuffer+2;
			unsigned int* indexPtr = data->indices+(data->indexCount*attributes);
			int attribute = 0;
			while (*linePtr)
			{
				// ignores any non entered vertex attributes.
				// ie. if missing texture coordinates, 
				// indices becomes 2 uints instead of 3
				if (isdigit(*linePtr))
				{
					// -1 because blender stores indices 
					//starting from 1 instead of 0
					*indexPtr = strtol(linePtr, NULL, 10) - 1;
					++attribute;
					while (isdigit(*linePtr))
					{
						++linePtr;
					}
					++indexPtr;
				}
				
				// all possible attributes have been read, so reset to be
				// ready for next index
				if (attribute == attributes)
				{
					// reallocate the index data since it has
					// grown too large
					if (data->indexCount >= maxIndex)
					{
						maxIndex += BUF_SZ;
						void* resultIndices = realloc(data->indices, maxIndex*sizeof(unsigned int)*3);

						if (resultIndices == NULL)
						{
							free_blend(data);
							return NULL;
						}

						data->indices = resultIndices;
					}
					
					++data->indexCount;
					attribute = 0;
				}

				++linePtr;
			}
		}
	}

	fclose(file);

	// realloc to be a tightfit (change this later... should just read through and figure out size
	// from the start, in order to only allocate once
	void* resultRealloc = realloc(data->vertices, data->vertexCount*sizeof(float)*3);
	if (data->vertexCount > 0 && !resultRealloc)
	{
		free_blend(data);
		return NULL;
	}
	else {
		data->vertices = resultRealloc;
	}

	resultRealloc = realloc(data->texCoords, data->texCoordCount*sizeof(float)*2);
	if (data->texCoordCount > 0 && !resultRealloc)
	{
		free_blend(data);
		return NULL;
	}
	else {
		data->texCoords = resultRealloc;
	}

	resultRealloc = realloc(data->normals, data->normalCount*sizeof(float)*3);
	if (data->normalCount > 0 && !resultRealloc)
	{
		free_blend(data);
		return NULL;
	}
	else {
		data->normals = resultRealloc;
	}

	resultRealloc = realloc(data->indices, data->indexCount*sizeof(unsigned int)*attributes);
	if (data->indexCount > 0 && !resultRealloc)
	{
		free_blend(data);
		return NULL;
	}
	else {
		data->indices = resultRealloc;
	}

	return data;
}

void free_blend(void* data)
{
	ModelData* md = (ModelData*)data;

	if (md)
	{
		if (md->vertices)
		{
			free(md->vertices);
		}
		if (md->texCoords)
		{
			free(md->texCoords);
		}
		if (md->normals)
		{
			free(md->normals);
		}
		if (md->indices)
		{
			free(md->indices);
		}

		free(md);
	}
}

float extract_float_from_str(char** str)
{
	while (**str && (**str != '+' && **str != '-' && !isdigit(**str)))
	{
		(*str)++;
	}
	float val = strtof(*str, NULL);

	return val;
}

int extract_long_from_str(char** str)
{
	while (**str && (isalpha(**str) || isspace(**str)) ||
		  (!isdigit(**str)))
	{
		(*str)++;
	}
	int val = strtol(*str, NULL, 10);

	return val;
}

void* _alloc_blend()
{
	ModelData* data = calloc(BUF_SZ, sizeof(ModelData));
	if (!data)
	{
		return NULL;
	}

	// allocate vertices
	data->vertices = calloc(BUF_SZ, sizeof(float)*3);
	if (!data->vertices)
	{
		free_blend(data);
		return NULL;
	}

	// allocate texture coordinates
	data->texCoords = calloc(BUF_SZ, sizeof(float)*2);
	if (!data->texCoords)
	{
		free_blend(data);
		return NULL;
	}

	// allocate normals
	data->normals = calloc(BUF_SZ, sizeof(float)*3);
	if (!data->normals)
	{
		free_blend(data);
		return NULL;
	}

	// allocate indices
	data->indices = calloc(BUF_SZ, sizeof(unsigned int)*3);
	if (!data->indices)
	{
		free_blend(data);
		return NULL;
	}

	return data;
}

char _get_symbol(char* symbol)
{
	if (!strcmp(symbol, "# "))
	{
		return COMMENT;
	}
	else if (!strcmp(symbol, "o "))
	{
		return NAME;
	}
	else if (!strcmp(symbol, "v "))
	{
		return VERTEX;
	}
	else if (!strcmp(symbol, "vt"))
	{
		return TEX_COORD;
	}
	else if (!strcmp(symbol, "vn"))
	{
		return NORMAL;
	}
	else if (!strcmp(symbol, "s "))
	{
		return SMOOTH;
	}
	else if (!strcmp(symbol, "f "))
	{
		return FACE;
	}
	else if (!strcmp(symbol, "off"))
	{
		return SMOOTH_OFF;
	}
	else if (!strcmp(symbol, "on"))
	{
		return SMOOTH_ON;
	}

	return 0;
}
