/*
 *  Copyright (C) 1995, 1996 Microsoft Corporation. All Rights Reserved.
 *
 *  File: d3dmain.h
 *
 */
#ifndef __D3DMAIN_H__
#define __D3DMAIN_H__

#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h> //extra
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <search.h>
#include <ddraw.h>
#include <d3d.h>
#include "d3dapp.hpp"	  // prototypes for D3D helper functions 
#include "d3dres.h"	  // defines constants used in d3dmain.rc 
#include "d3ddemo.hpp"  // prototypes for functions to communicate with each sample 


#define START_WIN_SIZE 320  /* initial size of the window */
#define KEY_DELAY 500
#define MAX_NUM_WEAPONS 62

#define USE_PRIMARY_D3DCARD 0
#define USE_SECONDARY_D3DCARD 1

//extern "C" {

typedef struct tagd3dmainglobals 
{
    HWND hWndMain;	    /* application window handle */
    HINSTANCE hInstApp;	    /* application instance for dialog boxes */

    D3DAppRenderState rstate; /* struct describing the current render state */

    BOOL bSingleStepMode;	 /* render one frame at a time */
    BOOL bDrawAFrame;		 /* render on this pass of the main loop */
    BOOL bClearsOn;		 /* clear the back and z-buffer each frame */
    BOOL bShowFrameRate;	 /* show the frame rate at the top */
    BOOL bShowInfo;		 /* show window information at the bottom */
    BOOL bResizingDisabled;	 /* do not allow resizing */
	
    BOOL bResized; /* the window has resized or some other drastic change, the
                      entire client area should be cleared */
    BOOL bQuit;	   /* program is about to terminate */

    LPDIRECTDRAWSURFACE7 lpFrameRateBuffer; /* frame rate surface */
    LPDIRECTDRAWSURFACE7 lpInfoBuffer;	   /* window info surface */
} d3dmainglobals;

void __cdecl Msg( LPSTR fmt, ... );
/*
 * STATS.CPP FUNCTION PROTOTYPES
 */
BOOL InitFontAndTextBuffers(void);
void ReleaseFontAndTextBuffers(void);
BOOL WriteInfoBuffer(void);
BOOL WriteFrameRateBuffer(float fps, int rx, int tx);
void ResetFrameRate(void);
BOOL CalculateFrameRate(void);
BOOL DisplayFrameRate(int* count, LPD3DRECT lpExtents );
void Render3DView(HWND hwnd);
BOOL DDTextOut(char *msg);
//};

DEFINE_GUID( ZBUFFER_GUID, 0x5bfdb062, 0x6a4, 0x11d0, 0x9c, 0x4f, 0x0, 0xa0, 0xc9, 0x5, 0x42, 0x5e);

#endif // __D3DMAIN_H__
