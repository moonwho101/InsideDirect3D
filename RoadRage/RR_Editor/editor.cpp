
// D3D Road Rage Editor version 1.1
// Written by William Chin
// Developed in MSDEV C++ v4.2 with DirectX 5 SDK
// Date : 5/10/98
// File : Editor.c

#include <windows.h>
#include <windowsx.h>
#include <math.h>
#include <stdio.h>
#include "editor.h"
#include "3Dengine.h"
#include "d3ddemo.h"
#include "toolbar.h"
#include "world.h"

#define ROAD_TEX	1
#define CURB_TEX	2
#define PAVE_TEX	3

#define MAX_NUM_DIALOGBAR_BUTTONS 18
#define MAX_NUM_TOOLBAR_BUTTONS   14

#define MAX_NUM_OBJECT_TYPES 50

extern WORLD world;

int current_poly=0;
float last_angle=(float)0;
float last_x;
float last_z;

int slpm; // start left  poly marker
int srpm; // start right poly marker
int elpm; // end   left  poly marker
int erpm; // end   right poly marker

int db_button_x = 559;
int db_button_y = 20;

int x_old = 0, y_old =0;

BOOL start_link_flag;

float joint_x,joint_z;



typedef struct Controlbar_typ
{
	int x1;
	int x2;
	int y1;
	int y2;
	int number_buttons;
	int x_button_offset;
	int y_button_offset;

}CONTROLBAR, *Controlbar_ptr;

typedef struct XRect_typ
{
	int x1;
	int x2;
	int y1;
	int y2;

}XRECT, *XRect_ptr;

CONTROLBAR	statusbar;	
CONTROLBAR	dialogbar;	
CONTROLBAR	toolbar;	

XRECT edmapwindow;
XRECT hscroll;
XRECT vscroll;


int last_link=0;
int current_link=0;
int bitmap_number; 
MAP edmap;
VERT last_editor_vertex;
LINK LinksList[MAX_NUM_LINKS];

HBRUSH hbr_color,holdbrush_color;
HPEN hpen_color,holdpen_color;
HBITMAP hBitmap1, hBitmap2, hBitmap3, hOldBitmap;
BITMAP Bitmap;
HDC hMemoryDC;

//WORLD world;

/////////////////////////////////////////////////////////////////////////////

void DeleteMapObject(HDC hdc, editor_ptr edptr)
{
	int i, j, start;


	if(edptr->wptr->oblist_length != 0)
	{
		for(i = 0; i < edptr->wptr->oblist_length; i++)	
		{
			if(edptr->wptr->oblist[i].IsHighlighted == TRUE)
			{
				start = i;

				for(j = start; j < edptr->wptr->oblist_length; j++)	
					edptr->wptr->oblist[j] = edptr->wptr->oblist[j+1];
							
				edptr->wptr->oblist_length--;
			}
		}
		DrawEditorMap(hdc, edptr);
	}
}

void DrawDialogControls(HDC hdc, editor_ptr edptr)
{
	toolbar.x1 = 0;
	toolbar.y1 = 0;
	toolbar.x2 = 549;
	toolbar.y2 = 32;

	dialogbar.x1 = 550;
	dialogbar.y1 = 0;
	dialogbar.x2 = 630;
	dialogbar.y2 = 430;

	statusbar.x1 = 0;
	statusbar.y1 = 430-32;
	statusbar.x2 = 549;
	statusbar.y2 = 430;

	edmapwindow.x1 = 0;
	edmapwindow.y1 = 32;
	edmapwindow.x2 = 536;
	edmapwindow.y2 = 384;

	hscroll.x1 = 0;
	hscroll.y1 = 384 + 1;
	hscroll.x2 = 536;
	hscroll.y2 = 397;

	vscroll.x1 = 536 + 1;
	vscroll.y1 = 32 + 1;
	vscroll.x2 = 549;
	vscroll.y2 = 384;


	DrawStatusBar(hdc,edptr);
	DrawToolBar(hdc);
	DrawDialogBar(hdc, edptr);
	DrawScrollBars(hdc,edptr);
}

void DrawScrollBars(HDC hdc, editor_ptr edptr)
{
	int x,y;
	int x1,y1,x2,y2;
	float dx,dy;
	int x_off,y_off;

	x1 = hscroll.x1+13+6, y1 = hscroll.y1;
	x2 = hscroll.x2-13-6, y2 = hscroll.y2;

	dx = (float)51200 / (float)(x2 - x1);
	x_off =	-1 * edptr->display_x_offset; 
	x = x1 + (int) ((float)x_off / dx);

	DrawScrollButton(hdc,x-6,y1,DRAW_BUTTON_ON);

	x1 = vscroll.x1, y1 = vscroll.y1+13+6;
	x2 = vscroll.x2, y2 = vscroll.y2-13-6;

	dy = (float)51200 / (float)(y2 - y1);
	y_off =	-1 * edptr->display_y_offset; 
	y = y1 + (int) ((float)y_off / dy);

	DrawScrollButton(hdc,x1,y-6,DRAW_BUTTON_ON);


	DrawScrollButton(hdc,hscroll.x1,    hscroll.y1, LEFT_BUTTON_DOWN);
	DrawScrollButton(hdc,hscroll.x2-12, hscroll.y1, LEFT_BUTTON_DOWN);

	DrawScrollButton(hdc,vscroll.x1 ,vscroll.y1,    LEFT_BUTTON_DOWN);
	DrawScrollButton(hdc,vscroll.x1 ,vscroll.y2-12, LEFT_BUTTON_DOWN);

}

int UpdateScrollBars(HDC hdc, int x, int y , int button_status, editor_ptr edptr)
{
	int x1,y1,x2,y2;
	float dx,dy;
	int x_off,y_off;

	x1 = hscroll.x1+13+6, y1 = hscroll.y1;
	x2 = hscroll.x2-13-6, y2 = hscroll.y2;

	if((x>=x1 && x<=x2) && (y>=y1 && y<=y2))
	{
		dx = (float)51200 / (float)(x2 - x1);
		x_off = (int) ((float)(x - x1) * dx);
		edptr->display_x_offset = -1 * x_off;
		if(x_old > 0)
			DrawScrollButton(hdc,x_old,y1,ERASE_BUTTON);

		DrawScrollButton(hdc,x-6,y1,DRAW_BUTTON_ON);
		x_old = x-6;
		DrawEditorMap(hdc, edptr);
		edptr->scroll_bar_flag = TRUE;
	}

	x1 = vscroll.x1, y1 = vscroll.y1+13+6;
	x2 = vscroll.x2, y2 = vscroll.y2-13-6;

	if((x>=x1 && x<=x2) && (y>=y1 && y<=y2))
	{
		dy = (float)51200 / (float)(y2 - y1);
		y_off = (int) ((float)(y - y1) * dy);
		edptr->display_y_offset = -1 * y_off;
		if(y_old > 0)
			DrawScrollButton(hdc,x1,y_old,ERASE_BUTTON);

		DrawScrollButton(hdc,x1,y-6,DRAW_BUTTON_ON);
		y_old = y-6;
		DrawEditorMap(hdc, edptr);
		edptr->scroll_bar_flag = TRUE;
	}

	if(edptr->scroll_bar_flag == TRUE)
		return 0;

	if(UpdateScrollBarButtons(hdc, x, y , hscroll.x1, hscroll.y1,
		hscroll.x1+12, hscroll.y1+12, button_status) == 1) 
		return 1;

	if(UpdateScrollBarButtons(hdc, x, y , hscroll.x2-12, hscroll.y1,
		hscroll.x2, hscroll.y1+12, button_status) == 1) 
		return 2;

	if(UpdateScrollBarButtons(hdc, x, y , vscroll.x1, vscroll.y1,
		vscroll.x1+12, vscroll.y1+12, button_status) == 1) 
		return 3;

	if(UpdateScrollBarButtons(hdc, x, y , vscroll.x1, vscroll.y2-12,
		vscroll.x1+12, vscroll.y2, button_status) == 1) 
		return 4;

	

	return 0;
}

int UpdateScrollBarButtons(HDC hdc, int x, int y , int x1, int y1, int x2, int y2, int button_status)
{
	if((x>=x1 && x<=x2) && (y>=y1 && y<=y2))
	{	
		if(button_status==LEFT_BUTTON_DOWN)
			DrawScrollButton(hdc,x1,y1,DRAW_BUTTON_OFF);
				
		if(button_status==LEFT_BUTTON_UP)
			DrawScrollButton(hdc,x1,y1,DRAW_BUTTON_ON);
		
		return 1;
	}
	else
		return 0;
}

int UpdateStatusBar(HDC hdc, int x, int y , editor_ptr edptr)
{
	float s;
	char buffer[50];
	LPTSTR buffer2;

	SetBkMode(hdc,OPAQUE);
	s = edptr->display_scale;

	itoa((int)edptr->object_rot_angle, buffer,10);
	buffer2 = lstrcat(buffer," deg");
	TextOut(hdc,220,406,"              ",14);
	TextOut(hdc,220,406,buffer2,lstrlen(buffer2));

	itoa((int)(s*100),buffer,10);
	buffer2 = lstrcat(buffer," %");
	TextOut(hdc,160,406,"          ",10);
	TextOut(hdc,160,406,buffer2,lstrlen(buffer2));

	return 0;	
}

int UpdateEditorMapWindow(HWND hwnd, HDC hdc, int x, int y ,int button_status, editor_ptr edptr)
{
	int x1,y1,x2,y2;
	int MouseX, MouseY;
	int x_off, z_off;
	float s;
	char buffer[50];


	x1 = edmapwindow.x1;
	y1 = edmapwindow.y1;
	x2 = edmapwindow.x2;
	y2 = edmapwindow.y2;

	if( !((x>=x1 && x<=x2) && (y>=y1 && y<=y2)))
	{
		KillTimer(hwnd, 2);
		return 0;
	}
	
	if(button_status == MIDDLE_BUTTON_DOWN)
	{
		s     = edptr->display_scale; // 0.25
		x_off =	edptr->display_x_offset; //320	
		z_off =	edptr->display_y_offset; //100
						
		MouseX=(int)(  (float)x				 / s) -x_off;
		MouseY=(int)( ((float)y - (float)32) / s) -z_off;
		HighLightObjectWithMouse(hdc, MouseX, MouseY, edptr);
		//DrawEditorMap(hdc, edptr);
	}


	if(button_status == LEFT_BUTTON_DOWN)
	{
		s     = edptr->display_scale; // 0.25
		x_off =	edptr->display_x_offset; //320	
		z_off =	edptr->display_y_offset; //100
						
		MouseX=(int)(  (float)x				 / s) -x_off;
		MouseY=(int)( ((float)y - (float)32) / s) -z_off;

		if(edptr->editor_mode == 0)
		{
			SetLinkWithMouse(MouseX,MouseY);
			DrawEditorMap(hdc, edptr);
		}
		else
		{
			PlaceObjectWithMouse(edptr, MouseX, MouseY);
			DrawEditorMap(hdc, edptr);
		}

	}

	if(button_status == MOUSE_MOVE)
	{
		s     = edptr->display_scale; // 0.25
		x_off =	edptr->display_x_offset; //320	
		z_off =	edptr->display_y_offset; //100
						
		MouseX=(int)(  (float)x				 / s) -x_off;
		MouseY=(int)( ((float)y - (float)32) / s) -z_off;
			
		itoa(MouseX,buffer,10);
		TextOut(hdc,10,406,"              ",14);
		TextOut(hdc,10,406,buffer,lstrlen(buffer));
		itoa(MouseY,buffer,10);
		TextOut(hdc,80,406,"              ",14);
		TextOut(hdc,80,406,buffer,lstrlen(buffer));
			
		edptr->MouseX = MouseX;
		edptr->MouseY = MouseY;

		if(edptr->editor_mode==1)
		{
			SetTimer(hwnd,2,200,NULL);
			if(edptr->Redraw_editor_map_flag == 1)
			{
				DrawEditorMap(hdc, edptr);
				edptr->Redraw_editor_map_flag = 0;
			}
		}
	}
	return 1;
}

int UpdateDialogBar(HDC hdc, int x, int y , int button_status, editor_ptr edptr)
{
	int col,row;
	int x1,x2;
	int y1,y2;
	int button_number;

	x1 = dialogbar.x1 + 9;
	x2 = x1 + 64;
	y1 = dialogbar.y1 + 20;
	y2 = y1 + (MAX_NUM_DIALOGBAR_BUTTONS / 2) *32;

	
	if(x >= x1 && x <= x2)
	{
		if(y >= y1 && y <= y2)
		{
			// find the exact button
			col=(x - x1) / 32;
			row=(y - y1) / 32;
	
			button_number = row * 2 + col + 1;
			bitmap_number = button_number - 1;

			if(button_status == LEFT_BUTTON_DOWN)
			{
				DrawButton(hdc,x1+col*32,y1+row*32,DRAW_BUTTON_OFF, edptr);
			}
			
			if(button_status==LEFT_BUTTON_UP)
			{
				DrawButton(hdc,x1+col*32,y1+row*32,DRAW_BUTTON_ON, edptr);
			}
			return button_number;
		}
	}	
	return 0;
}

int UpdateToolBar(HDC hdc, int x, int y , int button_status)
{
	int col;
	int x1,x2;
	int y1,y2;
	int button_number;

	x1 = toolbar.x1 + 4;
	x2 = x1 + 22 * MAX_NUM_TOOLBAR_BUTTONS;
	y1 = toolbar.y1 + 4;
	y2 = y1 + 22;

	
	if(x>=x1 && x<=x2)
	{
		if(y>=y1 && y<=y2)
		{
			// find the exact button
			col=(x-x1)/22;
			
			button_number=col + 1;
			bitmap_number=button_number-1;

			if(button_status == LEFT_BUTTON_DOWN)
			{
				DrawSmallButton(hdc, 4 + col * 22 + toolbar.x1 , toolbar.y1+4, DRAW_BUTTON_OFF);
			}
			
			if(button_status == LEFT_BUTTON_UP)
			{
				DrawSmallButton(hdc, 4 + col * 22 + toolbar.x1 , toolbar.y1+4, DRAW_BUTTON_ON);
			}
			return button_number;
		}
	}	
	return 0;
}

void LoadToolbarBitmaps(HINSTANCE hInst)
{
	hBitmap1 = LoadBitmap(hInst, "bitmap1");
    GetObject(hBitmap1, 16, (LPSTR) &Bitmap);   

	hBitmap2 = LoadBitmap(hInst, "TOOLBAR");
    GetObject(hBitmap2, 16, (LPSTR) &Bitmap);   

	hBitmap3 = LoadBitmap(hInst, "bitmap2");
    GetObject(hBitmap1, 16, (LPSTR) &Bitmap);   
}

void DrawBar(HDC hdc,int x1,int y1,int x2,int y2)
{
	int nIndex;

	nIndex = COLOR_ACTIVEBORDER;
	holdbrush_color = (HBRUSH)SelectObject(hdc,GetSysColorBrush(nIndex));  
	Rectangle(hdc,x1,y1,x2,y2); 
	Delete_Brush_Color(hdc);

	Select_Pen_Color(hdc,WHITE);    
	MoveToEx(hdc,x1,y2,NULL);
	LineTo(hdc,x1,y1);    // left
	LineTo(hdc,x2,y1);    // top	
	Delete_Pen_Color(hdc);    
	            
	Select_Pen_Color(hdc,GRAY);    
	MoveToEx(hdc,x1,y2,NULL);
	LineTo(hdc,x2,y2);  // bottom  
	LineTo(hdc,x2,y1);  // right	
	Delete_Pen_Color(hdc);		
}


void DrawToolBar(HDC hdc)
{
	int x;

	DrawBar(hdc,toolbar.x1, toolbar.y1, toolbar.x2,	toolbar.y2); 

	for(x = 0; x < MAX_NUM_TOOLBAR_BUTTONS; x++)
	{
		bitmap_number = x;
		DrawSmallButton(hdc, 4 + x * 22 + toolbar.x1 , toolbar.y1+4, DRAW_BUTTON_ON);
	}
}

void DrawStatusBar(HDC hdc, editor_ptr edptr)
{
	DrawBar(hdc,statusbar.x1, statusbar.y1, statusbar.x2, statusbar.y2); 	
	UpdateStatusBar(hdc, 0, 0, edptr);
}

void DrawDialogBar(HDC hdc, editor_ptr edptr)
{
	int x,y;

	DrawBar(hdc,dialogbar.x1, dialogbar.y1, dialogbar.x2, dialogbar.y2); 

	for(y = 0; y < (MAX_NUM_DIALOGBAR_BUTTONS/2) ; y++)
	{
		for(x=0;x<2;x++)
		{
			bitmap_number = y * 2 + x;
			DrawButton(hdc,db_button_x + x*32,db_button_y + y*32,DRAW_BUTTON_ON, edptr);
		}
	}
	
}

void Select_Brush_Color(HDC hdc,int color)
{
	hbr_color = CreateSolidBrush(RGB(red[color],green[color],blue[color])); 
	holdbrush_color = (HBRUSH)SelectObject(hdc,hbr_color);  
}   

void Select_Pen_Color(HDC hdc,int color)
{
	hpen_color = CreatePen(PS_SOLID,1,RGB(red[color],green[color],blue[color]));  
	holdpen_color = (HPEN)SelectObject(hdc,hpen_color);    
}	

void Delete_Brush_Color(HDC hdc)
{
	SelectObject(hdc,holdbrush_color);   
	DeleteObject(hbr_color); 
}

void Delete_Pen_Color(HDC hdc)
{  
	SelectObject(hdc,holdpen_color);   
	DeleteObject(hpen_color);
}  

void DrawButton(HDC hdc,int x,int y, int status, editor_ptr edptr)
{
	int sx1,sx2;
	int sy1,sy2;
	int nIndex;
	
	SetBkMode(hdc,TRANSPARENT);
	SetBkColor(hdc,RGB(255,255,255));
	nIndex = COLOR_ACTIVEBORDER;

	sx1=x;
	sx2=x+30;
	sy1=y;
	sy2=y+30;

	if(status == DRAW_BUTTON_ON)
	{
		holdbrush_color = (HBRUSH)SelectObject(hdc,GetSysColorBrush(nIndex));  
		Rectangle(hdc,sx1,sy1,sx2+1,sy2+1); 
              
		Select_Pen_Color(hdc,WHITE);    
		MoveToEx(hdc,sx1,sy2,NULL);
		LineTo(hdc,sx1,sy1);    // left
		LineTo(hdc,sx2,sy1);    // top	
		Delete_Pen_Color(hdc);    
	            
		Select_Pen_Color(hdc,BLACK);    
		MoveToEx(hdc,sx1+1,sy2-1,NULL);
		LineTo(hdc,sx2-1,sy2-1);  // bottom  
		LineTo(hdc,sx2-1,sy1+1);  // right	
		Delete_Pen_Color(hdc);	
  		
		if(edptr->dialogbar_mode == DIALOGBAR_MODE_ROADS)
		{
			hMemoryDC = CreateCompatibleDC(hdc);   
			hOldBitmap = (HBITMAP)SelectObject(hMemoryDC, hBitmap1); 
		}
		if(edptr->dialogbar_mode == DIALOGBAR_MODE_OBJECTS)
		{
			hMemoryDC = CreateCompatibleDC(hdc);   
			hOldBitmap = (HBITMAP)SelectObject(hMemoryDC, hBitmap3); 
		}
		
		if(bitmap_number < MAX_NUM_DIALOGBAR_BUTTONS)
			BitBlt(hdc,sx1+4, sy1+4,18,18, hMemoryDC,(bitmap_number)*20, 1, SRCAND);
   
		SelectObject(hMemoryDC,hOldBitmap);
		Delete_Brush_Color(hdc);
		DeleteDC(hMemoryDC);
	}

	if(status==DRAW_BUTTON_OFF)
	{		
		holdbrush_color = (HBRUSH)SelectObject(hdc,GetSysColorBrush(nIndex));  
		Rectangle(hdc,sx1,sy1,sx2+1,sy2+1); 
		
		Select_Pen_Color(hdc,GRAY);    
		MoveToEx(hdc,sx1,sy2,NULL);
		LineTo(hdc,sx1,sy1);    // left
		LineTo(hdc,sx2,sy1);    // top	
		Delete_Pen_Color(hdc);  
		
		MoveToEx(hdc,sx1,sy2,NULL);
		LineTo(hdc,sx2,sy2);  // bottom  
		LineTo(hdc,sx2,sy1);  // right	
		Delete_Pen_Color(hdc);  

		if(edptr->dialogbar_mode == DIALOGBAR_MODE_ROADS)
		{
			hMemoryDC = CreateCompatibleDC(hdc);   
			hOldBitmap = (HBITMAP)SelectObject(hMemoryDC, hBitmap1); 
		}
		if(edptr->dialogbar_mode == DIALOGBAR_MODE_OBJECTS)
		{
			hMemoryDC = CreateCompatibleDC(hdc);   
			hOldBitmap = (HBITMAP)SelectObject(hMemoryDC, hBitmap3); 
		} 
		
		if(bitmap_number <= MAX_NUM_DIALOGBAR_BUTTONS)
			BitBlt(hdc,sx1+6, sy1+6,18,18, hMemoryDC,(bitmap_number)*20, 1, SRCAND);
   
		SelectObject(hMemoryDC,hOldBitmap);
		DeleteDC(hMemoryDC);
		Delete_Brush_Color(hdc);
		Delete_Pen_Color(hdc);		
	}
}

void DrawSmallButton(HDC hdc,int x,int y, int status)
{
	int sx1,sx2;
	int sy1,sy2;
	int nIndex;
	
	SetBkMode(hdc,TRANSPARENT);
	SetBkColor(hdc,RGB(255,255,255));
	nIndex = COLOR_ACTIVEBORDER;

	sx1=x;
	sx2=x+21;
	sy1=y;
	sy2=y+21;

	if(status==DRAW_BUTTON_ON)
	{
		holdbrush_color = (HBRUSH)SelectObject(hdc,GetSysColorBrush(nIndex));  
		Rectangle(hdc,sx1,sy1,sx2,sy2); 
              
		Select_Pen_Color(hdc,WHITE);    
		MoveToEx(hdc,sx1,sy2,NULL);
		LineTo(hdc,sx1,sy1);    // left
		LineTo(hdc,sx2,sy1);    // top	
		Delete_Pen_Color(hdc);    
	            
		Select_Pen_Color(hdc,BLACK);    
		MoveToEx(hdc,sx1,sy2,NULL);
		LineTo(hdc,sx2,sy2);  // bottom  
		LineTo(hdc,sx2,sy1);  // right	
		Delete_Pen_Color(hdc);	
  	
		hMemoryDC = CreateCompatibleDC(hdc);   
		hOldBitmap = (HBITMAP)SelectObject(hMemoryDC, hBitmap2);   
		
		if(bitmap_number < MAX_NUM_TOOLBAR_BUTTONS)
			BitBlt(hdc,sx1+2, sy1+2,16,16, hMemoryDC,(bitmap_number)*16, 1, SRCAND);
   
		SelectObject(hMemoryDC,hOldBitmap);
		Delete_Brush_Color(hdc);
		DeleteDC(hMemoryDC);
	}

	if(status == DRAW_BUTTON_OFF)
	{		
		holdbrush_color = (HBRUSH)SelectObject(hdc,GetSysColorBrush(nIndex));  
		Rectangle(hdc,sx1,sy1,sx2,sy2); 
		
		Select_Pen_Color(hdc,GRAY);    
		MoveToEx(hdc,sx1,sy2,NULL);
		LineTo(hdc,sx1,sy1);    // left
		LineTo(hdc,sx2,sy1);    // top	
		Delete_Pen_Color(hdc);  
		
		MoveToEx(hdc,sx1,sy2,NULL);
		LineTo(hdc,sx2,sy2);  // bottom  
		LineTo(hdc,sx2,sy1);  // right	
		Delete_Pen_Color(hdc);  

		hMemoryDC = CreateCompatibleDC(hdc);   
		hOldBitmap = (HBITMAP)SelectObject(hMemoryDC, hBitmap2);   
		
		if(bitmap_number < MAX_NUM_TOOLBAR_BUTTONS)
			BitBlt(hdc,sx1+4, sy1+4,16,16, hMemoryDC,(bitmap_number)*16, 1, SRCAND);
   
		SelectObject(hMemoryDC,hOldBitmap);
		DeleteDC(hMemoryDC);
		Delete_Brush_Color(hdc);
		Delete_Pen_Color(hdc);		
	}
}

void DrawScrollButton(HDC hdc,int x,int y, int status)
{
	int sx1,sx2;
	int sy1,sy2;
	int nIndex;
	COLORREF color;
	
	SetBkMode(hdc,TRANSPARENT);
	SetBkColor(hdc,RGB(255,255,255));
	nIndex = COLOR_ACTIVEBORDER;

	sx1=x;
	sx2=x+12;
	sy1=y;
	sy2=y+12;

	if(status==DRAW_BUTTON_ON)
	{
		holdbrush_color = (HBRUSH)SelectObject(hdc,GetSysColorBrush(nIndex));  
		Rectangle(hdc,sx1,sy1,sx2,sy2); 
              
		Select_Pen_Color(hdc,WHITE);    
		MoveToEx(hdc,sx1,sy2,NULL);
		LineTo(hdc,sx1,sy1);    // left
		LineTo(hdc,sx2,sy1);    // top	
		Delete_Pen_Color(hdc);    
	            
		Select_Pen_Color(hdc,BLACK);    
		MoveToEx(hdc,sx1+1,sy2-1,NULL);
		LineTo(hdc,sx2-1,sy2-1);  // bottom  
		LineTo(hdc,sx2-1,sy1+1);  // right	
		Delete_Pen_Color(hdc);	
  	
		hMemoryDC = CreateCompatibleDC(hdc);   
		hOldBitmap = (HBITMAP)SelectObject(hMemoryDC, hBitmap2);   
		
		SelectObject(hMemoryDC,hOldBitmap);
		Delete_Brush_Color(hdc);
		DeleteDC(hMemoryDC);
	}

	if(status==DRAW_BUTTON_OFF)
	{		
		holdbrush_color = (HBRUSH)SelectObject(hdc,GetSysColorBrush(nIndex));  
		Rectangle(hdc,sx1,sy1,sx2,sy2); 
		
		Select_Pen_Color(hdc,GRAY);    
		MoveToEx(hdc,sx1,sy2,NULL);
		LineTo(hdc,sx1,sy1);    // left
		LineTo(hdc,sx2,sy1);    // top	
		Delete_Pen_Color(hdc);  
		
		MoveToEx(hdc,sx1,sy2,NULL);
		LineTo(hdc,sx2,sy2);  // bottom  
		LineTo(hdc,sx2,sy1);  // right	
		Delete_Pen_Color(hdc);  

		hMemoryDC = CreateCompatibleDC(hdc);   
		hOldBitmap = (HBITMAP)SelectObject(hMemoryDC, hBitmap2);   
		
		SelectObject(hMemoryDC,hOldBitmap);
		DeleteDC(hMemoryDC);
		Delete_Brush_Color(hdc);
		Delete_Pen_Color(hdc);		
	}

	if(status==ERASE_BUTTON)
	{
		color = (COLORREF)GetSysColor(nIndex);
		hpen_color = CreatePen(PS_SOLID,1,color);  
		holdpen_color = (HPEN)SelectObject(hdc,hpen_color);    

		holdbrush_color = (HBRUSH)SelectObject(hdc,GetSysColorBrush(nIndex));  
		Rectangle(hdc,sx1,sy1,sx2,sy2+1); 
		Delete_Brush_Color(hdc);
	
		SelectObject(hdc,holdpen_color);   
		DeleteObject(hpen_color);

	}
              
}

/////////////////////////////////////////////////////////////////////////////

void HighLightObjectWithMouse(HDC hdc, int x, int z, editor_ptr edptr)
{
	int i,j;
	int x_offset=20;
	int ob_type;
	int poly_cnt = 0;
	float lx,lz,k=(float)0.017453292;
	float x_off,z_off,s;
	float wx,wy,wz;
	float sx,sz;
	float sine,cosine;
	double ay;
	int start_sx, end_sx;
	int start_sz, end_sz;
	BOOL poly_visible_flag;
	float dist, min_dist;
	int closest_object = 0;
	float dx, dz;
	int num_verts;
	char buffer[256];



	if(edptr->wptr==NULL)
		return;

	min_dist = (float)10000;

	s = edptr->display_scale;
	x_off =	(float)edptr->display_x_offset;	
	z_off =	(float)edptr->display_y_offset;

	itoa(x,buffer,10);
	TextOut(hdc,20,50,buffer,strlen(buffer));

	itoa(z,buffer,10);
	TextOut(hdc,70,50,buffer,strlen(buffer));


	if(edptr->wptr->oblist_length != 0)
	{
		// scan all polys in map //////////////////////////////////////////////
	
		for(i = 0; i < edptr->wptr->oblist_length; i++)
		{		
			edptr->wptr->oblist[i].ed_poly_color.r = 0;
			edptr->wptr->oblist[i].ed_poly_color.g = 0;
			edptr->wptr->oblist[i].ed_poly_color.b = 0;

			edptr->wptr->oblist[i].IsHighlighted = FALSE;

			wx = edptr->wptr->oblist[i].x;
			wy = edptr->wptr->oblist[i].y;
			wz = edptr->wptr->oblist[i].z;
		
			ay = (double)edptr->wptr->oblist[i].rot_angle * k;
			cosine = (float)cos(ay);
			sine   = (float)sin(ay);	
			ob_type = edptr->wptr->oblist[i].type;
			
			poly_visible_flag = FALSE;
			num_verts = edptr->wptr->num_vert_per_object[ob_type];
		
			start_sx = -50;
			end_sx   = 550;
			start_sz = -50;
			end_sz   = 400;
			

			for(j = 0; j < num_verts; j++)
			{
				lx = edptr->wptr->obdata[ob_type].v[j].x;
				lz = edptr->wptr->obdata[ob_type].v[j].z;

				sx = wx + (lx * cosine - lz * sine); 
				sz = wz + (lx * sine   + lz * cosine);

				dx = (float)x - sx;
				dz = (float)z - sz;

				dist = (float)sqrt(dx * dx + dz * dz);

				if(dist < min_dist) 
				{
					min_dist = dist;
					closest_object = i;
				}	
			}	
		} // end for i
	} // end if

	edptr->wptr->oblist[closest_object].ed_poly_color.r = 0;
	edptr->wptr->oblist[closest_object].ed_poly_color.g = 0;
	edptr->wptr->oblist[closest_object].ed_poly_color.b = 255;

	edptr->wptr->oblist[closest_object].IsHighlighted = TRUE;
	
	DrawEditorMap(hdc, edptr);
}



void PlaceObjectWithMouse(editor_ptr edptr, int x,int z)
{
	int i,dx,dz;

	i = edptr->wptr->oblist_length;

	dx = x/20;
	x = dx *20;
	
	dz = z/20;
	z = dz *20;

	edptr->wptr->oblist[i].x = (float)x;
	edptr->wptr->oblist[i].y = (float)3;
	edptr->wptr->oblist[i].z = (float)z;
	edptr->wptr->oblist[i].type = edptr->current_object_id;
	edptr->wptr->oblist[i].rot_angle = (float)edptr->object_rot_angle;
	AddOblistLightArray(i);
	edptr->wptr->oblist_length++;
	
}

void SetLinkWithMouse(int x,int z)
{
	int i,count;
	double dx,dz;
	float dist;
	float min_dist=(float)100000;
	

	for(i=0;i<last_link;i++)
	{
		dx=fabs((float)x-LinksList[i].last_x);
		dz=fabs((float)z-LinksList[i].last_z);

		dist=(float)sqrt(dx*dx+dz*dz);
		if(dist < min_dist)
		{
			min_dist=dist;
			count=i;
		}
	}
	current_link=count;
	last_x=LinksList[current_link].last_x;
	last_z=LinksList[current_link].last_z;
	last_angle=LinksList[current_link].last_angle;
}

void InitLinkList(editor_ptr edptr)
{
	int i;

	for(i=0;i<MAX_NUM_LINKS;i++)
	{
		LinksList[i].last_x=(float)0;
		LinksList[i].last_z=(float)0;
		LinksList[i].last_angle=(float)0;
	}
	current_link=0;
	last_link=0;

	last_x=(float)760;
	last_z=(float)600;
	last_angle=(float)180;
	AddLinkToList();

	last_x=(float)960;
	last_z=(float)600;
	last_angle=(float)180;
	AddLinkToList();

	last_x=(float)600;
	last_z=(float)600;
	last_angle=(float)0;
	AddLinkToList();

	current_link=1;
}

void AddLinkToList()
{
	LinksList[last_link].last_x=last_x;
	LinksList[last_link].last_z=last_z;
	LinksList[last_link].last_angle=last_angle;

	current_link = last_link;
	last_link++;
}

void UndoLastLink(editor_ptr edptr)
{
	int i, obtype;
	int num_links=0;
	int oblist_length;

	oblist_length = edptr->wptr->oblist_length;
	obtype = edptr->wptr->oblist[oblist_length].type;

	switch(obtype)
	{
		case OBJECT_ID_STRAIGHT_ROAD:
			num_links = 1;
			break;

		case OBJECT_ID_SMALL_STRAIGHT_ROAD:		
			num_links = 1;
			break;

		case OBJECT_ID_LEFTCURVE_ROAD:	
			num_links = 1;
			break;

		case OBJECT_ID_RIGHTCURVE_ROAD:	
			num_links = 1;
			break;

		case OBJECT_ID_TJUNCTION:
			num_links = 2;
			break;

		case OBJECT_ID_CROSSROADS:
			num_links = 3;
			break;

		case OBJECT_ID_LEFTCORNER_ROAD:
			num_links = 1;
			break;

		case OBJECT_ID_RIGHTCORNER_ROAD:
			num_links = 1;
			break;

		case OBJECT_ID_LEFT_TJUNCTION:
			num_links = 2;
			break;

		case OBJECT_ID_RIGHT_TJUNCTION:
			num_links = 2;
			break;

		case OBJECT_ID_ZEBRA:
			num_links = 1;
			break;
	}

	for(i=0; i<num_links; i++)
	{
		last_link--;
		if(last_link < 2)
			last_link = 2;

		current_link = last_link-1;

		last_x=LinksList[current_link].last_x;
		last_z=LinksList[current_link].last_z;
		last_angle=LinksList[current_link].last_angle;
	}
}

void ReplaceLinkInList()
{
	AddLinkToList();
	//LinksList[current_link].active_link_flag = FALSE;
}

void ChangeCurrentLink(int direction)
{
	if(direction==1)
	{
		current_link++;
		if (current_link>=last_link)
			current_link=0;
		last_x=LinksList[current_link].last_x;
		last_z=LinksList[current_link].last_z;
		last_angle=LinksList[current_link].last_angle;
	}

}

void DisplayLinksList(HDC hdc)
{
	int x,z,a;
	int i;
	char buffer[50];

	

	for(i=0;i<last_link;i++)
	{
		x=(int)LinksList[i].last_x;
		z=(int)LinksList[i].last_z;
		a=(int)LinksList[i].last_angle;

		itoa(x,buffer,10);
		TextOut(hdc,20,20,"x =",3);
		TextOut(hdc,70+i*50,20,buffer,lstrlen(buffer));

		itoa(z,buffer,10);
		TextOut(hdc,20,40,"z =",3);
		TextOut(hdc,70+i*50,40,buffer,lstrlen(buffer));

		itoa(a,buffer,10);
		TextOut(hdc,20,60,"a =",3);
		TextOut(hdc,70+i*50,60,buffer,lstrlen(buffer));	
	}
	itoa(current_link,buffer,10);
	TextOut(hdc,20,100,"CL =",3);
	TextOut(hdc,70,100,buffer,lstrlen(buffer));

	itoa(last_link,buffer,10);
	TextOut(hdc,20,120,"LL =",3);
	TextOut(hdc,70,120,buffer,lstrlen(buffer));

	itoa((int)last_x,buffer,10);
	TextOut(hdc,20,140,"Lx =",3);
	TextOut(hdc,70,140,buffer,lstrlen(buffer));

	itoa((int)last_z,buffer,10);
	TextOut(hdc,20,160,"Lz =",3);
	TextOut(hdc,70,160,buffer,lstrlen(buffer));
}

void Draw_2D_Map(HDC hdc,CAMERA cam)
{
    
}

void DrawEditorMap(HDC hdc, editor_ptr edptr)
{
	HRGN hrgn; 
	HPEN hpen_color,holdpen_color;
	int i,j,w,z,x;
	int x_offset=20;
	int vert_cnt,ob_vert_count,ob_type;
	int poly,poly_cnt = 0;
	float lx,lz,k=(float)0.017453292;
	float x_off,z_off,s;
	float cross_x,cross_z;
	float wx,wy,wz;
	float sx,sz;
	float stx,stz;
	float sine,cosine;
	double ay;
	int start_x, start_z;
	int end_x, end_z;
	int start_sx, end_sx;
	int start_sz, end_sz;
	BOOL poly_visible_flag;

	Rectangle(hdc,0,32,536,384);
    hrgn=CreateRectRgn(0+1,32+1,536-1,384-1);
	SelectClipRgn(hdc,hrgn);
	
	s = edptr->display_scale;
	x_off =	(float)edptr->display_x_offset;	
	z_off =	(float)edptr->display_y_offset;

	hpen_color = CreatePen(PS_SOLID,1,RGB(200,200,200));  
	holdpen_color = (HPEN)SelectObject(hdc,hpen_color);    

	start_x = (int)x_off;
	start_z = (int)z_off;

	end_x = (int) ((float)600/s);
	end_z = (int) ((float)600/s);

	if(s*20 >2)
	{
		for(z = start_z; z < end_z; z += 20)
		{
			sx = s * start_x;
			sz = s * z;
			
			MoveToEx(hdc,(int)sx,32 + (int)sz,NULL);	

			sx = s * end_x;
		
			LineTo(hdc,(int)sx, 32 + (int)sz);
		}

		for(x = start_x; x < end_x; x += 20)
		{
			sz = s * start_z;
			sx = s * x;
			
			MoveToEx(hdc,(int)sx,32 + (int)sz,NULL);	

			sz = s * end_z;
		
			LineTo(hdc,(int)sx, 32 + (int)sz);
		}
	}	
	
	SelectObject(hdc,holdpen_color);   
	DeleteObject(hpen_color);

	// cell lines

	hpen_color = CreatePen(PS_SOLID,1,RGB(255,0,0));  
	holdpen_color = (HPEN)SelectObject(hdc,hpen_color);  

	for(z = start_z; z < end_z; z += 256)
	{
		sx = s * start_x;
		sz = s * z;
			
		MoveToEx(hdc,(int)sx,32 + (int)sz,NULL);	

		sx = s * end_x;
	
		LineTo(hdc,(int)sx, 32 + (int)sz);
	}

	for(x = start_x; x < end_x; x += 256)
	{
		sz = s * start_z;
		sx = s * x;
			
		MoveToEx(hdc,(int)sx,32 + (int)sz,NULL);	

		sz = s * end_z;
		
		LineTo(hdc,(int)sx, 32 + (int)sz);
	}

	SelectObject(hdc,holdpen_color);   
	DeleteObject(hpen_color);
		
	
	if(edptr->wptr==NULL)
		return;

	if(edptr->wptr->oblist_length != 0)
	{
		// Draw all polys in map //////////////////////////////////////////////
	
		for(i=0;i<edptr->wptr->oblist_length;i++)
		{		
			wx = edptr->wptr->oblist[i].x;
			wy = edptr->wptr->oblist[i].y;
			wz = edptr->wptr->oblist[i].z;
		
			ay=(double)edptr->wptr->oblist[i].rot_angle*k;
			cosine = (float)cos(ay);
			sine   = (float)sin(ay);	
			ob_type = edptr->wptr->oblist[i].type;
		
			j=0;
			ob_vert_count=0;
			poly=edptr->wptr->num_vert_per_object[ob_type]/4;
		
			start_sx = -50;
			end_sx = 600;

			start_sz = -50;
			end_sz = 400;

			for(w=0; w<poly; w++)
			{
				//edptr->MouseX;

				poly_visible_flag = FALSE;

				for(vert_cnt = 0; vert_cnt < 4; vert_cnt++)
				{
					lx = edptr->wptr->obdata[ob_type].v[j].x;
					lz = edptr->wptr->obdata[ob_type].v[j].z;

					sx = s * (x_off + wx + (lx*cosine - lz*sine)); 
					sz = s * (z_off + wz + (lx*sine + lz*cosine));

					// check at that at least one vert is visible on screen

					if((sx < end_sx && sx > start_sx) && 
				       (sz < end_sz && sz > start_sz))
					{
						poly_visible_flag = TRUE;
					}
				
				}

				if(poly_visible_flag == TRUE)
				{
					hpen_color = CreatePen(PS_SOLID, 1, RGB(
						edptr->wptr->oblist[i].ed_poly_color.r,
						edptr->wptr->oblist[i].ed_poly_color.g,
						edptr->wptr->oblist[i].ed_poly_color.b)); 
					
					holdpen_color = (HPEN)SelectObject(hdc, hpen_color);  
						
					lx = edptr->wptr->obdata[ob_type].v[j].x;
					lz = edptr->wptr->obdata[ob_type].v[j].z;
					sx = s * (x_off + wx + (lx*cosine - lz*sine)); 
					sz = s * (z_off + wz + (lx*sine + lz*cosine));
					
					MoveToEx(hdc,(int)sx,32+(int)sz,NULL);
					stx=sx;
					stz=sz;
						
					///////

					lx = edptr->wptr->obdata[ob_type].v[j+1].x;
					lz = edptr->wptr->obdata[ob_type].v[j+1].z;
					sx = s * (x_off + wx + (lx*cosine - lz*sine)); 
					sz = s * (z_off + wz + (lx*sine + lz*cosine));

					LineTo(hdc,(int)sx,32+(int)sz);

					///////

					lx = edptr->wptr->obdata[ob_type].v[j+3].x;
					lz = edptr->wptr->obdata[ob_type].v[j+3].z;
					sx = s * (x_off + wx + (lx*cosine - lz*sine)); 
					sz = s * (z_off + wz + (lx*sine + lz*cosine));

					LineTo(hdc,(int)sx,32+(int)sz);

					///////

					lx = edptr->wptr->obdata[ob_type].v[j+2].x;
					lz = edptr->wptr->obdata[ob_type].v[j+2].z;
					sx = s * (x_off + wx + (lx*cosine - lz*sine)); 
					sz = s * (z_off + wz + (lx*sine + lz*cosine));

					LineTo(hdc,(int)sx,32+(int)sz);

					///////

					sx = s * (x_off + wx + (lx*cosine - lz*sine)); 
					sz = s * (z_off + wz + (lx*sine + lz*cosine));

					LineTo(hdc,(int)stx,32+(int)stz);
				
					SelectObject(hdc,holdpen_color);   
					DeleteObject(hpen_color);
		
					j+=4;
					ob_vert_count+=4;
						
					poly_cnt++;

				} // end if
				 
			} // end for poly
		} // end for i
	} // end if

	// draw cross at current link
	cross_x=LinksList[current_link].last_x;
	cross_z=LinksList[current_link].last_z;

	hpen_color = CreatePen(PS_SOLID,1,RGB(255,0,0));  
	holdpen_color = (HPEN)SelectObject(hdc,hpen_color);    

	MoveToEx(hdc,(int)((cross_x-8+x_off)*s),32+(int)((cross_z-8+z_off)*s),NULL);
	LineTo(hdc,  (int)((cross_x+8+x_off)*s),32+(int)((cross_z+8+z_off)*s));

	MoveToEx(hdc,(int)((cross_x+8+x_off)*s),32+(int)((cross_z-8+z_off)*s),NULL);
	LineTo(hdc,  (int)((cross_x-8+x_off)*s),32+(int)((cross_z+8+z_off)*s));

	SelectObject(hdc,holdpen_color);   
	DeleteObject(hpen_color);
}


void DrawEditorMapObject(HDC hdc, editor_ptr edptr)
{
	HRGN hrgn; 
	HPEN hpen_color,holdpen_color;
	int w;
	int x_offset=20;
	int ob_type;
	float lx,lz,k=(float)0.017453292;
	float x_off,z_off,s;
	float wx,wy,wz;
	float sx,sz;
	float stx,stz;
	float sine,cosine;
	double ay;
	int dx,dz;
	int temp, poly;
	int curr_vert;
	int j;


    hrgn = CreateRectRgn(0+1,32+1,536-1,384-1);
	SelectClipRgn(hdc,hrgn);
	
	s = edptr->display_scale;
	x_off =	(float)edptr->display_x_offset;	
	z_off =	(float)edptr->display_y_offset;

	hpen_color = CreatePen(PS_SOLID,1,RGB(200,200,200));  
	holdpen_color = (HPEN)SelectObject(hdc,hpen_color);    

	SelectObject(hdc,holdpen_color);   
	DeleteObject(hpen_color);

	if(edptr->wptr == NULL)	
		return;
	
	
	wx = (float)edptr->MouseX;
	wy = (float)1;
	wz = (float)edptr->MouseY;

	dx = (int) (wx/(float)20);
	wx = (float) (dx *(float)20);
	
	dz = (int) (wz/(float)20);
	wz = (float) (dz *(float)20);
		
	ay=(double)edptr->object_rot_angle * k;
	cosine = (float)cos(ay);
	sine   = (float)sin(ay);	
	ob_type = edptr->current_object_id; //OBJECT_ID_HOUSE;
		
	curr_vert = 0;
	j=0;

	poly = world.num_polys_per_object[ob_type];
		
	for(w = 0; w < poly; w++)
	{
		temp = world.obdata[ob_type].num_vert[w];

	
		hpen_color = CreatePen(PS_SOLID, 1, RGB(128,128,128));
		
					
		holdpen_color = (HPEN)SelectObject(hdc, hpen_color);  
						
		lx = edptr->wptr->obdata[ob_type].v[j].x;
		lz = edptr->wptr->obdata[ob_type].v[j].z;
		sx = s * (x_off + wx + (lx*cosine - lz*sine)); 
		sz = s * (z_off + wz + (lx*sine + lz*cosine));
					
		MoveToEx(hdc,(int)sx,32+(int)sz,NULL);
		stx=sx;
		stz=sz;
						
		///////

		lx = edptr->wptr->obdata[ob_type].v[j+1].x;
		lz = edptr->wptr->obdata[ob_type].v[j+1].z;
		sx = s * (x_off + wx + (lx*cosine - lz*sine)); 
		sz = s * (z_off + wz + (lx*sine + lz*cosine));

		LineTo(hdc,(int)sx,32+(int)sz);

		///////

		lx = edptr->wptr->obdata[ob_type].v[j+3].x;
		lz = edptr->wptr->obdata[ob_type].v[j+3].z;
		sx = s * (x_off + wx + (lx*cosine - lz*sine)); 
		sz = s * (z_off + wz + (lx*sine + lz*cosine));

		LineTo(hdc,(int)sx,32+(int)sz);

		//////

		lx = edptr->wptr->obdata[ob_type].v[j+2].x;
		lz = edptr->wptr->obdata[ob_type].v[j+2].z;
		sx = s * (x_off + wx + (lx*cosine - lz*sine)); 
		sz = s * (z_off + wz + (lx*sine + lz*cosine));

		LineTo(hdc,(int)sx,32+(int)sz);

		///////

		sx = s * (x_off + wx + (lx*cosine - lz*sine)); 
		sz = s * (z_off + wz + (lx*sine + lz*cosine));

		LineTo(hdc,(int)stx,32+(int)stz);
				
		SelectObject(hdc,holdpen_color);   
		DeleteObject(hpen_color);
		
		j+=4;
	} 
}



void ReloadMainMap()
{
	edmap.num_poly=current_poly;
	//SetMap(&edmap);
}

void CheckAngle()
{
	if(last_angle < 0)
		last_angle = last_angle + 360;

	if(last_angle >= 360)
		last_angle = last_angle - 360;
}

void DrawRoadSection(HDC hdc, editor_ptr edptr, int instruction)
{
	switch(instruction)
	{
		case LEFT_HAND_CURVE:
			AddLeftCurve(hdc, edptr);
			ReplaceLinkInList();
			DrawEditorMap(hdc, edptr);
			break;

		case RIGHT_HAND_CURVE:
			AddRightCurve(hdc, edptr);
			ReplaceLinkInList();
			DrawEditorMap(hdc, edptr);
			break;

		case STRAIGHT:
			AddStraight(hdc, edptr);
			DrawEditorMap(hdc, edptr);
			break;

		case ZEBRA:
			AddZebra(hdc, edptr);
			DrawEditorMap(hdc, edptr);
			break;

		case SMALL_STRAIGHT:
			AddSmallStraight(hdc, edptr);
			DrawEditorMap(hdc, edptr);
			break;

		case T_JUNCTION:
			AddTJunction(OBJECT_ID_TJUNCTION, edptr);
			DrawEditorMap(hdc, edptr);
			break;

		case LEFT_T_JUNCTION:
			AddLeftTJunction(OBJECT_ID_LEFT_TJUNCTION, edptr);
			DrawEditorMap(hdc, edptr);
			break;

		case RIGHT_T_JUNCTION:
			AddRightTJunction(OBJECT_ID_RIGHT_TJUNCTION, edptr);
			DrawEditorMap(hdc, edptr);
			break;

		case ROUND_ABOUT:
			AddRoundAbout(hdc, edptr);
			DrawEditorMap(hdc, edptr);
			break;

		case CROSSROAD:
			AddCrossRoad(hdc, edptr);
			DrawEditorMap(hdc, edptr);
			break;

		case LEFT_CORNER:
			AddLeftCorner(hdc, edptr);
			DrawEditorMap(hdc, edptr);
			break;
			
		case RIGHT_CORNER:
			AddRightCorner(hdc, edptr);
			DrawEditorMap(hdc, edptr);
			break;


		case MAINROAD_STRAIGHT:
			AddMainRoadStraight(hdc, edptr);
			DrawEditorMap(hdc, edptr);
			break;

		case MAINROAD_TJUNCTION:
			AddTJunction(OBJECT_ID_MAINROAD_TJUNCTION, edptr);
			DrawEditorMap(hdc, edptr);
			break;

		case MAINROAD_TJUNCTION2:
			AddLeftTJunction(OBJECT_ID_MAINROAD_TJUNCTION2, edptr);
			DrawEditorMap(hdc, edptr);
			break;

		case MAINROAD_TJUNCTION3:
			AddRightTJunction(OBJECT_ID_MAINROAD_TJUNCTION3, edptr);
			DrawEditorMap(hdc, edptr);
			break;

		case MAINROAD_CROSSROADS:
			AddMainRoadCrossroads(OBJECT_ID_MAINROAD_CROSSROADS, edptr);
			DrawEditorMap(hdc, edptr);
			break;
		
		case MAINROAD_CROSSROADS2:
			AddMainRoadCrossroads(OBJECT_ID_MAINROAD_CROSSROADS2, edptr);
			DrawEditorMap(hdc, edptr);
			break;
	}

}

// OBJECT_ID_MAINROAD_STRAIGHT	
// OBJECT_ID_MAINROAD_TJUNCTION	
// OBJECT_ID_MAINROAD_CROSSROADS	

void AddMainRoadStraight(HDC hdc, editor_ptr edptr)
{
	int i;
	float x,z;
	float angle,k;
	
	k=(float)0.017453292;
	i=edptr->wptr->oblist_length;
	edptr->wptr->oblist[i].x = LinksList[current_link].last_x;
	edptr->wptr->oblist[i].y = (float)0;
	edptr->wptr->oblist[i].z = LinksList[current_link].last_z;
	edptr->wptr->oblist[i].rot_angle = LinksList[current_link].last_angle;
	edptr->wptr->oblist[i].type = OBJECT_ID_MAINROAD_STRAIGHT;


	angle = k * LinksList[current_link].last_angle;		
	
	x = edptr->wptr->obdata[OBJECT_ID_MAINROAD_STRAIGHT].connection[0].x;
	z = edptr->wptr->obdata[OBJECT_ID_MAINROAD_STRAIGHT].connection[0].z;
	last_x += (x*(float)cos(angle) - z*(float)sin(angle)); 
	last_z += (x*(float)sin(angle) + z*(float)cos(angle));
	
	ReplaceLinkInList();
	AddOblistLightArray(i);	
	edptr->wptr->oblist_length++;
	
}		
	

void AddMainRoadTJunction(DWORD object_id, editor_ptr edptr)
{
	int i;
	float x,z;
	float rad_angle,deg_angle,k;
	float start_x,start_z;
	
	k=(float)0.017453292;
	i=edptr->wptr->oblist_length;
	edptr->wptr->oblist[i].x = LinksList[current_link].last_x;
	edptr->wptr->oblist[i].y = (float)0;
	edptr->wptr->oblist[i].z = LinksList[current_link].last_z;
	edptr->wptr->oblist[i].rot_angle = LinksList[current_link].last_angle;
	edptr->wptr->oblist[i].type = object_id;
	

	start_x = last_x;
	start_z = last_z;
						
	rad_angle = k * LinksList[current_link].last_angle;
	deg_angle = LinksList[current_link].last_angle;

	last_angle = deg_angle + 270;
	CheckAngle();
	x = edptr->wptr->obdata[object_id].connection[0].x;
	z = edptr->wptr->obdata[object_id].connection[0].z;
	last_x += (x*(float)cos(rad_angle) - z*(float)sin(rad_angle)); 
	last_z +=  (x*(float)sin(rad_angle) + z*(float)cos(rad_angle));
	ReplaceLinkInList();
	current_link = last_link;

	last_angle = deg_angle + 90;
	CheckAngle();
	x = edptr->wptr->obdata[object_id].connection[1].x;
	z = edptr->wptr->obdata[object_id].connection[1].z;
	last_x = start_x + (x*(float)cos(rad_angle) - z*(float)sin(rad_angle)); 
	last_z = start_z + (x*(float)sin(rad_angle) + z*(float)cos(rad_angle));
	AddLinkToList();
	
	last_x=LinksList[current_link].last_x;
	last_z=LinksList[current_link].last_z;
	last_angle=LinksList[current_link].last_angle;

	AddOblistLightArray(i);
	edptr->wptr->oblist_length++;	

}


void AddMainRoadCrossroads(DWORD object_id, editor_ptr edptr)
{
		int i;
	float x,z;
	float rad_angle,deg_angle,k;
	float start_x,start_z;
	
	k=(float)0.017453292;
	i=edptr->wptr->oblist_length;
	edptr->wptr->oblist[i].x = LinksList[current_link].last_x;
	edptr->wptr->oblist[i].y = (float)0;
	edptr->wptr->oblist[i].z = LinksList[current_link].last_z;
	edptr->wptr->oblist[i].rot_angle = LinksList[current_link].last_angle;
	edptr->wptr->oblist[i].type = object_id;


	start_x = LinksList[current_link].last_x;
	start_z = LinksList[current_link].last_z;
			
	rad_angle = k * LinksList[current_link].last_angle;
	deg_angle = LinksList[current_link].last_angle;
	last_angle= deg_angle;
	x = edptr->wptr->obdata[object_id].connection[0].x;
	z = edptr->wptr->obdata[object_id].connection[0].z;
	last_x = start_x + (x*(float)cos(rad_angle) - z*(float)sin(rad_angle)); 
	last_z = start_z + (x*(float)sin(rad_angle) + z*(float)cos(rad_angle));
	ReplaceLinkInList();
	
	last_angle = deg_angle + 270;
	CheckAngle();
	x = edptr->wptr->obdata[object_id].connection[1].x;
	z = edptr->wptr->obdata[object_id].connection[1].z;
	last_x = start_x + (x*(float)cos(rad_angle) - z*(float)sin(rad_angle)); 
	last_z = start_z + (x*(float)sin(rad_angle) + z*(float)cos(rad_angle));
	AddLinkToList();

	last_angle = deg_angle + 90;
	CheckAngle();
	x = edptr->wptr->obdata[object_id].connection[2].x;
	z = edptr->wptr->obdata[object_id].connection[2].z;
	last_x = start_x + (x*(float)cos(rad_angle) - z*(float)sin(rad_angle)); 
	last_z = start_z + (x*(float)sin(rad_angle) + z*(float)cos(rad_angle));
	AddLinkToList();
	
	last_x=LinksList[current_link].last_x;
	last_z=LinksList[current_link].last_z;
	last_angle=LinksList[current_link].last_angle;

	AddOblistLightArray(i);
	edptr->wptr->oblist_length++;	
	
}


void AddLeftCurve(HDC hdc, editor_ptr edptr)
{
	int i;
	float x,z;
	float angle,k;
	
	k=(float)0.017453292;
	i=edptr->wptr->oblist_length;
	edptr->wptr->oblist[i].x = LinksList[current_link].last_x;
	edptr->wptr->oblist[i].y = (float)0;
	edptr->wptr->oblist[i].z = LinksList[current_link].last_z;
	edptr->wptr->oblist[i].rot_angle = LinksList[current_link].last_angle;
	edptr->wptr->oblist[i].type = OBJECT_ID_LEFTCURVE_ROAD;
//	edptr->wptr->oblist[i].light = -1;

	angle = k * LinksList[current_link].last_angle;		
	
	x = edptr->wptr->obdata[OBJECT_ID_LEFTCURVE_ROAD].connection[0].x;
	z = edptr->wptr->obdata[OBJECT_ID_LEFTCURVE_ROAD].connection[0].z;
	last_x += (x*(float)cos(angle) - z*(float)sin(angle)); 
	last_z += (x*(float)sin(angle) + z*(float)cos(angle));

	last_angle += 15;
	CheckAngle();

	AddOblistLightArray(i);
	edptr->wptr->oblist_length++;
}
	
void AddRightCurve(HDC hdc, editor_ptr edptr)
{
	int i;
	float x,z;
	float angle,k;
	
	k=(float)0.017453292;
	i=edptr->wptr->oblist_length;
	edptr->wptr->oblist[i].x = LinksList[current_link].last_x;
	edptr->wptr->oblist[i].y = (float)0;
	edptr->wptr->oblist[i].z = LinksList[current_link].last_z;
	edptr->wptr->oblist[i].rot_angle = LinksList[current_link].last_angle;
	edptr->wptr->oblist[i].type = OBJECT_ID_RIGHTCURVE_ROAD;
//	edptr->wptr->oblist[i].light = -1;

	angle = k * LinksList[current_link].last_angle;		
	
	x = edptr->wptr->obdata[OBJECT_ID_RIGHTCURVE_ROAD].connection[0].x;
	z = edptr->wptr->obdata[OBJECT_ID_RIGHTCURVE_ROAD].connection[0].z;
	last_x += (x*(float)cos(angle) - z*(float)sin(angle)); 
	last_z += (x*(float)sin(angle) + z*(float)cos(angle));

	last_angle -= 15;
	CheckAngle();

	AddOblistLightArray(i);
	edptr->wptr->oblist_length++;
}

void AddLeftCorner(HDC hdc, editor_ptr edptr)
{
	int i;
	float x,z;
	float angle,k,deg_angle;
	
	k=(float)0.017453292;
	i=edptr->wptr->oblist_length;
	edptr->wptr->oblist[i].x = LinksList[current_link].last_x;
	edptr->wptr->oblist[i].y = (float)0;
	edptr->wptr->oblist[i].z = LinksList[current_link].last_z;
	edptr->wptr->oblist[i].rot_angle = LinksList[current_link].last_angle;
	edptr->wptr->oblist[i].type = OBJECT_ID_LEFTCORNER_ROAD;
//	edptr->wptr->oblist[i].light = -1;

	deg_angle = LinksList[current_link].last_angle;
	last_angle = deg_angle + 90;
	CheckAngle();

	angle = k * LinksList[current_link].last_angle;		
	
	x = edptr->wptr->obdata[OBJECT_ID_LEFTCORNER_ROAD].connection[0].x;
	z = edptr->wptr->obdata[OBJECT_ID_LEFTCORNER_ROAD].connection[0].z;
	last_x += (x*(float)cos(angle) - z*(float)sin(angle)); 
	last_z += (x*(float)sin(angle) + z*(float)cos(angle));
	
	ReplaceLinkInList();
		
	AddOblistLightArray(i);
	edptr->wptr->oblist_length++;
	
}

void AddRightCorner(HDC hdc, editor_ptr edptr)
{
	int i;
	float x,z;
	float angle,k,deg_angle;
	
	k=(float)0.017453292;
	i=edptr->wptr->oblist_length;
	edptr->wptr->oblist[i].x = LinksList[current_link].last_x;
	edptr->wptr->oblist[i].y = (float)0;
	edptr->wptr->oblist[i].z = LinksList[current_link].last_z;
	edptr->wptr->oblist[i].rot_angle = LinksList[current_link].last_angle;
	edptr->wptr->oblist[i].type = OBJECT_ID_RIGHTCORNER_ROAD;
//	edptr->wptr->oblist[i].light = -1;

	deg_angle = LinksList[current_link].last_angle;
	last_angle = deg_angle - 90;
	CheckAngle();
	angle = k * LinksList[current_link].last_angle;		
	
	x = edptr->wptr->obdata[OBJECT_ID_RIGHTCORNER_ROAD].connection[0].x;
	z = edptr->wptr->obdata[OBJECT_ID_RIGHTCORNER_ROAD].connection[0].z;
	last_x += (x*(float)cos(angle) - z*(float)sin(angle)); 
	last_z += (x*(float)sin(angle) + z*(float)cos(angle));
	
	ReplaceLinkInList();
		
	AddOblistLightArray(i);
	edptr->wptr->oblist_length++;
	
}

void AddZebra(HDC hdc, editor_ptr edptr)
{
	int i;
	float x,z;
	float angle,k;
	
	k=(float)0.017453292;
	i=edptr->wptr->oblist_length;
	edptr->wptr->oblist[i].x = LinksList[current_link].last_x;
	edptr->wptr->oblist[i].y = (float)0;
	edptr->wptr->oblist[i].z = LinksList[current_link].last_z;
	edptr->wptr->oblist[i].rot_angle = LinksList[current_link].last_angle;
	edptr->wptr->oblist[i].type = OBJECT_ID_ZEBRA;
//	edptr->wptr->oblist[i].light = -1;

	angle = k * LinksList[current_link].last_angle;		
	
	x = edptr->wptr->obdata[OBJECT_ID_ZEBRA].connection[0].x;
	z = edptr->wptr->obdata[OBJECT_ID_ZEBRA].connection[0].z;
	last_x += (x*(float)cos(angle) - z*(float)sin(angle)); 
	last_z += (x*(float)sin(angle) + z*(float)cos(angle));
	
	ReplaceLinkInList();
		
	AddOblistLightArray(i);
	edptr->wptr->oblist_length++;
	
}

void AddStraight(HDC hdc, editor_ptr edptr)
{
	int i;
	float x,z;
	float angle,k;
	
	k=(float)0.017453292;
	i=edptr->wptr->oblist_length;
	edptr->wptr->oblist[i].x = LinksList[current_link].last_x;
	edptr->wptr->oblist[i].y = (float)0;
	edptr->wptr->oblist[i].z = LinksList[current_link].last_z;
	edptr->wptr->oblist[i].rot_angle = LinksList[current_link].last_angle;
	edptr->wptr->oblist[i].type = OBJECT_ID_STRAIGHT_ROAD;
//	edptr->wptr->oblist[i].light = -1;

	angle = k * LinksList[current_link].last_angle;		
	
	x = edptr->wptr->obdata[OBJECT_ID_STRAIGHT_ROAD].connection[0].x;
	z = edptr->wptr->obdata[OBJECT_ID_STRAIGHT_ROAD].connection[0].z;
	last_x += (x*(float)cos(angle) - z*(float)sin(angle)); 
	last_z += (x*(float)sin(angle) + z*(float)cos(angle));
	
	ReplaceLinkInList();
		
	AddOblistLightArray(i);
	edptr->wptr->oblist_length++;
	
}

void AddSmallStraight(HDC hdc, editor_ptr edptr)
{
	int i;
	float x,z;
	float angle,k;
	
	k=(float)0.017453292;
	i=edptr->wptr->oblist_length;
	edptr->wptr->oblist[i].x = LinksList[current_link].last_x;
	edptr->wptr->oblist[i].y = (float)0;
	edptr->wptr->oblist[i].z = LinksList[current_link].last_z;
	edptr->wptr->oblist[i].rot_angle = LinksList[current_link].last_angle;
	edptr->wptr->oblist[i].type = OBJECT_ID_SMALL_STRAIGHT_ROAD;
//	edptr->wptr->oblist[i].light = -1;

	angle = k * LinksList[current_link].last_angle;		
	
	x = edptr->wptr->obdata[OBJECT_ID_SMALL_STRAIGHT_ROAD].connection[0].x;
	z = edptr->wptr->obdata[OBJECT_ID_SMALL_STRAIGHT_ROAD].connection[0].z;
	last_x += (x*(float)cos(angle) - z*(float)sin(angle)); 
	last_z += (x*(float)sin(angle) + z*(float)cos(angle));
	
	ReplaceLinkInList();

	AddOblistLightArray(i);
	edptr->wptr->oblist_length++;
}

void AddTJunction(DWORD object_id, editor_ptr edptr)
{
	int i;
	float x,z;
	float rad_angle,deg_angle,k;
	float start_x,start_z;
	
	k=(float)0.017453292;
	i=edptr->wptr->oblist_length;
	edptr->wptr->oblist[i].x = LinksList[current_link].last_x;
	edptr->wptr->oblist[i].y = (float)0;
	edptr->wptr->oblist[i].z = LinksList[current_link].last_z;
	edptr->wptr->oblist[i].rot_angle = LinksList[current_link].last_angle;
	edptr->wptr->oblist[i].type = object_id;


	start_x = last_x;
	start_z = last_z;
						
	rad_angle = k * LinksList[current_link].last_angle;
	deg_angle = LinksList[current_link].last_angle;

	last_angle = deg_angle + 270;
	CheckAngle();
	x = edptr->wptr->obdata[object_id].connection[0].x;
	z = edptr->wptr->obdata[object_id].connection[0].z;
	last_x += (x*(float)cos(rad_angle) - z*(float)sin(rad_angle)); 
	last_z +=  (x*(float)sin(rad_angle) + z*(float)cos(rad_angle));
	ReplaceLinkInList();
	current_link = last_link;

	last_angle = deg_angle + 90;
	CheckAngle();
	x = edptr->wptr->obdata[object_id].connection[1].x;
	z = edptr->wptr->obdata[object_id].connection[1].z;
	last_x = start_x + (x*(float)cos(rad_angle) - z*(float)sin(rad_angle)); 
	last_z = start_z + (x*(float)sin(rad_angle) + z*(float)cos(rad_angle));
	AddLinkToList();
	
	last_x=LinksList[current_link].last_x;
	last_z=LinksList[current_link].last_z;
	last_angle=LinksList[current_link].last_angle;

	AddOblistLightArray(i);
	edptr->wptr->oblist_length++;	
}

void AddLeftTJunction(DWORD object_id, editor_ptr edptr)
{
	int i;
	float x,z;
	float rad_angle,deg_angle,k;
	float start_x,start_z;
	
	k=(float)0.017453292;
	i=edptr->wptr->oblist_length;
	edptr->wptr->oblist[i].x = LinksList[current_link].last_x;
	edptr->wptr->oblist[i].y = (float)0;
	edptr->wptr->oblist[i].z = LinksList[current_link].last_z;
	edptr->wptr->oblist[i].rot_angle = LinksList[current_link].last_angle;
	edptr->wptr->oblist[i].type = object_id;


	start_x = last_x;
	start_z = last_z;
						
	rad_angle = k * LinksList[current_link].last_angle;
	deg_angle = LinksList[current_link].last_angle;

	last_angle = deg_angle + 0;
	CheckAngle();
	x = edptr->wptr->obdata[object_id].connection[0].x;
	z = edptr->wptr->obdata[object_id].connection[0].z;
	last_x += (x*(float)cos(rad_angle) - z*(float)sin(rad_angle)); 
	last_z +=  (x*(float)sin(rad_angle) + z*(float)cos(rad_angle));
	ReplaceLinkInList();
	current_link = last_link;

	last_angle = deg_angle + 90;
	CheckAngle();
	x = edptr->wptr->obdata[object_id].connection[1].x;
	z = edptr->wptr->obdata[object_id].connection[1].z;
	last_x = start_x + (x*(float)cos(rad_angle) - z*(float)sin(rad_angle)); 
	last_z = start_z + (x*(float)sin(rad_angle) + z*(float)cos(rad_angle));
	AddLinkToList();
	
	last_x=LinksList[current_link].last_x;
	last_z=LinksList[current_link].last_z;
	last_angle=LinksList[current_link].last_angle;

	AddOblistLightArray(i);
	edptr->wptr->oblist_length++;	
}

void AddRightTJunction(DWORD object_id, editor_ptr edptr)
{
	int i;
	float x,z;
	float rad_angle,deg_angle,k;
	float start_x,start_z;
	
	k=(float)0.017453292;
	i=edptr->wptr->oblist_length;
	edptr->wptr->oblist[i].x = LinksList[current_link].last_x;
	edptr->wptr->oblist[i].y = (float)0;
	edptr->wptr->oblist[i].z = LinksList[current_link].last_z;
	edptr->wptr->oblist[i].rot_angle = LinksList[current_link].last_angle;
	edptr->wptr->oblist[i].type = object_id;


	start_x = last_x;
	start_z = last_z;
						
	rad_angle = k * LinksList[current_link].last_angle;
	deg_angle = LinksList[current_link].last_angle;

	last_angle = deg_angle + 0;
	CheckAngle();
	x = edptr->wptr->obdata[object_id].connection[0].x;
	z = edptr->wptr->obdata[object_id].connection[0].z;
	last_x += (x*(float)cos(rad_angle) - z*(float)sin(rad_angle)); 
	last_z +=  (x*(float)sin(rad_angle) + z*(float)cos(rad_angle));
	ReplaceLinkInList();
	current_link = last_link;

	last_angle = deg_angle + 270;
	CheckAngle();
	x = edptr->wptr->obdata[object_id].connection[1].x;
	z = edptr->wptr->obdata[object_id].connection[1].z;
	last_x = start_x + (x*(float)cos(rad_angle) - z*(float)sin(rad_angle)); 
	last_z = start_z + (x*(float)sin(rad_angle) + z*(float)cos(rad_angle));
	AddLinkToList();
	
	last_x=LinksList[current_link].last_x;
	last_z=LinksList[current_link].last_z;
	last_angle=LinksList[current_link].last_angle;

	AddOblistLightArray(i);
	edptr->wptr->oblist_length++;	
}

void AddCrossRoad(HDC hdc, editor_ptr edptr)
{
	int i;
	float x,z;
	float rad_angle,deg_angle,k;
	float start_x,start_z;
	
	k=(float)0.017453292;
	i=edptr->wptr->oblist_length;
	edptr->wptr->oblist[i].x = LinksList[current_link].last_x;
	edptr->wptr->oblist[i].y = (float)0;
	edptr->wptr->oblist[i].z = LinksList[current_link].last_z;
	edptr->wptr->oblist[i].rot_angle = LinksList[current_link].last_angle;
	edptr->wptr->oblist[i].type = OBJECT_ID_CROSSROADS;

	start_x = LinksList[current_link].last_x;
	start_z = LinksList[current_link].last_z;
			
	rad_angle = k * LinksList[current_link].last_angle;
	deg_angle = LinksList[current_link].last_angle;
	last_angle= deg_angle;
	x = edptr->wptr->obdata[OBJECT_ID_CROSSROADS].connection[0].x;
	z = edptr->wptr->obdata[OBJECT_ID_CROSSROADS].connection[0].z;
	last_x = start_x + (x*(float)cos(rad_angle) - z*(float)sin(rad_angle)); 
	last_z = start_z + (x*(float)sin(rad_angle) + z*(float)cos(rad_angle));
	ReplaceLinkInList();
	
	last_angle = deg_angle + 270;
	CheckAngle();
	x = edptr->wptr->obdata[OBJECT_ID_CROSSROADS].connection[1].x;
	z = edptr->wptr->obdata[OBJECT_ID_CROSSROADS].connection[1].z;
	last_x = start_x + (x*(float)cos(rad_angle) - z*(float)sin(rad_angle)); 
	last_z = start_z + (x*(float)sin(rad_angle) + z*(float)cos(rad_angle));
	AddLinkToList();

	last_angle = deg_angle + 90;
	CheckAngle();
	x = edptr->wptr->obdata[OBJECT_ID_CROSSROADS].connection[2].x;
	z = edptr->wptr->obdata[OBJECT_ID_CROSSROADS].connection[2].z;
	last_x = start_x + (x*(float)cos(rad_angle) - z*(float)sin(rad_angle)); 
	last_z = start_z + (x*(float)sin(rad_angle) + z*(float)cos(rad_angle));
	AddLinkToList();
	
	last_x=LinksList[current_link].last_x;
	last_z=LinksList[current_link].last_z;
	last_angle=LinksList[current_link].last_angle;

	AddOblistLightArray(i);
	edptr->wptr->oblist_length++;	
	
}

void AddRoundAbout(HDC hdc, editor_ptr edptr)
{
	// TODO : Add Roundabout code here
}


int LoadMapInEditor(HWND hwnd,char *filename,world_ptr wptr)
{
	FILE  *fp;    
	char  s[81];
	char  p[100];	
	int y_count=30;
	int done=0;
	int object_count=0;
	int vert_count=0;
	int pv_count=0;
	int poly_count=0;
	int object_id;	
	int lit_vert;
	int red,green,blue;
	BOOL start_flag=TRUE;

	last_link = 0;
	current_link=0;

    fp = fopen(filename,"r");
    if(fp==NULL)
    {     
        MessageBox(hwnd,"Error can't load file",NULL,MB_OK);
		return 1;
    }

	while(done==0)
	{
		fscanf( fp, "%s", &s );

		if(strcmp(s,"OBJECT")==0)
		{
			fscanf( fp, "%s", &p );
			
			object_id = CheckObjectId(hwnd, (char *)&p, wptr);
			if(object_id == -1)
			{
				MessageBox(hwnd,"Error Bad Object ID in: LoadMapInEditor",NULL,MB_OK);
				return 1;
			}

			if(start_flag==FALSE)
				object_count++;
			wptr->oblist[object_count].type = object_id;
			
			
//			wptr->oblist[object_count].light = -1;
			start_flag=FALSE;
		}

		if(strcmp(s,"CO_ORDINATES")==0)
		{
			fscanf( fp, "%s", &p );
			wptr->oblist[object_count].x = (float)atof(p);
			
			fscanf( fp, "%s", &p );
			wptr->oblist[object_count].y = (float)atof(p);
			
			fscanf( fp, "%s", &p );
			wptr->oblist[object_count].z = (float)atof(p);
		}

		if(strcmp(s,"ROT_ANGLE")==0)
		{
			fscanf( fp, "%s", &p );
			wptr->oblist[object_count].rot_angle = (float)atof(p);
		}

		if(strcmp(s,"LIGHT_ON_VERT")==0)
		{
			fscanf( fp, "%s", &p );
			lit_vert = atoi(p);

			fscanf( fp, "%s", &p );
			red = atoi(p);
			wptr->oblist[object_count].lit[lit_vert].r = red;

			fscanf( fp, "%s", &p );
			green = atoi(p);
			wptr->oblist[object_count].lit[lit_vert].g = green;

			fscanf( fp, "%s", &p );
			blue = atoi(p);
			wptr->oblist[object_count].lit[lit_vert].b = blue;
		}
		
		if(strcmp(s,"LINK")==0)
		{
			fscanf( fp, "%s", &p );
			LinksList[last_link].last_x = (float)atoi(p);

			fscanf( fp, "%s", &p );
			LinksList[last_link].last_z = (float)atoi(p);

			fscanf( fp, "%s", &p );
			LinksList[last_link].last_angle = (float)atoi(p);

			last_link++;
		}

		if(strcmp(s,"END_FILE")==0)
		{
			fscanf( fp, "%s", &p );	
			wptr->oblist_length = object_count+1;
			done=1;
		}
	}
	fclose(fp);		
	return 0;
}

int SaveMap(HWND hwnd,char *filename,world_ptr wptr)
{
	FILE  *fp; 
	int ob_list_length;
	int i,a,j;
	int obtype;
	float x,z;
	int num_lverts;
	int r, g, b;
	
	fp = fopen(filename,"w");
    if(fp==NULL)
    {     
        MessageBox(hwnd,"Error can't save file",NULL,MB_OK);
		return 1;
    }

	ob_list_length = wptr->oblist_length;

	for(i = 0; i < ob_list_length; i++)
	{
		fprintf( fp, "%s", "OBJECT " );
		obtype = wptr->oblist[i].type;
		
		if(obtype < MAX_NUM_OBJECT_TYPES)
			fprintf( fp, "%s\n",wptr->obdata[obtype].name );
		else
		{     
			MessageBox(hwnd,"Bad object ID Error in: SaveMap",NULL,MB_OK);
			return 1;
		}
		fprintf( fp, "%s", "CO_ORDINATES " );
		fprintf( fp, "%f%s",wptr->oblist[i].x," " );
		fprintf( fp, "%f%s",wptr->oblist[i].y," " );
		fprintf( fp, "%f%s\n",wptr->oblist[i].z," " );

		fprintf( fp, "%s", "ROT_ANGLE " );
		fprintf( fp, "%d\n",(int)wptr->oblist[i].rot_angle );

		num_lverts = wptr->num_vert_per_object[wptr->oblist[i].type];

		
		for(j = 0; j < num_lverts; j++)
		{
			if(wptr->oblist[i].lit != NULL)
			{
				r = (int)wptr->oblist[i].lit[j].r; 
				g = (int)wptr->oblist[i].lit[j].g;
				b = (int)wptr->oblist[i].lit[j].b;

				if( !((r == 0) && (g == 0) && (b == 0)) )
				{
					fprintf( fp, "%s", "LIGHT_ON_VERT ");
					fprintf( fp, "%d ",j );
					fprintf( fp, "%d %d %d\n\n", r, g, b );
					
				
				}
			}
		}
	}

	for(i=0;i<last_link;i++)
	{
		x=LinksList[i].last_x;
		z=LinksList[i].last_z;
		a=(int)LinksList[i].last_angle;

		fprintf( fp, "%s", "LINK " );
		fprintf( fp, "%f%s",x," ");
		fprintf( fp, "%f%s",z," ");
		fprintf( fp, "%d%s\n",a," ");

	}

	fprintf( fp, "%s", "END_FILE");
	fclose(fp);	
	return 0;
}

void AddOblistLightArray(int oblist_id)
{
	int i, object_id, num_lverts;
	
	
	object_id = world.oblist[oblist_id].type;
			
	num_lverts = world.num_vert_per_object[object_id];
	world.oblist[oblist_id].lit = new LIGHT[num_lverts];

	for(i = 0; i < num_lverts; i++)
	{
		world.oblist[oblist_id].lit[i].r = 0;
		world.oblist[oblist_id].lit[i].g = 0;
		world.oblist[oblist_id].lit[i].b = 0;
	}

}

