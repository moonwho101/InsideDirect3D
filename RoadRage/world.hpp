
// Road Rage version 1.1

// Written by William Chin
// Developed in MSDEV C++ v6.0 using the DirectX 6.0 SDK
// Date : 3/7/99
// File : world.hpp


#ifndef __WORLD_H__
#define __WORLD_H__

#include <windows.h>
#include <windowsx.h>
#include "d3dapp.hpp"

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
#define OBJECT_ID_MBSHOP1				24

#define POLY_CMD_ERROR					0
#define POLY_CMD_TRI_STRIP				1
#define POLY_CMD_TRI_FAN				2
#define POLY_CMD_TRI					3
#define POLY_CMD_QUAD					4
#define POLY_CMD_QUAD_TEX				5
#define POLY_CMD_INDEXED_TRI			6
#define POLY_CMD_INDEXED_TRI_FAN		7
#define POLY_CMD_INDEXED_TRI_STRIP		8

#define KEYBOARD			1
#define JOYSTICK			2

#define MODE_640_480		1
#define MODE_800_600		2
#define MODE_320_200		3

#define	FULLSCREEN_ONLY		1
#define	MAXIMISED_WINDOW	2

#define LOGFILE_ONLY		1
#define SCN_AND_FILE		2
#define WALK_MODE			0
#define DRIVE_MODE			1

#define MAX_NUM_X_MAP_CELLS 512
#define MAX_NUM_Z_MAP_CELLS 512

#define MAX_NUM_DAT_OBJECTS 100
#define MAX_NUM_MAP_OBJECTS 20000

typedef struct setupinfo_typ
{
	D3DAppMode vmode;	
	int screen;
	int control;
	int TextureSize;
	int NumTextures;
	DWORD TextureMemNeeded;
	BOOL sound;

} SETUPINFO,*setupinfo_ptr;

typedef struct texturemapping_typ
{
	float tu[4];
	float tv[4];
	int texture;
	BOOL is_alpha_texture;
	char tex_alias_name[100];

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
	int health;
	int armour;
	int frags;
	int ping;
	BOOL walk_or_drive_mode;
	BOOL draw_external_wep;
	DWORD RRnetID;
	DWORD dpid;
	char name[256];

} PLAYER,*player_ptr;

typedef struct pmdata_typ
{
	VERT **w;
	VERT  *t; 
	short *f;
	short *num_vert;
	short *poly_cmd;
	short *texture_list;
	short *num_verts_per_object;
	short *num_faces_per_object;

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
	VERT *v; 
	VERT *t; 
	short *num_vert;
	short *poly_cmd;
	short *tex;

	char name[64];
	VERT connection[4];

} OBJECTDATA,*objectdata_ptr;

typedef struct world_typ
{
	OBJECTLIST	oblist[2000]; //MAX_NUM_MAP_OBJECTS];
	int oblist_length;
	
	GUNLIST your_gun[12];
	GUNLIST other_players_guns[12];
	int gunlist_length;
	int current_gun;
	int current_gun_model_id;
	int current_car;
	int num_your_guns;
	int num_op_guns;
	
	PLAYER player_list[50];
	PLAYER car_list[50];
	PLAYER debug[50];

	int num_players;
	int num_cars;
	int num_debug_models;
	int current_frame;
	int current_sequence;

	PLAYERMODELDATA pmdata[50];

	OBJECTDATA obdata	      [MAX_NUM_DAT_OBJECTS];
	short num_vert_per_object [MAX_NUM_DAT_OBJECTS];
	short num_polys_per_object[MAX_NUM_DAT_OBJECTS];
	int obdata_length;

	short       *cell[MAX_NUM_X_MAP_CELLS][MAX_NUM_Z_MAP_CELLS];
	short cell_length[MAX_NUM_X_MAP_CELLS][MAX_NUM_Z_MAP_CELLS];
	BOOL   draw_flags[MAX_NUM_X_MAP_CELLS][MAX_NUM_Z_MAP_CELLS];

	TEXTUREMAPPING TexMap[100]; 
	int number_of_tex_aliases;

	int num_verts_in_scene;
	int num_triangles_in_scene;
	int num_dp_commands_in_scene;

	BOOL walk_mode_enabled;
	BOOL display_scores;

} WORLD,*world_ptr;



void LoadYourGunAnimationSequenceList(int model_id, world_ptr wptr);
void LoadPlayerAnimationSequenceList(int model_id, world_ptr wptr);
void LoadDebugAnimationSequenceList(HWND hwnd, char *filename, int model_id, world_ptr wptr);
BOOL LoadImportedModelList(HWND hwnd,char *filename,world_ptr wptr);
BOOL LoadObjectData(HWND hwnd,char *filename,world_ptr wptr);
BOOL LoadWorldMap(HWND hwnd,char *filename,world_ptr wptr); 
BOOL InitWorldMap(HWND hwnd, char *filename, world_ptr wptr);
BOOL InitPreCompiledWorldMap(HWND hwnd, char *filename, world_ptr wptr);
void PrintMessage(HWND hwnd,char *message1, char *message2, int message_mode);
int CheckObjectId(HWND hwnd, char *p, world_ptr wptr); 
world_ptr GetWorldData();
setupinfo_ptr LoadSetupInfo(HWND hwnd);
void InitRRvariables(world_ptr wptr);




#endif // __WORLD_H__
