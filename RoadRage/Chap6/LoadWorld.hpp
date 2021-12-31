
#ifndef __LOADWORLD_H
#define __LOADWORLD_H

#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <windows.h>
#include "D3DApp.h"
#include "Roadrage.hpp"

class CLoadWorld //: public CMyD3DApplication
{

public:

	CLoadWorld();

	BOOL LoadWorldMap(HWND hwnd,char *filename);
	int  CheckObjectId(HWND hwnd, char *p); 
	BOOL InitPreCompiledWorldMap(HWND hwnd, char *filename);
	BOOL LoadObjectData(HWND hwnd, char *filename);
	BOOL ReadObDataVert(FILE *fp, int object_id, int vert_count, float dat_scale);
	BOOL ReadObDataVertEx(FILE *fp, int object_id, int vert_count, float dat_scale);
	BOOL LoadImportedModelList(HWND hwnd,char *filename);
	void LoadYourGunAnimationSequenceList(int model_id);	
	void LoadPlayerAnimationSequenceList(int model_id);
	void LoadDebugAnimationSequenceList(HWND hwnd, char *filename, int model_id);	
};

#endif //__LOADWORLD_H