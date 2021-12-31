
// Road Rage 1.1

// FILE : texture.cpp
// This file was originally part of the Microsoft DirectX5 SDK
// Edited by William Chin
// Edited with MSDEV C++ v6.0 using the DirectX 6.0 SDK
// Edited on:  3/7/99

#include "d3dappi.hpp"
#include "texture.hpp"
#include <winuser.h>
#include "d3dmain.hpp"
#include "world.hpp"

#define MAGICBYTES 2

extern d3dmainglobals myglobs;
extern SETUPINFO setupinfo;


//  FUNCTION DECLARATIONS
 
static void D3DAppIAddPathList(const char *path);
static void D3DAppIInitialisePathList();
static FILE * D3DAppIFindFile(const char *name, const char *mode);
static BOOL loadPPMHeader(FILE *fp, DWORD *width, DWORD *height, int *maxgrey);

BOOL D3DAppILoadBMPintoTextureSurf(int n, BOOL* bInVideo);
BOOL D3DAppILoadPPMintoTextureSurf(int n, BOOL* bInVideo);


BOOL DebugInVideo;
int debug_last_tex_number;


/***************************************************************************/
/*                        Managing the texture list                        */
/***************************************************************************/

// D3DAppILoadTextureSurf
// Creates a texture map surface and texture object from the numbered PPM
// file.  This is done in a two step process.  A source texture surface and
// object are created in system memory.  A second, initially empty, texture
// surface is created (in video memory if hardware is present).  The source
// texture is loaded into the destination texture surface and then discarded.
// This process allows a device to compress or reformat a texture map as it
// enters video memory during the Load call.
 

BOOL D3DAppILoadTextureSurf(int n, BOOL* bInVideo)
{
	int i;


	if(d3dappi.ImageFile[n][0] == NULL)
	{
		PrintMessage(myglobs.hWndMain, "D3DAppILoadTextureSurf: FAILED Null image name", NULL, LOGFILE_ONLY);	
		return FALSE;
	}

	PrintMessage(myglobs.hWndMain, "D3DAppILoadTextureSurf: Loading ", d3dappi.ImageFile[n], LOGFILE_ONLY);	

	 
	for(i = 0; i < D3DAPP_MAX_TEXTURE_FILENAME_LENGTH; i++)
	{	
		if(strcmpi(&d3dappi.ImageFile[n][i], ".bmp") == 0)
			return D3DAppILoadBMPintoTextureSurf(n, bInVideo);
		

		if(strcmpi(&d3dappi.ImageFile[n][i], ".ppm") == 0)
			return D3DAppILoadPPMintoTextureSurf(n, bInVideo);
		

		if(d3dappi.ImageFile[n][i] == NULL)
			return FALSE;
	}

	// If we get this far there's an error in the images filename
	PrintMessage(myglobs.hWndMain, "D3DAppILoadTextureSurf: FAILED to load image ", d3dappi.ImageFile[n], LOGFILE_ONLY);	
	PrintMessage(myglobs.hWndMain, "\n", NULL, LOGFILE_ONLY);	
	return FALSE;
}


BOOL D3DAppILoadBMPintoTextureSurf(int n, BOOL* bInVideo)
{
	HBITMAP			hbm;
	BITMAP			bm;
	HDC				hdcbm;
	HDC				hdcDest;
    DDSURFACEDESC2  ddsd;
	HRESULT			LastError;
	DWORD testcaps;
	int TextureSize;


    TextureSize = setupinfo.TextureSize;

	// Release the surface if it is hanging around
	RELEASE(d3dappi.lpTextureSurf[n]);

	hbm = (HBITMAP)LoadImage(NULL, d3dappi.ImageFile[n], IMAGE_BITMAP, 256, 256, LR_LOADFROMFILE|LR_CREATEDIBSECTION);

    if (hbm == NULL)
	{
		MessageBox(myglobs.hWndMain, "Load Bitmap failed", NULL, MB_OK);
		PrintMessage(myglobs.hWndMain, "D3DAppILoadTextureSurf: FAILED to load bitmap\n", NULL, LOGFILE_ONLY);	
		return FALSE;
	}
    
  
	ZeroMemory(&ddsd, sizeof(DDSURFACEDESC2));
    ddsd.dwSize = sizeof(DDSURFACEDESC2);
	ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
    ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE; 
	ddsd.dwHeight = TextureSize;
    ddsd.dwWidth =  TextureSize;
	ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);	 
	//memcpy(&ddsd.ddpfPixelFormat, &d3dappi.ThisTextureFormat.ddsd.ddpfPixelFormat, sizeof(DDPIXELFORMAT));
	memcpy(&ddsd.ddpfPixelFormat, &d3dappi.ddpfTextureFormats, sizeof(DDPIXELFORMAT));


	if (!d3dappi.ThisDriver.bIsHardware)
		ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;

	LastError = D3DAppICreateSurface(&ddsd, &d3dappi.lpTextureSurf[n]);
    
	if (LastError != DD_OK) 
	{
		D3DAppISetErrorString("FAILED to create the destination texture surface.\n%s",
			      D3DAppErrorToString(LastError));
		goto exit_with_error;
    }

   	LastError = d3dappi.lpTextureSurf[n]->GetDC(&hdcDest);
	
	if (LastError != DD_OK) 
	{
		D3DAppISetErrorString("FAILED to obtain DC for a destination texture.\n%s",
			      D3DAppErrorToString(LastError));
		goto exit_with_error;
    }

	GetObject(hbm, sizeof(bm), &bm);

	hdcbm   = CreateCompatibleDC(hdcDest);
	SelectBitmap(hdcbm, hbm);

	if(hdcbm == NULL)
	{
		PrintMessage(myglobs.hWndMain, "D3DAppILoadTextureSurf : FAILED to create DC - hdcbm\n", NULL, LOGFILE_ONLY);	
		goto exit_with_error;
	}
	
	// LOAD IMAGE INTO TEXTURE SURFACE 

	// put DirectDraw Blt skrink and Blt stretch flags into testcaps
	testcaps = DDFXCAPS_BLTSHRINKX  | DDFXCAPS_BLTSHRINKY | 
			   DDFXCAPS_BLTSTRETCHX | DDFXCAPS_BLTSTRETCHY;

	// test if current driver can do Blt skrink and Blt stretch
	// from system to video memory

	if((d3dappi.DDDriverCaps.dwSVBFXCaps & testcaps) == testcaps)
	{
		StretchBlt(hdcDest, 0, 0, TextureSize, TextureSize, // destination bitmap
				hdcbm, 0, 0, (int)bm.bmWidth, (int)bm.bmHeight, // source bitmap
				SRCCOPY);   
	}
	else
	{
		TextureSize = 256;

		StretchBlt(hdcDest, 0, 0, bm.bmWidth, bm.bmHeight, // destination bitmap
				hdcbm, 0, 0, bm.bmWidth, bm.bmHeight, // source bitmap
				SRCCOPY);  
	}

	
	d3dappi.lpTextureSurf[n]->ReleaseDC(hdcDest);  

	DeleteDC(hdcbm);
	DeleteObject(hbm);
	return TRUE;


exit_with_error:
 
    RELEASE(d3dappi.lpTextureSurf[n]);
	PrintMessage(myglobs.hWndMain, "ERROR in D3DAppILoadTextureSurf\n", NULL, LOGFILE_ONLY);	
    return FALSE;    
}


BOOL D3DAppILoadPPMintoTextureSurf(int n, BOOL* bInVideo)
{
    DDSURFACEDESC2 ddsd;
    LPDIRECTDRAWSURFACE7 lpSrcTextureSurf = NULL;
	LPDIRECTDRAWSURFACE7 lpTempTextureSurf = NULL;
    LPDIRECTDRAWPALETTE lpDstPalette = NULL;
    PALETTEENTRY ppe[256];
    DWORD pcaps, testcaps;
	HDC hdcImage, hdcTemp, hdcDest;
	int TextureSize;

	TextureSize = setupinfo.TextureSize;

    // Release the surface if it is hanging around
     
    RELEASE(d3dappi.lpTextureSurf[n]);
    
    // Create a surface in system memory and load the PPM file into it.
    // Query for the texture interface.
     
    lpSrcTextureSurf = D3DAppILoadSurface(d3dappi.lpDD7, d3dappi.ImageFile[n],
					  &d3dappi.ThisTextureFormat.ddsd,
					   DDSCAPS_TEXTURE | DDSCAPS_SYSTEMMEMORY);
    
	if (!lpSrcTextureSurf)
	{
		PrintMessage(NULL,"D3DAppILoadTextureSurf: lpSrcTextureSurf = NULL", NULL, LOGFILE_ONLY);
		goto exit_with_error;
	}
    
    LastError = D3DAppIGetSurfDesc(&ddsd, lpSrcTextureSurf);

    if (LastError != DD_OK) 
	{
		D3DAppISetErrorString("Could not get the surface description of the source texture.\n%s",
			      D3DAppErrorToString(LastError));
		goto exit_with_error;
    }

	ZeroMemory(&ddsd, sizeof(DDSURFACEDESC2));
    ddsd.dwSize = sizeof(DDSURFACEDESC2);
    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
    ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE; // | DDSCAPS_ALLOCONLOAD;
	ddsd.dwHeight = TextureSize;
    ddsd.dwWidth =  TextureSize;
	ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT );
	memcpy(&ddsd.ddpfPixelFormat, &d3dappi.ddpfTextureFormats, sizeof(DDPIXELFORMAT));

	
	if (!d3dappi.ThisDriver.bIsHardware)
		ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;

	LastError = D3DAppICreateSurface(&ddsd, &d3dappi.lpTextureSurf[n]);
    
	if (LastError != DD_OK) 
	{
		D3DAppISetErrorString("Could not create the destination texture surface.\n%s",
			      D3DAppErrorToString(LastError));
		goto exit_with_error;
    }

    if (ddsd.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8) 
	{
		pcaps = DDPCAPS_8BIT | DDPCAPS_ALLOW256;
    } 
	else 
	{
		if (ddsd.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED4) 
		{
			pcaps = DDPCAPS_4BIT;
		} 
		else 
		{
			pcaps = 0;
		}
	}

    if (pcaps) 
	{
		memset(ppe, 0, sizeof(PALETTEENTRY) * 256);
	
		LastError = d3dappi.lpDD7->CreatePalette(pcaps,
						 ppe, &lpDstPalette, NULL);

		if (LastError != DD_OK) 
		{
			D3DAppISetErrorString("Failed to create a palette for the destination texture.\n%s",
				  D3DAppErrorToString(LastError));
			goto exit_with_error;
		}
		
		LastError = d3dappi.lpTextureSurf[n]->SetPalette(lpDstPalette);
		
		if (LastError != DD_OK) 
		{
			D3DAppISetErrorString("Failed to set the destination texture's palette.\n%s",
				  D3DAppErrorToString(LastError));
			goto exit_with_error;
		}
    }
    
	// Create a second temporary surface for texture stretching between two 
	// system memory surfaces. Note some card won't StretchBlt to a surface
	// in video memory.

	ZeroMemory(&ddsd, sizeof(DDSURFACEDESC2));
    ddsd.dwSize = sizeof(DDSURFACEDESC2);
    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;  
	ddsd.ddsCaps.dwCaps2  = 0L;
	ddsd.dwHeight = TextureSize;
    ddsd.dwWidth  = TextureSize;
	ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);	 
	memcpy(&ddsd.ddpfPixelFormat, &d3dappi.ThisTextureFormat.ddsd.ddpfPixelFormat, sizeof(DDPIXELFORMAT));


	LastError = d3dappi.lpDD7->CreateSurface(&ddsd, &lpTempTextureSurf, NULL);

	if (LastError != DD_OK) 
	{
		D3DAppISetErrorString("D3DAppILoadTextureSurf : Failed to create surface for second temp texture surface.\n%s", D3DAppErrorToString(LastError));
		goto exit_with_error;
    }

	
	// LOAD IMAGE INTO TEXTURE SURFACE 

	// put DirectDraw Blt skrink and Blt stretch flags into testcaps
	testcaps = DDFXCAPS_BLTSHRINKX  | DDFXCAPS_BLTSHRINKY | 
			   DDFXCAPS_BLTSTRETCHX | DDFXCAPS_BLTSTRETCHY;

	// test if current driver can do Blt skrink and Blt stretch
	// from system to video memory

	if((d3dappi.DDDriverCaps.dwSVBFXCaps & testcaps) == testcaps)
	{
		// driver can do Blt skrink and Blt stretch, so load the
		// image straight into the texture surface with the new size

		lpSrcTextureSurf->GetDC(&hdcImage);
		d3dappi.lpTextureSurf[n]->GetDC(&hdcDest);
	 
		// stretch or skrink image to new size in Destination texture surface
		StretchBlt(hdcDest, 0, 0, TextureSize, TextureSize, 
			hdcImage, 0, 0, 256, 256, SRCCOPY);     
	
		d3dappi.lpTextureSurf[n]->ReleaseDC(hdcDest);  
		lpSrcTextureSurf->ReleaseDC(hdcImage);
	}
    else
	{
		// nope driver can't do all of the caps we require to either stretch
		// or skrink a texture from system memory to video memory

		if((d3dappi.DDDriverCaps.dwSSBFXCaps & testcaps) == testcaps)
		{
			// Do the image resizing in system memory

			lpSrcTextureSurf->GetDC(&hdcImage);
			d3dappi.lpTextureSurf[n]->GetDC(&hdcDest);
			lpTempTextureSurf->GetDC(&hdcTemp);
	 
			// resize image and copy into another system memory temporary surface
			StretchBlt(hdcTemp, 0, 0, setupinfo.TextureSize, setupinfo.TextureSize, 
				hdcImage, 0, 0, 256, 256, SRCCOPY);     
	
			lpTempTextureSurf->ReleaseDC(hdcTemp);
			d3dappi.lpTextureSurf[n]->ReleaseDC(hdcDest);  
			lpSrcTextureSurf->ReleaseDC(hdcImage);  

			// now copy into our destination texture surface 
			d3dappi.lpTextureSurf[n]->BltFast(
				0, 0,				// Upper left xy of destination
				lpTempTextureSurf,  // Source surface
				NULL,				// Source rectangle = entire surface
				DDBLTFAST_WAIT );
		}
		else
		{
			// Textures can't be resized with this 3d card
			// should do a sofware resize here
			// but going to use 256 texture size and 
			// not resize textures, this may mean there
			// isn't enough video memory for all textures

			setupinfo.TextureSize = 256;

			// now copy into our destination texture surface 
			d3dappi.lpTextureSurf[n]->BltFast(
			0, 0,				// Upper left xy of destination
			lpSrcTextureSurf,   // Source surface
			NULL,				// Source rectangle = entire surface
			DDBLTFAST_WAIT );
		}
	}

	RELEASE(lpTempTextureSurf);
    RELEASE(lpSrcTextureSurf);
	

    
    // Did the texture end up in video memory?
    
    LastError = D3DAppIGetSurfDesc(&ddsd, d3dappi.lpTextureSurf[n]);
    
	if (LastError != DD_OK) 
	{
		D3DAppISetErrorString("Could not get the surface description of the loaded texture surface.\n%s",
			      D3DAppErrorToString(LastError));
		goto exit_with_error;
    }
    
	if (ddsd.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY)
		*bInVideo = TRUE;
    else
		*bInVideo = FALSE;
        
    return TRUE;

exit_with_error:
    RELEASE(lpSrcTextureSurf);
    RELEASE(lpDstPalette);
    RELEASE(d3dappi.lpTextureSurf[n]);
	PrintMessage(myglobs.hWndMain, "Error in D3DAppILoadTextureSurf\n", NULL, LOGFILE_ONLY);
	
    return FALSE;       
}


// D3DAppIReloadTextureSurf
// Reloads a lost and restored texture surface
 
BOOL D3DAppIReloadTextureSurf(int n)
{
    LPDIRECTDRAWSURFACE7 lpSrcTextureSurf = NULL;

    //PrintMessage(myglobs.hWndMain, "D3DAppIReloadTextureSurf: Attempting to Reload texture surface\n", NULL, LOGFILE_ONLY);
	
	RELEASE(d3dappi.lpTextureSurf[n]);

    // Create a surface in system memory and load the PPM file into it.
    // Query for the texture interface.
     
    lpSrcTextureSurf = D3DAppILoadSurface(d3dappi.lpDD7, d3dappi.ImageFile[n],
					  &d3dappi.ThisTextureFormat.ddsd,
					  DDSCAPS_TEXTURE | DDSCAPS_SYSTEMMEMORY);

    if (!lpSrcTextureSurf)
		goto exit_with_error;

    d3dappi.lpTextureSurf[n] = lpSrcTextureSurf;
     
    // Now we are done with the source texture
     
    RELEASE(lpSrcTextureSurf);

    return TRUE;

exit_with_error:
    RELEASE(lpSrcTextureSurf);
	PrintMessage(myglobs.hWndMain, "Error in D3DAppIReloadTextureSurf\n", NULL, LOGFILE_ONLY);
		
    return FALSE;       
}



// D3DAppIReleaseTexture
// Release this texture surface and texture interface.  Remember, a texture
// handle is NOT and object and does not need to be released or destroyed.
// The handle is no longer valid after the device is destroyed, so set it to
// zero here.
 
void D3DAppIReleaseTexture(int n)
{
    RELEASE(d3dappi.lpTextureSurf[n]);
}

// D3DAppIReleaseAllTextures
// Release all texture surfaces and texture interfaces
 
void D3DAppIReleaseAllTextures(void)
{
    int i;
	char buffer[256];

	itoa(d3dappi.NumTextures, buffer,10);
	PrintMessage(NULL, buffer," textures to be released *******", LOGFILE_ONLY);
    
	if(d3dappi.NumTextures == 0)
		return;

	for (i = 0; i < d3dappi.NumTextures; i++) 
	{
		//D3DAppIReleaseTexture(i);
		
		if(d3dappi.lpTextureSurf[i] != NULL)
			d3dappi.lpTextureSurf[i]->Release();

		if(d3dappi.lpTextureSurf[i] == NULL)
		{
				itoa(i, buffer, 10);
				PrintMessage(myglobs.hWndMain, "D3DAppIReleaseAllTextures : releasing texture ", buffer, LOGFILE_ONLY);
		}
    }

	
	PrintMessage(myglobs.hWndMain, "D3DAppIReleaseAllTextures : released all textures\n", NULL, LOGFILE_ONLY);
}


// D3DAppILoadAllTextures
// Load all texture surfaces, qeury them for texture interfaces 
// and get handles for them from the current D3D driver.
 
BOOL D3DAppILoadAllTextures(void)
{
    int i;
	//char buffer[256];

    if (d3dappi.ThisDriver.bDoesTextures) 
	{
		d3dappi.NumUsableTextures = 0;

		for (i = 0; i < d3dappi.NumTextures; i++) 
		{
			ATTEMPT(D3DAppILoadTextureSurf(i, &DebugInVideo));

				++d3dappi.NumUsableTextures;
		}
    } 
	else 
	{
		d3dappi.NumUsableTextures = 0;
    }
    return TRUE;

exit_with_error:
    for (i = 0; i < d3dappi.NumTextures; i++) 
	{
		D3DAppIReleaseTexture(i);
    }
    return FALSE;
}



/***************************************************************************/
/*                    Loading a PPM file into a surface                    */
/***************************************************************************/
/*
 * LoadSurface
 * Loads a ppm file into a texture map DD surface of the given format.  The
 * memory flag specifies DDSCAPS_SYSTEMMEMORY or DDSCAPS_VIDEOMEMORY.
 *
 */

LPDIRECTDRAWSURFACE7 D3DAppILoadSurface(LPDIRECTDRAW7 lpDD, LPCSTR lpName,
		   LPDDSURFACEDESC2 lpFormat, DWORD memoryflag)
{
    LPDIRECTDRAWSURFACE7 lpDDS;
    DDSURFACEDESC2 ddsd, format;//, ddsd_temp;
	LPDIRECTDRAWPALETTE lpDDPal;
	PALETTEENTRY ppe[256];
    D3DCOLOR colors[256];
    D3DCOLOR c;
    DWORD dwWidth, dwHeight;
	DWORD pcaps;
    FILE *fp;
    BOOL bQuant = FALSE;
    HRESULT ddrval;  
    int psize;
    int i, j;
    int color_count;
	char *lpC;
    
    
    // Find the image file and open it
    
    fp = D3DAppIFindFile(lpName, "rb");
    if (fp == NULL) 
	{
        D3DAppISetErrorString("Cannot find %s.\n", lpName);
		return NULL;
    }
    
    // Parse the PPM header
     
    if (!loadPPMHeader(fp, &dwWidth, &dwHeight, &i)) 
	{
		fclose(fp);
		D3DAppISetErrorString("Could not load or parse PPM header in %s.\n", lpName);
		return NULL;
    }
    
    // Create a surface of the given format using the dimensions of the PPM
    // file.
    
	
    memcpy(&format, lpFormat, sizeof(DDSURFACEDESC2));
    
	if (format.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8) 
	{
        bQuant = TRUE;
		psize = 256;
		pcaps = DDPCAPS_8BIT | DDPCAPS_ALLOW256;
    } 
	else if (format.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED4) 
	{
        bQuant = TRUE;
		psize = 16;
		pcaps = DDPCAPS_4BIT;
    }


	ZeroMemory(&ddsd, sizeof(DDSURFACEDESC2));

    ddsd.dwSize = sizeof(DDSURFACEDESC2);

    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
    ddsd.ddsCaps.dwCaps = memoryflag;  
	ddsd.ddsCaps.dwCaps2  = 0L;
	ddsd.dwHeight = dwHeight;
    ddsd.dwWidth  = dwWidth;
	ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
	 
	memcpy(&ddsd.ddpfPixelFormat, &lpFormat->ddpfPixelFormat, sizeof(DDPIXELFORMAT));

    ddrval = lpDD->CreateSurface(&ddsd, &lpDDS, NULL);
    if (ddrval != DD_OK) 
	{
        D3DAppISetErrorString("CreateSurface for texture failed (loadtex).\n%s",
			      D3DAppErrorToString(ddrval));
		return NULL;
    }
    

    // Lock the surface so it can be filled with the PPM file
     
    memset(&ddsd, 0, sizeof(DDSURFACEDESC2));
    ddsd.dwSize = sizeof(DDSURFACEDESC2);
	ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);

    ddrval = lpDDS->Lock(NULL, &ddsd, 0, NULL);
    
	if (ddrval != DD_OK) 
	{
		lpDDS->Release();
        D3DAppISetErrorString("Lock failed while loading surface (loadtex).\n%s",
			      D3DAppErrorToString(ddrval));
		return NULL;
    }
    
    // The method of loading depends on the pixel format of the dest surface
     
	memcpy(&format, &ddsd, sizeof(DDSURFACEDESC2));

    if (!bQuant) 
	{	
		// The texture surface is not palettized
	 
		unsigned long* lpLP;
		unsigned short* lpSP;
		unsigned char* lpCP;
		unsigned long m;
		int s;
		int red_shift, red_scale;
		int green_shift, green_scale;
		int blue_shift, blue_scale;

		// Determine the red, green and blue masks' shift and scale.
	 
        for (s = 0, m = format.ddpfPixelFormat.dwRBitMask; !(m & 1);
							       s++, m >>= 1);
        red_shift = s;
        red_scale = 255 / (format.ddpfPixelFormat.dwRBitMask >> s);
        for (s = 0, m = format.ddpfPixelFormat.dwGBitMask; !(m & 1);
							       s++, m >>= 1);
        green_shift = s;
        green_scale = 255 / (format.ddpfPixelFormat.dwGBitMask >> s);
        for (s = 0, m = format.ddpfPixelFormat.dwBBitMask; !(m & 1);
							       s++, m >>= 1);
        blue_shift = s;
        blue_scale = 255 / (format.ddpfPixelFormat.dwBBitMask >> s);
	
		// Each RGB bit count requires different pointers
	 
	switch (format.ddpfPixelFormat.dwRGBBitCount) 
	{
	    case 32 :
		for (j = 0; j < (int)dwHeight; j++) 
		{
		    // Point to next row in texture surface
		     
		    lpLP = (unsigned long*)(((char*)ddsd.lpSurface) + ddsd.lPitch * j);
		    for (i = 0; i < (int)dwWidth; i++) 
			{
				int r, g, b;
			
				// Read each value, scale it and shift it into position
			 
				r = getc(fp) / red_scale;
				g = getc(fp) / green_scale;
				b = getc(fp) / blue_scale;
				*lpLP = (r << red_shift) | (g << green_shift) |	(b << blue_shift);
				lpLP++;
		    }
		}
		break;

	    case 16 :
		for (j = 0; j < (int)dwHeight; j++) 
		{
		    lpSP = (unsigned short*)(((char*)ddsd.lpSurface) + (ddsd.lPitch) * j);
		    for (i = 0; i < (int)dwWidth; i++) 
			{
				int r, g, b;
				r = getc(fp) / red_scale;
				g = getc(fp) / green_scale;
				b = getc(fp) / blue_scale;
				*lpSP = (r << red_shift) | (g << green_shift) |	(b << blue_shift);
				lpSP++;
		    }
		}
		break;

	    case 8:
		for (j = 0; j < (int)dwHeight; j++) 
		{
		    lpCP = (unsigned char*)(((char*)ddsd.lpSurface) + ddsd.lPitch * j);
		    for (i = 0; i < (int)dwWidth; i++) 
			{
				int r, g, b;
				r = getc(fp) / red_scale;
				g = getc(fp) / green_scale;
				b = getc(fp) / blue_scale;
				*lpCP = (r << red_shift) | (g << green_shift) | (b << blue_shift);
				lpCP++;
		    }
		}
		break;
	    default:
		/*
		 * This wasn't a format I recognize
		 */
	    	lpDDS->Unlock(NULL);
		fclose(fp);
		lpDDS->Release();
                D3DAppISetErrorString("Unknown pixel format (loadtex).");
		return NULL;
	}
	/*
	 * Unlock the texture and return the surface pointer
	 */
	lpDDS->Unlock(NULL);
        fclose(fp);
        return (lpDDS);
    }

    /*
     * We assume the 8-bit palettized case
     */
    color_count = 0;	/* number of colors in the texture */
    for (j = 0; j < (int)dwHeight; j++) {
	/*
	 * Point to next row in surface
	 */
	lpC = ((char*)ddsd.lpSurface) + ddsd.lPitch * j;
	for (i = 0; i < (int)dwWidth; i++) {
	    int r, g, b, k;
	    /*
	     * Get the next red, green and blue values and turn them into a
	     * D3DCOLOR
	     */
	    r = getc(fp);
	    g = getc(fp);
	    b = getc(fp);
            c = RGB_MAKE(r, g, b);
	    /*
	     * Search for this color in a table of colors in this texture
	     */
            for (k = 0; k < color_count; k++)
                if (c == colors[k]) break;
            if (k == color_count) {
		/*
		 * This is a new color, so add it to the list
		 */
                color_count++;
		/*
		 * More than 256 and we fail (8-bit) 
		 */
                if (color_count > psize) {
		    color_count--;
		    k = color_count - 1;
                    //goto burst_colors;
		}
                colors[k] = c;
            }
	    /*
	     * Set the "pixel" value on the surface to be the index into the
	     * color table
	     */
	    if (psize == 16) {
		if ((i & 1) == 0)
		    *lpC = k & 0xf;
		else {
		    *lpC |= (k & 0xf) << 4;
		    lpC++;
		}
	    } else {
		*lpC = (char)k;
		lpC++;
	    }
        }
    }
    /*
     * Close the file and unlock the surface
     */
    fclose(fp);
    lpDDS->Unlock(NULL);

//burst_colors:
    if (color_count > psize) {
	/*
	 * If there are more than 256 colors, we overran our palette
	 */
        lpDDS->Unlock(NULL);
	lpDDS->Release();
        D3DAppISetErrorString("Palette burst. (loadtex).\n");
	return (NULL);
    }

    /*
     * Create a palette with the colors in our color table
     */
    memset(ppe, 0, sizeof(PALETTEENTRY) * 256);
    for (i = 0; i < color_count; i++) {
	ppe[i].peRed = (unsigned char)RGB_GETRED(colors[i]);
        ppe[i].peGreen = (unsigned char)RGB_GETGREEN(colors[i]);
	ppe[i].peBlue = (unsigned char)RGB_GETBLUE(colors[i]);
    }
    /*
     * Set all remaining entry flags to D3DPAL_RESERVED, which are ignored by
     * the renderer.
     */
    for (; i < 256; i++)
	ppe[i].peFlags = D3DPAL_RESERVED;
    /*
     * Create the palette with the DDPCAPS_ALLOW256 flag because we want to
     * have access to all entries.
     */
    ddrval = lpDD->CreatePalette(
					 DDPCAPS_INITIALIZE | pcaps,
					 ppe, &lpDDPal, NULL);
    if (ddrval != DD_OK) {
        lpDDS->Release();
        D3DAppISetErrorString("Create palette failed while loading surface (loadtex).\n%s",
			      D3DAppErrorToString(ddrval));
	return (NULL);
    }
    /*
     * Finally, bind the palette to the surface
     */
    ddrval = lpDDS->SetPalette(lpDDPal);
    if (ddrval != DD_OK) {
	lpDDS->Release();
	lpDDPal->Release();
        D3DAppISetErrorString("SetPalette failed while loading surface (loadtex).\n%s",
			      D3DAppErrorToString(ddrval));
	return (NULL);
    }

    lpDDPal->Release();

    return lpDDS;
}


static BOOL
ppm_getbyte(FILE *fp, char *newByte)
{
    char cchar;
    int cc;

    /* Get a byte, and dump comments */
    cchar = cc = getc(fp);
    if (cc == EOF) {
      return FALSE;
    }
  
    if (cchar == '#') {
	/* Read until next end of line */
	do {
	    cchar = cc = getc(fp);
	    if (cc == EOF)
		return FALSE;
	} while (cchar != '\n' && cchar != '\r');
    }

  *newByte = cchar;

  return TRUE;
}

static BOOL
ppm_getint(FILE *fp, int *newInt)
{
  int cint;
  char cchar;

  do {
    if (!ppm_getbyte(fp, &cchar)) return FALSE;
  } while (isspace(cchar));
  
  if (!isdigit(cchar)) {
    return FALSE;
  }
  
  cint = 0;
  
  do {
    cint = (cint * 10) + (cchar - '0');
    if (!ppm_getbyte(fp, &cchar)) return FALSE;
  } while(isdigit(cchar));

  *newInt = cint;

  return TRUE;
}
    
static BOOL loadPPMHeader(FILE *fp, DWORD *width, DWORD *height, int *maxgrey)
{
    char magic[MAGICBYTES], cchar;

    // Slurp up ppm header until we get width, height and maxgrey values 

    // Read and check the magic bytes 

    if (fread(magic, MAGICBYTES, 1, fp) != 1)
	return FALSE;
    
	//if (magic[0] != 'P' || magic[1] != '6')
	//return FALSE;

    /* Now we can actually read some numbers */
    if (!ppm_getint(fp, (int *)width))
	return FALSE;
    
	if (!ppm_getint(fp, (int *)height))
	return FALSE;
    
	if (!ppm_getint(fp, maxgrey))
	return FALSE;

    // Slurp up rest of white space so we get to actual data //
    do 
	{
		if (!ppm_getbyte(fp, &cchar))
			return FALSE;
    } 
	while (cchar == ' ' || cchar == '\t' || cchar == '\n' || cchar == '\r');

    fseek(fp, -1, SEEK_CUR);

    return TRUE;
}


/***************************************************************************/
/*                         Finding Textures                                */
/***************************************************************************/

#define MAXPATH    256
#define PATHSEP    ';'
#define FILESEP    '\\'
#define MAXCONTENTS 25
#define RESPATH     "Software\\Microsoft\\Direct3D"

static int PathListInitialised = FALSE;

/*
 * PathList structure
 * A list of directories in which to search for the texture.
 */
static struct {
    int count;
    char *contents[MAXCONTENTS];
} PathList;

/*
 * D3DAppIAddPathList
 * Add this string to the search path list
 */
static void 
D3DAppIAddPathList(const char *path)
{
    char *p;
    char *elt;
    int len;

    while (path) {
	p = LSTRCHR(path, PATHSEP);
	if (p)
	    len = p - path;
	else
	    len = lstrlen(path);
	elt = (char *) malloc(len + 1);
	if (elt == NULL)
	    return;
	lstrcpyn(elt, path, len + 1);
	elt[len] = '\0';
	PathList.contents[PathList.count] = elt;
	PathList.count++;
	if (p)
	    path = p + 1;
	else
	    path = NULL;
	if (PathList.count == MAXCONTENTS)
	    return;
    }
    return;
}

/*
 * D3DAppIInitialisePathList
 * Create a search path with the D3DPATH env. var and D3D Path registry entry
 */
static void 
D3DAppIInitialisePathList()
{
    long result;
    HKEY key;
    DWORD type, size;
    static char buf[512];
    char* path;

    if (PathListInitialised)
	return;
    PathListInitialised = TRUE;

    PathList.count = 0;
    path = getenv("D3DPATH");
    D3DAppIAddPathList(".");
    if (path != NULL) {
        D3DAppIAddPathList(path);
        return;
    }
    result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, RESPATH, 0, KEY_READ, &key);
    if (result == ERROR_SUCCESS) {
        size = sizeof(buf);
	result = RegQueryValueEx(key, "D3D Path", NULL, &type, (LPBYTE) buf,
	                         &size);
	RegCloseKey(key);
	if (result == ERROR_SUCCESS && type == REG_SZ)
	    D3DAppIAddPathList(buf);
    }
}


/*
 * D3DAppIFindFile
 * Find and open a file using the current search path.
 */
static FILE*
D3DAppIFindFile(const char *name, const char *mode)
{
    FILE *fp;
    char buf[MAXPATH];
    static char filesep[] = {FILESEP, 0};
    int i;

    D3DAppIInitialisePathList();

    fp = fopen(name, mode);
    if (fp != NULL)
	return fp;

    for (i = 0; i < PathList.count; i++) {
	lstrcpy(buf, PathList.contents[i]);
	lstrcat(buf, filesep);
	lstrcat(buf, name);
	fp = fopen(buf, mode);
	if (fp)
	    return fp;
    }
    return NULL;
}

/*
 * D3DAppIReleasePathList
 * Release the path list for program termination
 */
void
D3DAppIReleasePathList(void)
{
    int i;
    for (i = 0; i < PathList.count; i++) {
        free(PathList.contents[i]);
        PathList.contents[i] = NULL;
    }
    PathList.count = 0;
    PathListInitialised = FALSE;
}

int CheckValidTextureAlias(HWND hwnd, char *alias, world_ptr wptr)
{
	int i;
	char *buffer2;

	for(i = 0; i < wptr->number_of_tex_aliases-1; i++)
	{
		buffer2 = wptr->TexMap[i].tex_alias_name;

		if(strcmp(buffer2, alias)==0)
		{
			return i;
		}
	}
	MessageBox(hwnd,alias,NULL,MB_OK);
		
	return -1; //error
}

BOOL LoadTextures(HWND hwnd, char *filename , world_ptr wptr)
{
	FILE *fp;    
	char s[50];
	char p[100];
	int y_count=30;
	int done=0;
	int object_count=0;
	int vert_count=0;
	int pv_count=0;
	int poly_count=0;
	int tex_alias_counter = 0;
	int tex_counter = -1;
	int i;
	BOOL start_flag=TRUE;
	BOOL found;


    fp = fopen(filename,"r");
    if(fp==NULL)
    {     
		PrintMessage(hwnd, "ERROR can't open ", filename, SCN_AND_FILE);
        MessageBox(hwnd, filename,"Error can't open",MB_OK);
		return FALSE;
    }

	PrintMessage(hwnd, "Loading ", filename, SCN_AND_FILE);
	PrintMessage(hwnd, "Scanning texture info", NULL, LOGFILE_ONLY);
	PrintMessage(hwnd, "\n", NULL, LOGFILE_ONLY);
	
	for(i = 0; i < 100; i++)
		wptr->TexMap[i].is_alpha_texture = FALSE;

	while(done==0)
	{
		found = FALSE;
		fscanf( fp, "%s", &s );

		if(strcmp(s,"AddTexture")==0)
		{
			fscanf( fp, "%s", &p );
			PrintMessage(hwnd, "found AddTexture ", p, LOGFILE_ONLY);	
			tex_counter++;
			strcpy(&d3dappi.ImageFile[tex_counter][0], p);	
			d3dappi.NumTextures++;
			found = TRUE;		
		}

		if(strcmp(s,"Alias")==0)
		{
			fscanf( fp, "%s", &p );

			fscanf( fp, "%s", &p );
			strcpy((char *)wptr->TexMap[tex_alias_counter].tex_alias_name, (char *)&p);  

			wptr->TexMap[tex_alias_counter].texture = tex_counter;
			
			fscanf( fp, "%s", &p );			
			if(strcmp(p,"AlphaTransparent")==0)
				wptr->TexMap[tex_alias_counter].is_alpha_texture = TRUE;
			
			i = tex_alias_counter;
			fscanf( fp, "%s", &p ); 

			if(strcmp(p,"WHOLE")==0)
			{
				
				wptr->TexMap[i].tv[0] = (float)1.0;
				wptr->TexMap[i].tu[1] = (float)0.0;
				wptr->TexMap[i].tv[1] = (float)0.0;
				wptr->TexMap[i].tu[2] = (float)1.0;
				wptr->TexMap[i].tv[2] = (float)1.0;
				wptr->TexMap[i].tu[3] = (float)1.0;
				wptr->TexMap[i].tv[3] = (float)0.0;

			}

			if(strcmp(p,"TL_QUAD")==0)
			{
				wptr->TexMap[i].tu[0] = (float)0.0;
				wptr->TexMap[i].tv[0] = (float)0.5;
				wptr->TexMap[i].tu[1] = (float)0.0;
				wptr->TexMap[i].tv[1] = (float)0.0;
				wptr->TexMap[i].tu[2] = (float)0.5;
				wptr->TexMap[i].tv[2] = (float)0.5;
				wptr->TexMap[i].tu[3] = (float)0.5;
				wptr->TexMap[i].tv[3] = (float)0.0;
			}

			if(strcmp(p,"TR_QUAD")==0)
			{
				wptr->TexMap[i].tu[0] = (float)0.5;
				wptr->TexMap[i].tv[0] = (float)0.5;
				wptr->TexMap[i].tu[1] = (float)0.5;
				wptr->TexMap[i].tv[1] = (float)0.0;
				wptr->TexMap[i].tu[2] = (float)1.0;
				wptr->TexMap[i].tv[2] = (float)0.5;
				wptr->TexMap[i].tu[3] = (float)1.0;
				wptr->TexMap[i].tv[3] = (float)0.0;
			}
			if(strcmp(p,"LL_QUAD")==0)
			{
				wptr->TexMap[i].tu[0] = (float)0.0;
				wptr->TexMap[i].tv[0] = (float)0.99;
				wptr->TexMap[i].tu[1] = (float)0.0;
				wptr->TexMap[i].tv[1] = (float)0.50;
				wptr->TexMap[i].tu[2] = (float)0.5;
				wptr->TexMap[i].tv[2] = (float)0.99;
				wptr->TexMap[i].tu[3] = (float)0.5;
				wptr->TexMap[i].tv[3] = (float)0.50;
			}
			if(strcmp(p,"LR_QUAD")==0)
			{
				wptr->TexMap[i].tu[0] = (float)0.5;
				wptr->TexMap[i].tv[0] = (float)1.0;
				wptr->TexMap[i].tu[1] = (float)0.5;
				wptr->TexMap[i].tv[1] = (float)0.5;
				wptr->TexMap[i].tu[2] = (float)1.0;
				wptr->TexMap[i].tv[2] = (float)1.0;
				wptr->TexMap[i].tu[3] = (float)1.0;
				wptr->TexMap[i].tv[3] = (float)0.5;
			}
			if(strcmp(p,"TOP_HALF")==0)
			{
				wptr->TexMap[i].tu[0] = (float)0.0;
				wptr->TexMap[i].tv[0] = (float)0.5;
				wptr->TexMap[i].tu[1] = (float)0.0;
				wptr->TexMap[i].tv[1] = (float)0.0;
				wptr->TexMap[i].tu[2] = (float)1.0;
				wptr->TexMap[i].tv[2] = (float)0.5;
				wptr->TexMap[i].tu[3] = (float)1.0;
				wptr->TexMap[i].tv[3] = (float)0.0;
			}
			if(strcmp(p,"BOT_HALF")==0)
			{
				wptr->TexMap[i].tu[0] = (float)0.0;
				wptr->TexMap[i].tv[0] = (float)1.0;
				wptr->TexMap[i].tu[1] = (float)0.0;
				wptr->TexMap[i].tv[1] = (float)0.5;
				wptr->TexMap[i].tu[2] = (float)1.0;
				wptr->TexMap[i].tv[2] = (float)1.0;
				wptr->TexMap[i].tu[3] = (float)1.0;
				wptr->TexMap[i].tv[3] = (float)0.5;
			}
			if(strcmp(p,"LEFT_HALF")==0)
			{
				wptr->TexMap[i].tu[0] = (float)0.0;
				wptr->TexMap[i].tv[0] = (float)1.0;
				wptr->TexMap[i].tu[1] = (float)0.0;
				wptr->TexMap[i].tv[1] = (float)0.0;
				wptr->TexMap[i].tu[2] = (float)0.5;
				wptr->TexMap[i].tv[2] = (float)1.0;
				wptr->TexMap[i].tu[3] = (float)0.5;
				wptr->TexMap[i].tv[3] = (float)0.0;
			}
			if(strcmp(p,"RIGHT_HALF")==0)
			{
				wptr->TexMap[i].tu[0] = (float)0.5;
				wptr->TexMap[i].tv[0] = (float)1.0;
				wptr->TexMap[i].tu[1] = (float)0.5;
				wptr->TexMap[i].tv[1] = (float)0.0;
				wptr->TexMap[i].tu[2] = (float)1.0;
				wptr->TexMap[i].tv[2] = (float)1.0;
				wptr->TexMap[i].tu[3] = (float)1.0;
				wptr->TexMap[i].tv[3] = (float)0.0;
			}
			if(strcmp(p,"TL_TRI")==0)
			{
				wptr->TexMap[i].tu[0] = (float)0.0;
				wptr->TexMap[i].tv[0] = (float)1.0;
				wptr->TexMap[i].tu[1] = (float)0.0;
				wptr->TexMap[i].tv[1] = (float)0.0;
				wptr->TexMap[i].tu[2] = (float)1.0;
				wptr->TexMap[i].tv[2] = (float)1.0;
				wptr->TexMap[i].tu[3] = (float)1.0;
				wptr->TexMap[i].tv[3] = (float)0.0;
			}
			if(strcmp(p,"BR_TRI")==0)
			{

			}
			
			tex_alias_counter++;
			found = TRUE;
		}

		if(strcmp(s,"END_FILE")==0)
		{
			done=1;
			found = TRUE;
			PrintMessage(hwnd, "\n", NULL, LOGFILE_ONLY);
			wptr->number_of_tex_aliases = tex_alias_counter;
		}

		if(found == FALSE)
		{
			PrintMessage(hwnd, "File Error: Syntax problem :", p, SCN_AND_FILE);
			MessageBox(hwnd, "p","File Error: Syntax problem ",MB_OK);
			return FALSE;
		}
			
		
	}
	fclose(fp);	
	

	PrintMessage(hwnd, "Loading Textures into memory", NULL, SCN_AND_FILE);
			
	return TRUE;
}


// ******* START new code Added 2nd July 1999 *******
// Check amount of memory available for textures, and set Texture size

BOOL SetTextureSize()
{
	LPDIRECTDRAW7 lpDD7;
	DDSCAPS2    ddsCaps2; 
	DWORD       total_memory_for_textures; 
	DWORD       free_memory_for_textures;
	DWORD		free_tex_mem, mem_needed;
	DWORD		total_tex_mem;
	HRESULT     hr; 
	char  buffer[255], buffer2[256];

	hr = d3dappi.lpDD7->QueryInterface(IID_IDirectDraw7, (void **)&lpDD7); 
	if (FAILED(hr))
		return FALSE; 
 
	// Initialize the structure.
	ZeroMemory(&ddsCaps2, sizeof(ddsCaps2));
 
	ddsCaps2.dwCaps = DDSCAPS_TEXTURE; 
	hr = lpDD7->GetAvailableVidMem(&ddsCaps2, &total_memory_for_textures, &free_memory_for_textures); 
	if (FAILED(hr))
		return FALSE;

	
	free_tex_mem = free_memory_for_textures / 1024;
	total_tex_mem = total_memory_for_textures / 1024;

	mem_needed = (d3dappi.NumTextures + 1) * 128;

	itoa(d3dappi.NumTextures, buffer, 10);
	PrintMessage(NULL, "Working out texture size, NumTextures = ", buffer, LOGFILE_ONLY);

	strcpy(buffer, "Memory needed for textures + background = ");
	itoa(mem_needed, buffer2, 10);
	strcat(buffer, buffer2);
	PrintMessage(NULL, buffer, " KB", LOGFILE_ONLY);

	strcpy(buffer, "Free memory available for textures = ");
	itoa(free_tex_mem, buffer2, 10);
	strcat(buffer, buffer2);
	PrintMessage(NULL, buffer, " KB", LOGFILE_ONLY);

	strcpy(buffer, "Total memory available for textures = ");
	itoa(total_tex_mem, buffer2, 10);
	strcat(buffer, buffer2);
	PrintMessage(NULL, buffer, " KB", LOGFILE_ONLY);

	// Test if texture size auto mode has been set
	// If so, work out best texture size to use.
	// If not then leave at default setting.

	if(setupinfo.TextureSize == 0) 
	{
		if(free_tex_mem >= mem_needed) 
			setupinfo.TextureSize = 256;
		else
		{
			mem_needed = mem_needed / 4;
			if(free_tex_mem > mem_needed) 
				setupinfo.TextureSize = 128;
			else
			{
				mem_needed = mem_needed / 4;
				if(free_tex_mem > mem_needed) 
					setupinfo.TextureSize = 64;
				else
					setupinfo.TextureSize = 32;
			}
		}
	}

	itoa(setupinfo.TextureSize, buffer, 10);
	PrintMessage(NULL, "Texture size = ", buffer, LOGFILE_ONLY);
	PrintMessage(NULL, "\n", NULL, LOGFILE_ONLY);

return TRUE;
}

// ******* END new code Added 2nd July 1999 *******