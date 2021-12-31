
// D3D Road Rage version 1.1
// Written by William Chin
// Developed in MSDEV C++ v4.2 with DirectX 5 SDK
// Date : 8/10/98
// File : RoadRage.cpp

#include <d3d.h>
#include "d3ddemo.h"
#include "d3dmain.h"
#include <ddraw.h>
#include <stdio.h>
#include <math.h>
#include "matrix.h"
#include "d3ddemo.h"
#include "world.h"

// This program allows for a maximum of 4000 quads
// which equates to 8000 triagles and 24000 vertices

#define MAX_NUM_QUADS		6000
#define MAX_NUM_TRIANGLES	12000
#define MAX_NUM_VERTICES	24000
#define MAX_NUM_TEXTURES	50
 
MAP map;
world_ptr wptr;

D3DLVERTEX src_v[MAX_NUM_VERTICES];
LPDIRECT3DMATERIAL2 lpBmat, lpMat1[MAX_NUM_TEXTURES];

DWORD hTex[MAX_NUM_TEXTURES];
DWORD hMat[MAX_NUM_TEXTURES];

static const float pi = 3.141592654f;
static const float piover2 = (pi / 2.0f);

D3DMATRIX trans, roty;
D3DMATRIX WorldMatrix, ViewMatrix, ProjMatrix;
D3DVALUE posx, posy, posz; //Player position
D3DVALUE angx, angy, angz; //Player angles
D3DVECTOR from, at, up; // Used to set view matrix
D3DVALUE Z_NEAR = D3DVAL(6.0);
D3DVALUE Z_FAR = D3DVAL(10000.0);
D3DVALUE FOV = D3DVAL(piover2);

int cell_flag = 0;
int NUM_QUADS = 0;
int NUM_TRIANGLES;
int NUM_VERTICES;
int Num_Quads;
int Num_Triangles;
int Num_Vertices;
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
float sine, cosine;
float wx,wy,wz;

int number_of_polys_per_frame;
int verts_per_poly[MAX_NUM_QUADS];
int poly_commands [MAX_NUM_QUADS];

float sin_table[361];
float cos_table[361];
float gradient;
float c;
float inv_gradient;
float ncp_angle;
float view_point_x,view_point_z;
float *buffer2;
float k;

float px[4],py[4],pz[4],pw[4];
float mx[4],my[4],mz[4],mw[4];
float cx[4],cy[4],cz[4],cw[4];
float tx[100],ty[100];

int mlr[4],mlg[4],mlb[4];

int texture_list_buffer[MAX_NUM_QUADS];

int ctext; 
int poly_clip_flags[MAX_NUM_QUADS];
int poly_clip[MAX_NUM_QUADS];

BOOL oblist_overdraw_flags[MAX_NUM_MAP_OBJECTS];
BOOL render_first_frame = TRUE;

// prototype

void ObjectToD3DVertList(int ob_type, int angle, int oblist_id);
void PlayerToD3DVertList(int ob_type, int angle, int texture_alias , float skx, float sky);
void WeaponToD3DVertList(int ob_type, int angle, int texture_alias,float skx, float sky );


void OverrideDefaults(Defaults* defaults)
{
    lstrcpy(defaults->Name, "RR Map Editor v1.1 - 3D Editor");
    defaults->rs.bPerspCorrect = TRUE;
    defaults->bResizingDisabled = FALSE;
}


BOOL RenderScene(LPDIRECT3DDEVICE2 lpDev, LPDIRECT3DVIEWPORT2 lpView,
            LPD3DRECT lpExtent,CAMERA cam)
{
	int oblist_id, i;
	int angle;
	int last_texture_number;
	int texture_alias_number;
	int ob_type;
	int vert_index;
	int cell_x,cell_z;
	int vi,q,ix,iz,icnt;
	int vol_length;
	int vol_x[50];
	int vol_z[50];
	double k=0.017453292;
	DWORD color_key = 0;
	char buffer[256];
	


    D3DCLIPSTATUS status={D3DCLIPSTATUS_EXTENTS2, 0, 2048.0f, 0.0f, 2048.0f, 0.0f, 0.0f, 0.0f};
	
	number_of_polys_per_frame = 0;

	RampSize = cam.ramp_size;
	poly_cnt = 0;
	cnt = 0;

	cell_x = (int)(cam.view_point_x / 256);
	cell_z = (int)(cam.view_point_z / 256);

	icnt = 0;
	angy = (D3DVALUE)cam.view_angle;
	
	for(i = 0; i < MAX_NUM_MAP_OBJECTS; i++)
		oblist_overdraw_flags[i] = FALSE;

	if(angy >= 360)
		angy = angy-360;

	if(angy < 0)
		angy += 360;

	if((angy >= 315) || (angy <=45)) // 1
	{
		for(ix =- 3; ix <= 3; ix++)
		{
			vol_x[icnt]   = cell_x + ix, vol_z[icnt]   = cell_z-1;
			vol_x[icnt+1] = cell_x + ix, vol_z[icnt+1] = cell_z;
			vol_x[icnt+2] = cell_x + ix, vol_z[icnt+2] = cell_z+1;
			vol_x[icnt+3] = cell_x + ix, vol_z[icnt+3] = cell_z+2;
			vol_x[icnt+4] = cell_x + ix, vol_z[icnt+4] = cell_z+3;
			vol_x[icnt+5] = cell_x + ix, vol_z[icnt+5] = cell_z+4;
			vol_x[icnt+6] = cell_x + ix, vol_z[icnt+6] = cell_z+5;
			
			icnt+=7;
		}
	}

	if((angy < 315) && (angy > 225)) // 2
	{
		for(iz = -3;iz <= 3; iz++)
		{
			vol_x[icnt]   = cell_x ,    vol_z[icnt]   = cell_z + iz;
			vol_x[icnt+1] = cell_x - 1, vol_z[icnt+1] = cell_z + iz;
			vol_x[icnt+2] = cell_x - 2, vol_z[icnt+2] = cell_z + iz;
			vol_x[icnt+3] = cell_x - 3, vol_z[icnt+3] = cell_z + iz;
			vol_x[icnt+4] = cell_x - 4, vol_z[icnt+4] = cell_z + iz;
			vol_x[icnt+5] = cell_x - 5, vol_z[icnt+5] = cell_z + iz;
			vol_x[icnt+6] = cell_x - 6, vol_z[icnt+6] = cell_z + iz;

			icnt+=7;
		}
	}

	if((angy <= 225) && (angy >= 135)) // 3
	{
		for(ix = -3;ix <= 3; ix++)
		{
			vol_x[icnt]   = cell_x + ix, vol_z[icnt]=cell_z;
			vol_x[icnt+1] = cell_x + ix, vol_z[icnt+1]=cell_z-1;
			vol_x[icnt+2] = cell_x + ix, vol_z[icnt+2]=cell_z-2;
			vol_x[icnt+3] = cell_x + ix, vol_z[icnt+3]=cell_z-3;
			vol_x[icnt+4] = cell_x + ix, vol_z[icnt+4]=cell_z-4;
			vol_x[icnt+5] = cell_x + ix, vol_z[icnt+5]=cell_z-5;
			vol_x[icnt+6] = cell_x + ix, vol_z[icnt+6]=cell_z-6;

			icnt+=7;
		}
	}

	if((angy > 45) && (angy < 135)) // 4
	{
		for(iz =- 3; iz <= 3; iz++)
		{
			vol_x[icnt]   = cell_x ,    vol_z[icnt]   = cell_z + iz;
			vol_x[icnt+1] = cell_x + 1, vol_z[icnt+1] = cell_z + iz;
			vol_x[icnt+2] = cell_x + 2, vol_z[icnt+2] = cell_z + iz;
			vol_x[icnt+3] = cell_x + 3, vol_z[icnt+3] = cell_z + iz;
			vol_x[icnt+4] = cell_x + 4, vol_z[icnt+4] = cell_z + iz;
			vol_x[icnt+5] = cell_x + 5, vol_z[icnt+5] = cell_z + iz;
			vol_x[icnt+6] = cell_x + 6, vol_z[icnt+6] = cell_z + iz;

			icnt+=7;
		}
	}

	number_of_polys_per_frame = 0;
	cnt = 0;

	vol_length = icnt;

	for(vi = 0; vi < vol_length; vi++)
	{		
		cell_x = vol_x[vi];
		cell_z = vol_z[vi];
	
		if(wptr->cell[cell_x][cell_z] != NULL)
		{
			if(!(cell_x < 0) || (cell_z < 0))
			{
				cell_flag = 1;
				clength = wptr->cell_length[cell_x][cell_z];
			
				for(q = 0; q < clength; q++)
				{
					oblist_id = wptr->cell[cell_x][cell_z][q];

					// check map object list and prevent the same 
					// objects being drawn more than once

					if(oblist_overdraw_flags[oblist_id] == FALSE)
					{
						oblist_overdraw_flags[oblist_id] = TRUE;

						wx = wptr->oblist[oblist_id].x;
						wy = wptr->oblist[oblist_id].y;
						wz = wptr->oblist[oblist_id].z;
		
						angle = (int)wptr->oblist[oblist_id].rot_angle;
						ob_type = wptr->oblist[oblist_id].type;

						ObjectToD3DVertList(ob_type, angle, oblist_id);

					}
				} // end for q
			} // end if
		} // end if
	} // end for vi
	//}

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
	

	if (lpDev->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, TRUE) != D3D_OK) 
		return FALSE;	

	///////////////////////////////////////////////////////////////////////

	last_texture_number = 1000;
	vert_index = 0;

	wptr->num_polys_per_frame = number_of_polys_per_frame;

	for(i = 0; i < number_of_polys_per_frame; i++)
	{
		texture_alias_number = texture_list_buffer[i];
		texture_number = wptr->TexMap[texture_alias_number].texture; 

		if(texture_number!=last_texture_number)
		{
			if (lpDev->SetLightState(D3DLIGHTSTATE_MATERIAL, 
				hMat[texture_number]) != D3D_OK)
			{
				return FALSE; 
			}
		
			if (lpDev->SetRenderState(D3DRENDERSTATE_TEXTUREHANDLE, 
				hTex[texture_number]) != D3D_OK) 
			{
				return FALSE;
			}
		}


		if((poly_commands[i] == POLY_CMD_QUAD) || (poly_commands[i] == POLY_CMD_QUAD_TEX))
		{
			if (lpDev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 
				D3DVT_LVERTEX , (LPVOID)&src_v[vert_index], 4, 
				NULL) != D3D_OK) 
			{
				return FALSE;
			}
			last_texture_number = texture_number;
			vert_index += 4;

		}

		if(poly_commands[i] == POLY_CMD_TRI)
		{
			if (lpDev->DrawPrimitive(D3DPT_TRIANGLELIST, 
				D3DVT_LVERTEX , (LPVOID)&src_v[vert_index], 3, 
				NULL) != D3D_OK)
			{
				return FALSE;
			}
			last_texture_number = texture_number;
			vert_index += 3;
		}

		if(poly_commands[i] == POLY_CMD_TRI_STRIP)
		{
			if (lpDev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 
				D3DVT_LVERTEX , (LPVOID)&src_v[vert_index], verts_per_poly[i], 
				NULL) != D3D_OK)
			{
				return FALSE;
			}
			last_texture_number = texture_number;
			vert_index += verts_per_poly[i];
		}

		if(poly_commands[i] == POLY_CMD_TRI_FAN)
		{
			if (lpDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 
				D3DVT_LVERTEX, (LPVOID)&src_v[vert_index], verts_per_poly[i], 
				NULL) != D3D_OK)
			{
				return FALSE;
			}
			last_texture_number = texture_number;
			vert_index += verts_per_poly[i];
		}

	} // end for i

	wptr->num_verts_per_frame = vert_index;

    if (lpDev->GetClipStatus(&status) != D3D_OK)
        return FALSE; 
	
    if (lpDev->EndScene() != D3D_OK)	
        return FALSE;
	
	lpExtent->x1 = (LONG)floor((double)status.minx);
	lpExtent->x2 = (LONG)ceil((double)status.maxx);
	lpExtent->y1 = (LONG)floor((double)status.miny);
	lpExtent->y2 = (LONG)ceil((double)status.maxy);
	
	if(render_first_frame == TRUE)
	{
		PrintMessage(NULL, "rendering first frame suceeded", NULL, LOGFILE_ONLY);

		itoa(vert_index, buffer, 10);
		PrintMessage(NULL, "Num verts in scene: ", buffer, LOGFILE_ONLY);

		
		itoa(number_of_polys_per_frame, buffer, 10);
		PrintMessage(NULL, "Num polys in scene: ", buffer, LOGFILE_ONLY);

	}

	render_first_frame = FALSE;

    return TRUE;
}




void ObjectToD3DVertList(int ob_type, int angle, int oblist_id)
{ 
	int ob_vert_count = 0;
	int poly;
	int num_vert;
	int vert_cnt;
	int w, i;
	float x,y,z;
	int r,g,b;
	char buffer[256];
	char buf2[256];
	char buf3[256];
	

	cosine = cos_table[angle];
	sine   = sin_table[angle];


	if(render_first_frame == TRUE)
	{
		strcpy(buffer, "ob_type = ");
		itoa(ob_type, buf2, 10);
		strcat(buffer, buf2);
		strcat(buffer, "  oblist_id = ");

		itoa(oblist_id, buf3, 10);
		strcat(buffer, buf3);

		PrintMessage(NULL, "ObjectToD3DVertList: ", buffer, LOGFILE_ONLY);
	}

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

			mx[vert_cnt] = wx + (x * cosine - z * sine); 
			my[vert_cnt] = wy +  y;
			mz[vert_cnt] = wz + (x * sine   + z * cosine);
				
			if(wptr->oblist[oblist_id].lit != NULL)
			{
				r = wptr->oblist[oblist_id].lit[ob_vert_count].r;  // 155;
	 			g = wptr->oblist[oblist_id].lit[ob_vert_count].g;  // 100;
				b = wptr->oblist[oblist_id].lit[ob_vert_count].b;  // 0;
			}
			else
			{
				r = 0;
				g = 0;
				b = 0;
			}

			mlr[vert_cnt] = r;
			mlg[vert_cnt] = g;
			mlb[vert_cnt] = b;

			ob_vert_count++;

		} // end for vert_cnt
						
		verts_per_poly[number_of_polys_per_frame] = num_vert;
		poly_commands [number_of_polys_per_frame] = wptr->obdata[ob_type].poly_cmd[w];

		ctext = wptr->obdata[ob_type].tex[w];
		texture_list_buffer[number_of_polys_per_frame] = ctext;
				

					
		if(poly_commands [number_of_polys_per_frame] == POLY_CMD_QUAD)
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
		}

		if(poly_commands [number_of_polys_per_frame] == POLY_CMD_QUAD_TEX)
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
		}

		if(poly_commands [number_of_polys_per_frame] == POLY_CMD_TRI)
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
		}

		if(poly_commands [number_of_polys_per_frame] == POLY_CMD_TRI_FAN)
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
		}

		if(poly_commands [number_of_polys_per_frame] == POLY_CMD_TRI_STRIP)
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
		}

		number_of_polys_per_frame++;
	} // end for w

	return;
}



void ReleaseScene(void)
{
    return;
}

void ReleaseView(LPDIRECT3DVIEWPORT2 lpView)
{
    lpView;
    //RELEASE(lpMat1[10]);
    RELEASE(lpBmat);
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
	return TRUE;
}

BOOL InitView(LPDIRECTDRAW2 lpDD, LPDIRECT3D2 lpD3D, LPDIRECT3DDEVICE2 lpDev,
	   LPDIRECT3DVIEWPORT2 lpView, int NumTextures,
	   LPD3DTEXTUREHANDLE TextureHandle)
{
    D3DMATERIAL bmat, mat[MAX_NUM_TEXTURES];
    D3DMATERIALHANDLE hBmat, hMat1[MAX_NUM_TEXTURES];
	int i;
	
    if (lpD3D->CreateMaterial(&lpBmat, NULL) != D3D_OK) 
	{
		return FALSE;
    }
    memset(&bmat, 0, sizeof(D3DMATERIAL));
    bmat.dwSize = sizeof(D3DMATERIAL);
    bmat.diffuse.r = (D3DVALUE)1.0;
    bmat.diffuse.g = (D3DVALUE)1.0;
    bmat.diffuse.b = (D3DVALUE)1.0;
    bmat.ambient.r = (D3DVALUE)1.0;
    bmat.ambient.g = (D3DVALUE)1.0;
    bmat.ambient.b = (D3DVALUE)1.0;
    bmat.hTexture = TextureHandle[0];
    bmat.dwRampSize = 1;
    lpBmat->SetMaterial  (&bmat);
    lpBmat->GetHandle    (lpDev, &hBmat);
    lpView->SetBackground(hBmat);
	
	if(NumTextures>MAX_NUM_TEXTURES)
		return FALSE;
	
	for(i=0;i<NumTextures;i++)
	{

		if (lpD3D->CreateMaterial(&lpMat1[i], NULL) != D3D_OK) 
		{	
			return FALSE;
		}
    
		memset(&mat[i], 0, sizeof(D3DMATERIAL));
		mat[i].dwSize = sizeof(D3DMATERIAL);
		mat[i].diffuse.r = (D3DVALUE)1.0;
		mat[i].diffuse.g = (D3DVALUE)1.0;
		mat[i].diffuse.b = (D3DVALUE)1.0;
		mat[i].ambient.r = (D3DVALUE)1.0;
		mat[i].ambient.g = (D3DVALUE)1.0;
		mat[i].ambient.b = (D3DVALUE)1.0;
	#define SPECULAR
	#ifdef SPECULAR
		mat[i].specular.r = (D3DVALUE)1.0;
		mat[i].specular.g = (D3DVALUE)1.0;
		mat[i].specular.b = (D3DVALUE)1.0;
		mat[i].power = (float)40.0;
	#else
		mat[i].specular.r = (D3DVALUE)0.0;
		mat[i].specular.g = (D3DVALUE)0.0;
		mat[i].specular.b = (D3DVALUE)0.0;
		mat[i].power = (float)0.0;
	#endif    
		mat[i].hTexture = TextureHandle[i];
		mat[i].dwRampSize = RampSize;
		lpMat1[i]->SetMaterial(&mat[i]);
		lpMat1[i]->GetHandle  (lpDev, &hMat1[i]);
		hMat[i] = (DWORD) hMat1[i];
		hTex[i] = TextureHandle[i];
    
	} // end for i

    return TRUE;
}



