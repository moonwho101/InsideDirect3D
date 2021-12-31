/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *
 *  File:       ddutil.cpp
 *  Content:    Routines for loading bitmap and palettes from resources
 *
 ***************************************************************************/

extern IDirectDrawPalette * DDLoadPalette(IDirectDraw7 *pdd, LPCSTR szBitmap);
extern IDirectDrawSurface7 * DDLoadBitmap(IDirectDraw7 *pdd, LPCSTR szBitmap, int dx, int dy);
extern HRESULT              DDReLoadBitmap(IDirectDrawSurface7 *pdds, LPCSTR szBitmap);
extern HRESULT              DDCopyBitmap(IDirectDrawSurface7 *pdds, HBITMAP hbm, int x, int y, int dx, int dy);
extern DWORD                DDColorMatch(IDirectDrawSurface7 *pdds, COLORREF rgb);
extern HRESULT              DDSetColorKey(IDirectDrawSurface7 *pdds, COLORREF rgb);
