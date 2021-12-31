
#ifndef __WORLD_H__
#define __WORLD_H__

#include <windows.h>
#include <windowsx.h>
#include "d3dtypes.h"

#define OBJECT_ID_STRAIGHT_ROAD			0
#define OBJECT_ID_LEFTCURVE_ROAD		1
#define OBJECT_ID_RIGHTCURVE_ROAD		2
#define OBJECT_ID_TJUNCTION				3
#define OBJECT_ID_CROSSROADS			4
#define OBJECT_ID_ROUNDABOUT			5
#define OBJECT_ID_LAMP_POST				6
#define OBJECT_ID_LAMP					7
#define OBJECT_ID_HOUSE					8
#define OBJECT_ID_POLICE_CAR			9
#define OBJECT_ID_ROAD_SIGN				10
#define OBJECT_ID_SMALL_STRAIGHT_ROAD	11
#define OBJECT_ID_LEFTCORNER_ROAD		12
#define OBJECT_ID_RIGHTCORNER_ROAD		13
#define OBJECT_ID_LEFT_TJUNCTION		14
#define OBJECT_ID_RIGHT_TJUNCTION		15
#define OBJECT_ID_ZEBRA					16
#define OBJECT_ID_ISLAND				17
#define OBJECT_ID_HOUSE_END				18
#define OBJECT_ID_PETROL_STATION		19
#define OBJECT_ID_TRAFFIC_LIGHT			20
#define OBJECT_ID_SLOPE					21
#define OBJECT_ID_SLOPE_CORNER			22
#define OBJECT_ID_CENTRAL_RESERVATION	23
#define OBJECT_ID_WEAPON_AR15			24
#define OBJECT_ID_WEAPON_MP5			25
#define OBJECT_ID_PLAYER				26
#define	OBJECT_ID_WEAPON_SNIPER			27
#define	OBJECT_ID_WEAPON_PLASMA			28
#define	OBJECT_ID_WEAPON_1				29
#define	OBJECT_ID_WEAPON_2				30
#define	OBJECT_ID_WEAPON_3				31
#define	OBJECT_ID_WEAPON_4				32
#define	OBJECT_ID_WEAPON_5				33
#define	OBJECT_ID_WEAPON_6				34
#define	OBJECT_ID_WEAPON_7				35
#define	OBJECT_ID_WEAPON_8				36
#define	OBJECT_ID_WEAPON_9				37
#define	OBJECT_ID_WEAPON_10				38

#define POLY_CMD_ERROR					0
#define POLY_CMD_TRI_STRIP				1
#define POLY_CMD_TRI_FAN				2
#define POLY_CMD_TRI					3
#define POLY_CMD_TRI_TEX				4
#define POLY_CMD_QUAD					5
#define POLY_CMD_QUAD_TEX				6
#define POLY_CMD_INDEXED_TRI			7
#define POLY_CMD_INDEXED_TRI_FAN		8
#define POLY_CMD_INDEXED_TRI_STRIP		9

#define KEYBOARD			1
#define JOYSTICK			2

#define MODE_640_480		1
#define MODE_800_600		2
#define MODE_320_200		3

#define	FULLSCREEN_ONLY		1
#define	MAXIMISED_WINDOW	2

#define LOGFILE_ONLY		1
#define SCN_AND_FILE		2
#define WALK_MODE	0
#define DRIVE_MODE 1

#define SPOT_LIGHT_SOURCE			1
#define	DIRECTIONAL_LIGHT_SOURCE	2
#define	POINT_LIGHT_SOURCE			3

		
typedef struct texturemapping_typ
{
	float tu[4];
	float tv[4];
	int texture;
	char tex_alias_name[100];
	BOOL is_alpha_texture;

} TEXTUREMAPPING,*texturemapping_ptr;

typedef struct tagD3DAppMode {
    int     w;		      /* width */
    int	    h;		      /* height */
    int	    bpp;	      /* bits per pixel */
    BOOL    bThisDriverCanDo; /*can current D3D driver render in this mode?*/
} D3DAppMode;

typedef struct setupinfo_typ
{
	D3DAppMode vmode;	
	int screen;
	int control;
	BOOL sound;

} SETUPINFO,*setupinfo_ptr;

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

typedef struct lightsource_typ
{
	BYTE r;
	BYTE g;
	BYTE b;
	
	int command;

	float direction_x;
	float direction_y;
	float direction_z;

	float position_x;
	float position_y;
	float position_z;

} LIGHTSOURCE,*lightsouce_ptr;

typedef struct objectlist_typ
{
	float x;
	float y;
	float z; 
	float rot_angle; 
	int type;
	LIGHT *lit;
	LIGHTSOURCE *light_source;

} OBJECTLIST,*objectlist_ptr;

typedef struct gunlist_typ
{
	float x;
	float y;
	float z; 
	float x_offset;
	float y_offset;
	float z_offset;
	float rot_angle; 
	int type;
	int light;
	int current_frame;
	int current_sequence;
	char name[256];
	int model_id;
	int skin_tex_id;
	int sound_id;
	char file[256];

} GUNLIST,*gunlist_ptr;

typedef struct player_typ
{
	float x;
	float y;
	float z; 
	float rot_angle; 
	int model_id;
	int skin_tex_id;
	int current_weapon;
	int current_car;
	int current_frame;
	int current_sequence;
	int ping;
	int frags;
	int health;
	BOOL draw_external_wep;
	BOOL bIsFiring;
	BOOL bIsRunning;
	BOOL bIsPlayerAlive;
	BOOL bIsPlayerInWalkMode;
	BOOL bStopAnimating;
	BOOL bIsPlayerValid;
	char name[256];

	DWORD RRnetID;

} PLAYER,*player_ptr;

typedef struct pmdata_typ
{
	VERT **w;
	VERT  *t; 
	int *f;
	int *num_vert;
	D3DPRIMITIVETYPE *poly_cmd;
	int *texture_list;
	int *num_verts_per_object; // new line added by BILL
	int *num_faces_per_object; // new line added by BILL

	int tex_alias;
	int num_frames;
	int num_verts;
	int num_faces;
	int num_polys_per_frame;
	int num_verts_per_frame;
	int sequence_start_frame[50];
	int sequence_stop_frame[50];
	int texture_maps[100];

	BOOL use_indexed_primitive;
	float skx;
	float sky;
	float scale;
	char name[256];

} PLAYERMODELDATA,*pmdata_ptr;

typedef struct objectdata_typ
{
	VERT v[2000]; // 6000
	VERT t[2000]; // 6000
	int f[2000];
	int num_vert[2000];
	D3DPRIMITIVETYPE poly_cmd[2000];
	int tex[2000];
	BOOL use_texmap[2000];
	char name[256];
	VERT connection[4];
	
} OBJECTDATA,*objectdata_ptr;

void PrintMessage(HWND hwnd,char *message1, char *message2, int message_mode);

#endif // __WORLD_H__
