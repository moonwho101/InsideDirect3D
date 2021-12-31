
// Road Rage version 1.1

// Written by William Chin
// Developed in MSDEV C++ v6.0 using the DirectX 6.0 SDK
// Date : 3/7/99
// File : Import3DS.hpp


#ifndef __IMPORT3DS_H
#define __IMPORT3DS_H

#include <windows.h>
#include "world.hpp"

#ifdef __cplusplus
extern "C" {
#endif

BOOL Import3DS(HWND hwnd, char *filename, int pmodel_id, float scale, world_ptr wptr);

#ifdef __cplusplus
};
#endif


#endif //__IMPORT3DS_H