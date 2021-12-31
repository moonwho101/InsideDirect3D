
// Road Rage 1.1

// FILE : d3dcalls.cpp
// This file was originally part of the Microsoft DirectX5 SDK
// Edited by William Chin
// Edited with MSDEV C++ v6.0 using the DirectX 6.0 SDK
// Edited on:  3/7/99


//**************************************************************************
//                            Creation of D3D                              *
//**************************************************************************

#include "d3dappi.hpp"
#include "d3dmain.hpp"


extern d3dmainglobals myglobs;

static HRESULT WINAPI EnumZBufferFormatsCallback( LPDDPIXELFORMAT pddpf,
                                                  VOID* pddpfDesired );
void PrintPixelFormatFlags(DWORD dwFlags);

/**** ENUMERATE Z BUFFER *********************************/


BOOL D3DAppIEnumZbuffer(void)
{
	DDPIXELFORMAT ddpfZBuffer;


    RELEASE(d3dappi.lpZBuffer);
     
	// Test if this driver does z buffering
    if (!d3dappi.Driver[d3dappi.CurrDriver].bDoesZBuffer)
		return TRUE;


	ZeroMemory( (VOID*)&ddpfZBuffer, sizeof(DDPIXELFORMAT) );
  
	ddpfZBuffer.dwSize = sizeof(DDPIXELFORMAT);
    ddpfZBuffer.dwFlags = DDPF_ZBUFFER;


    // Get an appropiate pixel format from enumeration of the formats.
   
	LastError = d3dappi.lpD3D->EnumZBufferFormats( d3dappi.Driver[d3dappi.CurrDriver].Guid, 
		                EnumZBufferFormatsCallback, (VOID*)&ddpfZBuffer );
     
	if (LastError != DD_OK) 
	{
		D3DAppISetErrorString("EnumZBufferFormats failed\n%s",
			      D3DAppErrorToString(LastError));

		return FALSE;
	}

    if( sizeof(DDPIXELFORMAT) != ddpfZBuffer.dwSize )
    {
        MessageBox(myglobs.hWndMain, "Device doesn't support requested zbuffer format", NULL, MB_OK );
        return FALSE;
    }

	memcpy(&d3dappi.ddpfZBuffer, &ddpfZBuffer, sizeof(DDPIXELFORMAT) );

	return TRUE;
}



static HRESULT WINAPI EnumZBufferFormatsCallback( LPDDPIXELFORMAT pddpf,
                                                  VOID* pddpfDesired )
{
	FILE *fp;

    if( NULL == pddpf || NULL == pddpfDesired )
	{
		MessageBox(myglobs.hWndMain, "EnumZBufferFormatsCallback : NULL Error in DDPIXELFORMAT", NULL, MB_OK);
        return D3DENUMRET_CANCEL;
	}

    // If the current pixel format's match the desired ones (DDPF_ZBUFFER and
    // possibly DDPF_STENCILBUFFER), lets copy it and return. This function is
    // not choosy...it accepts the first valid format that comes along.

	fp = fopen("rrlogfile.txt","a");
	
	fprintf( fp, "Enumerating Z buffer pixel formats found :\n");
	fprintf( fp, "pixel format dwFlags = %d\n", pddpf->dwFlags);
	fprintf( fp, "Z buffer depth       = %d\n\n", pddpf->dwZBufferBitDepth);

	fclose(fp);

    if( pddpf->dwFlags == ((DDPIXELFORMAT*)pddpfDesired)->dwFlags )
    {
        memcpy( pddpfDesired, pddpf, sizeof(DDPIXELFORMAT) );

		// We're happy with a 16-bit z-buffer. Otherwise, keep looking.
		if( pddpf->dwZBufferBitDepth == 16 )
		{
			//MessageBox(myglobs.hWndMain, "found it",NULL,MB_OK);
			return D3DENUMRET_CANCEL;
		}
    }

    return D3DENUMRET_OK;
}




BOOL D3DAppICreateD3D(void)
{
    LastError = d3dappi.lpDD7->QueryInterface(
				    IID_IDirect3D7, (LPVOID*)&d3dappi.lpD3D);

    if (LastError != DD_OK) 
	{
		D3DAppISetErrorString("Creation of IDirect3D failed\n%s",
			      D3DAppErrorToString(LastError));

		return FALSE;
    }
    return TRUE;
}

//**************************************************************************
//                           D3D Device Enumeration                        *
//**************************************************************************
//
// enumDeviceFunc
// Device enumeration callback.  Record information about the D3D device
// reported by D3D.
//
static BOOL bRampHasEnumerated = FALSE;

static HRESULT WINAPI enumDeviceFunc(LPSTR lpDeviceDescription,
		      LPSTR lpDeviceName, LPD3DDEVICEDESC7 lpDesc,
		      LPVOID lpContext)
{
	FILE *fp;

    lpContext = lpContext;
    
    // Don't accept any hardware D3D devices if emulation only option is set    
    // Record the D3D driver's inforamation  
    memcpy(&d3dappi.Driver[d3dappi.NumDrivers].Guid, &lpDesc->deviceGUID, sizeof(GUID));
    lstrcpy(d3dappi.Driver[d3dappi.NumDrivers].About, lpDeviceDescription);
    lstrcpy(d3dappi.Driver[d3dappi.NumDrivers].Name, lpDeviceName);
    
    // Is this a hardware device or software emulation?  Checking the color
    // model for a valid model works.
    
	d3dappi.Driver[d3dappi.NumDrivers].bIsHardware = lpDesc->dwDevCaps & D3DDEVCAPS_HWRASTERIZATION;
	memcpy(&d3dappi.Driver[d3dappi.NumDrivers].Desc, lpDesc,
	      sizeof(D3DDEVICEDESC7));
    
    // Does this driver do texture mapping?    
    d3dappi.Driver[d3dappi.NumDrivers].bDoesTextures =
	(d3dappi.Driver[d3dappi.NumDrivers].Desc.dpcTriCaps.dwTextureCaps &
	 D3DPTEXTURECAPS_PERSPECTIVE) ? TRUE : FALSE;
    
    // Can this driver use a z-buffer?
    d3dappi.Driver[d3dappi.NumDrivers].bDoesZBuffer =
	d3dappi.Driver[d3dappi.NumDrivers].Desc.dwDeviceZBufferBitDepth
		? TRUE : FALSE;
    
    // Can this driver render to the Windows display depth
    d3dappi.Driver[d3dappi.NumDrivers].bCanDoWindow =
	(d3dappi.Driver[d3dappi.NumDrivers].Desc.dwDeviceRenderBitDepth &
	 D3DAppIBPPToDDBD(d3dappi.WindowsDisplay.bpp)) ? TRUE : FALSE;
    
	if (!d3dappi.bIsPrimary)
		d3dappi.Driver[d3dappi.NumDrivers].bCanDoWindow = FALSE;

	fp = fopen("rrlogfile.txt","a");
	
	fprintf( fp, "Enumerating D3D drivers, found :\n");
	fprintf( fp, "D3D Device Description = %s\n", lpDeviceDescription);
	fprintf( fp, "D3D Device Name        = %s\n\n", lpDeviceName);

	fclose(fp);

    d3dappi.NumDrivers++;

    if (d3dappi.NumDrivers == D3DAPP_MAXD3DDRIVERS)
        return (D3DENUMRET_CANCEL);
    
	return (D3DENUMRET_OK);
}



// D3DAppIEnumDevices
// Get the available drivers from Direct3D by enumeration.

BOOL D3DAppIEnumDevices(void)
{
    d3dappi.NumDrivers = 0;
    LastError = d3dappi.lpD3D->EnumDevices(enumDeviceFunc, NULL);
    if (LastError != DD_OK) 
	{
        D3DAppISetErrorString("Enumeration of drivers failed.\n%s",
			      D3DAppErrorToString(LastError));
		return FALSE;
    }
    d3dappi.CurrDriver = 0;
    return TRUE;
}


static HRESULT CALLBACK EnumTextureFormatsCallback(LPDDPIXELFORMAT lpDDSD,
												   LPVOID lpContext)
{
    unsigned long m;
    int r = 0;
	int g = 0;
	int b = 0; 
	FILE *fp;
    

	PrintMessage(NULL, "Enumerating Texture Formats, found :", NULL, LOGFILE_ONLY);


    // Record the DDSURFACEDESC of this texture format
    
    memset(&d3dappi.TextureFormat[d3dappi.NumTextureFormats], 0,
	   sizeof(D3DAppTextureFormat));
    

	PrintPixelFormatFlags(lpDDSD->dwFlags);

	fp = fopen("rrlogfile.txt","a");

	if (lpDDSD->dwFlags & DDPF_ALPHAPIXELS) 
	{	
		d3dappi.TextureFormat[d3dappi.NumTextureFormats].bAlphaPixels = TRUE;
		fprintf( fp, " ALPHAPIXELS\n");
	} 
	else
		d3dappi.TextureFormat[d3dappi.NumTextureFormats].bAlphaPixels = FALSE;

    // Is this format palettized?  How many bits?  Otherwise, how many RGB
    // bits?
    
    if (lpDDSD->dwFlags & DDPF_PALETTEINDEXED8) 
	{
			d3dappi.TextureFormat[d3dappi.NumTextureFormats].bPalettized = TRUE;
			d3dappi.TextureFormat[d3dappi.NumTextureFormats].IndexBPP = 8;
			fprintf( fp, " 8 bit palettised\n");
			goto end;
	}
	
	if (lpDDSD->dwFlags & DDPF_PALETTEINDEXED4) 
	{
			d3dappi.TextureFormat[d3dappi.NumTextureFormats].bPalettized = TRUE;
			d3dappi.TextureFormat[d3dappi.NumTextureFormats].IndexBPP = 4;
			fprintf( fp, " 4 bit palettised\n");
			goto end;
	}

	if(lpDDSD->dwFlags & DDPF_RGB)	
	{				
		if( (lpDDSD->dwRBitMask > 0) &&
			(lpDDSD->dwGBitMask > 0) &&
			(lpDDSD->dwBBitMask > 0) )
		{
			d3dappi.TextureFormat[d3dappi.NumTextureFormats].bPalettized = FALSE;
			d3dappi.TextureFormat[d3dappi.NumTextureFormats].IndexBPP = 0;
		
			for (r = 0, m = lpDDSD->dwRBitMask; !(m & 1); r++, m >>= 1);
			for (r = 0; m & 1; r++, m >>= 1);

			for (g = 0, m = lpDDSD->dwGBitMask; !(m & 1); g++, m >>= 1);
			for (g = 0; m & 1; g++, m >>= 1);

			for (b = 0, m = lpDDSD->dwBBitMask; !(m & 1); b++, m >>= 1);
			for (b = 0; m & 1; b++, m >>= 1);
		
			d3dappi.TextureFormat[d3dappi.NumTextureFormats].RedBPP = r;
			d3dappi.TextureFormat[d3dappi.NumTextureFormats].GreenBPP = g;
			d3dappi.TextureFormat[d3dappi.NumTextureFormats].BlueBPP = b;

			fprintf( fp, " RGB = %d %d %d\n", r,g,b);
			fprintf( fp, " dwRBitMask = %d\n", lpDDSD->dwRBitMask);
			fprintf( fp, " dwGBitMask = %d\n", lpDDSD->dwGBitMask);
			fprintf( fp, " dwBBitMask = %d\n", lpDDSD->dwBBitMask);
		}
	}

end :

	
	memcpy(&d3dappi.TextureFormat[d3dappi.NumTextureFormats].ddsd.ddpfPixelFormat, 
			lpDDSD, sizeof(DDPIXELFORMAT));

    d3dappi.NumTextureFormats++;

	fprintf( fp, "--------------------\n\n");
	fclose(fp);
    return DDENUMRET_OK;
}


// D3DAppIEnumTextureFormats
// Get a list of available texture map formats from the Direct3D driver by
// enumeration.  Choose a default format (paletted is prefered).

BOOL D3DAppIEnumTextureFormats(void)
{
    int StartFormat = 0;
	int r,g,b;
	int i;
	BOOL found_desired_texture_mode = FALSE;
	FILE *fp;
    
    
    d3dappi.NumTextureFormats = 0;

    LastError = d3dappi.lpD3DDevice->EnumTextureFormats(EnumTextureFormatsCallback,
				 (LPVOID)&StartFormat);

    if (LastError != DD_OK) 
	{
		D3DAppISetErrorString("Enumeration of texture formats failed.\n%s",
			      D3DAppErrorToString(LastError));
		return FALSE;
    }

	for(i = 0; i < d3dappi.NumTextureFormats; i++)
	{
		if( (d3dappi.TextureFormat[i].ddsd.ddpfPixelFormat.dwRBitMask == d3dappi.ddpfBackBuffer.dwRBitMask) &&
			(d3dappi.TextureFormat[i].ddsd.ddpfPixelFormat.dwGBitMask == d3dappi.ddpfBackBuffer.dwGBitMask) &&
			(d3dappi.TextureFormat[i].ddsd.ddpfPixelFormat.dwBBitMask == d3dappi.ddpfBackBuffer.dwBBitMask) &&
			(d3dappi.TextureFormat[i].ddsd.ddpfPixelFormat.dwFlags == d3dappi.ddpfBackBuffer.dwFlags) )
		{
			StartFormat = i;
			found_desired_texture_mode = TRUE;
		}
	}

	
	if(found_desired_texture_mode == FALSE)
	{
		PrintMessage(NULL, "using first texture format found\n", NULL, LOGFILE_ONLY);
		StartFormat = 0;
	}

    memcpy(&d3dappi.ThisTextureFormat, &d3dappi.TextureFormat[StartFormat],
	   sizeof(D3DAppTextureFormat));

    d3dappi.CurrTextureFormat = StartFormat;

	memcpy(&d3dappi.ddpfTextureFormats, 
			&d3dappi.ThisTextureFormat.ddsd.ddpfPixelFormat, 
			sizeof(DDPIXELFORMAT));

	r = d3dappi.ThisTextureFormat.RedBPP;
	g = d3dappi.ThisTextureFormat.GreenBPP;
	b =	d3dappi.ThisTextureFormat.BlueBPP;

	fp = fopen("rrlogfile.txt","a");
	fprintf( fp, "Using this texture format :\n");
	fprintf( fp, "RGB mode: %d %d %d\n",r,g,b);
	fprintf( fp, "Palettized: %d\n",d3dappi.ThisTextureFormat.bPalettized);
	fprintf( fp, "IndexBPP: %d\n",d3dappi.ThisTextureFormat.IndexBPP);
	fprintf( fp, "AlphaPixels: %d\n",d3dappi.ThisTextureFormat.bAlphaPixels);

	fprintf( fp, "D3DAppIEnumTextureFormats: suceeded\n\n");
	
	fclose(fp);

    return TRUE;
}


// D3DAppICreateDevice
// Create the D3D device and enumerate the texture formats

BOOL D3DAppICreateDevice(int driver)
{
    RELEASE(d3dappi.lpD3DDevice);

    if (d3dappi.Driver[driver].bIsHardware && !d3dappi.bBackBufferInVideo) 
	{
		D3DAppISetErrorString("Could not fit the rendering surfaces in video memory for this hardware device.\n");
		goto exit_with_error;
    }

    d3dappi.CurrDriver = driver;
    memcpy(&d3dappi.ThisDriver, &d3dappi.Driver[driver], sizeof(D3DAppD3DDriver));

    LastError = d3dappi.lpD3D->CreateDevice( d3dappi.Driver[driver].Guid,
        d3dappi.lpBackBuffer, &d3dappi.lpD3DDevice);

    if (LastError != DD_OK) 
	{
    	D3DAppISetErrorString("Create D3D device failed.\n%s",
			      D3DAppErrorToString(LastError));
        goto exit_with_error;
    }

    d3dappi.CurrDriver = driver;

    d3dappi.NumTextureFormats = 0;

    if (d3dappi.Driver[driver].bDoesTextures) 
	{
		if (!D3DAppIEnumTextureFormats())
			goto exit_with_error;
    }

    return TRUE;

exit_with_error:
    RELEASE(d3dappi.lpD3DDevice);
    return FALSE;
}


// D3DAppISetRenderState
// Create and execute an execute buffer which will set the render state and
// light state for the current viewport.

BOOL D3DAppISetRenderState()
{
    LPDIRECT3DDEVICE7 pDev = d3dappi.lpD3DDevice;

    LastError = pDev->BeginScene();
    if (LastError != D3D_OK) 
	{
        D3DAppISetErrorString("BeginScene failed in SetRenderState.\n%s",
                              D3DAppErrorToString(LastError));
        goto exit_with_error;
    }

    // Set render state
    
    pDev->SetRenderState(D3DRENDERSTATE_SHADEMODE, (DWORD)d3dapprs.ShadeMode);
    pDev->SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)d3dapprs.bPerspCorrect);
    pDev->SetRenderState(D3DRENDERSTATE_ZENABLE, (DWORD)(d3dapprs.bZBufferOn &&
                                  d3dappi.ThisDriver.bDoesZBuffer));
    pDev->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, (DWORD)d3dapprs.bZBufferOn);
    pDev->SetRenderState(D3DRENDERSTATE_ZFUNC, D3DCMP_LESSEQUAL);
    pDev->SetRenderState(D3DRENDERSTATE_TEXTUREMAG, (DWORD)d3dapprs.TextureFilter);
    pDev->SetRenderState(D3DRENDERSTATE_TEXTUREMIN, (DWORD)d3dapprs.TextureFilter);
    pDev->SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, (DWORD)d3dapprs.TextureBlend);
    pDev->SetRenderState(D3DRENDERSTATE_FILLMODE, (DWORD)d3dapprs.FillMode);
    pDev->SetRenderState(D3DRENDERSTATE_DITHERENABLE, (DWORD)d3dapprs.bDithering);
    pDev->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, (DWORD)d3dapprs.bSpecular);
    pDev->SetRenderState(D3DRENDERSTATE_ANTIALIAS, (DWORD)d3dapprs.bAntialiasing);
    pDev->SetRenderState(D3DRENDERSTATE_FOGENABLE, (DWORD)d3dapprs.bFogEnabled);
    pDev->SetRenderState(D3DRENDERSTATE_FOGCOLOR, (DWORD)d3dapprs.FogColor);
    
    // Set light state
    
    pDev->SetRenderState(D3DRENDERSTATE_FOGENABLE, (DWORD)d3dapprs.bFogEnabled ?
                 TRUE : FALSE);
    pDev->SetRenderState(D3DRENDERSTATE_FOGSTART, *(unsigned long*)&d3dapprs.FogStart);
    pDev->SetRenderState(D3DRENDERSTATE_FOGEND, *(unsigned long*)&d3dapprs.FogEnd);
	

    LastError = pDev->EndScene();
    if (LastError != D3D_OK) 
	{
        D3DAppISetErrorString("EndScene failed in SetRenderState.\n%s",
			      D3DAppErrorToString(LastError));
		goto exit_with_error;
    }
	return TRUE;
exit_with_error:
    return FALSE;
}

void PrintPixelFormatFlags(DWORD dwFlags)
{

	if(dwFlags & DDPF_ALPHA)
		PrintMessage(NULL, "DDPF_ALPHA", NULL, LOGFILE_ONLY);

	if(dwFlags & DDPF_ALPHAPIXELS)
		PrintMessage(NULL, "DDPF_ALPHAPIXELS", NULL, LOGFILE_ONLY);

	if(dwFlags & DDPF_ALPHAPREMULT)
		PrintMessage(NULL, "DDPF_ALPHAPREMULT", NULL, LOGFILE_ONLY);

	if(dwFlags & DDPF_BUMPLUMINANCE)
		PrintMessage(NULL, "DDPF_BUMPLUMINANCE", NULL, LOGFILE_ONLY);

	if(dwFlags & DDPF_BUMPDUDV)
		PrintMessage(NULL, "DDPF_BUMPDUDV", NULL, LOGFILE_ONLY);

	if(dwFlags & DDPF_COMPRESSED)
		PrintMessage(NULL, "DDPF_COMPRESSED", NULL, LOGFILE_ONLY);

	if(dwFlags & DDPF_FOURCC)
		PrintMessage(NULL, "DDPF_FOURCC", NULL, LOGFILE_ONLY);

	if(dwFlags & DDPF_LUMINANCE)
		PrintMessage(NULL, "DDPF_LUMINANCE", NULL, LOGFILE_ONLY);

	if(dwFlags & DDPF_PALETTEINDEXED1)
		PrintMessage(NULL, "DDPF_PALETTEINDEXED1", NULL, LOGFILE_ONLY);

	if(dwFlags & DDPF_PALETTEINDEXED2)
		PrintMessage(NULL, "DDPF_PALETTEINDEXED2", NULL, LOGFILE_ONLY);

	if(dwFlags & DDPF_PALETTEINDEXED4)
		PrintMessage(NULL, "DDPF_PALETTEINDEXED4", NULL, LOGFILE_ONLY);

	if(dwFlags & DDPF_PALETTEINDEXED8)
		PrintMessage(NULL, "DDPF_PALETTEINDEXED8", NULL, LOGFILE_ONLY);

	if(dwFlags & DDPF_PALETTEINDEXEDTO8)
		PrintMessage(NULL, "DDPF_PALETTEINDEXEDTO8", NULL, LOGFILE_ONLY);

	if(dwFlags & DDPF_RGB)
		PrintMessage(NULL, "DDPF_RGB", NULL, LOGFILE_ONLY);

	if(dwFlags & DDPF_RGBTOYUV)
		PrintMessage(NULL, "DDPF_RGBTOYUV", NULL, LOGFILE_ONLY);

	if(dwFlags & DDPF_STENCILBUFFER)
		PrintMessage(NULL, "DDPF_STENCILBUFFER", NULL, LOGFILE_ONLY);

	if(dwFlags & DDPF_YUV)
		PrintMessage(NULL, "DDPF_YUV", NULL, LOGFILE_ONLY);

	if(dwFlags & DDPF_ZBUFFER)
		PrintMessage(NULL, "DDPF_ZBUFFER", NULL, LOGFILE_ONLY);

	if(dwFlags & DDPF_ZPIXELS)
		PrintMessage(NULL, "DDPF_ZPIXELS", NULL, LOGFILE_ONLY);

}