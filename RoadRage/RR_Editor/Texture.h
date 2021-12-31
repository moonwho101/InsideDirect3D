
#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include "world.h"

#define LOAD_ALL		0
#define MAPPING_ONLY	1 	
	
#ifdef __cplusplus
extern "C" {
#endif


BOOL LoadTextures(HWND hwnd, BOOL mode, world_ptr wptr);
int CheckValidTextureAlias(HWND hwnd, char *alias, world_ptr wptr);

#ifdef __cplusplus
};
#endif

#endif // __TEXTURE_H__


