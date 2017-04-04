#pragma once

// TODO: handle materials and mtlib (and miscellaneous tags)
// TODO: change from realloc, to reading first, then allocating from the start

// A simple model loader for blender.

// **you do not need to include this header directly.
// instead use extern**

#define FALSE 0
#define TRUE 1

#define BUF_SZ 10240

enum {
	COMMENT='#', NAME='o', INDEX_SPLIT='/', VERTEX='v', 
	NORMAL='n', TEX_COORD='t', SMOOTH='s', FACE='f',
	SMOOTH_OFF=0, SMOOTH_ON=1
};

/*----------------------------------*
* LOADS A BLENDER .OBJ FORMAT MODEL
* ----------------------------------*
* Texture Formats: 0=normalized float, 
*				   1=unsigned short
*/

// **main functions for loading blender model data.**

// Call free_blend when done with the data.
// (vertices, tcoords, normals, indices)
void* load_blend(const char* filename);
// expects the original blender vertices returned by loadBlend,
// in the same format
void free_blend(void* data);

// **utility... may use elsewhere**

float extract_float_from_str(char** str);
int extract_long_from_str(char** str);
