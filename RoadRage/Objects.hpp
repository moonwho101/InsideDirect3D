
#ifndef __OBJECTS_H__
#define __OBJECTS_H__

#include <windows.h>
#include <windowsx.h>

typedef struct vert_typ
{
	float x;
	float y;
	float z;

} VERT,*vert_ptr;


typedef struct object_typ
{
	VERT v[1000]; 
	int tex[1000]; 

} OBJECT,*object_ptr;

#endif // __3DENGINE_H__
