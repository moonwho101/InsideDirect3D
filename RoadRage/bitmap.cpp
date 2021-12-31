
// Road Rage 1.1

// FILE : bitmap.cpp
// This file was originally part of the Microsoft DirectX5 SDK
// Edited by William Chin
// Edited with MSDEV C++ v6.0 using the DirectX 6.0 SDK
// Edited on:  3/7/99


#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <winuser.h>	
#include <wingdi.h>	
#include "world.hpp"
#include "d3dmain.hpp"
#include "bitmap.hpp"
	

#define MAXWRITE	10000000

int y_count=20;
int screenshot_counter = 0;

HBITMAP CopyScreenToBitmap(LPRECT lpRect);



void DDBitmapScreenDump(HWND hwnd, D3DAppInfo* d3dapp)
{
	HDC hdc;
	RECT rect;
	HBITMAP hBmp;
	PBITMAPINFO pbi;	
	char buffer[50];
	char filename[50];
	
	if(screenshot_counter > 99)
		return;

	rect.left = 0;
	rect.top  = 0;
	rect.right  = d3dapp->szClient.cx; 
	rect.bottom = d3dapp->szClient.cy;
	
	
	itoa(screenshot_counter,buffer,10);
			
	strcpy(filename,"scrnshot");
	strcat(filename,(char *)buffer);
	strcat(filename,".bmp");

	screenshot_counter++;

	hdc = GetDC(hwnd);
	hBmp = CopyScreenToBitmap(&rect);
	pbi = CreateBitmapInfoStruct(hwnd, hBmp); 
	CreateBMPFile(hwnd, filename , pbi, hBmp, hdc); 
	DeleteObject(hdc);
	ReleaseDC(hwnd,hdc);

	return;
}

HBITMAP CopyScreenToBitmap(LPRECT lpRect)
{
  HDC hScrDC, hMemDC;           // screen DC and memory DC
  HBITMAP hBitmap, hOldBitmap;  // handles to deice-dependent bitmaps
  int nX, nY, nX2, nY2;         // coordinates of rectangle to grab
  int nWidth, nHeight;          // DIB width and height
  int xScrn, yScrn;             // screen resolution

  /* check for an empty rectangle */
  if (IsRectEmpty(lpRect))
    return NULL;

  /*  create a DC for the screen and create
   *  a memory DC compatible to screen DC
   */
  hScrDC = CreateDC("DISPLAY", NULL, NULL, NULL);
  hMemDC = CreateCompatibleDC(hScrDC);

  /* get points of rectangle to grab */
  nX = lpRect->left;
  nY = lpRect->top;
  nX2 = lpRect->right;
  nY2 = lpRect->bottom;

  /* get screen resolution */
  xScrn = GetDeviceCaps(hScrDC, HORZRES);
  yScrn = GetDeviceCaps(hScrDC, VERTRES);

  /* make sure bitmap rectangle is visible */
  if (nX < 0)
    nX = 0;
  if (nY < 0)
    nY = 0;
  if (nX2 > xScrn)
    nX2 = xScrn;
  if (nY2 > yScrn)
    nY2 = yScrn;
  nWidth = nX2 - nX;
  nHeight = nY2 - nY;

  /* create a bitmap compatible with the screen DC */
  hBitmap = CreateCompatibleBitmap(hScrDC, nWidth, nHeight);

  /* select new bitmap into memory DC */
  hOldBitmap = (struct HBITMAP__ *)SelectObject(hMemDC, hBitmap);

  /* bitblt screen DC to memory DC */
  BitBlt(hMemDC, 0, 0, nWidth, nHeight, hScrDC, nX, nY, SRCCOPY) ;

  /*  select old bitmap back into memory DC and get handle to
   *  bitmap of the screen
   */
  hBitmap = (struct HBITMAP__ *)SelectObject(hMemDC, hOldBitmap);

  /* clean up */
  DeleteDC(hScrDC);
  DeleteDC(hMemDC);

  /* return handle to the bitmap */
  return hBitmap;
}


PBITMAPINFO CreateBitmapInfoStruct(HWND hwnd, HBITMAP hBmp) 
{  
    BITMAP bmp; 
    PBITMAPINFO pbmi; 
    WORD    cClrBits; 
	
    /* Retrieve the bitmap's color format, width, and height. */ 
 
    if (!GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp))	
        errhandler("GetObject", hwnd); 

    /* Convert the color format to a count of bits. */ 
 
    cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel); 
 
    if (cClrBits == 1) 
        cClrBits = 1; 
    else if (cClrBits <= 4) 
        cClrBits = 4; 
    else if (cClrBits <= 8) 
        cClrBits = 8; 
    else if (cClrBits <= 16) 
        cClrBits = 16; 
    else if (cClrBits <= 24) 
        cClrBits = 24; 
    else 
        cClrBits = 32; 
 
    /* 
     * Allocate memory for the BITMAPINFO structure. (This structure 
     * contains a BITMAPINFOHEADER structure and an array of RGBQUAD data 
     * structures.) 
     */ 
 
    if (cClrBits != 24) 
         pbmi = (PBITMAPINFO) LocalAlloc(LPTR, 
                    sizeof(BITMAPINFOHEADER) + 
                    sizeof(RGBQUAD) * (2^cClrBits)); 
 
    /* 
     * There is no RGBQUAD array for the 24-bit-per-pixel format. 
     */ 
 
    else 
         pbmi = (PBITMAPINFO) LocalAlloc(LPTR, 
                    sizeof(BITMAPINFOHEADER)); 
 
 
 
    /* Initialize the fields in the BITMAPINFO structure. */ 
 
    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
    pbmi->bmiHeader.biWidth = bmp.bmWidth; 
    pbmi->bmiHeader.biHeight = bmp.bmHeight; 
    pbmi->bmiHeader.biPlanes = bmp.bmPlanes; 
    pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel; 
    if (cClrBits < 24) 
        pbmi->bmiHeader.biClrUsed = 2^cClrBits; 
 
 
    /* If the bitmap is not compressed, set the BI_RGB flag. */ 
 
    pbmi->bmiHeader.biCompression = BI_RGB; 
 
    /* 
     * Compute the number of bytes in the array of color 
     * indices and store the result in biSizeImage. 
     */ 
 
    pbmi->bmiHeader.biSizeImage = (pbmi->bmiHeader.biWidth + 7) /8 
                                  * pbmi->bmiHeader.biHeight 
                                  * cClrBits; 
 
    /* 
     * Set biClrImportant to 0, indicating that all of the 
     * device colors are important. 
     */ 
 
    pbmi->bmiHeader.biClrImportant = 0; 
 
    return pbmi; 
 
} 
 
// The following example code defines a function that initializes the remaining structures, retrieves the array of palette indices, opens the file, copies the data, and closes the file. 

void CreateBMPFile(HWND hwnd, LPTSTR pszFile, PBITMAPINFO pbi,  
                  HBITMAP hBMP, HDC hDC) 
{ 
 
    HANDLE hf;                  /* file handle */ 
    BITMAPFILEHEADER hdr;       /* bitmap file-header */ 
    PBITMAPINFOHEADER pbih;     /* bitmap info-header */ 
    LPBYTE lpBits;              /* memory pointer */ 
    DWORD dwTotal;              /* total count of bytes */ 
    DWORD cb;                   /* incremental count of bytes */ 
    BYTE *hp;                   /* byte pointer */ 
    DWORD dwTmp; 
 
 
    pbih = (PBITMAPINFOHEADER) pbi; 
    lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);
    if (!lpBits)
	
         errhandler("GlobalAlloc", hwnd); 
 
    /* 
     * Retrieve the color table (RGBQUAD array) and the bits 
     * (array of palette indices) from the DIB. 
     */ 
 
    if (!GetDIBits(hDC, hBMP, 0, (WORD) pbih->biHeight, 
                   lpBits, pbi, DIB_RGB_COLORS)) 
	{
				  
        errhandler("GetDIBits", hwnd); 
	}
 
    /* Create the .BMP file. */ 
 
    hf = CreateFile(pszFile, 
                   GENERIC_READ | GENERIC_WRITE, 
                   (DWORD) 0, 
                   (LPSECURITY_ATTRIBUTES) NULL, 
                   CREATE_ALWAYS, 
                   FILE_ATTRIBUTE_NORMAL, 
                   (HANDLE) NULL); 
 
    if (hf == INVALID_HANDLE_VALUE) 
        errhandler("CreateFile", hwnd); 
 
    hdr.bfType = 0x4d42;        /* 0x42 = "B" 0x4d = "M" */ 
 
    /* Compute the size of the entire file. */ 
 
    hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) + 
                 pbih->biSize + pbih->biClrUsed 
                 * sizeof(RGBQUAD) + pbih->biSizeImage); 
 
    hdr.bfReserved1 = 0; 
    hdr.bfReserved2 = 0; 
 
    /* Compute the offset to the array of color indices. */ 
 
    hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + 
                    pbih->biSize + pbih->biClrUsed 
                    * sizeof (RGBQUAD); 
 
    /* Copy the BITMAPFILEHEADER into the .BMP file. */ 
 
    if (!WriteFile(hf, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER), 
       (LPDWORD) &dwTmp, (LPOVERLAPPED) NULL)) 
	   
       errhandler("WriteFile", hwnd); 
 
    /* Copy the BITMAPINFOHEADER and RGBQUAD array into the file. */ 
 
    if (!WriteFile(hf, (LPVOID) pbih, sizeof(BITMAPINFOHEADER) 
                  + pbih->biClrUsed * sizeof (RGBQUAD), 
                  (LPDWORD) &dwTmp, (LPOVERLAPPED) NULL)) 
				  
       errhandler("WriteFile", hwnd); 
 
    /* Copy the array of color indices into the .BMP file. */ 
 
    dwTotal = cb = pbih->biSizeImage; 
    hp = lpBits; 
    while (cb > MAXWRITE)  
	{ 
            if (!WriteFile(hf, (LPSTR) hp, (int) MAXWRITE, 
                          (LPDWORD) &dwTmp, (LPOVERLAPPED) NULL)) 
				errhandler("WriteFile", hwnd); 
            
			cb-= MAXWRITE; 
            hp += MAXWRITE; 
    } 
    if (!WriteFile(hf, (LPSTR) hp, (int) cb, 
         (LPDWORD) &dwTmp, (LPOVERLAPPED) NULL)) 
		errhandler("WriteFile", hwnd); 
 
    /* Close the .BMP file. */ 
 
    if (!CloseHandle(hf)) 
		errhandler("CloseHandle", hwnd); 
 
    /* Free memory. */

    GlobalFree((HGLOBAL)lpBits);
} 

void errhandler(char *msg,HWND hwnd)
{
	HDC hdc;

	hdc=GetDC(hwnd);
	TextOut(hdc,10,y_count,msg,strlen(msg));
	y_count+=20;
	ReleaseDC(hwnd,hdc);
}
 
