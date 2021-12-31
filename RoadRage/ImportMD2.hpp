
// Road Rage version 1.1

// Written by William Chin
// Developed in MSDEV C++ v6.0 using the DirectX 6.0 SDK
// Date : 3/7/99
// File : ImportMD2.hpp

 
#ifndef __IMPORTMD2_H
#define __IMPORTMD2_H

#include <windows.h>
#include "world.hpp"


typedef struct
{
	short s;
	short t;

} MD2TEXVERT;


typedef struct 
{
	short face_index[3];
	short tex_index[3];

} MD2FACE; 


typedef struct
{
	BYTE v[3]; 
	BYTE lightnormalindex;

} MD2VERTEX; 


typedef struct
{
	float s;
	float t;
	short index;

} MD2VERTINDEX;


typedef struct
{
	int ident;
	int version;

	int skinwidth;
	int skinheight;
	int framesize;	

	int num_skins;
	int num_verts;
	int num_tex_verts;	
	int num_faces;
	int num_glcmds; 
	int num_frames;

	int offset_skins;	
	int offset_tex_verts;		
	int offset_faces;	
	int offset_frames; 
	int offset_glcmds; 
	int offset_end;	

} MD2HEADER;

typedef struct
{		
	MD2VERTINDEX vert_index[3];
	
} FACE;


typedef struct
{				
	char framename[16];		
	VERT v[600];
} FRAME;





#ifdef __cplusplus
extern "C" {
#endif

BOOL ImportMD2      (HWND hwnd, char *filename, int texture_alias, int pmodel_id, world_ptr wptr);
BOOL ImportMD2_GLCMD(HWND hwnd, char *filename, int texture_alias, int pmodel_id, float scale, world_ptr wptr);
#ifdef __cplusplus
};
#endif


#endif // IMPORTMD2_H