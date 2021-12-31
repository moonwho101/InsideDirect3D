
// D3D Road Rage version 1.1
// Written by William Chin
// Developed in MSDEV C++ v4.2 with DirectX 5 SDK
// Date : 3/10/98
// File : World.cpp

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <winuser.h>			
#include "world.h"
#include "models.h"
#include "d3dmain.h"
#include <mbstring.h>
#include <tchar.h>
#include "texture.h"
#include <io.h>
#include <fcntl.h>
#include "editor.h"



#define MAX_NUM_OBJECTS_PER_CELL 50


extern LINK LinksList[MAX_NUM_LINKS];
extern current_link;
extern last_link;

SETUPINFO setupinfo;
HDC hdc;

int PrintMsgX = 10;
int PrintMsgY = 10;
BOOL logfile_start_flag = TRUE;



BOOL LoadObjectData(HWND hwnd, char *filename, world_ptr wptr)
{
	FILE *fp;  
	BOOL command_error;
	char s[100];
	char p[100];
	char buffer[100];
	int y_count=30;
	int done=0;
	int object_id = 0;
	int object_count = 0;
	int old_object_id = 0;
	int vert_count = 0;
	int pv_count = 0;
	int poly_count = 0;
	int texture=0;
	int i;
	int conn_cnt=0;
	int num_v;
	int memory_counter = 0;
	int  Q2M_counter = 0;
	int  Q2M_Anim_Counter = 0;
	float dat_scale = 1.0000;
	float x,y,z;
	VERT		 v[2000];
	VERT		 t[2000];
	short      tex[2000];
	short num_vert[2000];
	short poly_cmd[2000];
		


    fp = fopen(filename,"r");

    if(fp==NULL)
	{     
		PrintMessage(hwnd, "ERROR can't load ", filename, SCN_AND_LOGFILE);
		MessageBox(hwnd, filename, "Error can't load file",MB_OK);
		return FALSE;
    }

	PrintMessage(hwnd, "Loading ", filename, SCN_AND_LOGFILE);
	
	
	Q2M_counter = 0;
	Q2M_Anim_Counter = 0;

	while(done == 0)
	{
		fscanf( fp, "%s", &s );

		command_error = TRUE;

		if(strcmp(s,"OBJECT") == 0) 	
		{
			dat_scale = 1.0000;
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

			PrintMessage(hwnd, "loading dat object : ", p, LOGFILE_ONLY);

			wptr->num_vert_per_object [old_object_id] = vert_count;
			wptr->num_polys_per_object[old_object_id] = poly_count;

			if(vert_count != 0)
			{
				wptr->obdata[old_object_id].v = new VERT[vert_count];
				wptr->obdata[old_object_id].t = new VERT[vert_count];

				memory_counter += (sizeof(VERT) * vert_count *2);

				for(i = 0; i < vert_count; i++)
				{
					wptr->obdata[old_object_id].v[i].x = v[i].x;
					wptr->obdata[old_object_id].v[i].y = v[i].y;
					wptr->obdata[old_object_id].v[i].z = v[i].z;

					wptr->obdata[old_object_id].t[i].x = t[i].x;
					wptr->obdata[old_object_id].t[i].y = t[i].y;
				}
			}


			if(poly_count != 0)
			{
				wptr->obdata[old_object_id].tex		 = new short[poly_count];
				wptr->obdata[old_object_id].num_vert = new short[poly_count];
				wptr->obdata[old_object_id].poly_cmd = new short[poly_count];

				memory_counter += (sizeof(short) * poly_count * 3);

				for(i = 0; i < poly_count; i++)
				{
					wptr->obdata[old_object_id].tex[i]		= tex[i];
					wptr->obdata[old_object_id].num_vert[i] = num_vert[i];
					wptr->obdata[old_object_id].poly_cmd[i] = poly_cmd[i];
				}
			}

			vert_count = 0;
			poly_count = 0;
			conn_cnt = 0;
			
			fscanf( fp, "%s", &p );	// read object name
			strcpy((char *)wptr->obdata[object_id].name, (char *)&p);  
			
			command_error = FALSE;
		}


		if(strcmp(s,"SCALE")==0)
		{
			fscanf( fp, "%s", &p );
			dat_scale = (float)atof(p);
			command_error = FALSE;	
		}

		if(strcmp(s,"TEXTURE")==0)
		{
			fscanf( fp, "%s", &p );

			texture = CheckValidTextureAlias(hwnd, p, wptr);

			if(texture == -1)
			{
				MessageBox(hwnd,"Function : LoadObjectData","Error Bad Texture ID",MB_OK);
				MessageBox(hwnd,p,"Error Bad Texture ID",MB_OK);
				PrintMessage(hwnd, "Error Bad Texture ID", p, LOGFILE_ONLY);

				return FALSE;
			}
			command_error = FALSE;
		}

		if(strcmp(s,"TYPE")==0)
		{
			fscanf( fp, "%s", &p );
			command_error = FALSE;
		}

		if(strcmp(s,"TRI_STRIP")==0)
		{
			// Get numbers of verts in triangle fan
			fscanf( fp, "%s", &p );	
			num_v = atoi(p);

			for(i = 0; i < num_v; i++)
			{
				// vert
				fscanf( fp, "%s", &p );
				v[vert_count].x = (float)atof(p);
				
				fscanf( fp, "%s", &p );
				v[vert_count].y = (float)atof(p);

				fscanf( fp, "%s", &p );
				v[vert_count].z = (float)atof(p);

				// texture co-ordinate
				fscanf( fp, "%s", &p );
				t[vert_count].x = (float)atof(p);

				fscanf( fp, "%s", &p );
				t[vert_count].y = (float)atof(p);
				
				vert_count++;
			}

			tex[poly_count] = texture;
			num_vert[poly_count] = num_v;
			poly_cmd[poly_count] = POLY_CMD_TRI_STRIP;

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
				// vert
				fscanf( fp, "%s", &p );
				v[vert_count].x = (float)atof(p);

				fscanf( fp, "%s", &p );
				v[vert_count].y = (float)atof(p);

				fscanf( fp, "%s", &p );
				v[vert_count].z = (float)atof(p);

				// texture co-ordinate
				fscanf( fp, "%s", &p );
				t[vert_count].x = (float)atof(p);

				fscanf( fp, "%s", &p );
				t[vert_count].y = (float)atof(p);
				
				vert_count++;
			}

			tex[poly_count] = texture;
			num_vert[poly_count] = num_v;
			poly_cmd[poly_count] = POLY_CMD_TRI_FAN;

			poly_count++;
			command_error = FALSE;
		}

		if(strcmp(s,"QUAD")==0)
		{
			
			fscanf( fp, "%s", &p );
			v[vert_count].x = (float)atof(p);
		
			fscanf( fp, "%s", &p );
			v[vert_count].y = (float)atof(p);
		
			fscanf( fp, "%s", &p );
			v[vert_count].z = (float)atof(p);
		

			fscanf( fp, "%s", &p );
			v[vert_count+1].x = (float)atof(p);
		
			fscanf( fp, "%s", &p );
			v[vert_count+1].y = (float)atof(p);
		
			fscanf( fp, "%s", &p );
			v[vert_count+1].z = (float)atof(p);


			fscanf( fp, "%s", &p );
			v[vert_count+3].x = (float)atof(p);
		
			fscanf( fp, "%s", &p );
			v[vert_count+3].y = (float)atof(p);
		
			fscanf( fp, "%s", &p );
			v[vert_count+3].z = (float)atof(p);


			fscanf( fp, "%s", &p );
			v[vert_count+2].x = (float)atof(p);
		
			fscanf( fp, "%s", &p );
			v[vert_count+2].y = (float)atof(p);
		
			fscanf( fp, "%s", &p );
			v[vert_count+2].z = (float)atof(p);
				vert_count+=4;
			
			
			tex[poly_count] = texture;
			num_vert[poly_count] = 4;
			poly_cmd[poly_count] = POLY_CMD_QUAD;

			poly_count++;
			command_error = FALSE;
		}
		
		if(strcmp(s,"QUADTEX")==0)
		{
			
			fscanf( fp, "%s", &p );
			x = dat_scale * (float)atof(p);
			v[vert_count].x = x;
		
			fscanf( fp, "%s", &p );
			y = dat_scale * (float)atof(p);
			v[vert_count].y = y;
		
			fscanf( fp, "%s", &p );
			z = dat_scale * (float)atof(p);
			v[vert_count].z = z;

			fscanf( fp, "%s", &p );
			t[vert_count].x = (float)atof(p);

			fscanf( fp, "%s", &p );
			t[vert_count].y = (float)atof(p);
		


			fscanf( fp, "%s", &p );
			x = dat_scale * (float)atof(p);
			v[vert_count+1].x = x;
		
			fscanf( fp, "%s", &p );
			y = dat_scale * (float)atof(p);
			v[vert_count+1].y = y;
		
			fscanf( fp, "%s", &p );
			z = dat_scale * (float)atof(p);
			v[vert_count+1].z = z;
			
			fscanf( fp, "%s", &p );
			t[vert_count+1].x = (float)atof(p);

			fscanf( fp, "%s", &p );
			t[vert_count+1].y = (float)atof(p);



			fscanf( fp, "%s", &p );
			x = dat_scale * (float)atof(p);
			v[vert_count+3].x = x;
		
			fscanf( fp, "%s", &p );
			y = dat_scale * (float)atof(p);
			v[vert_count+3].y = y;
		
			fscanf( fp, "%s", &p );
			z = dat_scale * (float)atof(p);
			v[vert_count+3].z = z;

			fscanf( fp, "%s", &p );
			t[vert_count+3].x = (float)atof(p);

			fscanf( fp, "%s", &p );
			t[vert_count+3].y = (float)atof(p);



			fscanf( fp, "%s", &p );
			x = dat_scale * (float)atof(p);
			v[vert_count+2].x = x;
		
			fscanf( fp, "%s", &p );
			y = dat_scale * (float)atof(p);
			v[vert_count+2].y = y;
		
			fscanf( fp, "%s", &p );
			z = dat_scale * (float)atof(p);
			v[vert_count+2].z = z;

			fscanf( fp, "%s", &p );
			t[vert_count+2].x = (float)atof(p);

			fscanf( fp, "%s", &p );
			t[vert_count+2].y = (float)atof(p);


			vert_count+=4;
			
			
			tex[poly_count] = texture;
			num_vert[poly_count] = 4;
			poly_cmd[poly_count] = POLY_CMD_QUAD_TEX;

			poly_count++;
			command_error = FALSE;
		}

		if(strcmp(s,"TRI")==0)
		{
			for(i=0;i<3;i++)
			{
				fscanf( fp, "%s", &p );
				v[vert_count].x = (float)atof(p);
		
				fscanf( fp, "%s", &p );
				v[vert_count].y = (float)atof(p);
		
				fscanf( fp, "%s", &p );
				v[vert_count].z = (float)atof(p);
		
				vert_count++;
			}
			
			tex[poly_count]=texture;
			num_vert[poly_count] = 3;
			poly_cmd[poly_count] = POLY_CMD_TRI;

			poly_count++;
			command_error = FALSE;
		}

		if(strcmp(s,"CONNECTION")==0)
		{
			if(conn_cnt<4)
			{
				fscanf( fp, "%s", &p );
				wptr->obdata[object_id].connection[conn_cnt].x = (float)atof(p);
				fscanf( fp, "%s", &p );
				wptr->obdata[object_id].connection[conn_cnt].y = (float)atof(p);
				fscanf( fp, "%s", &p );
				wptr->obdata[object_id].connection[conn_cnt].z = (float)atof(p);
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
			wptr->num_vert_per_object[object_id]  = vert_count;
			wptr->num_polys_per_object[object_id] = poly_count;

			if(vert_count != 0)
			{
				wptr->obdata[object_id].v = new VERT[vert_count];
				wptr->obdata[object_id].t = new VERT[vert_count];

				memory_counter += (sizeof(VERT) * vert_count *2);

				for(i = 0; i < vert_count; i++)
				{
					wptr->obdata[object_id].v[i].x = v[i].x;
					wptr->obdata[object_id].v[i].y = v[i].y;
					wptr->obdata[object_id].v[i].z = v[i].z;

					wptr->obdata[object_id].t[i].x = t[i].x;
					wptr->obdata[object_id].t[i].y = t[i].y;
				}
			}


			if(poly_count != 0)
			{
				wptr->obdata[object_id].tex		 = new short[poly_count];
				wptr->obdata[object_id].num_vert = new short[poly_count];
				wptr->obdata[object_id].poly_cmd = new short[poly_count];

				memory_counter += (sizeof(short) * poly_count * 3);

				for(i = 0; i < poly_count; i++)
				{
					wptr->obdata[object_id].tex[i]		= tex[i];
					wptr->obdata[object_id].num_vert[i] = num_vert[i];
					wptr->obdata[object_id].poly_cmd[i] = poly_cmd[i];
				}
			}

			wptr->obdata_length = object_count+1;
			done=1;
			command_error = FALSE;
		}

		if(command_error == TRUE)
		{
			PrintMessage(hwnd, buffer, " Unrecognised command" ,SCN_AND_LOGFILE);
			return FALSE;
		}
	}
	fclose(fp);	
	
	itoa(wptr->obdata_length, buffer, 10);	
	PrintMessage(hwnd, buffer, " DAT objects loaded" ,SCN_AND_LOGFILE);

	memory_counter = memory_counter / 1024;
	itoa(memory_counter, buffer, 10);
	strcat(buffer, " KB\n");
	PrintMessage(hwnd, "memory allocated for DAT objects = ", buffer, LOGFILE_ONLY);
	
	return TRUE;
}
/*
BOOL LoadObjectData(HWND hwnd, char *filename, world_ptr wptr)
{
	FILE *fp;    
	char s[100];
	char p[100];
	char buffer[100];
	int y_count=30;
	int done=0;
	int object_id = 0;
	int object_count = 0;
	int old_object_id = 0;
	int vert_count=0;
	int pv_count=0;
	int poly_count=0;
	int texture=0;
	int i;
	int conn_cnt=0;
	int num_v;
	int  Q2M_counter = 0;
	int  Q2M_Anim_Counter = 0;
	float dat_scale = 1.0000;
	float x,y,z;

    fp = fopen(filename,"r");

    if(fp==NULL)
	{     
		PrintMessage(hwnd, "ERROR can't load ", filename, SCN_AND_LOGFILE);
		MessageBox(hwnd, filename, "Error can't load file",MB_OK);
		return FALSE;
    }

	PrintMessage(hwnd, "Loading ", filename, SCN_AND_LOGFILE);
	
	
	Q2M_counter = 0;
	Q2M_Anim_Counter = 0;

	while(done==0)
	{
		fscanf( fp, "%s", &s );

		if( strcmp(s,"OBJECT") == 0) 		
		{
			dat_scale = 1.0000;
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

			wptr->num_vert_per_object [old_object_id] = vert_count;
			wptr->num_polys_per_object[old_object_id] = poly_count;
			vert_count=0;
			poly_count=0;
			conn_cnt=0;
			
			fscanf( fp, "%s", &p );	// read object name
			strcpy((char *)wptr->obdata[object_id].name, (char *)&p);  
			
		}

		if(strcmp(s,"SCALE")==0)
		{
			fscanf( fp, "%s", &p );
			dat_scale = (float)atof(p);

		}

		if(strcmp(s,"TEXTURE")==0)
		{
			fscanf( fp, "%s", &p );

			texture = CheckValidTextureAlias(hwnd, p, wptr);

			if(texture == -1)
			{
				MessageBox(hwnd,"Function : LoadObjectData","Error Bad Texture ID",MB_OK);
				MessageBox(hwnd,p,"Error Bad Texture ID",MB_OK);
				return FALSE;
			}
		}

		if(strcmp(s,"TYPE")==0)
		{
			fscanf( fp, "%s", &p );

		}

		if(strcmp(s,"TRI_STRIP")==0)
		{
			// Get numbers of verts in triangle fan
			fscanf( fp, "%s", &p );	
			num_v = atoi(p);

			for(i = 0; i < num_v; i++)
			{
				// vert
				fscanf( fp, "%s", &p );
				wptr->obdata[object_id].v[vert_count].x = (float)atof(p);

				fscanf( fp, "%s", &p );
				wptr->obdata[object_id].v[vert_count].y = (float)atof(p);

				fscanf( fp, "%s", &p );
				wptr->obdata[object_id].v[vert_count].z = (float)atof(p);

				// texture co-ordinate
				fscanf( fp, "%s", &p );
				wptr->obdata[object_id].t[vert_count].x = (float)atof(p);

				fscanf( fp, "%s", &p );
				wptr->obdata[object_id].t[vert_count].y = (float)atof(p);
				
				vert_count++;
			}

			wptr->obdata[object_id].tex[poly_count] = texture;
			wptr->obdata[object_id].num_vert[poly_count] = num_v;
			wptr->obdata[object_id].poly_cmd[poly_count] = POLY_CMD_TRI_STRIP;
			poly_count++;
		}

		if(strcmp(s,"TRI_FAN")==0)
		{
			// Get numbers of verts in triangle fan
			fscanf( fp, "%s", &p );	
			num_v = atoi(p);

			for(i = 0; i < num_v; i++)
			{
				// vert
				fscanf( fp, "%s", &p );
				wptr->obdata[object_id].v[vert_count].x = (float)atof(p);

				fscanf( fp, "%s", &p );
				wptr->obdata[object_id].v[vert_count].y = (float)atof(p);

				fscanf( fp, "%s", &p );
				wptr->obdata[object_id].v[vert_count].z = (float)atof(p);

				// texture co-ordinate
				fscanf( fp, "%s", &p );
				wptr->obdata[object_id].t[vert_count].x = (float)atof(p);

				fscanf( fp, "%s", &p );
				wptr->obdata[object_id].t[vert_count].y = (float)atof(p);
				
				vert_count++;
			}

			wptr->obdata[object_id].tex[poly_count] = texture;
			wptr->obdata[object_id].num_vert[poly_count] = num_v;
			wptr->obdata[object_id].poly_cmd[poly_count] = POLY_CMD_TRI_FAN;
			poly_count++;
		}

		if(strcmp(s,"QUAD")==0)
		{
			
			fscanf( fp, "%s", &p );
			wptr->obdata[object_id].v[vert_count].x = (float)atof(p);
		
			fscanf( fp, "%s", &p );
			wptr->obdata[object_id].v[vert_count].y = (float)atof(p);
		
			fscanf( fp, "%s", &p );
			wptr->obdata[object_id].v[vert_count].z = (float)atof(p);
		

			fscanf( fp, "%s", &p );
			wptr->obdata[object_id].v[vert_count+1].x = (float)atof(p);
		
			fscanf( fp, "%s", &p );
			wptr->obdata[object_id].v[vert_count+1].y = (float)atof(p);
		
			fscanf( fp, "%s", &p );
			wptr->obdata[object_id].v[vert_count+1].z = (float)atof(p);


			fscanf( fp, "%s", &p );
			wptr->obdata[object_id].v[vert_count+3].x = (float)atof(p);
		
			fscanf( fp, "%s", &p );
			wptr->obdata[object_id].v[vert_count+3].y = (float)atof(p);
		
			fscanf( fp, "%s", &p );
			wptr->obdata[object_id].v[vert_count+3].z = (float)atof(p);


			fscanf( fp, "%s", &p );
			wptr->obdata[object_id].v[vert_count+2].x = (float)atof(p);
		
			fscanf( fp, "%s", &p );
			wptr->obdata[object_id].v[vert_count+2].y = (float)atof(p);
		
			fscanf( fp, "%s", &p );
			wptr->obdata[object_id].v[vert_count+2].z = (float)atof(p);
				vert_count+=4;
			
			
			wptr->obdata[object_id].tex[poly_count] = texture;
			wptr->obdata[object_id].num_vert[poly_count] = 4;
			wptr->obdata[object_id].poly_cmd[poly_count] = POLY_CMD_QUAD;
			poly_count++;
		
		}
		
		if(strcmp(s,"QUADTEX")==0)
		{
			
			fscanf( fp, "%s", &p );
			x = dat_scale * (float)atof(p);
			wptr->obdata[object_id].v[vert_count].x = x;
		
			fscanf( fp, "%s", &p );
			y = dat_scale * (float)atof(p);
			wptr->obdata[object_id].v[vert_count].y = y;
		
			fscanf( fp, "%s", &p );
			z = dat_scale * (float)atof(p);
			wptr->obdata[object_id].v[vert_count].z = z;

			fscanf( fp, "%s", &p );
			wptr->obdata[object_id].t[vert_count].x = (float)atof(p);

			fscanf( fp, "%s", &p );
			wptr->obdata[object_id].t[vert_count].y = (float)atof(p);
		


			fscanf( fp, "%s", &p );
			x = dat_scale * (float)atof(p);
			wptr->obdata[object_id].v[vert_count+1].x = x;
		
			fscanf( fp, "%s", &p );
			y = dat_scale * (float)atof(p);
			wptr->obdata[object_id].v[vert_count+1].y = y;
		
			fscanf( fp, "%s", &p );
			z = dat_scale * (float)atof(p);
			wptr->obdata[object_id].v[vert_count+1].z = z;
			
			fscanf( fp, "%s", &p );
			wptr->obdata[object_id].t[vert_count+1].x = (float)atof(p);

			fscanf( fp, "%s", &p );
			wptr->obdata[object_id].t[vert_count+1].y = (float)atof(p);



			fscanf( fp, "%s", &p );
			x = dat_scale * (float)atof(p);
			wptr->obdata[object_id].v[vert_count+3].x = x;
		
			fscanf( fp, "%s", &p );
			y = dat_scale * (float)atof(p);
			wptr->obdata[object_id].v[vert_count+3].y = y;
		
			fscanf( fp, "%s", &p );
			z = dat_scale * (float)atof(p);
			wptr->obdata[object_id].v[vert_count+3].z = z;

			fscanf( fp, "%s", &p );
			wptr->obdata[object_id].t[vert_count+3].x = (float)atof(p);

			fscanf( fp, "%s", &p );
			wptr->obdata[object_id].t[vert_count+3].y = (float)atof(p);



			fscanf( fp, "%s", &p );
			x = dat_scale * (float)atof(p);
			wptr->obdata[object_id].v[vert_count+2].x = x;
		
			fscanf( fp, "%s", &p );
			y = dat_scale * (float)atof(p);
			wptr->obdata[object_id].v[vert_count+2].y = y;
		
			fscanf( fp, "%s", &p );
			z = dat_scale * (float)atof(p);
			wptr->obdata[object_id].v[vert_count+2].z = z;

			fscanf( fp, "%s", &p );
			wptr->obdata[object_id].t[vert_count+2].x = (float)atof(p);

			fscanf( fp, "%s", &p );
			wptr->obdata[object_id].t[vert_count+2].y = (float)atof(p);


			vert_count+=4;
			
			
			wptr->obdata[object_id].tex[poly_count] = texture;
			wptr->obdata[object_id].num_vert[poly_count] = 4;
			wptr->obdata[object_id].poly_cmd[poly_count] = POLY_CMD_QUAD_TEX;
			poly_count++;
		
		}
			
		

		if(strcmp(s,"TRI")==0)
		{
			for(i=0;i<3;i++)
			{
				fscanf( fp, "%s", &p );
				wptr->obdata[object_id].v[vert_count].x = (float)atof(p);
		
				fscanf( fp, "%s", &p );
				wptr->obdata[object_id].v[vert_count].y = (float)atof(p);
		
				fscanf( fp, "%s", &p );
				wptr->obdata[object_id].v[vert_count].z = (float)atof(p);
		
				vert_count++;
			}
			
			wptr->obdata[object_id].tex[poly_count]=texture;
			wptr->obdata[object_id].num_vert[poly_count] = 3;
			wptr->obdata[object_id].poly_cmd[poly_count] = POLY_CMD_TRI;
			poly_count++;
		
		}

		if(strcmp(s,"CONNECTION")==0)
		{
			if(conn_cnt<4)
			{
				fscanf( fp, "%s", &p );
				wptr->obdata[object_id].connection[conn_cnt].x = (float)atof(p);
				fscanf( fp, "%s", &p );
				wptr->obdata[object_id].connection[conn_cnt].y = (float)atof(p);
				fscanf( fp, "%s", &p );
				wptr->obdata[object_id].connection[conn_cnt].z = (float)atof(p);
				conn_cnt++;
			}
			else
			{
				fscanf( fp, "%s", &p );
				fscanf( fp, "%s", &p );
				fscanf( fp, "%s", &p );
			}
		}

		if(strcmp(s,"END_FILE")==0)
		{		
			wptr->num_vert_per_object[object_id]  = vert_count;
			wptr->num_polys_per_object[object_id] = poly_count;
			wptr->obdata_length = object_count+1;
			done = 1;
		}
	}
	fclose(fp);	
	
	itoa(wptr->obdata_length, buffer, 10);
	
	PrintMessage(hwnd, buffer, " DAT objects loaded" ,SCN_AND_LOGFILE);
	PrintMessage(hwnd, "\n", NULL, LOGFILE_ONLY);

	return TRUE;
}

*/
BOOL LoadWorldMap(HWND hwnd,char *filename,world_ptr wptr)
{
	FILE *fp;    
	char s[81];
	char p[100];
	char buffer[100];
	char buf2[100];
	char buf3[100];
	char buf4[100];
	int y_count = 30;
	int done = 0;
	int object_count = -1;
	int vert_count = 0;
	int pv_count = 0;
	int poly_count = 0;
	int object_id;
	int lit_vert;
	int red,green,blue;
	int i;
	int num_lverts;
	int mem_counter = 0;


    fp = fopen(filename,"r");
    if(fp==NULL)
    {     
		PrintMessage(hwnd, "Error can't load file ", filename, SCN_AND_LOGFILE);
        MessageBox(hwnd,"Error can't load file",NULL,MB_OK);
		return FALSE;
    }

	PrintMessage(hwnd, "Loading map ", filename, SCN_AND_LOGFILE);
	

	while(done == 0)
	{
		if(fscanf( fp, "%s", &s ) == EOF)
		{
            PrintMessage(hwnd, "Error unexpected end of file in:  ", filename, SCN_AND_LOGFILE);
			MessageBox(hwnd,"Error unexpected end of file in:  ", filename, MB_OK);
			return FALSE;
		}

		if(strcmp(s,"OBJECT") == 0)
		{
			object_count++;

			fscanf( fp, "%s", &p );
			
			object_id = CheckObjectId(hwnd, (char *)&p, wptr);
			if(object_id == -1)
			{
				PrintMessage(hwnd, "Error Bad Object ID in: LoadWorld ", p, SCN_AND_LOGFILE);
				MessageBox(hwnd,"Error Bad Object ID in: LoadWorld",NULL,MB_OK);
				return FALSE;
			}

			wptr->oblist[object_count].type = object_id;
			
			num_lverts = wptr->num_vert_per_object[object_id];
			wptr->oblist[object_count].lit = new LIGHT[num_lverts];
			mem_counter += sizeof(LIGHT) * num_lverts;

			strcpy(buffer, "object_count: ");

			itoa(object_count, buf2, 10);
			strcat(buffer, buf2);

			strcat(buffer, "  object_id: ");
			itoa(object_id, buf3, 10);
			strcat(buffer, buf3);

			strcat(buffer, "  num_lverts: ");
			itoa(num_lverts, buf4, 10);
			strcat(buffer, buf4);
			
			PrintMessage(hwnd, buffer, NULL, LOGFILE_ONLY);


			for(i = 0; i < num_lverts; i++)
			{
				wptr->oblist[object_count].lit[i].r = 0;
				wptr->oblist[object_count].lit[i].g = 0;
				wptr->oblist[object_count].lit[i].b = 0;
			}
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
			LinksList[last_link].last_x = (float)atof(p);
			fscanf( fp, "%s", &p );
			LinksList[last_link].last_z = (float)atof(p);
			fscanf( fp, "%s", &p );
			LinksList[last_link].last_angle = (float)atof(p);
			
			current_link ++;
			last_link++;
		}

		if(strcmp(s,"END_FILE")==0)
		{
			fscanf( fp, "%s", &p );	
			object_count++;
			wptr->oblist_length = object_count;
			done = 1;
		}
	}
	fclose(fp);	
	
	itoa(wptr->oblist_length, buffer, 10);
	PrintMessage(hwnd, buffer, " map objects loaded",SCN_AND_LOGFILE);
	PrintMessage(hwnd, "\n", NULL, LOGFILE_ONLY);

	mem_counter = mem_counter / 1024;
	itoa(mem_counter, buffer, 10);
	strcat(buffer, " KB");
	PrintMessage(hwnd, "memory allocated to LIGHT array ", buffer, LOGFILE_ONLY);
	
	return TRUE;
}


int CheckObjectId(HWND hwnd, char *p, world_ptr wptr) 
{
	int i;
	char *buffer2;

	for(i = 0; i < wptr->obdata_length; i++)
	{
		buffer2 = wptr->obdata[i].name;

		if(strcmp(buffer2, p)==0)
		{
			return i;
		}
	}
	MessageBox(hwnd,buffer2,NULL,MB_OK);
		
	return -1; //error
}

setupinfo_ptr LoadSetupInfo(HWND hwnd)
{
	FILE  *fp;    
	char  s[100];
	char  p[100];
	char  q[100];
	int done=0;
	BOOL mode_error		= TRUE;
	BOOL screen_error	= TRUE;
	BOOL control_error	= TRUE;
	BOOL sound_error	= TRUE;

	setupinfo.vmode.bpp = 16;
	setupinfo.screen = FULLSCREEN_ONLY;

    fp = fopen("rrsetup.ini","r");
    
	if(fp==NULL)    
	{
		Msg("error in rrsetup.ini");
		return NULL;
	}
       
	//PrintMessage(hwnd, "Loading rrsetup.ini");

	while(done==0)
	{
		fscanf( fp, "%s", &s );

		if(strcmp(s,"SCREEN")==0)
		{
			fscanf( fp, "%s", &p );
		
			if(strcmp(p,"FULLSCREEN_ONLY")==0)	
			{
				setupinfo.screen = FULLSCREEN_ONLY;
				screen_error = FALSE;
			}
			
			if(strcmp(p,"MAXIMISED_WINDOW")==0)
			{
				setupinfo.screen = MAXIMISED_WINDOW;
				screen_error = FALSE;
			}
			if(screen_error == TRUE)
				return NULL;
			
		}

		if(strcmp(s,"MODE")==0)
		{
			fscanf( fp, "%s", &p );
			fscanf( fp, "%s", &q );	

			if( (strcmp(p,"640")==0) && (strcmp(q,"480")==0) )
			{
				setupinfo.vmode.w = 640;
				setupinfo.vmode.h = 480;
				mode_error=FALSE;	
			}

			if( (strcmp(p,"800")==0) && (strcmp(q,"600")) )
			{
				setupinfo.vmode.w = 800;
				setupinfo.vmode.h = 600;
				mode_error=FALSE;
			}

			if( (strcmp(p,"320")==0) && (strcmp(q,"200")) )
			{
				setupinfo.vmode.w = 320;
				setupinfo.vmode.h = 200;
				mode_error=FALSE;
			}

			if(mode_error == TRUE)
			{
				Msg("error in rrsetup.ini :  Mode");
				return NULL;
			}
		}

		if(strcmp(s,"CONTROL")==0)
		{
			fscanf( fp, "%s", &p );

			if(strcmp(p,"JOYSTICK")==0)
			{
				setupinfo.control = JOYSTICK;
				control_error = FALSE;
			}
			

			if(strcmp(p,"KEYBOARD")==0)
			{
				setupinfo.control = KEYBOARD; 
				control_error = FALSE;
			}
			if(control_error == TRUE)
				return NULL;
		}
		
		if(strcmp(s,"SOUND")==0)
		{
			fscanf( fp, "%s", &p );

			if(strcmp(p,"ON")==0)
			{
				setupinfo.sound = TRUE; // sound On
				sound_error = FALSE;
			}

			if(strcmp(p,"OFF")==0)
			{
				setupinfo.sound = FALSE; // Sound Off
				sound_error = FALSE;
			}
			if(sound_error == TRUE)
				return NULL;
		}

		if(strcmp(s,"END_FILE")==0)
		{
			fscanf( fp, "%s", &p );				
			done=1;
		}
	}
	fclose(fp);	
	return &setupinfo;
}


// InitWorldMap 
// Compiles map and saves a .cmp file

void InitWorldMap(HWND hwnd, world_ptr wptr)
{
	FILE *fp;
	int i,j,w;
	int cnt=0;
	int progress_counter = 0;
	int angle;
	int ob_type;
	int vert_cnt,poly_cnt;
	int count;
	int poly;
	int length;
	int cx,cz;
	int cell_count;
	int cell_x,cell_z;
	int temp[20];
	int mem_count=0;
	int num_vert;
	float x,z;
	float wx,wy,wz;
	float tx,tz;
	float sine,cosine;
	float last_rot_angle=(float)0;
	float k=(float)0.017453292;
	float sin_table[361];
	float cos_table[361];
	float fangle;
	char buffer[50];
	char buf2[50];
	BOOL *map[MAX_NUM_MAP_CELLS];
	int cell_length[MAX_NUM_MAP_OBJECTS];
	int *cell_xlist[MAX_NUM_MAP_OBJECTS];
	int *cell_zlist[MAX_NUM_MAP_OBJECTS];


	fp = fopen("level1.cmp","w");
    if(fp==NULL)
    {     
        MessageBox(hwnd,"Error can't load file",NULL,MB_OK);
		return;
    }

	for(i = 0; i < MAX_NUM_MAP_CELLS; i++)
		map[i] = new BOOL[MAX_NUM_MAP_CELLS];
		
	for(i = 0; i < MAX_NUM_MAP_OBJECTS; i++)
	{
		cell_xlist[i] = new int[50];
		cell_zlist[i] = new int[50];
	}

	PrintMessage(hwnd, "Processing Map Info", NULL, SCN_AND_LOGFILE);

	for(i = 0; i < 360; i++)
	{
		fangle = (float)i * k;
		sin_table[i] = (float)sin(fangle);
		cos_table[i] = (float)cos(fangle);
	}
	
	poly_cnt=0;
	j=0;
	progress_counter = 0;

	for(i = 0; i < wptr->oblist_length; i++)
	{	
		wx = wptr->oblist[i].x;
		wy = wptr->oblist[i].y;
		wz = wptr->oblist[i].z;
		
		angle = (int)wptr->oblist[i].rot_angle;

		cosine = cos_table[angle];
		sine   = sin_table[angle];
		
		ob_type = wptr->oblist[i].type;
	
	
		
		if(progress_counter > 49)
		{
			hdc = GetDC(hwnd);
			itoa(i, buffer, 10);
			strcat(buffer, "                    ");
			TextOut(hdc, PrintMsgX, PrintMsgY, buffer, strlen(buffer));
			progress_counter = 0;
			ReleaseDC(hwnd, hdc);
		}
		progress_counter++;

		for(cz = 0; cz < MAX_NUM_MAP_CELLS; cz++)
		{
			for(cx = 0; cx < MAX_NUM_MAP_CELLS; cx++)
				map[cx][cz] = FALSE;
		}

		j = 0;
		cell_count = 0;
		cell_length[i] = cell_count;
		poly = wptr->num_polys_per_object[ob_type];

		for(w = 0; w < poly; w++)
		{
			num_vert = wptr->obdata[ob_type].num_vert[w];

			for(vert_cnt = 0; vert_cnt < num_vert; vert_cnt++)
			{
				x = wptr->obdata[ob_type].v[j].x; 
				z = wptr->obdata[ob_type].v[j].z;
				
				j++;

				tx = wx + (x*cosine - z*sine); 
				tz = wz + (x*sine + z*cosine);

				cell_x = (int)(tx/(float)256);
				cell_z = (int)(tz/(float)256);
				
				if(cell_x >= 0 && cell_z >= 0)
				{
					if(map[cell_x][cell_z] == FALSE)
					{
						if(cell_count < MAX_NUM_OBJECTS_PER_CELL)
						{
							cell_xlist[i][cell_count] = cell_x;
							cell_zlist[i][cell_count] = cell_z;
							cell_count++;
							cell_length[i] = cell_count;
							map[cell_x][cell_z] = TRUE;
						}
					}
				}
				
			}
			poly_cnt++;

		} // end for w
	} // end for i
	
	//////////////////////////////////////////////////////

	progress_counter = 0;

	strcpy(buffer, "Map size : ");
	itoa(MAX_NUM_MAP_CELLS, buf2, 10);
	strcat(buffer, buf2);
	strcat(buffer, " x ");
	strcat(buffer, buf2);
	strcat(buffer, " cells");
		
	PrintMessage(hwnd, buffer,  NULL, SCN_AND_LOGFILE);
	PrintMessage(hwnd, "Loading map data into cells", NULL, SCN_AND_LOGFILE);

	for(cz = 0; cz < MAX_NUM_MAP_CELLS; cz++)
	{
		
		if(progress_counter > 9)
		{
			hdc = GetDC(hwnd);
			strcpy(buffer,"cell_z: ");
			itoa(cz, buf2, 10);
			strcat(buffer, buf2);
			TextOut(hdc, PrintMsgX, PrintMsgY, buffer, strlen(buffer));
			progress_counter = 0;
			ReleaseDC(hwnd, hdc);
		}
		
		progress_counter++;

		for(cx = 0; cx < MAX_NUM_MAP_CELLS; cx++)
		{
			count = 0;

			for(i = 0; i <= wptr->oblist_length; i++)
			{
				length = cell_length[i];

				for(j = 0; j < length; j++)
				{
					if(cell_xlist[i][j] == cx)
					{
						if(cell_zlist[i][j] == cz)
						{
							if(count < MAX_NUM_OBJECTS_PER_CELL)
							{
								temp[count] = i;
								count++;
							}
						}
					}
				}
			}

			if(count > 0)
			{
				wptr->cell[cx][cz] = new int[count];
				mem_count = mem_count + (count* sizeof(int));

				itoa(cx, buffer,10);
				fprintf( fp, "%s", buffer );

				itoa(cz, buffer,10);
				fprintf( fp, " %s", buffer );

				itoa(count, buffer,10);
				fprintf( fp, "  %s ", buffer );
								
				for(w = 0; w < count; w++)
				{
					wptr->cell[cx][cz][w] = temp[w];
					
					itoa(temp[w], buffer,10);
					fprintf( fp, " %s", buffer );

				}
				wptr->cell_length[cx][cz] = count;

				fprintf( fp, " %s\n", " " );

			}
			else
				wptr->cell[cx][cz] = NULL;
		}
	}
	fprintf( fp, "\n%s\n", "END_FILE" );
	fclose(fp);	

	for(i = 0; i < MAX_NUM_MAP_OBJECTS; i++)
	{
		delete cell_xlist[i];
		delete cell_zlist[i];
	}

	for(i = 0; i < MAX_NUM_MAP_CELLS; i++)
		delete map[i];

	

}

void InitPreCompiledWorldMap(HWND hwnd, world_ptr wptr)
{
	FILE *fp;
	int w;
	int done = 0;
	int count;
	int cell_x, cell_z;
	int mem_count = 0;
	char  s[100];
	char buffer[256];

	mem_count = sizeof(WORLD);
	mem_count = mem_count / 1024;
	itoa(mem_count, buffer, 10);
	strcat(buffer, " KB\n");
	PrintMessage(hwnd, "memory used by WORLD structure = ", buffer, LOGFILE_ONLY);
	
	mem_count = 0;

	fp = fopen("level1.cmp","r");
    if(fp==NULL)
    {     
		PrintMessage(hwnd, "InitPreCompiledWorldMap: ERROR can't load: level1.cmp", NULL, SCN_AND_LOGFILE);
        MessageBox(hwnd,"Error can't load file",NULL,MB_OK);
		return;
	}

	for(cell_z = 0; cell_z < MAX_NUM_MAP_CELLS; cell_z++)
	{
		for(cell_x = 0; cell_x < MAX_NUM_MAP_CELLS; cell_x++)
		{
			wptr->cell[cell_x][cell_z] = NULL;
		}
	}
	
	mem_count = 0;

	PrintMessage(hwnd, "InitPreCompiledWorldMap: Loading precompiled map data", NULL, SCN_AND_LOGFILE);
	
	while(done == 0)
	{
		if(fscanf( fp, "%s", &s ) == EOF)
		{
            PrintMessage(hwnd, "Error unexpected end of file in:  ", "level1.cmp", SCN_AND_LOGFILE);
			MessageBox(hwnd,"Error unexpected end of file in:  ", "level1.cmp", MB_OK);
			done = 1;
		}

		if(strcmp(s,"END_FILE")==0)
		{	
			done = 1;
			fclose(fp);	
			PrintMessage(hwnd, "InitPreCompiledWorldMap: Suceeded", NULL, SCN_AND_LOGFILE);
			
			mem_count = mem_count / 1024;
			itoa(mem_count, buffer, 10);
			strcat(buffer, " KB\n");
			PrintMessage(hwnd, "memory allocated for map = ", buffer, LOGFILE_ONLY);
	
			return;
		}

		cell_x = atoi(s);

		if((cell_x >=0 ) && (cell_x <= MAX_NUM_MAP_CELLS))
		{
			fscanf( fp, "%s", &s );
			cell_z = atoi(s);

			if((cell_z >= 0) && (cell_z <= MAX_NUM_MAP_CELLS))
			{
				fscanf( fp, "%s", &s );
				count = atoi(s);
			
				wptr->cell[cell_x][cell_z] = new int[count];
				mem_count += (count * sizeof(int));
							
				for(w = 0; w < count; w++)
				{
					fscanf( fp, "%s", &s );
					wptr->cell[cell_x][cell_z][w] = atoi(s);						
				}
				
				wptr->cell_length[cell_x][cell_z] = count;
			}
		}
		
	} // end while		

	PrintMessage(hwnd, "InitPreCompiledWorldMap: FAILED", NULL, SCN_AND_LOGFILE);
	
	return;
}


void PrintMessage(HWND hwnd,char *message1, char *message2, int message_mode)
{
	FILE *fp;
	char tmessage[100];

	
	if((message1 == NULL) && (message2 == NULL))
		return;

	if((message1 != NULL) && (message2 != NULL))
	{
		strcpy(tmessage, message1);
		strcat(tmessage, message2);
	}
	else
	{
		if(message1 != NULL) 
			strcpy(tmessage, message1);

		if(message2 != NULL) 
			strcpy(tmessage, message2);
	}


	if(logfile_start_flag == TRUE)
	{
		fp = fopen("rrlogfile.txt","w");
		fprintf( fp, "%s\n\n", "RR Logfile");
	}
	else
		fp = fopen("rrlogfile.txt","a");

	logfile_start_flag = FALSE;
    
	if(fp == NULL)
    {     
        MessageBox(hwnd,"Can't load logfile","File Error",MB_OK);
		fclose(fp);
		return;
	}

	fprintf( fp, " %s\n", tmessage );

	

	if(message_mode != LOGFILE_ONLY)
	{
		hdc=GetDC(hwnd);
		TextOut(hdc,PrintMsgX,PrintMsgY, tmessage,strlen(tmessage));
		PrintMsgY +=20;
		ReleaseDC(hwnd,hdc);
	}
	fclose(fp);
}


