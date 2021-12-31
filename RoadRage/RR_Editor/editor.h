
#ifndef __EDITOR_H__
#define __EDITOR_H__

#include <windows.h>
#include <windowsx.h>
#include "3Dengine.h"
#include "world.h"

#define  LEFT_HAND_CURVE		1			
#define	 RIGHT_HAND_CURVE		2		
#define	 STRAIGHT				3
#define	 T_JUNCTION				4
#define	 ROUND_ABOUT			5
#define  CROSSROAD				6
#define  SMALL_STRAIGHT			7
#define  LEFT_CORNER			8
#define  RIGHT_CORNER			9
#define  LEFT_T_JUNCTION		10
#define  RIGHT_T_JUNCTION		11
#define  ZEBRA					12
#define  MAINROAD_STRAIGHT		13
#define  MAINROAD_TJUNCTION		14
#define  MAINROAD_TJUNCTION2	15
#define  MAINROAD_TJUNCTION3	16
#define  MAINROAD_CROSSROADS	17
#define  MAINROAD_CROSSROADS2	18



#define HOUSE				1
#define CAR					2
#define ISLAND				3	
#define TRAFFIC_LIGHTS		4
#define LAMP_POST			5
#define ROAD_SIGN			6
#define ZEBRA_CROSSING		7
#define RAILINGS			8
#define	TELEPHONE_BOX		9

#define UPDATELINKS			0
#define NOUPDATELINKS		1

#define DIALOGBAR_MODE_ROADS	1
#define DIALOGBAR_MODE_OBJECTS	2

#define MAX_NUM_LINKS 1000

#ifdef __cplusplus
extern "C" {
#endif

void HighLightObjectWithMouse(HDC hdc, int x, int z, editor_ptr edptr);
void SetLinkWithMouse(int x,int z);
void PlaceObjectWithMouse(editor_ptr edptr, int x,int z);
void InitLinkList(editor_ptr edptr);
void AddLinkToList();
void UndoLastLink(editor_ptr edptr);
void ReplaceLinkInList();
void DisplayLinksList(HDC hdc);
void ChangeCurrentLink(int direction);

void Draw_2D_Map(HDC hdc, CAMERA cam);
void DrawEditorMap(HDC hdc, editor_ptr edptr);
void DrawEditorMapObject(HDC hdc, editor_ptr edptr);
void ReloadMainMap();
void DrawRoadSection(HDC hdc, editor_ptr edptr,int instruction);
void DrawObject(HDC hdc,editor_ptr ed,int object_type);
void DrawLampPost(HDC hdc,editor_ptr edptr);
void DrawHouse(HDC hdc,editor_ptr edptr);
void DrawCar(HDC hdc,editor_ptr edptr);
void RecalculateLinks(HDC hdc, editor_ptr edptr);
int LoadMapInEditor(HWND hwnd,char *filename,world_ptr wptr);
int SaveMap(HWND hwnd,char *filename,world_ptr wptr);


void AddLeftCurve (HDC hdc, editor_ptr edptr);
void AddRightCurve(HDC hdc, editor_ptr edptr);		
void AddStraight  (HDC hdc, editor_ptr edptr);	
void AddSmallStraight(HDC hdc, editor_ptr edptr);	
void AddTJunction (DWORD object_id, editor_ptr edptr);
void AddRoundAbout(HDC hdc, editor_ptr edptr);
void AddCrossRoad (HDC hdc, editor_ptr edptr);
void AddLeftCorner(HDC hdc, editor_ptr edptr);
void AddRightCorner(HDC hdc, editor_ptr edptr);
void DrawRoadSurface(HDC hdc, editor_ptr edptr);
void AddLeftTJunction(DWORD object_id, editor_ptr edptr);
void AddRightTJunction(DWORD object_id, editor_ptr edptr);
void AddZebra(HDC hdc, editor_ptr edptr);
void AddMainRoadStraight(HDC hdc, editor_ptr edptr);
void AddMainRoadTJunction(DWORD object_id, editor_ptr edptr);	
void AddMainRoadCrossroads(DWORD object_id, editor_ptr edptr);
void AddOblistLightArray(int oblist_id);
void DeleteMapObject(HDC hdc, editor_ptr edptr);		
void CheckAngle();

#ifdef __cplusplus
}
#endif

typedef struct Link_typ
{
	float last_x;
	float last_z;
	float last_angle;
	//BOOL active_link_flag;
	
}LINK, *Link_ptr;



#endif //__EDITOR_H__
