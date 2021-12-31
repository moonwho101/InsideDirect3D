
// Road Rage version 1.1

// Written by William Chin
// Developed in MSDEV C++ v6.0 using the DirectX 6.0 SDK
// Date : 3/7/99
// File : 3Dengine.hpp


#ifndef __3DENGINE_H__
#define __3DENGINE_H__

#include <windows.h>
#include <windowsx.h>
#include "world.hpp"

#define CLEAR			0
#define ROAD_SURFACE	1
#define LEFT_CURB		2
#define RIGHT_CURB		4
#define	LEFT_PAVE		8
#define RIGHT_PAVE		16
#define	ROAD_STRIPES	32
#define	LAMP_POSTS		64
#define	ALL				127

static int view_angle;
static float car_speed;

typedef float trans_matrix[4][4];
typedef float point_matrix[4];

static point_matrix p;

typedef struct Camera_typ
{
	float direction_x;
	float direction_y;
	float direction_z;
	
	float view_point_x;
	float view_point_y;
	float view_point_z;

	float last_view_point_x;
	float last_view_point_y;
	float last_view_point_z;

	int view_angle;
	int shit;
	int ramp_size;

	point_matrix p;
	trans_matrix mx,my,mz,mw;

} CAMERA,*Camera_ptr; 



typedef struct Map_typ
{
	int num_poly;
	int	poly_clip_flags[1000];
	int texture[1000];
	int sector[1000];
	int type[1000]; // curb, pave, stripe ect.

	float mx[4][1000];
	float my[4][1000];
	float mz[4][1000];
	float mw[4][1000];

} MAP, *Map_ptr;

typedef struct VPoint_typ
{
	float x;
	float y;
	float z;
	
} VPOINT, *VPoint_ptr;

typedef struct editor_typ
{
	VPOINT position;
	BOOL update_flag;
	BOOL scroll_bar_flag;
	unsigned int draw_road_section_flags;
	int current_poly;
	int num_sections;
	int display_x_offset;
	int display_y_offset;
	int object_rot_angle;
	int MouseX;
	int MouseY;
	int current_object_id;
	BOOL Redraw_editor_map_flag;
	BOOL editor_mode;
	float angle;	
	float delta_angle;
	float pave_width;
    float road_width;
	float display_scale;
	int dialogbar_mode;
	world_ptr wptr;

} EDITOR, *editor_ptr;

float *Rotate(CAMERA cam);


#endif // __3DENGINE_H__
