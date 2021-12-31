
// Road Rage version 1.1

// Written by William Chin
// Developed in MSDEV C++ v6.0 using the DirectX 6.0 SDK
// Date : 3/7/99
// File : info.cpp


#include "info.hpp"

extern D3DAppInfo* d3dapp;	 
extern WORLD world;
extern setupinfo_ptr setupinfo;
//extern char D3Ddevicename[256];
//extern char D3Ddevicedesc[256];



void DisplayCredits(HWND hwnd)
{
	HDC hdc;
	int sx1 = 15, sx2 = 200, sy = 70;
	int nIndex;
	char  buffer[255];
	HBRUSH holdbrush_color;
	

	
	hdc=GetDC(hwnd);

	nIndex = COLOR_ACTIVEBORDER;
	holdbrush_color = (HBRUSH)SelectObject(hdc, GetSysColorBrush(nIndex)); 
	Rectangle(hdc,10,58,512,360);

	SetBkMode(hdc, TRANSPARENT);
			
	
	strcpy(buffer, "Credits for Road Rage :");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	sy +=40;

	strcpy(buffer, "William Chin");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	strcpy(buffer, "Project Leader / C++ Programmer");
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy +=20;
	
	strcpy(buffer, "Peter Kovach");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	strcpy(buffer, "C++ Programmer");
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy +=20;

	strcpy(buffer, "Mark Bracey");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	strcpy(buffer, "3DS Modeller / Mapper ");
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy +=20;

	strcpy(buffer, "NEO XS");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	strcpy(buffer, "MD2 Modeller");
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy +=20;

	strcpy(buffer, "James Glendenning");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	strcpy(buffer, "3DS Modeller");
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy +=20;

	strcpy(buffer, "Adam Bardsley");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	strcpy(buffer, "Artist / 3DS Modeller");
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy +=20;

	strcpy(buffer, "Corréia Emmanuel");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	strcpy(buffer, "Artist - textures, website :");
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy +=20;

	strcpy(buffer, "    Axem Textues");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	strcpy(buffer, "http://axem2.simplenet.com");
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy +=40;

	strcpy(buffer, "Further Info on Road Rage :");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	sy +=20;

	strcpy(buffer, "Bill's DirectX Page");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	strcpy(buffer, "http://members.aol.com/billybop7");
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy +=20;

	strcpy(buffer, "Email me at:");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	strcpy(buffer, "billybop7@aol.com");
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy +=20;

	SetBkMode(hdc, OPAQUE);
	ReleaseDC(hwnd,hdc);  

}

void DisplayControls(HWND hwnd)
{
	HDC hdc;
	int sx1 = 15, sx2 = 200, sy = 70;
	int nIndex;
	char  buffer[255];
	HBRUSH holdbrush_color;
	

	
	hdc=GetDC(hwnd);

	nIndex = COLOR_ACTIVEBORDER;
	holdbrush_color = (HBRUSH)SelectObject(hdc, GetSysColorBrush(nIndex)); 
	Rectangle(hdc,10,58,512,360);

	SetBkMode(hdc, TRANSPARENT);
			
	
	strcpy(buffer, "Controls for Road Rage :");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	sy +=40;

	strcpy(buffer, "Keypad Up Arrow");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	strcpy(buffer, "Forward / accelerate");
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy +=20;
	
	strcpy(buffer, "Keypad Down Arrow");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	strcpy(buffer, "Backward / brake");
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy +=20;

	strcpy(buffer, "Keypad Left Arrow");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	strcpy(buffer, "Rotate left / steer left ");
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy +=20;

	strcpy(buffer, "Keypad Right Arrow");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	strcpy(buffer, "Rotate right / right left ");
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy +=20;

	strcpy(buffer, "Spacebar");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	strcpy(buffer, "Enter / exit car");
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy +=20;

	strcpy(buffer, "Mouse 1");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	strcpy(buffer, "Fire");
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy +=20;

	strcpy(buffer, "Keypad + / -");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	strcpy(buffer, "Change weapon up / down");
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy +=20;

	strcpy(buffer, "Keypad 9 / 7");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	strcpy(buffer, "Change gear up / down");
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy +=20;

	strcpy(buffer, "F12");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	strcpy(buffer, "Take screen shot");
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy +=20;

	strcpy(buffer, "TAB");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	strcpy(buffer, "FPS counter / network stats");
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy +=20;

	SetBkMode(hdc, OPAQUE);
	ReleaseDC(hwnd,hdc);  
}

void DisplayRRStats(HWND hwnd)
{
	HDC hdc;
	int mem;	
	int sx1 = 15, sx2 = 320, sy = 70;
	int nIndex;
	char  buffer[255];
	char  buf2[255];//, buf3[255];
	LPTSTR buffer2;
	DDSCAPS    ddsCaps2; 
	DWORD       total_memory_for_textures; 
	DWORD       free_memory_for_textures;
	DWORD		total_video_memory;
	DWORD		free_video_memory;
	HRESULT     hr; 
	MEMORYSTATUS memStatus;
	HBRUSH holdbrush_color;
	LPDIRECTDRAW2 lpDD2;


	hdc = GetDC(hwnd);

	nIndex = COLOR_ACTIVEBORDER;

	holdbrush_color = (HBRUSH)SelectObject(hdc, GetSysColorBrush(nIndex)); 
	Rectangle(hdc,10,58,512,360);

	SetBkMode(hdc, TRANSPARENT);
	memStatus.dwLength=sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&memStatus);
		
	mem=(int)memStatus.dwMemoryLoad;
	itoa(mem,buffer,10);
	buffer2 = lstrcat(buffer, " %");
	strcpy(buf2, "System memory in use :");
	TextOut(hdc, sx1, sy, buf2,    strlen(buf2));
	TextOut(hdc, sx2, sy, buffer2, strlen(buffer2));
	sy +=20;

	mem=(int)memStatus.dwTotalPhys;
	mem = mem /1024;
	itoa(mem, buffer,10);
	strcat(buffer, " KB");
	strcpy(buf2, "Total system memory :");
	TextOut(hdc, sx1, sy, buf2,   strlen(buf2));  
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy +=20;
			
 
	hr = d3dappi.lpDD->QueryInterface(IID_IDirectDraw2, (void **)&lpDD2);

	if (FAILED(hr))
		return; 
 
	// Initialize the structure.
	ZeroMemory(&ddsCaps2, sizeof(ddsCaps2));
 
	ddsCaps2.dwCaps = DDSCAPS_TEXTURE; 
	hr =lpDD2->GetAvailableVidMem(&ddsCaps2, &total_memory_for_textures, &free_memory_for_textures); 
	if (FAILED(hr))
		return;
 
	ZeroMemory(&ddsCaps2, sizeof(ddsCaps2));
 
	ddsCaps2.dwCaps = DDSCAPS_VIDEOMEMORY  ; 
	hr = lpDD2->GetAvailableVidMem(&ddsCaps2, &total_video_memory, &free_video_memory); 
	if (FAILED(hr))
		return;

	mem= total_video_memory;
	mem = mem /1024;
	itoa(mem,buffer,10);
	strcat(buffer, " KB");
	strcpy(buf2, "Total video memory :");
	TextOut(hdc, sx1, sy, buf2,   strlen(buf2));
	TextOut(hdc, sx2, sy, buffer, strlen(buffer)); 
	sy +=20;

	mem= free_video_memory;
	mem = mem /1024;
	itoa(mem,buffer,10);
	strcat(buffer, " KB");
	strcpy(buf2, "Free video memory :");
	TextOut(hdc, sx1, sy, buf2,   strlen(buf2));
	TextOut(hdc, sx2, sy, buffer, strlen(buffer)); 
	sy +=20;

	mem= free_memory_for_textures;
	mem = mem /1024;
	itoa(mem,buffer,10);
	strcat(buffer, " KB");
	strcpy(buf2, "Free video & system memory for textures :");
	TextOut(hdc, sx1, sy, buf2,   strlen(buf2));
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy +=20;

	mem= total_memory_for_textures;
	mem = mem /1024;
	itoa(mem,buffer,10);
	strcat(buffer, " KB");
	strcpy(buf2, "Total video & system memory for textures :");
	TextOut(hdc, sx1, sy, buf2,   strlen(buf2));
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy +=20;

	wsprintf(buffer, "%d%d%d",
	d3dapp->ThisTextureFormat.RedBPP,
	d3dapp->ThisTextureFormat.GreenBPP,
	d3dapp->ThisTextureFormat.BlueBPP);
	
	if(d3dapp->ThisTextureFormat.bPalettized == TRUE)
		strcat(buffer," Palettised");
	
	SetBkMode(hdc, OPAQUE);
	ReleaseDC(hwnd,hdc);  
}


void DisplayLegalInfo(HWND hwnd)
{
	HDC hdc;
	int sx1 = 15, sx2 = 200, sy = 70;
	int nIndex;
	char  buffer[255];
	HBRUSH holdbrush_color;
	

	
	hdc=GetDC(hwnd);

	nIndex = COLOR_ACTIVEBORDER;
	holdbrush_color = (HBRUSH)SelectObject(hdc, GetSysColorBrush(nIndex)); 
	Rectangle(hdc,10,58,512,360);

	SetBkMode(hdc, TRANSPARENT);
			
	
	strcpy(buffer, "Legal info for Road Rage :");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	sy +=40;

	strcpy(buffer, "The code, 3D models, artwork and sounds in Road Rage,");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	sy +=20;
	strcpy(buffer, "are to be used, for your own personal use only.");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	sy +=40;

	strcpy(buffer, "Anyone wishing to use any part of Road Rage for commercial purposes");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	sy +=20;
	strcpy(buffer, "must have the writers / artists written permission first.");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	sy +=40;

	strcpy(buffer, "Copyright (C) 1999");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	sy +=20;

	SetBkMode(hdc, OPAQUE);
	ReleaseDC(hwnd,hdc);  

}


 
