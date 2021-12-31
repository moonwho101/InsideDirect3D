//-----------------------------------------------------------------------------
// File: info.cpp
//
// Desc: Code for handling output of program information to the user
//
// Copyright (c) 1999 William Chin and Peter Kovach. All rights reserved.
//-----------------------------------------------------------------------------
#include "resource.h"
#include <ddraw.h>
#include "d3dframe.h"
#include "D3DApp.h"
#include "roadrage.hpp"

extern CMyD3DApplication* pCMyApp;
extern char D3Ddevicename[256];

void CMyD3DApplication::DisplayCredits(HWND hwnd)
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

	strcpy(buffer, "Head Tapper Software");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	strcpy(buffer, "http://www.headtappersoftware.com");
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy +=20;

	strcpy(buffer, "Email us at:");
	TextOut(hdc, sx1, sy, buffer, strlen(buffer));
	strcpy(buffer, "roadrage@headtappersoftware.com");
	TextOut(hdc, sx2, sy, buffer, strlen(buffer));
	sy +=20;

	SetBkMode(hdc, OPAQUE);
	ReleaseDC(hwnd,hdc);  

}


void CMyD3DApplication::DisplayRRStats(HWND hwnd)
{
	HDC hdc;
	int mem;	
	int sx1 = 15, sx2 = 320, sy = 70;
	int nIndex;
	char  buffer[255];
	char  buf2[255];
	LPTSTR buffer2;
	LPDIRECTDRAW7 lpDD7;
	DDSCAPS2    ddsCaps2; 
	DWORD		total_video_memory;
	DWORD		free_video_memory;
	HRESULT     hr; 
	MEMORYSTATUS memStatus;
	HBRUSH holdbrush_color;

	hdc=GetDC(hwnd);

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
			
 
	hr = GetFramework()->GetDirectDraw()->QueryInterface(IID_IDirectDraw7, (void **)&lpDD7); 
	if (FAILED(hr))
		return; 
 
	// Initialize the structure.
	ZeroMemory(&ddsCaps2, sizeof(ddsCaps2));
  
	ddsCaps2.dwCaps = DDSCAPS_VIDEOMEMORY  ; 
	hr = GetFramework()->GetDirectDraw()->GetAvailableVidMem(&ddsCaps2, &total_video_memory, &free_video_memory); 
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

	TextOut(hdc, sx1, sy, "D3D Device Name :",17);
	TextOut(hdc, sx2, sy, D3Ddevicename, strlen(D3Ddevicename));
	sy +=20;
			
	SetBkMode(hdc, OPAQUE);
	ReleaseDC(hwnd,hdc);  
}


void CMyD3DApplication::DisplayLegalInfo(HWND hwnd)
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
	strcpy(buffer, "are to be used for your own personal use only.");
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



BOOL FAR PASCAL AppAbout(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_INITDIALOG:
			SetTimer(hwnd,1,100,NULL);
			break;

		case WM_LBUTTONDOWN:
			break;
			
		case WM_RBUTTONDOWN:
			break;

		case WM_TIMER:
			KillTimer(hwnd,1);
			pCMyApp->DisplayCredits(hwnd);
			break;

		case WM_COMMAND:
			switch(LOWORD(wParam)) 
			{
				case IDOK:
				case IDCANCEL:      
					EndDialog(hwnd, TRUE);
					break;

				case IDC_BUTTON1:
					pCMyApp->DisplayCredits(hwnd);
					break;
				
				case IDC_BUTTON2:
					pCMyApp->DisplayRRStats(hwnd);
					break;

				case IDC_BUTTON4:
					pCMyApp->DisplayLegalInfo(hwnd);	
					break;
			break;
			}
	break;
		
	}

	return FALSE;
}

 
