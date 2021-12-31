/*==========================================================================
 *
 *  Copyright (C) 1995, 1996 Microsoft Corporation. All Rights Reserved.
 *
 *  File: d3ddemo.h
 *
 ***************************************************************************/
#ifndef __D3DDEMO_H__
#define __D3DDEMO_H__

#include <ddraw.h>
#include <d3d.h>
#include "d3dapp.hpp"
#include "d3dmath.hpp"
#include "d3dsphr.hpp"
#include "d3dmacs.hpp"

#include "3Dengine.hpp"
//extern "C" {
    /*
     * SetMouseCallback
     * Called in an example to set a callback function for all WM messages
     * dealing with the mouse.  The handler should return whether or not
     * it handled the message.
     */
    BOOL SetMouseCallback(BOOL(*)(UINT, WPARAM, LPARAM));
    /*
     * SetKeyDownCallback
     * Called in an example to set a callback function for keyboard
     * messages. The handler should return whether or not it handled the
     * message.
     */
    BOOL SetKeyboardCallback(BOOL(*)(UINT, WPARAM, LPARAM));

/*
 * These functions are found in the source for each example.
 */
    /*
     * RenderScene
     * Clears the viewport and viewport Z-buffer and render the scene.
     * The extent of rendering is returned in the rectangle.
     */
    BOOL RenderScene(LPDIRECT3DDEVICE7 lpDev,
		     LPD3DRECT lpExtent,CAMERA cam);
    /*
     * InitScene
     * Builds the model which will be rendered.
     */
    BOOL InitScene(void);

    /*
     * InitView
     * Builds execute buffer and all components needed to be added to viewport.
     */
    BOOL InitView(LPDIRECTDRAW7 lpDD,
		  LPDIRECT3D7 lpD3D,
		  LPDIRECT3DDEVICE7 lpDev,
		  int NumTextures);
    /*
     * Release all objects and free all memory allocated in InitScene
     */
    void ReleaseScene(void);
	
    /*
     * Release all objects and free all memory allocated in InitView
     */
    void ReleaseView();

    typedef struct Defaultstag {
	D3DAppRenderState rs;
	BOOL bTexturesDisabled;
	BOOL bResizingDisabled;
	BOOL bClearsOn;
	char Name[30];
    } Defaults;

    /*
     * Allows each example to begin with different defaults
     */
    void OverrideDefaults(Defaults* defaults);

//};

#endif // __D3DDEMO_H__
