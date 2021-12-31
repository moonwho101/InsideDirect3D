//-----------------------------------------------------------------------------
// File: LoadWorld.cpp
//
// Desc: Code for loading the RoadRage 3D world
//
// Copyright (c) 1999 William Chin and Peter Kovach. All rights reserved.
//-----------------------------------------------------------------------------

#include "resource.h"
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <dinput.h>
#include <dplobby.h>
#include "D3DApp.h"
#include <dsound.h>
#include "D3DUtil.h"
#include "world.hpp"
#include "3Dengine.hpp"
#include "RoadRage.hpp"
#include "D3DTextr.h" 
#include "ImportMD2.hpp"
#include "Import3DS.hpp"
#include "LoadWorld.hpp"

#define MD2_MODEL  0
#define	k3DS_MODEL 1	

extern CMD2* pCMD2;
extern C3DS* pC3DS;
extern CMyD3DApplication* pCMyApp;

int player_count = 0;
int op_gun_count = 0;
int your_gun_count = 0;
int car_count = 0;
int type_num;
int num_imported_models_loaded = 0;
char g_model_filename[256];


CLoadWorld* pCWorld;


CLoadWorld::CLoadWorld()
{
	pCWorld = this;
}

BOOL CLoadWorld::LoadWorldMap(HWND hwnd,char *filename)
{ 
	FILE *fp;    
	char s[256];
	char p[256];
	char buffer[100];
	int y_count=30;
	int done=0;
	int object_count=0;
	int vert_count=0;
	int pv_count=0;
	int poly_count=0;
	int object_id;
	BOOL lwm_start_flag=TRUE;
	int num_lverts;
	int i;
	int mem_counter;
	int lit_vert;
	BYTE red, green, blue;


    fp = fopen(filename,"r");
    if(fp==NULL)
    {     
		PrintMessage(hwnd, "Error can't load file ", filename, SCN_AND_FILE);
        MessageBox(hwnd,"Error can't load file",NULL,MB_OK);
		return FALSE;
    }

	PrintMessage(hwnd, "Loading map ", filename, SCN_AND_FILE);
	pCMyApp->num_light_sources_in_map = 0;

	while(done==0)
	{
		fscanf( fp, "%s", &s );

		if(strcmp(s,"OBJECT")==0)
		{
			fscanf( fp, "%s", &p );
			
			object_id = CheckObjectId(hwnd, (char *)&p);
			if(object_id == -1)
			{
				PrintMessage(hwnd, "Error Bad Object ID in: LoadWorld ", p, SCN_AND_FILE);
				MessageBox(hwnd,"Error Bad Object ID in: LoadWorld",NULL,MB_OK);
				return FALSE;
			}
			if(lwm_start_flag == FALSE)
				object_count++;

			pCMyApp->oblist[object_count].type = object_id;
			
			num_lverts = pCMyApp->num_vert_per_object[object_id];
			pCMyApp->oblist[object_count].lit = new LIGHT[num_lverts];
			mem_counter += sizeof(LIGHT) * num_lverts;

			pCMyApp->oblist[object_count].light_source = new LIGHTSOURCE;
			pCMyApp->oblist[object_count].light_source->command = 0;
			pCMyApp->num_light_sources_in_map++;	
		
			for(i = 0; i < num_lverts; i++)
			{
				pCMyApp->oblist[object_count].lit[i].r = 0;
				pCMyApp->oblist[object_count].lit[i].g = 0;
				pCMyApp->oblist[object_count].lit[i].b = 0;
			}

			lwm_start_flag = FALSE;
		}
		
		if(strcmp(s,"CO_ORDINATES")==0)
		{
			fscanf( fp, "%s", &p );
			pCMyApp->oblist[object_count].x = (float)atof(p);
			
			fscanf( fp, "%s", &p );
			pCMyApp->oblist[object_count].y = (float)atof(p);
			
			fscanf( fp, "%s", &p );
			pCMyApp->oblist[object_count].z = (float)atof(p);
		}

		if(strcmp(s,"ROT_ANGLE")==0)
		{
			fscanf( fp, "%s", &p );
			pCMyApp->oblist[object_count].rot_angle = (float)atof(p);
		}

		if(strcmp(s,"LIGHT_ON_VERT")==0)
		{	
			fscanf( fp, "%s", &p );
			lit_vert = atoi(p);

			fscanf( fp, "%s", &p );
			red = atoi(p);
			pCMyApp->oblist[object_count].lit[lit_vert].r = red;

			fscanf( fp, "%s", &p );
			green = atoi(p);
			pCMyApp->oblist[object_count].lit[lit_vert].g = green;

			fscanf( fp, "%s", &p );
			blue = atoi(p);
			pCMyApp->oblist[object_count].lit[lit_vert].b = blue;
		}
		
		
		if(strcmp(s,"LIGHT_SOURCE")==0)
		{	
			fscanf( fp, "%s", &p );

			if(strcmp(p,"Spotlight") == 0)
				pCMyApp->oblist[object_count].light_source->command = SPOT_LIGHT_SOURCE;
				
			if(strcmp(p,"directional") == 0)
				pCMyApp->oblist[object_count].light_source->command = DIRECTIONAL_LIGHT_SOURCE;

			if(strcmp(p,"point") == 0)
				pCMyApp->oblist[object_count].light_source->command = POINT_LIGHT_SOURCE;
			
			fscanf( fp, "%s", &p );

			if(strcmp(p,"POS")==0)	
			{
				fscanf( fp, "%s", &p );
				pCMyApp->oblist[object_count].light_source->position_x = (float)atof(p);
				fscanf( fp, "%s", &p );
				pCMyApp->oblist[object_count].light_source->position_y = (float)atof(p);
				fscanf( fp, "%s", &p );
				pCMyApp->oblist[object_count].light_source->position_z = (float)atof(p);
			}

			fscanf( fp, "%s", &p );

			if(strcmp(p,"DIR")==0)
			{
				fscanf( fp, "%s", &p );
				pCMyApp->oblist[object_count].light_source->direction_x = (float)atof(p);
				fscanf( fp, "%s", &p );
				pCMyApp->oblist[object_count].light_source->direction_y = (float)atof(p);
				fscanf( fp, "%s", &p );
				pCMyApp->oblist[object_count].light_source->direction_z = (float)atof(p);
			}

			
		}

		if(strcmp(s,"END_FILE")==0)
		{
			fscanf( fp, "%s", &p );	
			pCMyApp->oblist_length = object_count+1;
			done=1;
		}
	}
	fclose(fp);	
	
	itoa(pCMyApp->oblist_length, buffer, 10);
	PrintMessage(hwnd, buffer, " map objects loaded (oblist_length)",SCN_AND_FILE);
	PrintMessage(hwnd, "\n", NULL, LOGFILE_ONLY);
	
	return TRUE;
}

int CLoadWorld::CheckObjectId(HWND hwnd, char *p) 
{
	int i;
	char *buffer2;

	for(i = 0; i < pCMyApp->obdata_length; i++)
	{
		buffer2 = pCMyApp->obdata[i].name;

		if(strcmp(buffer2, p)==0)
		{
			return i;
		}
	}
	PrintMessage(hwnd, buffer2, "ERROR bad ID in : CheckObjectId ",SCN_AND_FILE);
	MessageBox(hwnd,buffer2,"Bad ID in :CheckObjectId", MB_OK);
	
	return -1; //error
}

BOOL CLoadWorld::InitPreCompiledWorldMap(HWND hwnd, char *filename)
{
	FILE *fp;    
	char s[256];
	int w;
	int done = 0;
	int count;
	int cell_x, cell_z;
	int mem_count;


	PrintMessage(hwnd, "InitPreCompiledWorldMap: starting\n", NULL, LOGFILE_ONLY);
		
	fp = fopen(filename,"r");
    if(fp==NULL)
    {     
		PrintMessage(hwnd,"Error can't load ", filename, SCN_AND_FILE);
        MessageBox(hwnd,"Error can't load file",NULL,MB_OK);
		return FALSE;
	}

	for(cell_z = 0; cell_z < 200; cell_z++)
	{
		for(cell_x = 0; cell_x < 200; cell_x++)
		{
			pCMyApp->cell[cell_x][cell_z] = NULL;
			pCMyApp->cell_length[cell_x][cell_z] = 0;
		}
	}
	
	mem_count = 0;

	while(done==0)
	{
		fscanf( fp, "%s", &s );

		if(strcmp(s,"END_FILE")==0)
		{	
			done=1;
			fclose(fp);	
			PrintMessage(hwnd, "InitPreCompiledWorldMap: sucess\n\n", NULL, LOGFILE_ONLY);
			return TRUE;
		}

		cell_x = atoi(s);

		if((cell_x >=0) && (cell_x <=200))
		{
			fscanf( fp, "%s", &s );
			cell_z = atoi(s);

			if((cell_z >=0) && (cell_z <=200))
			{
				fscanf( fp, "%s", &s );
				count = atoi(s);
			
				pCMyApp->cell[cell_x][cell_z] = new int[count];
				mem_count = mem_count + (count* sizeof(int));
							
				for(w=0;w<count;w++)
				{
					fscanf( fp, "%s", &s );
					pCMyApp->cell[cell_x][cell_z][w] = atoi(s);						
				}
				
				pCMyApp->cell_length[cell_x][cell_z] = count;
			}
		}
		
	} // end while		

	return FALSE;
}

BOOL CLoadWorld::LoadObjectData(HWND hwnd, char *filename)
{
	FILE *fp;    
	int i;
	int done		  = 0;
	int object_id	  = 0;
	int object_count  = 0;
	int old_object_id = 0;
	int vert_count	  = 0;
	int pv_count	  = 0;
	int poly_count	  = 0;
	int texture		  = 0;
	int conn_cnt      = 0;
	int num_v;
	BOOL command_error;
	float dat_scale;
	char buffer[256];
	char s[256];	
	char p[256];


    fp = fopen(filename,"r");

    if(fp==NULL)
	{     
		PrintMessage(hwnd, "ERROR can't load ", filename, SCN_AND_FILE);
		MessageBox(hwnd, filename, "Error can't load file",MB_OK);
		return FALSE;
    }

	PrintMessage(hwnd, "Loading ", filename, SCN_AND_FILE);
	
	while(done==0)
	{
		command_error = TRUE;

		fscanf( fp, "%s", &s );

		if( (strcmp(s,"OBJECT")==0) || (strcmp(s,"Q2M_OBJECT")==0))
			
		{
			dat_scale = 1.0; 

			old_object_id = object_id;

			fscanf( fp, "%s", &p );	// read object ID
			object_id = atoi(p);

			// find the highest object_id

			if(object_id > object_count)
				object_count = object_id;

			if((object_id < 0) || (object_id > 99))
			{
				MessageBox(hwnd,"Error Bad Object ID in: LoadObjectData",NULL,MB_OK);
				return FALSE;
			}

			pCMyApp->num_vert_per_object [old_object_id] = vert_count;
			pCMyApp->num_polys_per_object[old_object_id] = poly_count;
			vert_count=0;
			poly_count=0;
			conn_cnt=0;
			
			fscanf( fp, "%s", &p );	// read object name
			strcpy((char *)pCMyApp->obdata[object_id].name, (char *)&p);  
			command_error = FALSE;
		}

		if(strcmp(s,"SCALE")==0)
		{
			fscanf( fp, "%s", &p );
			dat_scale = (float)atof(p);
			command_error = FALSE;
		}

		if(strcmp(s,"TEXTURE")== 0)
		{
			fscanf( fp, "%s", &p );

			texture = pCMyApp->CheckValidTextureAlias(pCMyApp->Get_hWnd(), p);

			if(texture == -1)
			{
				PrintMessage(hwnd, "Error in objects.dat - Bad Texture ID", p, LOGFILE_ONLY);
				MessageBox(hwnd,"Error in objects.dat","Bad Texture ID",MB_OK);
				fclose(fp);	
				return FALSE;
			}
			command_error = FALSE;
		}

		if(strcmp(s,"TYPE")==0)
		{
			fscanf( fp, "%s", &p );
			command_error = FALSE;
		}

		if(strcmp(s,"TRI")==0)
		{
			for(i=0;i<3;i++)
			{
				ReadObDataVert(fp, object_id, vert_count, dat_scale);
				vert_count++;
			}
			
			pCMyApp->obdata[object_id].use_texmap[poly_count] = TRUE;
			pCMyApp->obdata[object_id].tex[poly_count]=texture;
			pCMyApp->obdata[object_id].num_vert[poly_count] = 3;
			pCMyApp->obdata[object_id].poly_cmd[poly_count] = D3DPT_TRIANGLELIST; //POLY_CMD_TRI;
			poly_count++;
			command_error = FALSE;
		}

		if(strcmp(s,"QUAD")==0)
		{
			ReadObDataVert(fp, object_id, vert_count,   dat_scale);
			ReadObDataVert(fp, object_id, vert_count+1, dat_scale);
			ReadObDataVert(fp, object_id, vert_count+3, dat_scale);
			ReadObDataVert(fp, object_id, vert_count+2, dat_scale);
			
			vert_count+=4;
			
			pCMyApp->obdata[object_id].use_texmap[poly_count] = TRUE;
			pCMyApp->obdata[object_id].tex[poly_count] = texture;
			pCMyApp->obdata[object_id].num_vert[poly_count] = 4;
			pCMyApp->obdata[object_id].poly_cmd[poly_count] = D3DPT_TRIANGLESTRIP; //POLY_CMD_QUAD;
			poly_count++;
			command_error = FALSE;
		}

		if(strcmp(s,"TRITEX")==0)
		{
			for(i=0;i<3;i++)
			{
				ReadObDataVertEx(fp, object_id, vert_count, dat_scale);
				vert_count++;
			}
			
			pCMyApp->obdata[object_id].use_texmap[poly_count] = FALSE;
			pCMyApp->obdata[object_id].tex[poly_count]=texture;
			pCMyApp->obdata[object_id].num_vert[poly_count] = 3;
			pCMyApp->obdata[object_id].poly_cmd[poly_count] = D3DPT_TRIANGLELIST;// POLY_CMD_TRI_TEX;
			poly_count++;
			command_error = FALSE;
		}

		if(strcmp(s,"QUADTEX")==0)
		{
			ReadObDataVertEx(fp, object_id, vert_count,   dat_scale);
			ReadObDataVertEx(fp, object_id, vert_count+1, dat_scale);
			ReadObDataVertEx(fp, object_id, vert_count+3, dat_scale);
			ReadObDataVertEx(fp, object_id, vert_count+2, dat_scale);
			
			vert_count+=4;
			
			pCMyApp->obdata[object_id].use_texmap[poly_count] = FALSE;
			pCMyApp->obdata[object_id].tex[poly_count] = texture;
			pCMyApp->obdata[object_id].num_vert[poly_count] = 4;
			pCMyApp->obdata[object_id].poly_cmd[poly_count] = D3DPT_TRIANGLESTRIP; //POLY_CMD_QUAD_TEX;
			poly_count++;
			command_error = FALSE;
		}

		if(strcmp(s,"TRI_STRIP")==0)
		{
			// Get numbers of verts in triangle strip
			fscanf( fp, "%s", &p );	
			num_v = atoi(p);

			for(i = 0; i < num_v; i++)
			{
				ReadObDataVertEx(fp, object_id, vert_count, dat_scale);
				vert_count++;
			}

			pCMyApp->obdata[object_id].use_texmap[poly_count] = TRUE;
			pCMyApp->obdata[object_id].tex[poly_count] = texture;
			pCMyApp->obdata[object_id].num_vert[poly_count] = num_v;
			pCMyApp->obdata[object_id].poly_cmd[poly_count] = D3DPT_TRIANGLESTRIP; //POLY_CMD_TRI_STRIP;
			poly_count++;
			command_error = FALSE;
		}

		if(strcmp(s,"TRI_FAN")==0)
		{
			// Get numbers of verts in triangle fan
			fscanf( fp, "%s", &p );	
			num_v = atoi(p);

			for(i = 0; i < num_v; i++)
			{
				ReadObDataVertEx(fp, object_id, vert_count, dat_scale);
				vert_count++;
			}

			pCMyApp->obdata[object_id].use_texmap[poly_count] = TRUE;
			pCMyApp->obdata[object_id].tex[poly_count] = texture;
			pCMyApp->obdata[object_id].num_vert[poly_count] = num_v;
			pCMyApp->obdata[object_id].poly_cmd[poly_count] = D3DPT_TRIANGLEFAN; //POLY_CMD_TRI_FAN;
			poly_count++;
			command_error = FALSE;
		}

		if(strcmp(s,"CONNECTION")==0)
		{
			if(conn_cnt<4)
			{
				fscanf( fp, "%s", &p );
				pCMyApp->obdata[object_id].connection[conn_cnt].x = (float)atof(p);
				fscanf( fp, "%s", &p );
				pCMyApp->obdata[object_id].connection[conn_cnt].y = (float)atof(p);
				fscanf( fp, "%s", &p );
				pCMyApp->obdata[object_id].connection[conn_cnt].z = (float)atof(p);
				conn_cnt++;
			}
			else
			{
				fscanf( fp, "%s", &p );
				fscanf( fp, "%s", &p );
				fscanf( fp, "%s", &p );
			}
			command_error = FALSE;
		}

		if(strcmp(s,"END_FILE")==0)
		{		
			pCMyApp->num_vert_per_object[object_id]=vert_count;
			pCMyApp->num_polys_per_object[object_id]=poly_count;
			pCMyApp->obdata_length = object_count+1;
			command_error = FALSE;
			done=1;
		}

		if(command_error == TRUE)
		{
			itoa(object_id, buffer, 10);
			PrintMessage(NULL, "CLoadWorld::LoadObjectData - ERROR in objects.dat, object : ", buffer, LOGFILE_ONLY);
			MessageBox(hwnd, s,"Unrecognised command",MB_OK);
			fclose(fp);	
			return FALSE;
		}
	}
	fclose(fp);	
	
	itoa(pCMyApp->obdata_length, buffer, 10);
	
	PrintMessage(hwnd, buffer, " DAT objects loaded" ,SCN_AND_FILE);
	PrintMessage(hwnd, "\n", NULL, LOGFILE_ONLY);
	
	return TRUE;
}

BOOL CLoadWorld::ReadObDataVertEx(FILE *fp, int object_id, int vert_count, float dat_scale)
{ 
	float x,y,z;
	char p[256];

	fscanf( fp, "%s", &p );
	x = dat_scale * (float)atof(p);
	pCMyApp->obdata[object_id].v[vert_count].x = x;
		
	fscanf( fp, "%s", &p );
	y = dat_scale * (float)atof(p);
	pCMyApp->obdata[object_id].v[vert_count].y = y;
		
	fscanf( fp, "%s", &p );
	z = dat_scale * (float)atof(p);
	pCMyApp->obdata[object_id].v[vert_count].z = z;


	fscanf( fp, "%s", &p );
	pCMyApp->obdata[object_id].t[vert_count].x = (float)atof(p);

	fscanf( fp, "%s", &p );
	pCMyApp->obdata[object_id].t[vert_count].y = (float)atof(p);
		
	return TRUE;
}

BOOL CLoadWorld::ReadObDataVert(FILE *fp, int object_id, int vert_count, float dat_scale)
{  
	float x,y,z;
	char p[256];

	fscanf( fp, "%s", &p );
	x = dat_scale * (float)atof(p);
	pCMyApp->obdata[object_id].v[vert_count].x = x;
		
	fscanf( fp, "%s", &p );
	y = dat_scale * (float)atof(p);
	pCMyApp->obdata[object_id].v[vert_count].y = y;
		
	fscanf( fp, "%s", &p );
	z = dat_scale * (float)atof(p);
	pCMyApp->obdata[object_id].v[vert_count].z = z;

	return TRUE;
}


BOOL CLoadWorld::LoadRRTextures(HWND hwnd, char *filename)
{    
	FILE *fp;    
	char s[256];
	char p[256];

	int y_count=30;
	int done=0;
	int object_count=0;
	int vert_count=0;
	int pv_count=0;
	int poly_count=0;
	int tex_alias_counter = 0;
	int tex_counter = 0;
	int i;
	BOOL start_flag=TRUE;
	BOOL found;
	LPDIRECTDRAWSURFACE7 lpDDsurface;


    fp = fopen(filename,"r");
    if(fp==NULL)
    {     
		PrintMessage(hwnd, "ERROR can't open ", filename, SCN_AND_FILE);
        MessageBox(hwnd, filename,"Error can't open",MB_OK);
		return FALSE;
    }

	PrintMessage(hwnd, "Loading ", filename, SCN_AND_FILE);
	PrintMessage(hwnd, "Scanning texture info", NULL, LOGFILE_ONLY);
	PrintMessage(hwnd, "\n", NULL, LOGFILE_ONLY);

	
	D3DTextr_InvalidateAllTextures();


	for(i = 0; i < 100; i++)
		pCMyApp->TexMap[i].is_alpha_texture = FALSE;

	for(i = 0; i < MAX_NUM_TEXTURES; i++) 
		pCMyApp->lpddsImagePtr[i] = NULL;

	pCMyApp->NumTextures = 0;


	while(done==0)
	{
		found = FALSE;
		fscanf( fp, "%s", &s );

		if(strcmp(s,"AddTexture")==0)
		{
			fscanf( fp, "%s", &p );
			PrintMessage(hwnd, "Loading ", p, LOGFILE_ONLY);	
			strcpy(pCMyApp->ImageFile[tex_counter], p);
			
			if(D3DTextr_CreateTextureFromFile( p ,0,0) != S_OK)
				PrintMessage(NULL,"CLoadWorld::LoadRRTextures() - Can't load texture ", p, LOGFILE_ONLY);
			
			tex_counter++;
			found = TRUE;		
		}

		if(strcmp(s,"Alias")==0)
		{
			fscanf( fp, "%s", &p );

			fscanf( fp, "%s", &p );
			strcpy((char *)pCMyApp->TexMap[tex_alias_counter].tex_alias_name, (char *)&p);  

			pCMyApp->TexMap[tex_alias_counter].texture = tex_counter - 1;
			
			fscanf( fp, "%s", &p );			
			if(strcmp(p,"AlphaTransparent")==0)
				pCMyApp->TexMap[tex_alias_counter].is_alpha_texture = TRUE;
			
			i = tex_alias_counter;
			fscanf( fp, "%s", &p ); 

			if(strcmp(p,"WHOLE")==0)
			{
				pCMyApp->TexMap[i].tu[0] = (float)0.0;
				pCMyApp->TexMap[i].tv[0] = (float)1.0;
				pCMyApp->TexMap[i].tu[1] = (float)0.0;
				pCMyApp->TexMap[i].tv[1] = (float)0.0;
				pCMyApp->TexMap[i].tu[2] = (float)1.0;
				pCMyApp->TexMap[i].tv[2] = (float)1.0;
				pCMyApp->TexMap[i].tu[3] = (float)1.0;
				pCMyApp->TexMap[i].tv[3] = (float)0.0;

			}

			if(strcmp(p,"TL_QUAD")==0)
			{
				pCMyApp->TexMap[i].tu[0] = (float)0.0;
				pCMyApp->TexMap[i].tv[0] = (float)0.5;
				pCMyApp->TexMap[i].tu[1] = (float)0.0;
				pCMyApp->TexMap[i].tv[1] = (float)0.0;
				pCMyApp->TexMap[i].tu[2] = (float)0.5;
				pCMyApp->TexMap[i].tv[2] = (float)0.5;
				pCMyApp->TexMap[i].tu[3] = (float)0.5;
				pCMyApp->TexMap[i].tv[3] = (float)0.0;
			}

			if(strcmp(p,"TR_QUAD")==0)
			{
				pCMyApp->TexMap[i].tu[0] = (float)0.5;
				pCMyApp->TexMap[i].tv[0] = (float)0.5;
				pCMyApp->TexMap[i].tu[1] = (float)0.5;
				pCMyApp->TexMap[i].tv[1] = (float)0.0;
				pCMyApp->TexMap[i].tu[2] = (float)1.0;
				pCMyApp->TexMap[i].tv[2] = (float)0.5;
				pCMyApp->TexMap[i].tu[3] = (float)1.0;
				pCMyApp->TexMap[i].tv[3] = (float)0.0;
			}
			if(strcmp(p,"LL_QUAD")==0)
			{
				pCMyApp->TexMap[i].tu[0] = (float)0.0;
				pCMyApp->TexMap[i].tv[0] = (float)1.0;
				pCMyApp->TexMap[i].tu[1] = (float)0.0;
				pCMyApp->TexMap[i].tv[1] = (float)0.5;
				pCMyApp->TexMap[i].tu[2] = (float)0.5;
				pCMyApp->TexMap[i].tv[2] = (float)1.0;
				pCMyApp->TexMap[i].tu[3] = (float)0.5;
				pCMyApp->TexMap[i].tv[3] = (float)0.5;
			}
			if(strcmp(p,"LR_QUAD")==0)
			{
				pCMyApp->TexMap[i].tu[0] = (float)0.5;
				pCMyApp->TexMap[i].tv[0] = (float)1.0;
				pCMyApp->TexMap[i].tu[1] = (float)0.5;
				pCMyApp->TexMap[i].tv[1] = (float)0.5;
				pCMyApp->TexMap[i].tu[2] = (float)1.0;
				pCMyApp->TexMap[i].tv[2] = (float)1.0;
				pCMyApp->TexMap[i].tu[3] = (float)1.0;
				pCMyApp->TexMap[i].tv[3] = (float)0.5;
			}
			if(strcmp(p,"TOP_HALF")==0)
			{
				pCMyApp->TexMap[i].tu[0] = (float)0.0;
				pCMyApp->TexMap[i].tv[0] = (float)0.5;
				pCMyApp->TexMap[i].tu[1] = (float)0.0;
				pCMyApp->TexMap[i].tv[1] = (float)0.0;
				pCMyApp->TexMap[i].tu[2] = (float)1.0;
				pCMyApp->TexMap[i].tv[2] = (float)0.5;
				pCMyApp->TexMap[i].tu[3] = (float)1.0;
				pCMyApp->TexMap[i].tv[3] = (float)0.0;
			}
			if(strcmp(p,"BOT_HALF")==0)
			{
				pCMyApp->TexMap[i].tu[0] = (float)0.0;
				pCMyApp->TexMap[i].tv[0] = (float)1.0;
				pCMyApp->TexMap[i].tu[1] = (float)0.0;
				pCMyApp->TexMap[i].tv[1] = (float)0.5;
				pCMyApp->TexMap[i].tu[2] = (float)1.0;
				pCMyApp->TexMap[i].tv[2] = (float)1.0;
				pCMyApp->TexMap[i].tu[3] = (float)1.0;
				pCMyApp->TexMap[i].tv[3] = (float)0.5;
			}
			if(strcmp(p,"LEFT_HALF")==0)
			{
				pCMyApp->TexMap[i].tu[0] = (float)0.0;
				pCMyApp->TexMap[i].tv[0] = (float)1.0;
				pCMyApp->TexMap[i].tu[1] = (float)0.0;
				pCMyApp->TexMap[i].tv[1] = (float)0.0;
				pCMyApp->TexMap[i].tu[2] = (float)0.5;
				pCMyApp->TexMap[i].tv[2] = (float)1.0;
				pCMyApp->TexMap[i].tu[3] = (float)0.5;
				pCMyApp->TexMap[i].tv[3] = (float)0.0;
			}
			if(strcmp(p,"RIGHT_HALF")==0)
			{
				pCMyApp->TexMap[i].tu[0] = (float)0.5;
				pCMyApp->TexMap[i].tv[0] = (float)1.0;
				pCMyApp->TexMap[i].tu[1] = (float)0.5;
				pCMyApp->TexMap[i].tv[1] = (float)0.0;
				pCMyApp->TexMap[i].tu[2] = (float)1.0;
				pCMyApp->TexMap[i].tv[2] = (float)1.0;
				pCMyApp->TexMap[i].tu[3] = (float)1.0;
				pCMyApp->TexMap[i].tv[3] = (float)0.0;
			}
			if(strcmp(p,"TL_TRI")==0)
			{
				pCMyApp->TexMap[i].tu[0] = (float)0.0;
				pCMyApp->TexMap[i].tv[0] = (float)0.0;
				pCMyApp->TexMap[i].tu[1] = (float)1.0;
				pCMyApp->TexMap[i].tv[1] = (float)0.0;
				pCMyApp->TexMap[i].tu[2] = (float)0.0;
				pCMyApp->TexMap[i].tv[2] = (float)1.0;
			}
			if(strcmp(p,"BR_TRI")==0)
			{

			}
			
			tex_alias_counter++;
			found = TRUE;
		}

		if(strcmp(s,"END_FILE")==0)
		{
			PrintMessage(hwnd, "\n", NULL, LOGFILE_ONLY);
			pCMyApp->number_of_tex_aliases = tex_alias_counter;
			pCMyApp->NumTextures = tex_counter;
			found = TRUE;
			done = 1;
		}

		if(found == FALSE)
		{
			PrintMessage(hwnd, "File Error: Syntax problem :", p, SCN_AND_FILE);
			MessageBox(hwnd, "p","File Error: Syntax problem ",MB_OK);
			return FALSE;
		}
			
		
	}
	fclose(fp);	
	
	D3DTextr_RestoreAllTextures( pCMyApp->GetDevice() );

	for(i = 0; i < pCMyApp->NumTextures; i++)
	{
		lpDDsurface	= D3DTextr_GetSurface( pCMyApp->ImageFile[i] );
		pCMyApp->lpddsImagePtr[i] = lpDDsurface;
	} 
	PrintMessage(hwnd, "CMyD3DApplication::LoadRRTextures - suceeded", NULL, SCN_AND_FILE);
			
	return TRUE;
}

BOOL CLoadWorld::LoadImportedModelList(HWND hwnd, char *filename)
{
	FILE *fp;    
	char p[256];
	int done = 0;
	int i;
	char command[256];
	float f;
	int model_id;
	//char model_name[256];
	char model_filename[256];
	int model_tex_alias;
	float scale;
	int  Q2M_Anim_Counter = 0;
	BOOL command_recognised;	
	BOOL Model_loaded_flags[1000] = {FALSE};

	pCMyApp->num_players   = 0;
	pCMyApp->num_your_guns = 0;
	pCMyApp->num_op_guns   = 0;

	for(i = 0; i < 10; i++)
	{
		pCMyApp->debug[i].current_sequence = 0;
		pCMyApp->debug[i].current_frame = 0;
	}

	fp = fopen(filename,"r");

    if(fp==NULL)
	{     
		PrintMessage(hwnd, "ERROR can't load ", filename, SCN_AND_FILE);
		MessageBox(hwnd, filename, "Error can't load file",MB_OK);
		return FALSE;
    }

	PrintMessage(hwnd, "Loading ", filename, SCN_AND_FILE);

	fscanf( fp, "%s", &command );

	if(strcmp(command,"FILENAME") == 0)
		fscanf( fp, "%s", &p );	// ignore comment
	else
		return FALSE;

	while(done == 0)
	{
		command_recognised = FALSE;
		scale = (float)0.4;

		fscanf( fp, "%s", &command ); // get next command

		
		fscanf( fp, "%s", &p );	// read player number
		type_num = atoi(p);
			
		fscanf( fp, "%s", &p );	// find model file foramt, MD2 or 3DS ?
		
		if(strcmp(p,"MD2") == 0)
			pCMyApp->model_format = MD2_MODEL;
			
		if(strcmp(p,"3DS") == 0)
			pCMyApp->model_format = k3DS_MODEL;

		fscanf( fp, "%s", &p );	//  ignore comment "model_id"  

		fscanf( fp, "%s", &p );	// read model id
		model_id = atoi(p);
			
		fscanf( fp, "%s", &model_filename ); // read filename for object

		fscanf( fp, "%s", &p );	//  ignore comment "tex"  

		fscanf( fp, "%s", &p );	// read texture alias id				
		model_tex_alias = atoi(p);		


		if(strcmp(command,"PLAYER") == 0)
		{
			fscanf( fp, "%s", &p ); // ignore comment pos
					
			fscanf( fp, "%s", &p ); // x pos
			pCMyApp->player_list[type_num].x = (float)atoi(p);
					
			fscanf( fp, "%s", &p ); // y pos
			pCMyApp->player_list[type_num].y = (float)atoi(p);
					
			fscanf( fp, "%s", &p ); // z pos
			pCMyApp->player_list[type_num].z = (float)atoi(p);
					
			fscanf( fp, "%s", &p ); // ignore comment angle
			fscanf( fp, "%s", &p ); // angle
			pCMyApp->player_list[type_num].rot_angle = (float)atoi(p);

			pCMyApp->player_list[type_num].model_id = model_id;
			pCMyApp->player_list[type_num].skin_tex_id = model_tex_alias;
			pCMyApp->num_players++;

			fscanf( fp, "%s", &p ); // ignore comment scale
			fscanf( fp, "%f", &f );
			scale = f;

			fscanf( fp, "%s", &p ); // Don't draw players external weapon
			if(strcmp(p, "NO_EXTERNAL_WEP") == 0)
				pCMyApp->player_list[type_num].draw_external_wep = FALSE;
			
			if(strcmp(p, "USE_EXTERNAL_WEP") == 0)
				pCMyApp->player_list[type_num].draw_external_wep = TRUE;
			

			player_count++;
			
			LoadPlayerAnimationSequenceList(model_id);
			LoadDebugAnimationSequenceList(hwnd, model_filename, model_id);
			command_recognised = TRUE;
		}

		if(strcmp(command,"CAR") == 0)
		{
			fscanf( fp, "%s", &p ); // ignore comment pos
					
			fscanf( fp, "%s", &p ); // x pos
			pCMyApp->car_list[type_num].x = (float)atoi(p);
					
			fscanf( fp, "%s", &p ); // y pos
			pCMyApp->car_list[type_num].y = (float)atoi(p);
					
			fscanf( fp, "%s", &p ); // z pos
			pCMyApp->car_list[type_num].z = (float)atoi(p);
					
			fscanf( fp, "%s", &p ); // ignore comment angle
			fscanf( fp, "%s", &p ); // angle
			pCMyApp->car_list[type_num].rot_angle = (float)atoi(p);

			pCMyApp->car_list[type_num].model_id = model_id;
			pCMyApp->car_list[type_num].skin_tex_id = model_tex_alias;
			pCMyApp->num_cars++;

			fscanf( fp, "%s", &p ); // ignore comment scale
			fscanf( fp, "%f", &f );
			scale = f;

			car_count++;
			command_recognised = TRUE;
		}

		if(strcmp(command,"YOURGUN") == 0)
		{
			fscanf( fp, "%s", &p ); // ignore comment pos
				
			fscanf( fp, "%s", &p ); // x pos
			pCMyApp->your_gun[type_num].x_offset = (float)atoi(p);
					
			fscanf( fp, "%s", &p ); // y pos
			pCMyApp->your_gun[type_num].z_offset = (float)atoi(p);
				
			fscanf( fp, "%s", &p ); // z pos
			pCMyApp->your_gun[type_num].y_offset = (float)atoi(p);
					
			fscanf( fp, "%s", &p ); // ignore comment angle
			fscanf( fp, "%s", &p ); // angle
			pCMyApp->your_gun[type_num].rot_angle = (float)atoi(p);

			pCMyApp->your_gun[type_num].model_id = model_id;
			pCMyApp->your_gun[type_num].skin_tex_id = model_tex_alias;
			pCMyApp->num_your_guns++;

			fscanf( fp, "%s", &p ); // ignore comment scale
			fscanf( fp, "%f", &f );
			scale = f;

			your_gun_count++;
			LoadYourGunAnimationSequenceList(model_id);
			//LoadDebugAnimationSequenceList(hwnd, model_filename, model_id, wptr);
			command_recognised = TRUE;
		}

		if(strcmp(command,"DEBUG") == 0)
		{
			fscanf( fp, "%s", &p ); // ignore comment pos
				
			fscanf( fp, "%s", &p ); // x pos
			pCMyApp->debug[type_num].x = (float)atoi(p);
					
			fscanf( fp, "%s", &p ); // y pos
			pCMyApp->debug[type_num].y = (float)atoi(p);
				
			fscanf( fp, "%s", &p ); // z pos
			pCMyApp->debug[type_num].z = (float)atoi(p);
					
			fscanf( fp, "%s", &p ); // ignore comment angle
			fscanf( fp, "%s", &p ); // angle
			pCMyApp->debug[type_num].rot_angle = (float)atoi(p);

			pCMyApp->debug[type_num].model_id = model_id;
			pCMyApp->debug[type_num].skin_tex_id = model_tex_alias;
			pCMyApp->num_debug_models++;
			
			fscanf( fp, "%s", &p ); // ignore comment scale
			fscanf( fp, "%f", &f );
			scale = f;

			LoadDebugAnimationSequenceList(hwnd, model_filename, model_id);

			command_recognised = TRUE;
		}

		if(strcmp(command,"OPGUN") == 0)
		{			
			pCMyApp->other_players_guns[type_num].model_id = model_id;
			pCMyApp->other_players_guns[type_num].skin_tex_id = model_tex_alias;
			pCMyApp->num_op_guns++;
			
			fscanf( fp, "%s", &p ); // ignore comment scale
			fscanf( fp, "%f", &f );
			scale = f;

			op_gun_count++;
			command_recognised = TRUE;
		}
		
		if(strcmp(command,"END_FILE") == 0)
		{
			done = 1;
			command_recognised = TRUE;
		}

		if(command_recognised == TRUE)
		{
			if(Model_loaded_flags[model_id] == FALSE)
			{
				PrintMessage(hwnd, "loading  ", model_filename, SCN_AND_FILE);
				
				if(pCMyApp->model_format == MD2_MODEL)
					pCMD2->ImportMD2_GLCMD(hwnd, model_filename,  model_tex_alias, model_id, scale);
			
				if(pCMyApp->model_format == k3DS_MODEL)
					pC3DS->Import3DS(hwnd, model_filename, model_id, scale);
			
				Model_loaded_flags[model_id] = TRUE;
				Q2M_Anim_Counter++;
			}
		}
		else
		{
			PrintMessage(hwnd, "command unrecognised ", command, SCN_AND_FILE);
			MessageBox(hwnd, command, "command unrecognised",MB_OK);
			return FALSE;
		}
		

	} // end while loop

	num_imported_models_loaded = player_count + your_gun_count + op_gun_count;
	PrintMessage(hwnd, "\n", NULL, LOGFILE_ONLY);
	fclose(fp);	
	return TRUE;
}

void CLoadWorld::LoadDebugAnimationSequenceList(HWND hwnd, char *filename, int model_id)
{
	FILE *fp;    
	int done = 0;
	int start_frame;
	int stop_frame;
	int sequence_number;
	int file_ex_start;
	int i;
	char command[256];
	char p[256];
	char name[256];

	BOOL command_recognised;	


	strcpy(g_model_filename, filename);

	for (i = 0; i < 255; i++)
	{
		if(g_model_filename[i] == '.')
		{
			if(g_model_filename[i+1] == '.')
			{
				i++;
			}
			else
			{
				file_ex_start = i;
				break;
			}
		}
	}

	strcpy(&g_model_filename[file_ex_start+1], "seq");

	fp = fopen(g_model_filename,"r");

    if(fp==NULL)
	{     
		PrintMessage(hwnd, "ERROR can't load ", g_model_filename, SCN_AND_FILE);
		MessageBox(hwnd, g_model_filename, "Error can't load file",MB_OK);
		return;
    }

	PrintMessage(hwnd, "Loading ", g_model_filename, SCN_AND_FILE);

	fscanf( fp, "%s", &command );

	if(strcmp(command,"FILENAME") == 0)
		fscanf( fp, "%s", &p );	// ignore comment
	else
		return;

	// SEQUENCE 0  START_FRAME   0  STOP_FRAME  39 NAME Stand
	// SEQUENCE 1  START_FRAME  40  STOP_FRAME  45 NAME Run
	// SEQUENCE 2  START_FRAME  46  STOP_FRAME  53 NAME Attack

	while(done == 0)
	{
		command_recognised = FALSE;
	
	
		// read sequence number
		fscanf( fp, "%s", &command ); 		
		if(strcmpi(command, "SEQUENCE") == 0)
		{	
			fscanf( fp, "%d", &sequence_number );


			// read start frame
			fscanf( fp, "%s", &p );
			if(strcmpi(p, "START_FRAME") != 0)
				return;

			fscanf( fp, "%d", &start_frame );	
			pCMyApp->pmdata[model_id].sequence_start_frame[sequence_number] = start_frame;
			

			// read stop frame
			fscanf( fp, "%s", &p );
			if(strcmpi(p, "STOP_FRAME") != 0)
				return;

			fscanf( fp, "%d", &stop_frame );	
			pCMyApp->pmdata[model_id].sequence_stop_frame[sequence_number] = stop_frame;

			// read name
			fscanf( fp, "%s", &p );
			if(strcmpi(p, "NAME") != 0)
				return;

			fscanf( fp, "%s", &name );	
			
		}

		if(strcmpi(command, "END_FILE") == 0)
			done = 1;		
		
	} // end while loop

	fclose(fp);	
}


void CLoadWorld::LoadPlayerAnimationSequenceList(int model_id)
{
	int i;

	i = model_id;

	pCMyApp->pmdata[i].sequence_start_frame[0] =0;  // Stand
	pCMyApp->pmdata[i].sequence_stop_frame [0] =39;

	pCMyApp->pmdata[i].sequence_start_frame[1] =40; // run
	pCMyApp->pmdata[i].sequence_stop_frame [1] =45;

	pCMyApp->pmdata[i].sequence_start_frame[2] =46; // attack
	pCMyApp->pmdata[i].sequence_stop_frame [2] =53;

	pCMyApp->pmdata[i].sequence_start_frame[3] =54; // pain1
	pCMyApp->pmdata[i].sequence_stop_frame [3] =57;

	pCMyApp->pmdata[i].sequence_start_frame[4] =58; // pain2
	pCMyApp->pmdata[i].sequence_stop_frame [4] =61;

	pCMyApp->pmdata[i].sequence_start_frame[5] =62; // pain3
	pCMyApp->pmdata[i].sequence_stop_frame [5] =65;

	pCMyApp->pmdata[i].sequence_start_frame[6] =66; // jump
	pCMyApp->pmdata[i].sequence_stop_frame [6] =71;

	pCMyApp->pmdata[i].sequence_start_frame[7] =72; // flip
	pCMyApp->pmdata[i].sequence_stop_frame [7] =83;

	pCMyApp->pmdata[i].sequence_start_frame[8] =84; // Salute
	pCMyApp->pmdata[i].sequence_stop_frame [8] =94;

	pCMyApp->pmdata[i].sequence_start_frame[9] =95; // Taunt
	pCMyApp->pmdata[i].sequence_stop_frame [9] =111;

	pCMyApp->pmdata[i].sequence_start_frame[10] =112; // Wave
	pCMyApp->pmdata[i].sequence_stop_frame [10] =122;

	pCMyApp->pmdata[i].sequence_start_frame[11] =123; // Point
	pCMyApp->pmdata[i].sequence_stop_frame [11] =134;
	
}

void CLoadWorld::LoadYourGunAnimationSequenceList(int model_id)
{
	int i;

	i = model_id;

	pCMyApp->your_gun[0].current_sequence = 2;
	pCMyApp->your_gun[0].current_frame = 13;
	pCMyApp->your_gun[1].current_sequence = 2;
	pCMyApp->your_gun[1].current_frame = 13;
	pCMyApp->your_gun[2].current_sequence = 0;
	pCMyApp->your_gun[2].current_frame = 0;
		
	pCMyApp->pmdata[i].sequence_start_frame[0] =0;  // Active1
	pCMyApp->pmdata[i].sequence_stop_frame [0] =10;

	pCMyApp->pmdata[i].sequence_start_frame[1] =11; // fire
	pCMyApp->pmdata[i].sequence_stop_frame [1] =12;

	pCMyApp->pmdata[i].sequence_start_frame[2] =13; // Idle
	pCMyApp->pmdata[i].sequence_stop_frame [2] =39;

	pCMyApp->pmdata[i].sequence_start_frame[3] =40; // putaway
	pCMyApp->pmdata[i].sequence_stop_frame [3] =44;

	pCMyApp->pmdata[i].sequence_start_frame[4] =45; // reload
	pCMyApp->pmdata[i].sequence_stop_frame [4] =63;

	pCMyApp->pmdata[i].sequence_start_frame[5] =64; // burstfire
	pCMyApp->pmdata[i].sequence_stop_frame [5] =69;

	pCMyApp->pmdata[i].sequence_start_frame[6] =70; // lastround
	pCMyApp->pmdata[i].sequence_stop_frame [6] =71;

}


