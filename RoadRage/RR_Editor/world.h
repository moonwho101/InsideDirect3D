
#ifndef __WORLD_H__
#define __WORLD_H__

#include <windows.h>
#include <windowsx.h>
#include "d3dapp.h"

#define OBJECT_ID_STRAIGHT_ROAD			0
#define OBJECT_ID_LEFTCURVE_ROAD		1
#define OBJECT_ID_RIGHTCURVE_ROAD		2
#define OBJECT_ID_TJUNCTION				3
#define OBJECT_ID_CROSSROADS			4
#define OBJECT_ID_ROUNDABOUT			5
#define OBJECT_ID_LAMP_POST				6
#define OBJECT_ID_LAMP					7
#define OBJECT_ID_HOUSE_SEMI_LEFT		8
#define OBJECT_ID_POLICE_CAR			9
#define OBJECT_ID_ROAD_SIGN				10
#define OBJECT_ID_SMALL_STRAIGHT_ROAD	11
#define OBJECT_ID_LEFTCORNER_ROAD		12
#define OBJECT_ID_RIGHTCORNER_ROAD		13
#define OBJECT_ID_LEFT_TJUNCTION		14
#define OBJECT_ID_RIGHT_TJUNCTION		15
#define OBJECT_ID_ZEBRA					16
#define OBJECT_ID_ISLAND				17
#define OBJECT_ID_HOUSE_SEMI_RIGHT		18
#define OBJECT_ID_PETROL_STATION		19
#define OBJECT_ID_TRAFFIC_LIGHT			20
#define OBJECT_ID_SLOPE					21
#define OBJECT_ID_SLOPE_CORNER			22
#define OBJECT_ID_CENTRAL_RESERVATION	23
#define OBJECT_ID_MBSHOP1				24

#define OBJECT_ID_MAINROAD_STRAIGHT		25
#define OBJECT_ID_MAINROAD_TJUNCTION	26
#define OBJECT_ID_MAINROAD_TJUNCTION2	27
#define OBJECT_ID_MAINROAD_TJUNCTION3	28
#define OBJECT_ID_MAINROAD_CROSSROADS	29
#define OBJECT_ID_MAINROAD_CROSSROADS2	30

#define OBJECT_ID_DOUBLE_LAMP_POST		31


#define POLY_CMD_TRI_STRIP				1
#define POLY_CMD_TRI_FAN				2
#define POLY_CMD_TRI					3
#define POLY_CMD_QUAD					4
#define POLY_CMD_QUAD_TEX				5

#define KEYBOARD			1
#define JOYSTICK			2

#define MODE_640_480		1
#define MODE_800_600		2
#define MODE_320_200		3

#define	FULLSCREEN_ONLY		1
#define	MAXIMISED_WINDOW	2

#define LOGFILE_ONLY        0
#define SCN_AND_LOGFILE     1

#define MAX_NUM_MAP_OBJECTS 20000
#define MAX_NUM_MAP_CELLS 256


typedef struct setupinfo_typ
{
	D3DAppMode vmode;	
	int screen;
	int control;
	BOOL sound;

} SETUPINFO,*setupinfo_ptr;

typedef struct texturemapping_typ
{
	float tu[4];
	float tv[4];
	int texture;
	BOOL is_alpha_texture;
	char tex_alias_name[50];

} TEXTUREMAPPING,*texturemapping_ptr;


typedef struct vert_typ
{
	float x;
	float y;
	float z;

} VERT,*vert_ptr;

typedef struct light_typ
{
	BYTE r;
	BYTE g;
	BYTE b;

} LIGHT,*light_ptr;

typedef struct objectlist_typ
{
	float x;
	float y;
	float z; 
	float rot_angle; 
	int type;
	LIGHT *lit;
	LIGHT ed_poly_color;
	BOOL IsHighlighted;

} OBJECTLIST,*objectlist_ptr;


typedef struct objectdata_typ
{
	VERT *v; 
	VERT *t; 
	VERT connection[4];
	short *tex;
	short *num_vert;
	short *poly_cmd;
	char name[50];

} OBJECTDATA,*objectdata_ptr;

typedef struct world_typ
{
	OBJECTLIST	oblist[MAX_NUM_MAP_OBJECTS];
	int oblist_length;

	OBJECTDATA	obdata[100];
	int num_vert_per_object[100];
	int num_polys_per_object[100];
	int obdata_length;

	int         *cell[MAX_NUM_MAP_CELLS][MAX_NUM_MAP_CELLS];
	short cell_length[MAX_NUM_MAP_CELLS][MAX_NUM_MAP_CELLS];
	BOOL   draw_flags[MAX_NUM_MAP_CELLS][MAX_NUM_MAP_CELLS];

	TEXTUREMAPPING TexMap[50]; 
	int number_of_tex_aliases;

	int num_polys_per_frame;
	int num_verts_per_frame;

	BOOL walk_mode_enabled;

} WORLD,*world_ptr;


#ifdef __cplusplus
extern "C" {
#endif

int LoadObjectData(HWND hwnd,char *filename,world_ptr wptr);
int LoadWorldMap(HWND hwnd,char *filename,world_ptr wptr); 
int CheckObjectId(HWND hwnd, char *p, world_ptr wptr); 
void InitWorldMap(HWND hwnd, world_ptr wptr);
void InitPreCompiledWorldMap(HWND hwnd, world_ptr wptr);
void PrintMessage(HWND hwnd,char *message1, char *message2, int message_mode);
world_ptr GetWorldData();
setupinfo_ptr LoadSetupInfo(HWND hwnd);


#ifdef __cplusplus
};
#endif

#endif // __WORLD_H__
