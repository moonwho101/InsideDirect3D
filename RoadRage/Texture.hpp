
// Road Rage 1.1

// FILE : texture.hpp
// This file was originally part of the Microsoft DirectX5 SDK
// Edited by William Chin
// Edited with MSDEV C++ v6.0 using the DirectX 6.0 SDK
// Edited on:  3/7/99


#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include "world.hpp"

#define LOAD_ALL		0
#define MAPPING_ONLY	1 	
	

BOOL LoadTextures(HWND hwnd, char *filename, world_ptr wptr);
BOOL SetTextureSize();
int CheckValidTextureAlias(HWND hwnd, char *alias, world_ptr wptr);



#endif // __TEXTURE_H__


