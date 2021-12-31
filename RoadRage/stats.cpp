
// Road Rage 1.1

// FILE : stats.cpp
// This file was originally part of the Microsoft DirectX5 SDK
// Edited by William Chin
// Edited with MSDEV C++ v6.0 using the DirectX 6.0 SDK
// Edited on:  3/7/99


#include "d3dmain.hpp"

/*
 * GLOBAL VARIABLES
 */
extern D3DAppInfo* d3dapp;	    /* Pointer to read only collection of DD and D3D
			       objects maintained by D3DApp */
extern d3dmainglobals myglobs;	    /* collection of global variables */

static struct {
    HFONT hFont;
    SIZE szFrameRate;
    SIZE szInfo;
} statglobs;

extern num_packets_sent;
extern num_packets_received;

 /************************************************************************
  Frame rate and info text
 ************************************************************************/

BOOL DDTextOut(char *msg)
{
    HRESULT LastError;
    HDC hdc;
    RECT rc;
    char buf[60];
    int len;

    if (!myglobs.lpFrameRateBuffer)
	return TRUE;
	
	len = wsprintf(buf, msg);

	if (!myglobs.lpFrameRateBuffer)
        return FALSE;
    
	LastError = myglobs.lpFrameRateBuffer->GetDC(&hdc);
	//LastError = d3dapp->lpBackBuffer->GetDC(&hdc);
    if (LastError != DD_OK) 
	{	
		// If this fails, it's not vital, so don't report an error.
		return TRUE;
    }

    SelectObject(hdc, statglobs.hFont);
    SetTextColor(hdc, RGB(255,255,0));
    SetBkColor(hdc, RGB(0,0,0));
    SetBkMode(hdc, OPAQUE);
    GetTextExtentPoint32(hdc, buf, len, &statglobs.szFrameRate);
    SetRect(&rc, 0, 0, 0, 10);
	//TextOut(hdc,0,90,"Bollocks",8);
    ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rc, buf, len, NULL);
    myglobs.lpFrameRateBuffer->ReleaseDC(hdc);
	
	//d3dapp->lpBackBuffer->ReleaseDC(hdc);
    return TRUE;
}

BOOL WriteFrameRateBuffer(float fps, int rx, int tx)
{
    HRESULT LastError;
    HDC hdc;
    RECT rc;
    char buf1[30], buf2[30], buf3[30], buf[90];
    int len;

    if (!myglobs.lpFrameRateBuffer)
	return TRUE;
	
    
    wsprintf(buf1, "%d.%02d fps   ", (int)( fps * 100 ) / 100, (int)( fps * 100 ) % 100);
    
    wsprintf(buf2, "rx = %ld  ", rx);
	wsprintf(buf3, "tx = %ld  ", tx);
    
    len = wsprintf(buf, "%s%s%s", buf1, buf2, buf3);
    
	if (!myglobs.lpFrameRateBuffer)
        return FALSE;
    
	
	LastError = myglobs.lpFrameRateBuffer->GetDC(&hdc);
    if (LastError != DD_OK) 
	{	
		// If this fails, it's not vital, so don't report an error.
		return TRUE;
    }

    SelectObject(hdc, statglobs.hFont);
    SetTextColor(hdc, RGB(255,255,0));
    SetBkColor(hdc, RGB(0,0,0));
    SetBkMode(hdc, OPAQUE);
    GetTextExtentPoint32(hdc, buf, len, &statglobs.szFrameRate);
    SetRect(&rc, 0, 0, statglobs.szFrameRate.cx, statglobs.szFrameRate.cy);
    ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rc, buf, len, NULL);
    myglobs.lpFrameRateBuffer->ReleaseDC(hdc);

    return TRUE;
}

BOOL
WriteInfoBuffer(void)
{
    HRESULT LastError;
    HDC hdc;
    RECT rc;
    char buf[40];
    int len=0;

    if (!myglobs.lpInfoBuffer)
	return TRUE;
    if (!myglobs.lpInfoBuffer)
        return FALSE;
    LastError = myglobs.lpInfoBuffer->GetDC(&hdc);
    if (LastError != DD_OK) {
	/*
	 * If this fails, it's not vital, so don't report an error.
	 */
	return TRUE;
    }

    SelectObject(hdc, statglobs.hFont);
    SetTextColor(hdc, RGB(255,255,0));
    SetBkColor(hdc, RGB(0,0,0));
    SetBkMode(hdc, OPAQUE);
    GetTextExtentPoint32(hdc, buf, len, &statglobs.szInfo);
    SetRect(&rc, 0, 0, statglobs.szInfo.cx, statglobs.szInfo.cy);
    ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rc, buf, len, NULL);
    myglobs.lpInfoBuffer->ReleaseDC(hdc);
    return TRUE;
}

void
ReleaseFontAndTextBuffers(void)
{
    RELEASE(myglobs.lpInfoBuffer);
    RELEASE(myglobs.lpFrameRateBuffer);
    if (statglobs.hFont != NULL) {
        DeleteObject(statglobs.hFont);
	statglobs.hFont = NULL;
    }
}

BOOL
InitFontAndTextBuffers(void)
{
    DDCOLORKEY          ddck;
    DDSURFACEDESC2       ddsd;
    HDC hdc;
    HRESULT ddrval;
    char dummyinfo[] = "000x000x00 (MONO) 0000";
    char dummyfps[] = "000.00 fps 00000000.00 tps 0000.00 mppps";
   
    /*
     * Create the font.
     */
    RELEASE(myglobs.lpInfoBuffer);
    RELEASE(myglobs.lpFrameRateBuffer);
    if (statglobs.hFont != NULL) {
        DeleteObject(statglobs.hFont);
	statglobs.hFont = NULL;
    }
    statglobs.hFont = CreateFont(
        d3dapp->szClient.cx <= 600 ? 12 : 24,
        0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        ANSI_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY,
        VARIABLE_PITCH,
        "Arial" );

    hdc = GetDC(NULL);
    SelectObject(hdc, statglobs.hFont);
    GetTextExtentPoint(hdc, dummyfps, strlen(dummyfps), &statglobs.szFrameRate);
    GetTextExtentPoint(hdc, dummyinfo, strlen(dummyinfo), &statglobs.szInfo);
    ReleaseDC(NULL, hdc);

    memset( &ddsd, 0, sizeof( ddsd ) );
    ddsd.dwSize = sizeof( ddsd );
    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT |DDSD_WIDTH;
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    ddsd.dwHeight = statglobs.szFrameRate.cy;
    ddsd.dwWidth = statglobs.szFrameRate.cx;
    ddrval = D3DAppCreateSurface(&ddsd, &myglobs.lpFrameRateBuffer);
    if (ddrval != DD_OK) {
	Msg("Could not create frame rate buffer.\n%s", D3DAppErrorToString(ddrval));	
        goto exit_with_error;
    }
    memset(&ddck, 0, sizeof(ddck));
    myglobs.lpFrameRateBuffer->SetColorKey(DDCKEY_SRCBLT, &ddck);
    if (!WriteFrameRateBuffer(0.0f, 0, 0)) 
	{
        goto exit_with_error;
    }
    memset( &ddsd, 0, sizeof( ddsd ) );
    ddsd.dwSize = sizeof( ddsd );
    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT |DDSD_WIDTH;
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    ddsd.dwHeight = statglobs.szInfo.cy;
    ddsd.dwWidth = statglobs.szInfo.cx;
    ddrval = D3DAppCreateSurface(&ddsd, &myglobs.lpInfoBuffer);
    if (ddrval != DD_OK) {
	Msg("Could not create info buffer.\n%s", D3DAppErrorToString(ddrval));
	goto exit_with_error;
    }
    memset(&ddck, 0, sizeof(ddck));
    myglobs.lpInfoBuffer->SetColorKey(DDCKEY_SRCBLT, &ddck);
    if (!WriteInfoBuffer())
	goto exit_with_error;
    return TRUE;
exit_with_error:
    RELEASE(myglobs.lpInfoBuffer);
    RELEASE(myglobs.lpFrameRateBuffer);
    if (statglobs.hFont != NULL) {
        DeleteObject(statglobs.hFont);
    }
    return FALSE;
}

/*************************************************************************
  Frame rate output.
 *************************************************************************/

#define INTERVAL 100

char StatTxt[100];
int StatTxtLen;
int count = 0;
int last_polygons = 0;
int this_frames = 0;
time_t last_time;
float fps;
long tps;

/*
 * ResetFrameRate
 * Initializes the frame rate counter.
 */
void
ResetFrameRate(void)
{
    last_time = clock();
    count = 0;
    last_polygons = 0;
    this_frames = 0;
    fps = 0.0f; tps = 0;
    StatTxt[0] = 0;
    StatTxtLen = 0;
}

BOOL CalculateFrameRate(void)
{
    // Calculate the frame rate and get other stats.
     
    count++;
    this_frames++;

    if (count == INTERVAL) 
	{
    	double t;
		int f;
		long rx,tx;
		D3DSTATS stats;
        time_t this_time;

        count = 0;
		this_time = clock();
		t = (this_time - last_time) / (double)CLOCKS_PER_SEC;
		last_time = this_time;

        memset(&stats, 0, sizeof(D3DSTATS));
        stats.dwSize = sizeof(D3DSTATS);

		f = this_frames;
		this_frames = 0;

        fps = (float)(f / t);
     
		rx = (long)(num_packets_received / t);
		tx = (long)(num_packets_sent / t);
		

		num_packets_sent = 0;
		num_packets_received = 0;

        if (myglobs.bShowFrameRate) 
		{
            if (!WriteFrameRateBuffer(fps, rx, tx))
                return FALSE;            
        }
    }
    return TRUE;
}

/*
 * DisplayFrameRate
 * Outputs frame rate info and screen mode to back buffer when appropriate.
 */
BOOL
DisplayFrameRate(int* count, LPD3DRECT lpExtents )
{
    RECT rc;
    int x, y;
    HRESULT ddrval = DD_OK;

    if (myglobs.bShowFrameRate && !myglobs.bSingleStepMode && statglobs.szFrameRate.cx > 0 && statglobs.szFrameRate.cy > 0 &&
        statglobs.szFrameRate.cx < d3dapp->szClient.cx && statglobs.szFrameRate.cy < d3dapp->szClient.cy) 
	{
        SetRect(&rc, 0, 0, statglobs.szFrameRate.cx, statglobs.szFrameRate.cy);
        x = (int)(0.5 * (d3dapp->szClient.cx - statglobs.szFrameRate.cx) + 0.5);
        y = 0;
		
		if (myglobs.lpFrameRateBuffer)
			ddrval = d3dapp->lpBackBuffer->BltFast(x, y, myglobs.lpFrameRateBuffer, &rc,
						 DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
		
		if (ddrval != DD_OK) 
		{
			// Blting the frame rate has failed. Since it is not vital, we
			// aren't going to report an error.
	     
			return TRUE;
		}

		SetRect((LPRECT)(lpExtents), x, y, statglobs.szFrameRate.cx + x, statglobs.szFrameRate.cy);
		++(*count);
		++lpExtents;
    }

    if (myglobs.bShowInfo && statglobs.szInfo.cx < d3dapp->szClient.cx && statglobs.szInfo.cy < d3dapp->szClient.cy) 
	{
        SetRect(&rc, 0, 0, statglobs.szInfo.cx, statglobs.szInfo.cy);
        x = (int)(0.5 * (d3dapp->szClient.cx - statglobs.szInfo.cx) + 0.5);
        y = d3dapp->szClient.cy - statglobs.szInfo.cy;
	
		if (myglobs.lpInfoBuffer)
	    ddrval = d3dapp->lpBackBuffer->BltFast(x,y, myglobs.lpInfoBuffer, &rc,
						 DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
		if (ddrval != DD_OK) 
		{
			// Blting the info has failed. Since it is not vital, we
			// aren't going to report an error.
	     
			return TRUE;
		}
		SetRect((LPRECT)(lpExtents), x, y, x + statglobs.szInfo.cx, y + statglobs.szInfo.cy);
		++(*count);
	}
    return TRUE;
}
