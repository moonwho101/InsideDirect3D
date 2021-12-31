
// Toolbar.h
#ifndef __TOOLBAR_H__
#define __TOOLBAR_H__

#include <windows.h>
#include <windowsx.h>   
#include "stdlib.h"   
#include "editor.h"
//#include "io.h"
//#include "stdio.h"
//#include "string.h"
//#include "conio.h" 

#define WHITE		0
#define TURQ		1 
#define PINK		2
#define GREEN		3
#define YELLOW		4
#define LIGHTGRAY	5
#define GRAY		6
#define DARKGRAY	7
#define BLACK		8  
#define RED			9
#define BLUE		10

#define DRAW_BUTTON_ON		1
#define DRAW_BUTTON_OFF		2
#define ERASE_BUTTON		3

#define LEFT_BUTTON_DOWN		 1
#define LEFT_BUTTON_UP			 2	
#define RIGHT_BUTTON_DOWN		 3
#define RIGHT_BUTTON_UP			 4
#define MOUSE_MOVE				 5
#define LEFT_BUTTON_DOUBLE_CLICK 6
#define MIDDLE_BUTTON_DOWN		 7	

#ifdef __cplusplus
extern "C" {
#endif

static int red[16]   = {255,0,  255, 0,  255,192,128,64,0,255,0  };                                           
static int green[16] = {255,255,64,  255,255,192,128,64,0,0  ,0  };   
static int blue[16]  = {255,255,64,  0,  0  ,192,128,64,0,0  ,255};   

//int Aspect_X(int x1);
//int Aspect_Y(int y1); 
//void DrawTextOut(HDC hDC,int x1,int y1,int n);  
//void Get_Aspect_Ratio(HDC hDC);
//void DrawString(HDC hDC,int x1,int y1,const char * string); 
//void SelectFont(HDC hDC); 
//void DeleteFont(HDC hDC);
void Select_Brush_Color(HDC hdc,int color);
void Select_Pen_Color(HDC hdc,int color); 
void Delete_Brush_Color(HDC hdc);
void Delete_Pen_Color(HDC hdc);
  
void DrawButton(HDC hdc,int x,int y, int status, editor_ptr edptr);  
void DrawSmallButton(HDC hdc,int x,int y, int status);
void DrawScrollButton(HDC hdc,int x,int y, int status);
void DrawToolBar(HDC hdc);   
void DrawStatusBar(HDC hdc, editor_ptr edptr);
void DrawDialogBar(HDC hdc, editor_ptr edptr);
void LoadToolbarBitmaps(HINSTANCE hInst);
int UpdateDialogBar(HDC hdc, int x, int y , int button_status, editor_ptr edptr);
int UpdateToolBar(HDC hdc, int x, int y , int button_status);
int UpdateScrollBars(HDC hdc, int x, int y , int button_status, editor_ptr edptr);
void DrawDialogControls(HDC hdc, editor_ptr edptr);
void DrawFrame(HDC hdc);
void DrawBar(HDC hdc,int x1,int y1,int x2,int y2);
void DrawScrollBars(HDC hdc, editor_ptr edptr);
int UpdateScrollBarButtons(HDC hdc, int x, int y , int x1, int y1, int x2, int y2, int button_status);
int UpdateEditorMapWindow(HWND hwnd, HDC hdc, int x, int y , int button_status, editor_ptr edptr);
int UpdateStatusBar(HDC hdc, int x, int y , editor_ptr edptr);

#ifdef __cplusplus
};
#endif

#endif // __TOOLBAR_H__

