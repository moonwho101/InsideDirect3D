
// 3Dengine.h
#ifndef __3DENGINE_H__
#define __3DENGINE_H__

#include <windows.h>
#include <windowsx.h>
#include "world.h"

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

	int view_angle;
	int shit;
	int ramp_size;

	point_matrix p;
	trans_matrix mx,my,mz,mw;

} CAMERA,*Camera_ptr; 


typedef struct Results_typ
{
	float px[4][1000];
	float py[4][1000];
	float pz[4][1000];
	float pw[4][1000];

	float mx[4][1000];
	float my[4][1000];
	float mz[4][1000];
	float mw[4][1000];

	float cx[4][1000];
	float cy[4][1000];
	float cz[4][1000];
	float cw[4][1000];

	float gradient;
	float inv_gradient;
	float c;
	float ncp_angle;
	float view_angle;
	int num_clipped_polys;
	int num_polys_after_culling;

} RESULTS, *Results_ptr;

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
	BOOL select_mode;
	float angle;	
	float delta_angle;
	float pave_width;
    float road_width;
	float display_scale;
	int dialogbar_mode;
	world_ptr wptr;

} EDITOR, *editor_ptr;

#ifdef __cplusplus
extern "C" {
#endif

float *Rotate(CAMERA cam);
Results_ptr zippy(void);
	Map_ptr GetMap(void);
	void SetMap(Map_ptr edmap);

#ifdef __cplusplus
};
#endif


#endif // __3DENGINE_H__
