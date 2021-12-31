
// Road Rage version 1.1

// Written by William Chin
// Developed in MSDEV C++ v6.0 using the DirectX 6.0 SDK
// Date : 3/7/99
// File : RoadRage.cpp


#include <d3d.h>
#include <d3dtypes.h>
#include <ddraw.h>
#include <stdio.h>
#include <math.h>
#include "d3ddemo.hpp"
#include "d3dmain.hpp"
#include "matrix.hpp"
#include "world.hpp"



#define MAX_NUM_QUADS			25000
#define MAX_NUM_TRIANGLES		25000
#define MAX_NUM_VERTICES		25000
#define MAX_NUM_FACE_INDICES	25000
#define MAX_NUM_TEXTURES		100

#define USE_DEFAULT_MODEL_TEX	0
#define USE_PLAYERS_SKIN		1
 
#define USE_INDEXED_DP			0
#define USE_NON_INDEXED_DP		1

MAP map;
world_ptr wptr;

extern D3DAppInfo d3dappi;
extern d3dmainglobals myglobs;
extern firing_gun_flag;
extern BOOL App_starting_for_first_time;
extern DWORD MyRRnetID;
extern BOOL multiplay_flag;

BOOL rendering_first_frame = TRUE;

D3DLVERTEX	   src_v[MAX_NUM_VERTICES];
unsigned short src_f[MAX_NUM_FACE_INDICES];

LPD3DMATERIAL7 lpMat[MAX_NUM_TEXTURES];
D3DMATERIALHANDLE    hMat[MAX_NUM_TEXTURES];


static const float pi = 3.141592654f;
static const float piover2 = (pi / 2.0f);

D3DMATRIX trans, roty;
D3DMATRIX WorldMatrix, ViewMatrix, ProjMatrix;
D3DVALUE posx, posy, posz; //Player position
D3DVALUE angx, angy, angz; //Player angles
D3DVECTOR from, at, up; // Used to set view matrix
D3DVALUE Z_NEAR = D3DVAL(6.0);
D3DVALUE Z_FAR = D3DVAL(10000.0);
D3DVALUE FOV = D3DVAL(pi / 2.0f); // (pi / 3.0f) 

int sx,sy,sz;
int display_poly_flag;
int clip_display_poly_flag;
int num_clipped_polys;
int color;
int texture_number;
int RampSize;
int clength;
int poly_cnt;
int cnt;
int cnt_f;
float sine, cosine;
float wx,wy,wz;
float x_off, y_off, z_off;

int number_of_polys_per_frame;
int num_triangles_in_scene;
int num_verts_in_scene;
int num_dp_commands_in_scene;


unsigned short verts_per_poly[MAX_NUM_QUADS];
unsigned short faces_per_poly[MAX_NUM_QUADS];

BOOL dp_command_index_mode[MAX_NUM_QUADS];
D3DPRIMITIVETYPE dp_commands[MAX_NUM_QUADS];

float sin_table[361];
float cos_table[361];
float gradient;
float c;
float inv_gradient;
float ncp_angle;
float *buffer2;
static float k;


float mx[100],my[100],mz[100],mw[100];
float tx[100],ty[100];

int mlr[100],mlg[100],mlb[100];

int number_of_verts_per_frame;
int number_of_faces_per_frame;
int texture_list_buffer[MAX_NUM_QUADS];
int ctext; 
int poly_clip_flags[MAX_NUM_QUADS];
int poly_clip[MAX_NUM_QUADS];
int num_map_objects_rendered;
int num_map_objects;

BOOL oblist_overdraw_flags[MAX_NUM_QUADS];

float player_x, player_z;

static float gv_gradient;
static float gv_target_angle;
static float gv_distance;
static float gv_dx, gv_dz;

int lcnt;
DWORD intensity;

// prototype

void ObjectToD3DVertList(int ob_type, int angle, int lit_v);
void PlayerToD3DVertList(int pmodel_id, int curr_frame, int angle, int texture_alias, int tex_flag);
void WeaponToD3DVertList(int ob_type, int angle, int texture_alias,float skx, float sky );
void PlayerToD3DIndexedVertList(int pmodel_id, int curr_frame, int angle, int texture_alias, int tex_flag);



// FUNCTIONS

void OverrideDefaults(Defaults* defaults)
{
    lstrcpy(defaults->Name, "Road Rage 1.1");
    defaults->rs.bPerspCorrect = TRUE;
    defaults->bResizingDisabled = FALSE;
}


BOOL RenderScene(LPDIRECT3DDEVICE7 lpDev, 
            LPD3DRECT lpExtent,CAMERA cam)
{
	int i,j;
	int angle;
	int oblist_length;
	int last_texture_number;
	int texture_alias_number;
	int ob_type;
	int vert_index;
	int face_index;
	int lit_v;
	int cell_x,cell_z;
	int vi,q,ix,iz,icnt;
	int vol_length;
	int vol_x[MAX_NUM_QUADS];
	int vol_z[MAX_NUM_QUADS];
	float skx, sky;

	double k=0.017453292;
	DWORD color_key=0;
	BOOL use_player_skins_flag = TRUE;
	int curr_car;
	int curr_wep;
	int current_frame;
	int ap_cnt = 0;
	int	alpha_poly_index[MAX_NUM_QUADS];
	int	alpha_vert_index[MAX_NUM_QUADS];


	// ******* START new code Added 2nd July 1999 *******
	
	DWORD dwIndexCount;
	DWORD dwVertexCount;
	// ******* END new code Added 2nd July 1999 *******


	if(rendering_first_frame == TRUE)
	{
		PrintMessage(myglobs.hWndMain, "RenderScene : rendering first frame", NULL, LOGFILE_ONLY);
		rendering_first_frame = FALSE;
	}


    D3DCLIPSTATUS status={D3DCLIPSTATUS_EXTENTS2, 0, 2048.0f, 0.0f, 2048.0f, 0.0f, 0.0f, 0.0f};
	
	number_of_polys_per_frame = 0;
	number_of_verts_per_frame = 0;
	number_of_faces_per_frame = 0;
	num_triangles_in_scene = 0;
	num_verts_in_scene = 0;
	num_dp_commands_in_scene = 0;

	RampSize=cam.ramp_size;
	poly_cnt=0;
	cnt = 0;
	cnt_f = 0;

	cell_x = (int)(cam.view_point_x / 256);
	cell_z = (int)(cam.view_point_z / 256);

	player_x = cam.view_point_x ;
	player_z = cam.view_point_z ;

	icnt = 0;
	angy = (D3DVALUE)cam.view_angle;
	
	for(i = 0; i < 10000; i++)
		oblist_overdraw_flags[i] = FALSE;

	if(angy>=360)
		angy=angy-360;

	if(angy<0)
		angy+=360;

	if((angy >= 315) || (angy <=45)) // 1
	{
		for(ix = -2; ix <= 2; ix++)
		{
			vol_x[icnt]   = cell_x + ix, vol_z[icnt]   = cell_z - 1;
			vol_x[icnt+1] = cell_x + ix, vol_z[icnt+1] = cell_z;
			vol_x[icnt+2] = cell_x + ix, vol_z[icnt+2] = cell_z + 1;
			vol_x[icnt+3] = cell_x + ix, vol_z[icnt+3] = cell_z + 2;
			vol_x[icnt+4] = cell_x + ix, vol_z[icnt+4] = cell_z + 3;

			icnt+=5;
		}
	}

	if((angy < 315) && (angy > 225)) // 2
	{
		for(iz = -2; iz <= 2; iz++)
		{
			vol_x[icnt]   = cell_x ,    vol_z[icnt]   = cell_z + iz;
			vol_x[icnt+1] = cell_x - 1, vol_z[icnt+1] = cell_z + iz;
			vol_x[icnt+2] = cell_x - 2, vol_z[icnt+2] = cell_z + iz;
			vol_x[icnt+3] = cell_x - 3, vol_z[icnt+3] = cell_z + iz;
			vol_x[icnt+4] = cell_x - 4, vol_z[icnt+4] = cell_z + iz;

			icnt+=5;
		}
	}

	if((angy <= 225) && (angy >= 135)) // 3
	{
		for(ix = -2; ix <= 2; ix++)
		{
			vol_x[icnt]   = cell_x + ix, vol_z[icnt]   = cell_z;
			vol_x[icnt+1] = cell_x + ix, vol_z[icnt+1] = cell_z - 1;
			vol_x[icnt+2] = cell_x + ix, vol_z[icnt+2] = cell_z - 2;
			vol_x[icnt+3] = cell_x + ix, vol_z[icnt+3] = cell_z - 3;
			vol_x[icnt+4] = cell_x + ix, vol_z[icnt+4] = cell_z - 4;

			icnt+=5;
		}
	}

	if((angy > 45) && (angy < 135)) // 4
	{
		for(iz = -2; iz <= 2; iz++)
		{
			vol_x[icnt]   = cell_x ,    vol_z[icnt]   = cell_z + iz;
			vol_x[icnt+1] = cell_x + 1, vol_z[icnt+1] = cell_z + iz;
			vol_x[icnt+2] = cell_x + 2, vol_z[icnt+2] = cell_z + iz;
			vol_x[icnt+3] = cell_x + 3, vol_z[icnt+3] = cell_z + iz;
			vol_x[icnt+4] = cell_x + 4, vol_z[icnt+4] = cell_z + iz;

			icnt+=5;
		}
	}

	oblist_length = wptr->oblist_length;
	number_of_polys_per_frame = 0;
	cnt = 0;
	num_map_objects_rendered = 0;
	num_map_objects = 0;

	vol_length = icnt;

	for(vi = 0; vi < vol_length; vi++)
	{		
		cell_x = vol_x[vi];
		cell_z = vol_z[vi];
	
		if(wptr->cell[cell_x][cell_z] != NULL)
		{
			if(!(cell_x < 0) || (cell_z < 0))
			{
				clength = wptr->cell_length[cell_x][cell_z];
			
				for(q = 0; q < clength; q++)
				{
					i = wptr->cell[cell_x][cell_z][q];
					
					if((i >= 0) && (i < oblist_length))
					{
						// check map object list and prevent the same 
						// objects being drawn more than once

						num_map_objects++;

						if(oblist_overdraw_flags[i] == FALSE)
						{
							oblist_overdraw_flags[i] = TRUE;

							wx = wptr->oblist[i].x;
							wy = wptr->oblist[i].y;
							wz = wptr->oblist[i].z;
		
							angle = (int)wptr->oblist[i].rot_angle;
							ob_type = wptr->oblist[i].type;
				
							ObjectToD3DVertList(ob_type, angle, i);
							num_map_objects_rendered++;
						}
					}
				} // end for q
			} // end if
		} // end if
	} // end for vi

	
	// DRAW YOUR GUN ///////////////////////////////////////////
	
	if(wptr->walk_mode_enabled == TRUE)
	{
		skx = (float)0.40000000 / (float)256;
		sky = (float)1.28000000 / (float)256;
		use_player_skins_flag = 1;

		i = wptr->current_gun;

		wx = wptr->your_gun[i].x; 
		wy = wptr->your_gun[i].y;
		wz = wptr->your_gun[i].z; 

		x_off = wptr->your_gun[i].x_offset;
		y_off = wptr->your_gun[i].y_offset;
		z_off = wptr->your_gun[i].z_offset; 

		ob_type = wptr->your_gun[i].model_id;
		current_frame = wptr->your_gun[i].current_frame;
		angle = (int)wptr->your_gun[i].rot_angle;
		lit_v = wptr->your_gun[i].light =255;

		PlayerToD3DVertList(ob_type, 
			current_frame, 
			angle, 
			NULL, 
			USE_DEFAULT_MODEL_TEX);
	}

	
	// DRAW Players ///////////////////////////////////////////
		
	ob_type = 30;
	angle = 0;	

	x_off = 0;
	y_off = 0;
	z_off = 0;

	for(i = 0; i < wptr->num_players; i++)
	{
		if((multiplay_flag == TRUE) && (wptr->player_list[i].dpid == MyRRnetID))
			i++;

		wx = wptr->player_list[i].x; 
		wy = wptr->player_list[i].y;		
		wz = wptr->player_list[i].z;
		angle = (int)wptr->player_list[i].rot_angle;
		use_player_skins_flag = 1;
		current_frame = wptr->player_list[i].current_frame;
	
		if(wptr->player_list[i].walk_or_drive_mode == WALK_MODE)
		{
			
			if( (abs((int)(wx - player_x)) < 512) && (abs((int)(wz - player_z) < 512)) ) 
			{
				PlayerToD3DVertList(wptr->player_list[i].model_id, 
								current_frame,	angle, 
								wptr->player_list[i].skin_tex_id,
								USE_PLAYERS_SKIN);
		
				if(wptr->player_list[i].draw_external_wep == TRUE)
				{
					curr_wep = wptr->player_list[i].current_weapon;

					PlayerToD3DVertList(wptr->other_players_guns[curr_wep].model_id, 
								current_frame, angle, 
								wptr->other_players_guns[curr_wep].skin_tex_id,
								USE_PLAYERS_SKIN);
				}
			}
		}
		else
		{
			
			if( (abs((int)(wx - player_x) < 512)) && (abs((int)(wz - player_z) < 512)) ) 
			{
				curr_car = wptr->player_list[i].current_car;
				PlayerToD3DVertList(wptr->car_list[curr_car].model_id, 
								current_frame, angle, 
								wptr->car_list[curr_car].skin_tex_id,
								USE_PLAYERS_SKIN);
			}
			
			
		}
	}
	
	
	// DRAW DEBUG MODELS
	
	for(i = 0; i < wptr->num_debug_models; i++)
	{
		wx = wptr->debug[i].x; 
		wy = wptr->debug[i].y;		
		wz = wptr->debug[i].z;
		angle = (int)wptr->debug[i].rot_angle;

		if( (abs((int)(wx - player_x)) < 512) && (abs((int)(wz - player_z)) < 512) ) 
		{
			use_player_skins_flag = 1;
			current_frame = wptr->debug[i].current_frame;

			PlayerToD3DVertList(wptr->debug[i].model_id, 
						current_frame,
						angle, 
						wptr->debug[i].skin_tex_id,
						USE_DEFAULT_MODEL_TEX);
		}
	}

	if(number_of_polys_per_frame == 0)
		return TRUE;

    car_speed=(float)0;

    if (lpDev->BeginScene() != D3D_OK)
        return FALSE;
	
	if (lpDev->SetClipStatus(&status) != D3D_OK)
        return FALSE; 

	k=(float)0.017453292; // pi/180
	angy=(float)cam.view_angle*(float)k;

	// camera location      
	from.x = -cam.view_point_x;
    from.y = -cam.view_point_y;
    from.z = -cam.view_point_z;
	
    WorldMatrix = IdentityMatrix();    
	WorldMatrix = MatrixMult(Translate(from.x,from.y,from.z),WorldMatrix);
	WorldMatrix = MatrixMult(RotateY(angy), WorldMatrix); 
	    
	if (lpDev->SetTransform(D3DTRANSFORMSTATE_WORLD, &WorldMatrix) != D3D_OK)
        return FALSE; 	
	
    ProjMatrix = SetProjectionMatrix(Z_NEAR, Z_FAR, FOV);

    if (lpDev->SetTransform(D3DTRANSFORMSTATE_PROJECTION, &ProjMatrix) != D3D_OK)
        return FALSE; 
    

	if (lpDev->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE) != D3D_OK) 
		return FALSE;

	if (lpDev->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, FALSE)!= D3D_OK) 
		return FALSE;


	///////////////////////////////////////////////////////////////////////

	last_texture_number = 1000;
	vert_index = 0;
	face_index = 0;
	ap_cnt = 0;


	for(i = 0; i < number_of_polys_per_frame; i++)
	{
		texture_alias_number = texture_list_buffer[i];
		texture_number = wptr->TexMap[texture_alias_number].texture; 

		num_verts_in_scene += verts_per_poly[i];

		if(wptr->TexMap[texture_alias_number].is_alpha_texture == TRUE)
		{
			alpha_poly_index[ap_cnt] = i;
			alpha_vert_index[ap_cnt] = vert_index;
			vert_index += verts_per_poly[i];
			ap_cnt++;
		} 
		else
		{
			if(texture_number != last_texture_number)
			{
				
				if (lpDev->SetTexture(0, d3dappi.lpTextureSurf[texture_number]) != D3D_OK)
					return FALSE; 
		
			}

			// ******* START new code Added 2nd July 1999 *******

			if(dp_command_index_mode[i] == USE_NON_INDEXED_DP) 
			{
				if (lpDev->DrawPrimitive(dp_commands[i], 
					D3DFVF_LVERTEX , (LPVOID)&src_v[vert_index], verts_per_poly[i], 
					NULL) != D3D_OK) 
				{
					return FALSE;
				}
				last_texture_number = texture_number;
				vert_index += verts_per_poly[i];

				num_dp_commands_in_scene++;
			}

			if(dp_command_index_mode[i] == USE_INDEXED_DP) 	
			{
				// Note : All vertex and face lists should be local to the object
				// your rendering, with DrawIndexedPrimitive. Face indexes should not 
				// refer to vertices outside the object. 
				// i.e if your rendering a rectangle as a triangle strip with two triangles
				// all the faces indices should have values of 0 to 3.
			
			
				dwIndexCount  = faces_per_poly[i]* 3;
				dwVertexCount = verts_per_poly[i];

				if (lpDev->DrawIndexedPrimitive(dp_commands[i],  
							D3DFVF_LVERTEX,                
							(LPVOID)&src_v[vert_index],	// lpvVertices             
							dwVertexCount,	             
							(LPWORD)&src_f[face_index],	// lpwIndices,                    
							dwIndexCount,                    
							NULL) != D3D_OK)
				{
					Msg("DrawIndexedPrimitive failed.\n");
					return FALSE;
				}

				last_texture_number = texture_number;

				face_index += dwIndexCount;
				vert_index += dwVertexCount;

				num_dp_commands_in_scene++;
			}

			
			// ******* END new code Added 2nd July 1999 *******
			
		}

	} // end for i

	
	wptr->num_verts_in_scene = num_verts_in_scene; 


	if(myglobs.rstate.bAlphaTransparency == TRUE)
	{
		lpDev->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);

		lpDev->SetRenderState(D3DRENDERSTATE_SRCBLEND, 
                            D3DBLEND_SRCCOLOR); 
		
		lpDev->SetRenderState(D3DRENDERSTATE_DESTBLEND, 
                          D3DBLEND_INVSRCCOLOR); 

	}

	for(j = 0; j < ap_cnt; j++)
	{
		i = alpha_poly_index[j];
		vert_index = alpha_vert_index[j];
		

		texture_alias_number = texture_list_buffer[i];
		texture_number = wptr->TexMap[texture_alias_number].texture; 
		
		if(texture_number != last_texture_number)
		{
			if (lpDev->SetTexture(0, d3dappi.lpTextureSurf[texture_number]) != D3D_OK)
				return FALSE; 
		}
		// ******* START new code Added 2nd July 1999 *******

			if(dp_command_index_mode[i] == USE_NON_INDEXED_DP) 
			{
				if (lpDev->DrawPrimitive(dp_commands[i], 
					D3DFVF_LVERTEX , (LPVOID)&src_v[vert_index], verts_per_poly[i], 
					NULL) != D3D_OK) 
				{
					return FALSE;
				}
				last_texture_number = texture_number;
				vert_index += verts_per_poly[i];

				num_dp_commands_in_scene++;
			}

			if(dp_command_index_mode[i] == USE_INDEXED_DP) 	
			{
				// Note : All vertex and face lists should be local to the object
				// your rendering, with DrawIndexedPrimitive. Face indexes should not 
				// refer to vertices outside the object. 
				// i.e if your rendering a rectangle as a triangle strip with two triangles
				// all the faces indices should have values of 0 to 3.
			
			
				dwIndexCount  = faces_per_poly[i]* 3;
				dwVertexCount = verts_per_poly[i];

				if (lpDev->DrawIndexedPrimitive(dp_commands[i],  
							D3DFVF_LVERTEX,                
							(LPVOID)&src_v[vert_index],	// lpvVertices             
							dwVertexCount,	             
							(LPWORD)&src_f[face_index],	// lpwIndices,                    
							dwIndexCount,                    
							NULL) != D3D_OK)
				{
					Msg("DrawIndexedPrimitive failed.\n");
					return FALSE;
				}

				last_texture_number = texture_number;

				face_index += dwIndexCount;
				vert_index += dwVertexCount;

				num_dp_commands_in_scene++;
			}

			
			// ******* END new code Added 2nd July 1999 *******
			

	

	} // end for j

	wptr->num_triangles_in_scene   = num_triangles_in_scene;
	wptr->num_dp_commands_in_scene = num_dp_commands_in_scene;


    if (lpDev->GetClipStatus(&status) != D3D_OK)
        return FALSE; 
	
    if (lpDev->EndScene() != D3D_OK)	
        return FALSE;
	
	lpExtent->x1 = (LONG)floor((double)status.minx);
	lpExtent->x2 = (LONG)ceil((double)status.maxx);
	lpExtent->y1 = (LONG)floor((double)status.miny);
	lpExtent->y2 = (LONG)ceil((double)status.maxy);
	
    return TRUE;
}



void PlayerToD3DVertList(int pmodel_id, int curr_frame, int angle, int texture_alias, int tex_flag)
{ 
	int i,j;
	int num_verts_per_poly;
	int num_poly;
	int p_command;
	int i_count;
	short v_index;
	float x,y,z;
	float rx,ry,rz;
	float tx,ty;
	DWORD r,g,b;
	vert_ptr tp;	
	BOOL error = TRUE;

	// ******* START new code Added 2nd July 1999 *******

	if(wptr->pmdata[pmodel_id].use_indexed_primitive == TRUE)
	{
		PlayerToD3DIndexedVertList(pmodel_id, curr_frame, angle, texture_alias, tex_flag);
		return;
	}

	// ******* END new code Added 2nd July 1999 *******

	cosine = cos_table[angle];
	sine   = sin_table[angle];

	if(curr_frame >= wptr->pmdata[pmodel_id].num_frames)
		curr_frame = 0;

	i_count = 0;

	num_poly = wptr->pmdata[pmodel_id].num_polys_per_frame;
	
	for(i = 0; i < num_poly; i++)
	{
		p_command	   = wptr->pmdata[pmodel_id].poly_cmd[i];
		num_verts_per_poly = wptr->pmdata[pmodel_id].num_vert[i];
		
		for(j = 0; j < num_verts_per_poly; j++)
		{
			v_index = wptr->pmdata[pmodel_id].f[i_count];

			tp = &wptr->pmdata[pmodel_id].w[curr_frame][v_index];
			x = tp->x + x_off;
			z = tp->y + y_off;
			y = tp->z + z_off;

			rx = wx + (x*cosine - z*sine); 
			ry = wy + y;
			rz = wz + (x*sine + z*cosine);
				
			tx = wptr->pmdata[pmodel_id].t[i_count].x * wptr->pmdata[pmodel_id].skx; 
			ty = wptr->pmdata[pmodel_id].t[i_count].y * wptr->pmdata[pmodel_id].sky; 

			r=0;
			g=0;
			b=0;

			src_v[cnt].x = D3DVAL(rx);
			src_v[cnt].y = D3DVAL(ry);
			src_v[cnt].z = D3DVAL(rz);
			src_v[cnt].color = RGBA_MAKE(255, 255, 255, 255);
			src_v[cnt].specular = RGB_MAKE(r,g,b);
			src_v[cnt].tu = D3DVAL(tx);
			src_v[cnt].tv = D3DVAL(ty);

			cnt++;
			i_count++;

		}
	
		verts_per_poly[number_of_polys_per_frame] = num_verts_per_poly;
		dp_command_index_mode[number_of_polys_per_frame] = USE_NON_INDEXED_DP;
		error = TRUE;

		if(p_command == POLY_CMD_TRI_FAN)
		{
			dp_commands[number_of_polys_per_frame] = D3DPT_TRIANGLEFAN;
			num_triangles_in_scene += (num_verts_per_poly - 2);
			error = FALSE;
		}
			
		if(p_command == POLY_CMD_TRI_STRIP)
		{
			dp_commands[number_of_polys_per_frame] = D3DPT_TRIANGLESTRIP;
			num_triangles_in_scene += (num_verts_per_poly - 2);
			error = FALSE;
		}

		if(p_command == POLY_CMD_TRI)
		{
			dp_commands[number_of_polys_per_frame] = D3DPT_TRIANGLELIST;
			num_triangles_in_scene += (num_verts_per_poly / 3);
			error = FALSE;
		}
			

		if(error == TRUE)
			PrintMessage(myglobs.hWndMain, "ERROR UNRECOGNISED COMMAND", NULL , LOGFILE_ONLY);
	
		if (tex_flag == USE_PLAYERS_SKIN)
			texture_list_buffer[number_of_polys_per_frame] = texture_alias;
		else
			texture_list_buffer[number_of_polys_per_frame] = wptr->pmdata[pmodel_id].texture_list[i]; 
						
		number_of_polys_per_frame++;

	} // end for vert_cnt
		
	return;
}



// ******* START new code Added 2nd July 1999 *******

void PlayerToD3DIndexedVertList(int pmodel_id, int curr_frame, int angle, int texture_alias, int tex_flag)
{ 
	int i,j;
	int num_verts_per_poly;
	int num_faces_per_poly;
	int num_poly;
	int poly_command;
	int i_count, face_i_count;
	float x,y,z;
	float rx,ry,rz;
	float tx,ty;
	DWORD r,g,b;
	vert_ptr tp;	


	if(curr_frame >= wptr->pmdata[pmodel_id].num_frames)
		curr_frame = 0;

	cosine = cos_table[angle];
	sine   = sin_table[angle];

	i_count = 0;
	face_i_count = 0;

	// process and transfer the model data from the pmdata structure 
	// to the array of D3DLVERTEX structures, src_v 
	
	num_poly = wptr->pmdata[pmodel_id].num_polys_per_frame;
	
	for(i = 0; i < num_poly; i++)
	{
		poly_command	   = wptr->pmdata[pmodel_id].poly_cmd[i];
		num_verts_per_poly = wptr->pmdata[pmodel_id].num_verts_per_object[i];
		num_faces_per_poly = wptr->pmdata[pmodel_id].num_faces_per_object[i];
		
		number_of_faces_per_frame += num_faces_per_poly;

		for(j = 0; j < num_verts_per_poly; j++)
		{
			tp = &wptr->pmdata[pmodel_id].w[curr_frame][i_count];
			x = tp->x + x_off;
			z = tp->y + y_off;
			y = tp->z + z_off;

			rx = wx + (x*cosine - z*sine); 
			ry = wy + y;
			rz = wz + (x*sine + z*cosine);
				
			tx = wptr->pmdata[pmodel_id].t[i_count].x * wptr->pmdata[pmodel_id].skx; 
			ty = wptr->pmdata[pmodel_id].t[i_count].y * wptr->pmdata[pmodel_id].sky; 

			r=0;
			g=0;
			b=0;

			src_v[cnt].x = D3DVAL(rx);
			src_v[cnt].y = D3DVAL(ry);
			src_v[cnt].z = D3DVAL(rz);
			src_v[cnt].color = RGBA_MAKE(255, 255, 255, 255);
			src_v[cnt].specular = RGB_MAKE(r,g,b);
			src_v[cnt].tu = D3DVAL(tx);
			src_v[cnt].tv = D3DVAL(ty);

			cnt++;
			i_count++;
			number_of_verts_per_frame++;

		} // end for j
	
		for(j = 0; j < num_faces_per_poly*3; j++)
		{
			src_f[cnt_f] = wptr->pmdata[pmodel_id].f[face_i_count];
			cnt_f++;
			face_i_count++;
		}

		verts_per_poly[number_of_polys_per_frame] = num_verts_per_poly;
		faces_per_poly[number_of_polys_per_frame] = num_faces_per_poly;

		dp_command_index_mode[number_of_polys_per_frame] = USE_INDEXED_DP;
		dp_commands			 [number_of_polys_per_frame] = D3DPT_TRIANGLELIST;
		
		num_triangles_in_scene +=3;
			
		if (tex_flag == USE_PLAYERS_SKIN)
			texture_list_buffer[number_of_polys_per_frame] = texture_alias;
		else
			texture_list_buffer[number_of_polys_per_frame] = wptr->pmdata[pmodel_id].texture_list[i]; 
								
		number_of_polys_per_frame++;

	} // end for vert_cnt
		
	
	return;
}

// ******* END new code Added 2nd July 1999 *******



void ObjectToD3DVertList(int ob_type, int angle, int lit_v)
{ 
	int ob_vert_count = 0;
	int poly;
	int num_vert;
	int vert_cnt;
	int w, i;
	float x,y,z;
	int r,g,b;
	int poly_command;
	


	cosine = cos_table[angle];
	sine   = sin_table[angle];


	ob_vert_count = 0;
	poly = wptr->num_polys_per_object[ob_type];

	for(w = 0; w < poly; w++)
	{
		num_vert = wptr->obdata[ob_type].num_vert[w];

		for(vert_cnt = 0; vert_cnt < num_vert; vert_cnt++)
		{
			x = wptr->obdata[ob_type].v[ob_vert_count].x;
			y = wptr->obdata[ob_type].v[ob_vert_count].y;
			z = wptr->obdata[ob_type].v[ob_vert_count].z;

			tx[vert_cnt] = wptr->obdata[ob_type].t[ob_vert_count].x; 
			ty[vert_cnt] = wptr->obdata[ob_type].t[ob_vert_count].y; 

			mx[vert_cnt] = wx + (x*cosine - z*sine); 
			my[vert_cnt] = wy + y;
			mz[vert_cnt] = wz + (x*sine + z*cosine);
				
			r = wptr->oblist[lit_v].lit[ob_vert_count].r;  // 155;
	 		g = wptr->oblist[lit_v].lit[ob_vert_count].g;  // 100;
			b = wptr->oblist[lit_v].lit[ob_vert_count].b;  // 0;
			
		

			if(firing_gun_flag == TRUE)
			{
				gv_dx = mx[vert_cnt] - player_x;
				gv_dz = mz[vert_cnt] - player_z;

				gv_distance		= (float)sqrt(gv_dx * gv_dx + gv_dz * gv_dz);
		
				intensity = (int)( (float)255 * 4 * ((float)1 / gv_distance) );
		
				r += intensity;
				g += intensity;
				b += intensity;

				if(r > 255)
					r = 255;

				if(g > 255)
					g = 255;

				if(b > 255)
					b = 255;
			}
		
			mlr[vert_cnt] = r;
			mlg[vert_cnt] = g;
			mlb[vert_cnt] = b;

			ob_vert_count++;

		} // end for vert_cnt
						
		verts_per_poly[number_of_polys_per_frame] = num_vert;
		poly_command = wptr->obdata[ob_type].poly_cmd[w];

		ctext = wptr->obdata[ob_type].tex[w];
		texture_list_buffer[number_of_polys_per_frame] = ctext;
				

					
		if(poly_command == POLY_CMD_QUAD)
		{
			// Upper Triangle //////////////
			src_v[cnt].x = D3DVAL(mx[0]);
			src_v[cnt].y = D3DVAL(my[0]);
			src_v[cnt].z = D3DVAL(mz[0]);
			src_v[cnt].color = RGBA_MAKE(255, 255, 255, 255);
			src_v[cnt].specular = RGB_MAKE(mlr[0],mlg[0],mlb[0]);
			src_v[cnt].tu = D3DVAL(wptr->TexMap[ctext].tu[0]); // D3DVAL(0.0);
			src_v[cnt].tv = D3DVAL(wptr->TexMap[ctext].tv[0]); // D3DVAL(1.0);
	
			src_v[cnt+1].x = D3DVAL(mx[1]);
			src_v[cnt+1].y = D3DVAL(my[1]);
			src_v[cnt+1].z = D3DVAL(mz[1]);
			src_v[cnt+1].color = RGBA_MAKE(255, 255, 255, 255);
			src_v[cnt+1].specular = RGB_MAKE(mlr[1],mlg[1],mlb[1]);
			src_v[cnt+1].tu = D3DVAL(wptr->TexMap[ctext].tu[1]); // D3DVAL(0.0); //wptr->obdata[object_id].v[v_count].x
			src_v[cnt+1].tv = D3DVAL(wptr->TexMap[ctext].tv[1]); // D3DVAL(0.0); //wptr->obdata[object_id].v[v_count].y
		
			src_v[cnt+2].x = D3DVAL(mx[2]);
			src_v[cnt+2].y = D3DVAL(my[2]);
			src_v[cnt+2].z = D3DVAL(mz[2]);
			src_v[cnt+2].color = RGBA_MAKE(255, 255, 255, 255);
			src_v[cnt+2].specular = RGB_MAKE(mlr[2],mlg[2],mlb[2]);
			src_v[cnt+2].tu = D3DVAL(wptr->TexMap[ctext].tu[2]); //D3DVAL(1.0);
			src_v[cnt+2].tv = D3DVAL(wptr->TexMap[ctext].tv[2]); //D3DVAL(1.0);
		
			// Lower Triangle ////////////
		
			src_v[cnt+3].x = D3DVAL(mx[3]);
			src_v[cnt+3].y = D3DVAL(my[3]);
			src_v[cnt+3].z = D3DVAL(mz[3]);
			src_v[cnt+3].color = RGBA_MAKE(255,255,255,255);
			src_v[cnt+3].specular = RGB_MAKE(mlr[3],mlg[3],mlb[3]);
			src_v[cnt+3].tu = D3DVAL(wptr->TexMap[ctext].tu[3]); //D3DVAL(1.0);
			src_v[cnt+3].tv = D3DVAL(wptr->TexMap[ctext].tv[3]); //D3DVAL(0.0);

			cnt+=4;
			num_triangles_in_scene += 2;
			dp_commands[number_of_polys_per_frame] = D3DPT_TRIANGLESTRIP;
			dp_command_index_mode[number_of_polys_per_frame] = USE_NON_INDEXED_DP;
		}

		if(poly_command == POLY_CMD_QUAD_TEX)
		{
			// Upper Triangle //////////////
			src_v[cnt].x = D3DVAL(mx[0]);
			src_v[cnt].y = D3DVAL(my[0]);
			src_v[cnt].z = D3DVAL(mz[0]);
			src_v[cnt].color = RGBA_MAKE(255, 255, 255, 255);
			src_v[cnt].specular = RGB_MAKE(mlr[0],mlg[0],mlb[0]);
			src_v[cnt].tu = D3DVAL(tx[0]); 
			src_v[cnt].tv = D3DVAL(ty[0]);
	
			src_v[cnt+1].x = D3DVAL(mx[1]);
			src_v[cnt+1].y = D3DVAL(my[1]);
			src_v[cnt+1].z = D3DVAL(mz[1]);
			src_v[cnt+1].color = RGBA_MAKE(255, 255, 255, 255);
			src_v[cnt+1].specular = RGB_MAKE(mlr[1],mlg[1],mlb[1]);
			src_v[cnt+1].tu = D3DVAL(tx[1]);
			src_v[cnt+1].tv = D3DVAL(ty[1]); 
		
			src_v[cnt+2].x = D3DVAL(mx[2]);
			src_v[cnt+2].y = D3DVAL(my[2]);
			src_v[cnt+2].z = D3DVAL(mz[2]);
			src_v[cnt+2].color = RGBA_MAKE(255, 255, 255, 255);
			src_v[cnt+2].specular = RGB_MAKE(mlr[2],mlg[2],mlb[2]);
			src_v[cnt+2].tu = D3DVAL(tx[2]); 
			src_v[cnt+2].tv = D3DVAL(ty[2]);
		
			// Lower Triangle ////////////
		
			src_v[cnt+3].x = D3DVAL(mx[3]);
			src_v[cnt+3].y = D3DVAL(my[3]);
			src_v[cnt+3].z = D3DVAL(mz[3]);
			src_v[cnt+3].color = RGBA_MAKE(255,255,255,255);
			src_v[cnt+3].specular = RGB_MAKE(mlr[3],mlg[3],mlb[3]);
			src_v[cnt+3].tu = D3DVAL(tx[3]); 
			src_v[cnt+3].tv = D3DVAL(ty[3]); 

			cnt+=4;
			num_triangles_in_scene += 2;
			dp_commands[number_of_polys_per_frame] = D3DPT_TRIANGLESTRIP;
			dp_command_index_mode[number_of_polys_per_frame] = USE_NON_INDEXED_DP;
		}

		if(poly_command == POLY_CMD_TRI)
		{
			// Triangle //////////////

			src_v[cnt].x = D3DVAL(mx[0]);
			src_v[cnt].y = D3DVAL(my[0]);
			src_v[cnt].z = D3DVAL(mz[0]);
			src_v[cnt].color = RGBA_MAKE(255, 255, 255, 255);
			src_v[cnt].specular = RGB_MAKE(mlr[0],mlg[0],mlb[0]);
			src_v[cnt].tu = D3DVAL(0.0);
			src_v[cnt].tv = D3DVAL(0.0);
	
			src_v[cnt+1].x = D3DVAL(mx[1]);
			src_v[cnt+1].y = D3DVAL(my[1]);
			src_v[cnt+1].z = D3DVAL(mz[1]);
			src_v[cnt+1].color = RGBA_MAKE(255, 255, 255, 255);
			src_v[cnt+1].specular = RGB_MAKE(mlr[1],mlg[1],mlb[1]);
			src_v[cnt+1].tu = D3DVAL(1.0);
			src_v[cnt+1].tv = D3DVAL(0.0);
		
			src_v[cnt+2].x = D3DVAL(mx[2]);
			src_v[cnt+2].y = D3DVAL(my[2]);
			src_v[cnt+2].z = D3DVAL(mz[2]);
			src_v[cnt+2].color = RGBA_MAKE(255, 255, 255, 255);
			src_v[cnt+2].specular = RGB_MAKE(mlr[2],mlg[2],mlb[2]);
			src_v[cnt+2].tu = D3DVAL(0.0);
			src_v[cnt+2].tv = D3DVAL(1.0);
				
			cnt+=3;
			num_triangles_in_scene ++;
			dp_commands[number_of_polys_per_frame] = D3DPT_TRIANGLELIST;
			dp_command_index_mode[number_of_polys_per_frame] = USE_NON_INDEXED_DP;
		}

		if(poly_command == POLY_CMD_TRI_FAN)
		{
			// Triangle //////////////

			for(i = 0; i < verts_per_poly[number_of_polys_per_frame]; i++)
			{
				src_v[cnt].x = D3DVAL(mx[i]);
				src_v[cnt].y = D3DVAL(my[i]);
				src_v[cnt].z = D3DVAL(mz[i]);
				src_v[cnt].color = RGBA_MAKE(255, 255, 255, 255);
				src_v[cnt].specular = RGB_MAKE(mlr[i],mlg[i],mlb[i]);
				src_v[cnt].tu = D3DVAL( tx[i] );
				src_v[cnt].tv = D3DVAL( ty[i] );
				cnt++;
			}
			num_triangles_in_scene += (verts_per_poly[number_of_polys_per_frame] - 2);
			dp_commands[number_of_polys_per_frame] = D3DPT_TRIANGLEFAN;
			dp_command_index_mode[number_of_polys_per_frame] = USE_NON_INDEXED_DP;
		}

		if(poly_command == POLY_CMD_TRI_STRIP)
		{
			// Triangle //////////////

			for(i = 0; i < verts_per_poly[number_of_polys_per_frame]; i++)
			{
				src_v[cnt].x = D3DVAL(mx[i]);
				src_v[cnt].y = D3DVAL(my[i]);
				src_v[cnt].z = D3DVAL(mz[i]);
				src_v[cnt].color = RGBA_MAKE(255, 255, 255, 255);
				src_v[cnt].specular = RGB_MAKE(mlr[i],mlg[i],mlb[i]);
				src_v[cnt].tu = D3DVAL( tx[i] );
				src_v[cnt].tv = D3DVAL( ty[i] );
				cnt++;
			}
			num_triangles_in_scene += (verts_per_poly[number_of_polys_per_frame] - 2);
			dp_commands[number_of_polys_per_frame] = D3DPT_TRIANGLESTRIP;
			dp_command_index_mode[number_of_polys_per_frame] = USE_NON_INDEXED_DP;
		}

		number_of_polys_per_frame++;
	} // end for w

	return;
}


void ReleaseScene(void)
{
	PrintMessage(myglobs.hWndMain, "ReleaseScene : suceeded", NULL, LOGFILE_ONLY);	
    return;
}

void ReleaseView()
{
	PrintMessage(myglobs.hWndMain, "ReleaseView : suceeded", NULL, LOGFILE_ONLY);	
	return;
}

BOOL InitScene(void)
{
	int i;
	float angle;
	float k=(float)0.017453292;

	wptr=GetWorldData();

	for(i=0;i<360;i++)
	{
		angle = (float)i * k;
		sin_table[i] = (float)sin(angle);
		cos_table[i] = (float)cos(angle);
	}
	PrintMessage(myglobs.hWndMain, "InitScene : suceeded", NULL, LOGFILE_ONLY);
	return TRUE;
}

BOOL InitView(LPDIRECTDRAW7 lpDD, LPDIRECT3D7 lpD3D, LPDIRECT3DDEVICE7 lpDev,
	   int NumTextures)
	  
{ 
	D3DMATERIAL7 mtrl[MAX_NUM_TEXTURES]; 
	int i;
	

	if(NumTextures > MAX_NUM_TEXTURES)
		return FALSE;
	

	for(i = 0; i < NumTextures; i++)
	{		
    
		memset(&mtrl[i], 0, sizeof(D3DMATERIAL));
		mtrl[i].diffuse.r = (D3DVALUE)0.0;
		mtrl[i].diffuse.g = (D3DVALUE)0.0;
		mtrl[i].diffuse.b = (D3DVALUE)0.0;
		mtrl[i].ambient.r = (D3DVALUE)0.0;
		mtrl[i].ambient.g = (D3DVALUE)0.0;
		mtrl[i].ambient.b = (D3DVALUE)0.0;
	
		mtrl[i].specular.r = (D3DVALUE)1.0;
		mtrl[i].specular.g = (D3DVALUE)1.0;
		mtrl[i].specular.b = (D3DVALUE)1.0;
		mtrl[i].power = (float)40.0;
	
//		if (lpDev->SetMaterial(lpMat[i]) != D3D_OK) 	
//			return FALSE;

		lpMat[i] = &mtrl[i];
		if (lpDev->SetMaterial(&mtrl[i]) != D3D_OK) 	
			return FALSE;
		    
	} // end for i

	PrintMessage(myglobs.hWndMain, "InitView : suceeded", NULL, LOGFILE_ONLY);

    return TRUE;
}



