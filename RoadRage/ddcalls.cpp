
// Road Rage 1.1

// FILE : ddcalls.cpp
// This file was originally part of the Microsoft DirectX5 SDK
// Edited by William Chin
// Edited with MSDEV C++ v6.0 using the DirectX 6.0 SDK
// Edited on:  3/7/99


#include "d3dappi.hpp"
#include "d3dmain.hpp"


extern d3dmainglobals myglobs;

extern char D3Ddevicename[256];	
extern char D3Ddevicedesc[256];	 
extern char FindD3Ddevicename[256];	 
extern char D3Ddevicelist[10][256];
extern int numD3Ddevices;

extern BOOL App_starting_for_first_time;
extern BOOL d3dcard_driver_flag;

char DisplayDevice[256];



/***************************************************************************/
/*                         Direct Draw Creation                            */
/***************************************************************************/
/*
 * D3DAppIDDEnumCallback
 * Callback function used during enumeration of DirectDraw drivers.
 * During enumeration, if a 3D capable hardware device is found, it is 
 * created and *(LPDIRECTDRAW*)lpContext is set to it.  Otherwise, does
 * nothing.
 */

BOOL FAR PASCAL D3DAppIDDEnumCallback(GUID FAR* lpGUID, LPSTR lpDriverDesc,
				      LPSTR lpDriverName, LPVOID lpContext, HMONITOR hm)
{
    LPDIRECTDRAW7 lpDD;
    //DDCAPS DriverCaps, HELCaps;
	FILE *fp;
	HRESULT LastError;
	

	fp = fopen("rrlogfile.txt","a");
	
	fprintf( fp, "Enumerating 3D Cards, found :\n");
	fprintf( fp, "D3D Driver Description = %s\n", lpDriverDesc);
	fprintf( fp, "D3D Driver Name = %s\n\n", lpDriverName);
	//fprintf( fp, "lpContext    = %s\n", lpContext);
	//fprintf( fp, "FindD3Ddevicename = %s\n", FindD3Ddevicename);
	//fprintf( fp, "\n");
	
	strcpy(D3Ddevicelist[numD3Ddevices],lpDriverName);
	numD3Ddevices ++;


	if (App_starting_for_first_time == TRUE)
	{
	
		if( d3dcard_driver_flag == USE_PRIMARY_D3DCARD )
		{
			if( lstrcmpi("primary", lpDriverName) != 0 )
			{
				fclose(fp);
				return DDENUMRET_OK;
			}
		}

		if(d3dcard_driver_flag == USE_SECONDARY_D3DCARD)
		{
			if(numD3Ddevices != 2)
			{
				fclose(fp);
				return DDENUMRET_OK;
			}
		}
		
	}
	else if (lstrcmpi(FindD3Ddevicename, lpDriverName) != 0)
	{
		fclose(fp);
		return DDENUMRET_OK;
	}
	
	fprintf( fp, "Using this 3d card :\n");
	fprintf( fp, "D3D Driver Description = %s\n", lpDriverDesc);
	fprintf( fp, "D3D Driver Name        = %s\n\n", lpDriverName);

	LastError = DirectDrawCreateEx(lpGUID, (LPVOID*)&lpDD, IID_IDirectDraw7, NULL);

	fprintf( fp, "DirectDrawCreateEx suceeded:\n");
		
	if(LastError != DD_OK)
	{
		fprintf( fp, "DirectDrawCreateEx failed\n");
		MessageBox(myglobs.hWndMain, "DD Error", "DirectDrawCreateEx", MB_OK);		
		fclose(fp);
		return DDENUMRET_OK;
	}
		
	strcpy(D3Ddevicename, lpDriverName);
	strcpy(D3Ddevicedesc, lpDriverDesc);
	//fprintf( fp, "Using %s\n\n", lpDriverName);
		
	if(lstrcmpi("display", lpDriverName) == 0)
		d3dappi.bIsPrimary = TRUE;
	else
		d3dappi.bIsPrimary = FALSE;

	*(LPDIRECTDRAW7*)lpContext = lpDD;
	fclose(fp);
    return DDENUMRET_CANCEL;	
}

/*
 * D3DAppICreateDD
 * Creates the DirectDraw device and saves the current palette. If a 3D 
 * capable DD driver is available, use it as the DD device, otherwise, use
 * the HEL.  It is assumed that a 3D capable DD hardware driver is not the
 * primary device and hence cannot operate in a window (ie it's a fullscreen
 * only device displaying on a second monitor).  Valid flags:
 *     D3DAPP_ONLYDDEMULATION    Always use the DirectDraw HEL
 */

BOOL D3DAppICreateDD(DWORD flags)
{
    HDC hdc;
    int i;
    LPDIRECTDRAW7 lpDD = NULL;

	
	PrintMessage(myglobs.hWndMain, "D3DAppICreateDD: starting", NULL, LOGFILE_ONLY);

    // If we aren't forced to use the DirectDraw HEL, search for a 3D capable
    // DirectDraw hardware driver and create it.
     
    if (!(flags & D3DAPP_ONLYDDEMULATION)) 
	{
		numD3Ddevices = 0;
		LastError = DirectDrawEnumerateEx(D3DAppIDDEnumCallback, &lpDD, DDENUM_ATTACHEDSECONDARYDEVICES );

		if (LastError != DD_OK) 
		{
			D3DAppISetErrorString("D3DAppICreateDD: DirectDrawEnumerateEx failed.\n%s",
				  D3DAppErrorToString(LastError));
			return FALSE;
		}
	}

    if (lpDD == NULL) 
	{	
		// If we haven't created a hardware DD device by now, resort to HEL
	 
		d3dappi.bIsPrimary = TRUE;
		LastError = DirectDrawCreateEx(NULL, (LPVOID*)&lpDD, IID_IDirectDraw7, NULL);
	
		if (LastError != DD_OK) 
		{
			D3DAppISetErrorString("D3DAppICreateDD: DirectDrawCreateEx failed.\n%s",
				  D3DAppErrorToString(LastError));
			return FALSE;
		}
    } 
	
		LastError = lpDD->QueryInterface( IID_IDirectDraw7, 
			(LPVOID*)&d3dappi.lpDD7);

		if (LastError != DD_OK) 
		{
			lpDD->Release();
			D3DAppISetErrorString("D3DAppICreateDD: Couldn't query for DirectDraw7.\n%s",
				  D3DAppErrorToString(LastError));
			return FALSE;
		}

		if(d3dappi.lpDD7 == 0)
		{
			D3DAppISetErrorString("D3DAppICreateDD: d3dappi.lpDD7 = NULL.\n%s",
				  D3DAppErrorToString(LastError));
			return FALSE;
		}
    
    
    // Save the original palette for when we are paused.  Just in case we
    // start in a fullscreen mode, put them in ppe.
     
    hdc = GetDC(NULL);
    GetSystemPaletteEntries(hdc, 0, (1 << 8),
			    (LPPALETTEENTRY)(&Originalppe[0]));
    
	for (i = 0; i < 256; i++)
        ppe[i] = Originalppe[i];

    ReleaseDC(NULL, hdc);
	PrintMessage(myglobs.hWndMain, "D3DAppICreateDD: suceeded\n", NULL, LOGFILE_ONLY);
    return TRUE;
}

/***************************************************************************/
/*                   Enumerating the display modes                         */
/***************************************************************************/
/*
 * EnumDisplayModesCallback
 * Callback to save the display mode information.
 */
static HRESULT CALLBACK EnumDisplayModesCallback(LPDDSURFACEDESC2 pddsd, LPVOID lpContext)
{
    //if (pddsd->dwWidth > 2000 || pddsd->dwHeight > 2000)
	//return DDENUMRET_OK;
    
    // Save this mode at the end of the mode array and increment mode count
     
    d3dappi.Mode[d3dappi.NumModes].w = pddsd->dwWidth;
    d3dappi.Mode[d3dappi.NumModes].h = pddsd->dwHeight;
    d3dappi.Mode[d3dappi.NumModes].bpp = pddsd->ddpfPixelFormat.dwRGBBitCount;
    d3dappi.Mode[d3dappi.NumModes].bThisDriverCanDo = FALSE;
    d3dappi.NumModes++;
    if (d3dappi.NumModes == D3DAPP_MAXMODES)
	return DDENUMRET_CANCEL;
    else
	return DDENUMRET_OK;
}

/*
 * CompareModes
 * Compare two display modes during sorting.  Modes are sorted by depth and
 * then resolution.
 */
static int
CompareModes(const void* element1, const void* element2)
{
    D3DAppMode *lpMode1, *lpMode2;

    lpMode1 = (D3DAppMode*)element1;
    lpMode2 = (D3DAppMode*)element2;

    if (lpMode1->bpp > lpMode2->bpp)
        return -1;
    else if (lpMode2->bpp > lpMode1->bpp)
        return 1;
    else if (lpMode1->w > lpMode2->w)
        return -1;
    else if (lpMode2->w > lpMode1->w)
        return 1;
    else if (lpMode1->h > lpMode2->h)
        return -1;
    else if (lpMode2->h > lpMode1->h)
        return 1;
    else
        return 0;
}


 // EnumerateDisplayModes
 // Generates the list of available display modes.

BOOL D3DAppIEnumDisplayModes(void)
{
    int i;
    
    // Get a list of available display modes from DirectDraw
    
    d3dappi.NumModes = 0;
    LastError = d3dappi.lpDD7->EnumDisplayModes(0, NULL,
						0, EnumDisplayModesCallback);

    if(LastError != DD_OK ) 
	{
        D3DAppISetErrorString("EnumDisplayModes failed.\n%s",
			      D3DAppErrorToString(LastError));
		d3dappi.NumModes = 0;
        return FALSE;
    }
    
    // Sort the list of display modes
     
    qsort((void *)&d3dappi.Mode[0], (size_t)d3dappi.NumModes, sizeof(D3DAppMode),
          CompareModes);
    /*
     * Pick a default display mode.  640x480x16 is a very good mode for
     * rendering, so choose it over all others.  Otherwise, just take the
     * first one.  This selection may be overriden later if a driver is
     * created which cannot render in this mode.
     */
    d3dappi.CurrMode = 0;
    for (i = 0; i < d3dappi.NumModes; i++) 
	{
		if (d3dappi.Mode[i].w == 640 && d3dappi.Mode[i].h == 480 &&
			d3dappi.Mode[i].bpp == 16)
	    d3dappi.CurrMode = i;
    }
    memcpy(&d3dappi.ThisMode, &d3dappi.Mode[d3dappi.CurrMode],
	   sizeof(D3DAppMode));
    return TRUE;
}

/***************************************************************************/
/*               Creating Front and Back Buffers (and misc surf funcs)     */
/***************************************************************************/
/*
 * D3DAppICreateSurface
 * Create a DirectDraw Surface of the given description.  Using this function
 * ensures that all surfaces end up in system memory if that option was set.
 * Returns the result of the CreateSurface call.
 */

HRESULT D3DAppICreateSurface(LPDDSURFACEDESC2 lpDDSurfDesc,
                LPDIRECTDRAWSURFACE7 FAR *lpDDSurface) 
{
    HRESULT result;


    if (d3dappi.bOnlySystemMemory)
        lpDDSurfDesc->ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;

    result = d3dappi.lpDD7->CreateSurface(lpDDSurfDesc,
						 lpDDSurface, NULL);
    return result;
}

/*
 * D3DAppIGetSurfDesc
 * Get the description of the given surface.  Returns the result of the
 * GetSurfaceDesc call.
 */
HRESULT
D3DAppIGetSurfDesc(LPDDSURFACEDESC2 lpDDSurfDesc,LPDIRECTDRAWSURFACE7 lpDDSurf)
{
    HRESULT result;
    memset(lpDDSurfDesc, 0, sizeof(DDSURFACEDESC2));
    lpDDSurfDesc->dwSize = sizeof(DDSURFACEDESC2);
    result = lpDDSurf->GetSurfaceDesc(lpDDSurfDesc);
    return result;
}


 // D3DAppICreateBuffers
 // Creates the front and back buffers for the window or fullscreen case
 // depending on the bFullscreen flag.  In the window case, bpp is ignored.
 
BOOL D3DAppICreateBuffers(HWND hwnd, int w, int h, int bpp, BOOL bFullscreen, BOOL bIsHardware)
{
    DDSURFACEDESC2 ddsd;
    DDSCAPS2 ddscaps;
	FILE *fp;

    
    // Release any old objects that might be lying around.  This should have
    // already been taken care of, but just in case...
     
    RELEASE(lpClipper);
	lpClipper = NULL;
    RELEASE(d3dappi.lpBackBuffer);
	d3dappi.lpBackBuffer = NULL;
    RELEASE(d3dappi.lpFrontBuffer);
	d3dappi.lpFrontBuffer = NULL;
    
    // The size of the buffers is going to be w x h, so record it now
     
    if (w < D3DAPP_WINDOWMINIMUM)
		w = D3DAPP_WINDOWMINIMUM;
    if (h < D3DAPP_WINDOWMINIMUM)
		h = D3DAPP_WINDOWMINIMUM;
    szBuffers.cx = w;
    szBuffers.cy = h;

	fp = fopen("rrlogfile.txt","a");
	
	fprintf( fp, "D3DAppICreateBuffers : Creating front and back buffers\n");
	
	

    if (bFullscreen) 
	{
        fprintf( fp, "D3DAppICreateBuffers : Fullscreen mode\n");
	
        // Create a complex flipping surface for fullscreen mode with one
		// back buffer.
        
        memset(&ddsd,0,sizeof(DDSURFACEDESC2));
		ddsd.dwSize = sizeof( ddsd );
    	ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
    	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP |
    	    DDSCAPS_3DDEVICE | DDSCAPS_COMPLEX;

    	ddsd.dwBackBufferCount = 1;
		
		if (bIsHardware)
			ddsd.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
		   
		LastError = D3DAppICreateSurface(&ddsd, &d3dappi.lpFrontBuffer);
    	
		if(LastError != DD_OK) 
		{
			if (LastError == DDERR_OUTOFMEMORY || LastError == DDERR_OUTOFVIDEOMEMORY) 
			{
				D3DAppISetErrorString("There was not enough video memory to create the rendering surface.\nPlease restart the program and try another fullscreen mode with less resolution or lower bit depth.");
			} 
			else 
			{
				D3DAppISetErrorString("CreateSurface for fullscreen flipping surface failed.\n%s",
				      D3DAppErrorToString(LastError));
			}
			goto exit_with_error;
		}
	 
		fprintf( fp, "D3DAppICreateBuffers : sucessfully created front buffer ");

		if (bIsHardware == TRUE)
			fprintf( fp, "in video memory\n");
		else
			fprintf( fp, "in system memory\n");

		// Obtain a pointer to the back buffer surface created above so we
		// can use it later.  For now, just check to see if it ended up in
		// video memory (FYI).
	 
    	ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
    	LastError = d3dappi.lpFrontBuffer->GetAttachedSurface(&ddscaps, &d3dappi.lpBackBuffer);
    	
		if(LastError != DD_OK) 
		{
            D3DAppISetErrorString("GetAttachedSurface failed to get back buffer.\n%s",
				  D3DAppErrorToString(LastError));
            goto exit_with_error;
		}

		LastError = D3DAppIGetSurfDesc(&ddsd, d3dappi.lpBackBuffer);

		if (LastError != DD_OK) 
		{
			D3DAppISetErrorString("Failed to get surface description of back buffer.\n%s",
				  D3DAppErrorToString(LastError));
			goto exit_with_error;
		}
        d3dappi.bBackBufferInVideo =
	          (ddsd.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) ? TRUE : FALSE;

		if(d3dappi.bBackBufferInVideo == TRUE)
			fprintf( fp, "D3DAppICreateBuffers : sucessfully created back  buffer in video memory\n");
		else
			fprintf( fp, "D3DAppICreateBuffers : sucessfully created front buffer in system memory\n");
    }
    else 
	{   
		fprintf( fp, "D3DAppICreateBuffers : Windowed mode\n");

		// In the window case, create a front buffer which is the primary
		// surface and a back buffer which is an offscreen pl6ane surface.
         
        memset(&ddsd,0,sizeof(DDSURFACEDESC2));
        ddsd.dwSize = sizeof(DDSURFACEDESC2);
		ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
        ddsd.dwFlags = DDSD_CAPS;
        ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	
		// If we specify system memory when creating a primary surface, we
		// won't get the actual primary surface in video memory.  So, don't
		// use D3DAppICreateSurface().
	 
		LastError = d3dappi.lpDD7->CreateSurface(
					&ddsd, &d3dappi.lpFrontBuffer, NULL);

        if(LastError != DD_OK ) 
		{
			if (LastError == DDERR_OUTOFMEMORY || LastError == DDERR_OUTOFVIDEOMEMORY) 
			{
				D3DAppISetErrorString("There was not enough video memory to create the rendering surface.\nTo run this program in a window of this size, please adjust your display settings for a smaller desktop area or a lower palette size and restart the program.");
			} 
			else 
			{
				D3DAppISetErrorString("CreateSurface for window front buffer failed.\n%s",
				      D3DAppErrorToString(LastError));
			}
            goto exit_with_error;
        }

		fprintf( fp, "D3DAppICreateBuffers : sucessfully created front buffer ");

		if (bIsHardware == TRUE)
			fprintf( fp, "in video memory\n");
		else
			fprintf( fp, "in system memory\n");


    	ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
    	ddsd.dwWidth = w;
    	ddsd.dwHeight = h;
    	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;
	
		if (bIsHardware)
			ddsd.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
		else
			ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
		
		LastError = D3DAppICreateSurface(&ddsd, &d3dappi.lpBackBuffer);
    
		if (LastError != DD_OK) 
		{
			if (LastError == DDERR_OUTOFMEMORY || LastError == DDERR_OUTOFVIDEOMEMORY) 
			{
				D3DAppISetErrorString("There was not enough video memory to create the rendering surface.\nTo run this program in a window of this size, please adjust your display settings for a smaller desktop area or a lower palette size and restart the program.");
			} 
			else 
			{
				D3DAppISetErrorString("CreateSurface for window back buffer failed.\n%s",
			      D3DAppErrorToString(LastError));
			}
			goto exit_with_error;
		}

		// Check to see if the back buffer is in video memory (FYI).

		ddsd.dwSize = sizeof(DDSURFACEDESC2);
		ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
		ddsd.dwFlags = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps =  DDSCAPS_3DDEVICE;
	
		LastError = D3DAppIGetSurfDesc(&ddsd, d3dappi.lpBackBuffer);
	
		if (LastError != DD_OK) 
		{
			D3DAppISetErrorString("D3DAppIGetSurfDesc failed.\n%s",
			  D3DAppErrorToString(LastError));
			goto exit_with_error;
		}
			
		d3dappi.bBackBufferInVideo =
	          (ddsd.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) ? TRUE : FALSE;
        
		if(d3dappi.bBackBufferInVideo == TRUE)
			fprintf( fp, "D3DAppICreateBuffers : sucessfully created back  buffer in video memory\n");
		else
			fprintf( fp, "D3DAppICreateBuffers : sucessfully created front buffer in system memory\n");

		// Create the DirectDraw Clipper object and attach it to the window
		// and front buffer.
         
        LastError = d3dappi.lpDD7->CreateClipper(0, &lpClipper, NULL);
        if(LastError != DD_OK )
		{
            D3DAppISetErrorString("CreateClipper failed.\n%s",
				  D3DAppErrorToString(LastError));
            goto exit_with_error;
		}
		fprintf( fp, "D3DAppICreateBuffers : created clipper\n");

    	LastError = lpClipper->SetHWnd(0, hwnd);
        if(LastError != DD_OK ) 
		{
            D3DAppISetErrorString("Attaching clipper to window failed.\n%s",
				  D3DAppErrorToString(LastError));
            goto exit_with_error;
		}
		fprintf( fp, "D3DAppICreateBuffers : attached clipper to window\n");

    	LastError = d3dappi.lpFrontBuffer->SetClipper(lpClipper);
		if(LastError != DD_OK ) 
		{
            D3DAppISetErrorString("Attaching clipper to front buffer failed.\n%s",
				  D3DAppErrorToString(LastError));
            goto exit_with_error;
		}
		fprintf( fp, "D3DAppICreateBuffers : set clipper\n");
    }

    D3DAppIClearBuffers();
	fprintf( fp, "D3DAppICreateBuffers : sucessfully created buffers\n\n");

	fclose(fp);

	FILE *fplog;
	HRESULT hr;

	ZeroMemory(&d3dappi.ddpfBackBuffer, sizeof(DDPIXELFORMAT));
	d3dappi.ddpfBackBuffer.dwSize = sizeof(DDPIXELFORMAT);

	hr = d3dappi.lpBackBuffer->GetPixelFormat(&d3dappi.ddpfBackBuffer);

	fplog = fopen("rrlogfile.txt","a");

	if(hr == DD_OK)
	{		
		fprintf( fplog, "Backbuffer Pixelformat RGB bits  = : ");
		fprintf( fplog, "%d\n",d3dappi.ddpfBackBuffer.dwRGBBitCount);

		fprintf( fplog, "Backbuffer Pixelformat RGB masks = : ");
		fprintf( fplog, "%d %d %d\n\n",
		d3dappi.ddpfBackBuffer.dwRBitMask,
		d3dappi.ddpfBackBuffer.dwGBitMask,
		d3dappi.ddpfBackBuffer.dwBBitMask);
	}
	else
		fprintf( fplog, "Check on backbuffer pixel format FAILED ");

	fclose(fplog);
	return TRUE;

exit_with_error:
    RELEASE(d3dappi.lpFrontBuffer);
    RELEASE(d3dappi.lpBackBuffer);
    RELEASE(lpClipper);
	fprintf( fp, "D3DAppICreateBuffers : failed\n\n");
	fclose(fp);
    return FALSE;
}


// D3DAppICheckForPalettized
// If the front/back buffer is palettized, we need to create a palette.
 
BOOL D3DAppICheckForPalettized(void)
{
    DDSURFACEDESC2 ddsd;
    /*
     * Get the back buffer surface description and check to see if it's
     * palettized
     */
    LastError = D3DAppIGetSurfDesc(&ddsd, d3dappi.lpBackBuffer);
    if (LastError != DD_OK) 
	{
		D3DAppISetErrorString("Failed to get surface description for back buffer for palettizing.\n%s",
			      D3DAppErrorToString(LastError));
        goto exit_with_error;
    }
    bPrimaryPalettized = 
	(ddsd.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8) ? TRUE : FALSE;

    if (bPrimaryPalettized) {
	int i;
	/*
	 * Get the current palette.
	 */
 	HDC hdc = GetDC(NULL);
	GetSystemPaletteEntries(hdc, 0, (1 << 8), ppe);
	ReleaseDC(NULL, hdc);
	/*
	 * Change the flags on the palette entries to allow D3D to change
	 * some of them.  In the window case, we must not change the top and
	 * bottom ten (system colors), but in a fullscreen mode we can have
	 * all but the first and last.
	 */
        if (!d3dappi.bFullscreen) {
	    for (i = 0; i < 10; i++) ppe[i].peFlags = D3DPAL_READONLY;
	    for (i = 10; i < 256 - 10; i++) ppe[i].peFlags = D3DPAL_FREE | PC_RESERVED;
	    for (i = 256 - 10; i < 256; i++) ppe[i].peFlags = D3DPAL_READONLY;
        } else {
	    ppe[0].peFlags = D3DPAL_READONLY;
	    for (i = 1; i < 255; i++) ppe[i].peFlags = D3DPAL_FREE | PC_RESERVED;
	    ppe[255].peFlags = D3DPAL_READONLY;
	}
	/*
	 * Create a palette using the old colors and new flags
	 */
	LastError = d3dappi.lpDD7->CreatePalette(
					   DDPCAPS_8BIT | DDPCAPS_INITIALIZE,
					   ppe, &lpPalette, NULL);
	if (LastError != DD_OK) {
	    D3DAppISetErrorString("CreatePalette failed.\n%s",
				  D3DAppErrorToString(LastError));
            goto exit_with_error;
	}
	/*
	 * Set this as the front and back buffers' palette
	 */
	LastError =
	       d3dappi.lpBackBuffer->SetPalette(lpPalette);
        if(LastError != DD_OK ) {
            D3DAppISetErrorString("SetPalette failed on back buffer.\n%s",
				  D3DAppErrorToString(LastError));
            goto exit_with_error;
	}
	LastError =
	     d3dappi.lpFrontBuffer->SetPalette(lpPalette);
        if(LastError != DD_OK ) {
            D3DAppISetErrorString("SetPalette failed on front buffer.\n%s",
				  D3DAppErrorToString(LastError));
            goto exit_with_error;
	}
	/*
	 * The palette is now valid, so set it again on anyt WM_ACTIVATE
	 */
	bPaletteActivate = TRUE;
    }
    return TRUE;
exit_with_error:
    RELEASE(lpPalette);
    return FALSE;
}


BOOL D3DAppICreateZBuffer(int w, int h, int driver)
{
    DDSURFACEDESC2 ddsd;
	FILE *fp;

	
    fp = fopen("rrlogfile.txt","a");
     
	fprintf( fp, "D3DAppICreateZBuffer : creating Z buffer\n");

    RELEASE(d3dappi.lpZBuffer);
     
    if (!d3dappi.Driver[driver].bDoesZBuffer)
	{
		fprintf( fp, "D3DAppICreateZBuffer : this driver doesn't do Z buffers\n\n");
		fclose(fp);
		return TRUE;
	}


	ZeroMemory(&ddsd, sizeof(DDSURFACEDESC2));

    ddsd.dwSize                 = sizeof(DDSURFACEDESC2);
    ddsd.dwFlags                = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS 
									| DDSD_PIXELFORMAT;
	ddsd.dwWidth				= w;
    ddsd.dwHeight				= h;

    ddsd.ddsCaps.dwCaps         = DDSCAPS_ZBUFFER;
    ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
	ddsd.ddpfPixelFormat.dwFlags = DDPF_ZBUFFER ;
	ddsd.ddpfPixelFormat.dwZBufferBitDepth = 16;

    memcpy( &ddsd.ddpfPixelFormat, &d3dappi.ddpfZBuffer, sizeof(DDPIXELFORMAT) );

    
    // If this is a hardware D3D driver, the Z-Buffer MUST end up in video
    // memory.  Otherwise, it MUST end up in system memory.
     
    if (d3dappi.Driver[driver].bIsHardware)
		ddsd.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
    else
		ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
    
   
    LastError = d3dappi.lpDD7->CreateSurface(&ddsd,
						    &d3dappi.lpZBuffer,
						    NULL);
    if(LastError != DD_OK) 
	{
		if (LastError == DDERR_OUTOFMEMORY || LastError == DDERR_OUTOFVIDEOMEMORY) 
		{
			if (d3dappi.bFullscreen) 
			{
				D3DAppISetErrorString("There was not enough video memory to create the Z-buffer surface.\nPlease restart the program and try another fullscreen mode with less resolution or lower bit depth.");
			} 
			else 
			{
				D3DAppISetErrorString("There was not enough video memory to create the Z-buffer surface.\nTo run this program in a window of this size, please adjust your display settings for a smaller desktop area or a lower palette size and restart the program.");
			}
		} 
		if (LastError == DDERR_INVALIDPARAMS) 
		{
			D3DAppISetErrorString("Failed to create Z buffer : DDERR_INVALIDPARAMS");
			goto exit_with_error;
		}

		if (LastError == DDERR_INVALIDPIXELFORMAT ) 
		{
			D3DAppISetErrorString("Failed to create Z buffer : DDERR_INVALIDPIXELFORMAT ");
			goto exit_with_error;
		}
		
		D3DAppISetErrorString("Failed to create Z buffer");
		goto exit_with_error;
    }
    
    // Attach the Z-buffer to the back buffer so D3D will find it
     
    LastError = d3dappi.lpBackBuffer->AddAttachedSurface(d3dappi.lpZBuffer);

    if(LastError != DD_OK) 
	{
        D3DAppISetErrorString("AddAttachedBuffer failed for Z-Buffer.\n%s",
			      D3DAppErrorToString(LastError));
		goto exit_with_error;
    }
    
    // Find out if it ended up in video memory.
     
    LastError = D3DAppIGetSurfDesc(&ddsd, d3dappi.lpZBuffer);
    
	if (LastError != DD_OK) 
	{
		D3DAppISetErrorString("Failed to get surface description of Z buffer.\n%s",
			      D3DAppErrorToString(LastError));
        goto exit_with_error;
    }
    
	d3dappi.bZBufferInVideo = (ddsd.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) ? TRUE : FALSE;
    
	if (d3dappi.Driver[driver].bIsHardware && !d3dappi.bZBufferInVideo) 
	{
		D3DAppISetErrorString("Could not fit the Z-buffer in video memory for this hardware device.\n");
		goto exit_with_error;
    }
	
	fprintf( fp, "D3DAppICreateZBuffer : sucessfully created Z buffer ");

	if(d3dappi.bZBufferInVideo == TRUE)
		fprintf( fp, "in Video memory\n\n");
	else
		fprintf( fp, "in system memory\n\n");

	fclose(fp);

    return TRUE;

exit_with_error:
    RELEASE(d3dappi.lpZBuffer);
	fprintf( fp, "D3DAppICreateZBuffer : failed to create Z buffer\n\n");
	fclose(fp);
    return FALSE;
}

/***************************************************************************/
/*                             WM_SIZE Handler                             */
/***************************************************************************/

 // D3DAppIHandleWM_SIZE
 // Processes the WM_SIZE message.  Resizes all the buffers and re-creates
 // device if necessary.
 
BOOL D3DAppIHandleWM_SIZE(LRESULT* lresult, HWND hwnd, UINT message,
		     WPARAM wParam, LPARAM lParam)
{
    int w, h;
    
    // If we have minimzied, take note and call the default window proc
     

	PrintMessage(myglobs.hWndMain, "D3DAppIHandleWM_SIZE : starting\n", NULL, LOGFILE_ONLY);
    
    if (wParam == SIZE_MINIMIZED) 
	{
		PrintMessage(myglobs.hWndMain, "D3DAppIHandleWM_SIZE : SIZE_MINIMIZED = TRUE\n", NULL, LOGFILE_ONLY);
		d3dappi.bMinimized = TRUE;
		*lresult = DefWindowProc(hwnd, message, wParam, lParam);
		return TRUE;
    }
    
    // In fullscreen mode, restore our surfaces and let DDraw take
    // care of the rest.
     
    if (d3dappi.bFullscreen) 
	{
		PrintMessage(myglobs.hWndMain, "D3DAppIHandleWM_SIZE : bFullscreen = TRUE\n", NULL, LOGFILE_ONLY);
		D3DAppIValidateDirtyRects();
		//D3DAppCheckForLostSurfaces();
		d3dappi.bMinimized = FALSE;
		*lresult = DefWindowProc(hwnd, message, wParam, lParam);
		return TRUE;
    }
    
    // If we are minimized, this is the un-minimized size message.
     
    if (d3dappi.bMinimized) 
	{
		// Restore our surfaces and update the dirty rectangle info
	 
		D3DAppIValidateDirtyRects();
		D3DAppCheckForLostSurfaces();
		d3dappi.bMinimized = FALSE;
		*lresult = DefWindowProc(hwnd, message, wParam, lParam);
		return TRUE;
    }
    
    // Since we are still here, this must be a regular, window resize
    // message.  A new viewport will definitely be needed, but the
    // device and buffers will only be re-created if they have gotten bigger
    // or change size by a very large amount.
     
    D3DAppIGetClientWin(hwnd);
    w = LOWORD(lParam);
    h = HIWORD(lParam);
    
    // If w and h are under the minimum, create buffers of the minimum size
     
    if (w < D3DAPP_WINDOWMINIMUM)
        w = D3DAPP_WINDOWMINIMUM;
    if (h < D3DAPP_WINDOWMINIMUM)
        h = D3DAPP_WINDOWMINIMUM;
    
    // Destroy the viewport and all execute buffers
     
    d3dappi.bRenderingIsOK = FALSE;
    ATTEMPT(D3DAppICallDeviceDestroyCallback());
    
    // Only create a new device and buffers if they changed significantly,
    // otherwise just make sure the old buffers aren't lost.
     
    if ((w > szBuffers.cx || h > szBuffers.cy) ||
		(w < szBuffers.cx / 2 || h < szBuffers.cy / 2)) 
	{
	
		// Release the device
	 
		RELEASE(d3dappi.lpD3DDevice);
	
		// Release the old buffers
	 
		RELEASE(d3dappi.lpZBuffer);
		RELEASE(lpPalette);
		RELEASE(lpClipper);
		RELEASE(d3dappi.lpBackBuffer);
		RELEASE(d3dappi.lpFrontBuffer);
	
		// Create new ones
	 
		ATTEMPT(D3DAppICreateBuffers(hwnd, w, h, D3DAPP_BOGUS, FALSE, d3dappi.ThisDriver.bIsHardware));
		ATTEMPT(D3DAppICheckForPalettized());
		ATTEMPT(D3DAppICreateZBuffer(w, h, d3dappi.CurrDriver));
	
		// Create the driver
	 
		ATTEMPT(D3DAppICreateDevice(d3dappi.CurrDriver));
    } 
	else 
	{
		D3DAppCheckForLostSurfaces();
    }
    
    // Call the device create callback to create the viewport, set the render
    // state and clear the dirty rectangle info
     
    ATTEMPT(D3DAppICallDeviceCreateCallback(w, h));
    ATTEMPT(D3DAppISetRenderState());
    D3DAppIValidateDirtyRects();
    d3dappi.bRenderingIsOK = TRUE;
    
    // Call the default window proc
     
    *lresult = DefWindowProc(hwnd, message, wParam, lParam);
	
	PrintMessage(myglobs.hWndMain, "D3DAppIHandleWM_SIZE : suceeded\n", NULL, LOGFILE_ONLY);
    return TRUE;

exit_with_error:
    D3DAppICallDeviceDestroyCallback();
    RELEASE(d3dappi.lpD3DDevice);
    RELEASE(d3dappi.lpZBuffer);
    RELEASE(lpPalette);
    RELEASE(lpClipper);
    RELEASE(d3dappi.lpBackBuffer);
    RELEASE(d3dappi.lpFrontBuffer);
	PrintMessage(myglobs.hWndMain, "D3DAppIHandleWM_SIZE : failed\n", NULL, LOGFILE_ONLY);
    return FALSE;
}

/***************************************************************************/
/*              Setting the display mode and cooperative level             */
/***************************************************************************/
/*
 * D3DAppISetCoopLevel
 * Set the cooperative level to exclusive mode for fullscreen and normal for
 * a window.  Set the bIgnoreWM_SIZE flag because SetCooperativeLevel
 * generates a WM_SIZE message you do not have to resize the buffers on.
 */
BOOL D3DAppISetCoopLevel(HWND hwnd, BOOL bFullscreen)
{
    if (bFullscreen) 
	{
		bIgnoreWM_SIZE = TRUE;
		LastError = d3dappi.lpDD7->SetCooperativeLevel(
				   hwnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN);
		bIgnoreWM_SIZE = FALSE;
	
		if(LastError != DD_OK ) 
		{
			D3DAppISetErrorString("SetCooperativeLevel to fullscreen failed.\n%s",
				  D3DAppErrorToString(LastError));
			return FALSE;
		}
	d3dappi.bFullscreen = TRUE;
    } 
	else 
	{
		bIgnoreWM_SIZE = TRUE;
        LastError = d3dappi.lpDD7->SetCooperativeLevel(
							 hwnd, DDSCL_NORMAL);
		bIgnoreWM_SIZE = FALSE;
		if(LastError != DD_OK ) 
		{
            D3DAppISetErrorString("SetCooperativeLevel to normal failed.\n%s",
				  D3DAppErrorToString(LastError));
            return FALSE;
        }
		d3dappi.bFullscreen = FALSE;
    }
    return TRUE;
}

/*
 * D3DAppISetDisplayMode
 * Set the display mode to the given dimensions and bits per pixel.  The
 * bIgnoreWM_SIZE message is set because the display change generates a
 * WM_SIZE message which we don't want to resize the buffers on.
 */
BOOL
D3DAppISetDisplayMode(int w, int h, int bpp)
{
    d3dappi.ThisMode.w = w; d3dappi.ThisMode.h = h;
    d3dappi.ThisMode.bpp = bpp;
    bIgnoreWM_SIZE = TRUE;
    LastError = d3dappi.lpDD7->SetDisplayMode( w, h, bpp, 0, 0);
    bIgnoreWM_SIZE = FALSE;
    
	if(LastError != DD_OK ) 
	{
        D3DAppISetErrorString("SetDisplayMode to %dx%dx%d failed\n%s",
			      w, h, bpp, D3DAppErrorToString(LastError));
        return FALSE;
    }
    return TRUE;
}

/*
 * D3DAppIRestoreDispMode
 * Restores the display mode to the current windows display mode.  The
 * bIgnoreWM_SIZE message is set because the display change generates a
 * WM_SIZE message which we don't want to resize the buffers on.
 */
BOOL
D3DAppIRestoreDispMode(void)
{
    bIgnoreWM_SIZE = TRUE;
    LastError = d3dappi.lpDD7->RestoreDisplayMode();
    if (LastError != DD_OK) 
	{
		D3DAppISetErrorString("RestoreDisplayMode failed.\n%s",
			      D3DAppErrorToString(LastError));
		return FALSE;
    }
    bIgnoreWM_SIZE = FALSE;
    return TRUE;
}

/*
 * D3DAppRememberWindowsMode
 * Record the current display mode in d3dappi.WindowsDisplay
 */
BOOL D3DAppIRememberWindowsMode(void)
{
    DDSURFACEDESC2 ddsd;

    memset(&ddsd, 0, sizeof(DDSURFACEDESC2));
    ddsd.dwSize = sizeof(DDSURFACEDESC2);
	ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
    LastError = d3dappi.lpDD7->GetDisplayMode(&ddsd);

    if (LastError != DD_OK) 
	{
		D3DAppISetErrorString("Getting the current display mode failed.\n%s",
			      D3DAppErrorToString(LastError));
		return FALSE;
    }
    d3dappi.WindowsDisplay.w = ddsd.dwWidth;
    d3dappi.WindowsDisplay.h = ddsd.dwHeight;
    d3dappi.WindowsDisplay.bpp = ddsd.ddpfPixelFormat.dwRGBBitCount;
    return TRUE;
}

/***************************************************************************/
/*                          Misc DD Utilities                              */
/***************************************************************************/

/*
 * D3DAppIClearBuffers
 * Clear the front and back buffers to black
 */
BOOL D3DAppIClearBuffers(void)
{
    DDSURFACEDESC2 ddsd;
    RECT dst;
    DDBLTFX ddbltfx;
    
    // Find the width and height of the front buffer by getting its
    // DDSURFACEDESC2
     
    if (d3dappi.lpFrontBuffer) 
	{
		LastError = D3DAppIGetSurfDesc(&ddsd, d3dappi.lpFrontBuffer);
		if (LastError != DD_OK) 
		{
			D3DAppISetErrorString("Failure getting the surface description of the front buffer before clearing.\n%s",
				  D3DAppErrorToString(LastError));
	    return FALSE;
	}
	
	// Clear the front buffer to black
	 
	memset(&ddbltfx, 0, sizeof(ddbltfx));
	ddbltfx.dwSize = sizeof(DDBLTFX);
	SetRect(&dst, 0, 0, ddsd.dwWidth, ddsd.dwHeight);
	LastError = d3dappi.lpFrontBuffer->Blt(
						    &dst, NULL, NULL, 
						    DDBLT_COLORFILL | DDBLT_WAIT,
						    &ddbltfx);
	if (LastError != DD_OK) {
	    D3DAppISetErrorString("Clearing the front buffer failed.\n%s",
				  D3DAppErrorToString(LastError));
	    return FALSE;
	}
    }
    if (d3dappi.lpBackBuffer) {
	/*
	 * Find the width and height of the back buffer by getting its
	 * DDSURFACEDESC2
	 */
	LastError = D3DAppIGetSurfDesc(&ddsd, d3dappi.lpBackBuffer);
	if (LastError != DD_OK) {
	    D3DAppISetErrorString("Failure while getting the surface description of the back buffer before clearing.\n%s",
				  D3DAppErrorToString(LastError));
	    return FALSE;
	}
	/*
	 * Clear the back buffer to black
	 */
	memset(&ddbltfx, 0, sizeof(ddbltfx));
	ddbltfx.dwSize = sizeof(DDBLTFX);
	SetRect(&dst, 0, 0, ddsd.dwWidth, ddsd.dwHeight);
	LastError = d3dappi.lpBackBuffer->Blt( &dst,
						     NULL, NULL,
						     DDBLT_COLORFILL | DDBLT_WAIT,
						     &ddbltfx);
	if (LastError != DD_OK) {
	    D3DAppISetErrorString("Clearing the front buffer failed.\n%s",
				  D3DAppErrorToString(LastError));
	    return FALSE;
	}
    }
    return TRUE;
}

/*
 * D3DAppIBPPToDDBD
 * Convert an integer bit per pixel number to a DirectDraw bit depth flag
 */
DWORD
D3DAppIBPPToDDBD(int bpp)
{
    switch(bpp) {
	case 1:
	    return DDBD_1;
	case 2:
	    return DDBD_2;
	case 4:
	    return DDBD_4;
	case 8:
	    return DDBD_8;
	case 16:
	    return DDBD_16;
	case 24:
	    return DDBD_24;
	case 32:
	    return DDBD_32;
	default:
	    return (DWORD)D3DAPP_BOGUS;
    }
}

/*
 * D3DAppTotalVideoMemory
 * Returns the amount of total video memory supported (not free)
 */
DWORD
D3DAppTotalVideoMemory(void)
{
    DDCAPS DriverCaps, HELCaps;
    memset (&DriverCaps, 0, sizeof(DDCAPS));
    DriverCaps.dwSize = sizeof(DDCAPS);
    memset (&HELCaps, 0, sizeof(DDCAPS));
    HELCaps.dwSize = sizeof(DDCAPS);
    LastError = d3dappi.lpDD7->GetCaps(&DriverCaps,
					      &HELCaps);
    if (LastError != DD_OK) {
	D3DAppISetErrorString("Getting DD capabilities failed while checking total video memory.\n%s",
			      D3DAppErrorToString(LastError));
	return 0L;
    }
    if (DriverCaps.dwVidMemTotal)
	return DriverCaps.dwVidMemTotal;
    else
	return HELCaps.dwVidMemTotal;
}
