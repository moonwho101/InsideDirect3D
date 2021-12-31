
#ifndef __BITMAP_H__
#define __BITMAP_H__

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <winuser.h>	
#include <wingdi.h>		

#ifdef __cplusplus
extern "C" {
#endif

void DDBitmapScreenDump(HWND hwnd, D3DAppInfo* d3dapp);
PBITMAPINFO CreateBitmapInfoStruct(HWND hwnd, HBITMAP hBmp);
void CreateBMPFile(HWND hwnd, LPTSTR pszFile, PBITMAPINFO pbi,  
                  HBITMAP hBMP, HDC hDC);  
void errhandler(char *msg,HWND hwnd);

#ifdef __cplusplus
};
#endif

#endif // __BITMAP_H__
