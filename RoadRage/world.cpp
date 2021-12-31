
// Road Rage version 1.1

// Written by William Chin
// Developed in MSDEV C++ v6.0 using the DirectX 6.0 SDK
// Date : 3/7/99
// File : world.cpp


#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <winuser.h>			
#include "world.hpp"
#include "error.hpp"
#include "d3dmain.hpp"
#include <mbstring.h>
#include <tchar.h>
#include "texture.hpp"
#include <io.h>
#include <fcntl.h>
#include "ImportMD2.hpp"
#include "Import3DS.hpp"
#include "sound.hpp"

#define MAX_NUM_OBJECTS_PER_CELL 50
#define MD2_MODEL 0
#define	k3DS_MODEL 1	
		
extern CAMERA cam;	
extern Joystick_Enabled_Flag;
extern BOOL d3dcard_driver_flag;	
static int temp_view_angle;

SETUPINFO setupinfo;
HDC hdc;

int model_format;
int cell_length[2000];
int cell_xlist[2000][20];
int cell_zlist[2000][20];

int PrintMsgX = 10;
int PrintMsgY = 10;

int num_tex;
int num_imported_models_loaded = 0;
int num_op_guns = 0;
int num_cars;

int player_count = 0;
int op_gun_count = 0;
int your_gun_count = 0;
int car_count = 0;
int type_num;
char g_model_filename[256];

BOOL logfile_start_flag = TRUE;




BOOL LoadImportedModelList(HWND hwnd, char *filename, world_ptr wptr)
{
	FILE *fp; 
	int done = 0;
	int i;
	char command[256];
	char p[256];
	float f;
	int model_id;
	//char model_name[256];
	char model_filename[256];
	int model_tex_alias;
	float scale;
	int  Q2M_Anim_Counter = 0;
	BOOL command_recognised;	
	BOOL Model_loaded_flags[1000] = {FALSE};

	wptr->num_players   = 0;
	wptr->num_your_guns = 0;
	wptr->num_op_guns   = 0;

	for(i = 0; i < wptr->num_debug_models; i++)
	{
		wptr->debug[i].current_sequence = 0;
		wptr->debug[i].current_frame = 0;
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
			model_format = MD2_MODEL;
			
		if(strcmp(p,"3DS") == 0)
			model_format = k3DS_MODEL;

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
			wptr->player_list[type_num].x = (float)atoi(p);
					
			fscanf( fp, "%s", &p ); // y pos
			wptr->player_list[type_num].y = (float)atoi(p);
					
			fscanf( fp, "%s", &p ); // z pos
			wptr->player_list[type_num].z = (float)atoi(p);
					
			fscanf( fp, "%s", &p ); // ignore comment angle
			fscanf( fp, "%s", &p ); // angle
			wptr->player_list[type_num].rot_angle = (float)atoi(p);

			wptr->player_list[type_num].model_id = model_id;
			wptr->player_list[type_num].skin_tex_id = model_tex_alias;
			wptr->num_players++;

			fscanf( fp, "%s", &p ); // ignore comment scale
			fscanf( fp, "%f", &f );
			scale = f;

			fscanf( fp, "%s", &p ); // Don't draw players external weapon
			if(strcmp(p, "NO_EXTERNAL_WEP") == 0)
				wptr->player_list[type_num].draw_external_wep = FALSE;
			
			if(strcmp(p, "USE_EXTERNAL_WEP") == 0)
				wptr->player_list[type_num].draw_external_wep = TRUE;
			

			player_count++;
			
			LoadPlayerAnimationSequenceList(model_id, wptr);
			LoadDebugAnimationSequenceList(hwnd, model_filename, model_id, wptr);
			command_recognised = TRUE;
		}

		if(strcmp(command,"CAR") == 0)
		{
			fscanf( fp, "%s", &p ); // ignore comment pos
					
			fscanf( fp, "%s", &p ); // x pos
			wptr->car_list[type_num].x = (float)atoi(p);
					
			fscanf( fp, "%s", &p ); // y pos
			wptr->car_list[type_num].y = (float)atoi(p);
					
			fscanf( fp, "%s", &p ); // z pos
			wptr->car_list[type_num].z = (float)atoi(p);
					
			fscanf( fp, "%s", &p ); // ignore comment angle
			fscanf( fp, "%s", &p ); // angle
			wptr->car_list[type_num].rot_angle = (float)atoi(p);

			wptr->car_list[type_num].model_id = model_id;
			wptr->car_list[type_num].skin_tex_id = model_tex_alias;
			wptr->num_cars++;

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
			wptr->your_gun[type_num].x_offset = (float)atoi(p);
					
			fscanf( fp, "%s", &p ); // y pos
			wptr->your_gun[type_num].z_offset = (float)atoi(p);
				
			fscanf( fp, "%s", &p ); // z pos
			wptr->your_gun[type_num].y_offset = (float)atoi(p);
					
			fscanf( fp, "%s", &p ); // ignore comment angle
			fscanf( fp, "%s", &p ); // angle
			wptr->your_gun[type_num].rot_angle = (float)atoi(p);

			wptr->your_gun[type_num].model_id = model_id;
			wptr->your_gun[type_num].skin_tex_id = model_tex_alias;
			wptr->num_your_guns++;

			fscanf( fp, "%s", &p ); // ignore comment scale
			fscanf( fp, "%f", &f );
			scale = f;

			your_gun_count++;
			LoadYourGunAnimationSequenceList(model_id, wptr);
			//LoadDebugAnimationSequenceList(hwnd, model_filename, model_id, wptr);
			command_recognised = TRUE;
		}

		if(strcmp(command,"DEBUG") == 0)
		{
			fscanf( fp, "%s", &p ); // ignore comment pos
				
			fscanf( fp, "%s", &p ); // x pos
			wptr->debug[type_num].x = (float)atoi(p);
					
			fscanf( fp, "%s", &p ); // y pos
			wptr->debug[type_num].y = (float)atoi(p);
				
			fscanf( fp, "%s", &p ); // z pos
			wptr->debug[type_num].z = (float)atoi(p);
					
			fscanf( fp, "%s", &p ); // ignore comment angle
			fscanf( fp, "%s", &p ); // angle
			wptr->debug[type_num].rot_angle = (float)atoi(p);

			wptr->debug[type_num].model_id = model_id;
			wptr->debug[type_num].skin_tex_id = model_tex_alias;
			wptr->num_debug_models++;
			
			fscanf( fp, "%s", &p ); // ignore comment scale
			fscanf( fp, "%f", &f );
			scale = f;

			LoadDebugAnimationSequenceList(hwnd, model_filename, model_id, wptr);

			command_recognised = TRUE;
		}

		if(strcmp(command,"OPGUN") == 0)
		{			
			wptr->other_players_guns[type_num].model_id = model_id;
			wptr->other_players_guns[type_num].skin_tex_id = model_tex_alias;
			wptr->num_op_guns++;
			
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
				
				if(model_format == MD2_MODEL)
					ImportMD2_GLCMD(hwnd, model_filename,  model_tex_alias, model_id, scale, wptr);
			
				if(model_format == k3DS_MODEL)
					Import3DS(hwnd, model_filename, model_id, scale, wptr);
			
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


void InitRRvariables(world_ptr wptr)
{
	int i;

	wptr->walk_mode_enabled = TRUE;
	wptr->display_scores = FALSE;
	wptr->player_list[0].frags = 0;
	wptr->player_list[0].health = 100;
	wptr->player_list[0].armour = 0;
	strcpy(wptr->player_list[0].name, "Player"); 
	wptr->current_gun = 0;

	car_speed = (float)0;

	cam.direction_x = (float)0;
	cam.direction_y = (float)0;
	cam.direction_z = (float)0;
			
	cam.view_angle = 0;
	cam.view_point_x = (float)680;
	cam.view_point_y = (float)25;
	cam.view_point_z = (float)480;
			
	cam.ramp_size = 1;
	Joystick_Enabled_Flag = FALSE;
			
	for(i = 0; i < 10; i++)
	{
		wptr->player_list[i].walk_or_drive_mode = WALK_MODE;
		wptr->player_list[i].current_frame = 0;
		wptr->player_list[i].current_sequence = 0;
		wptr->player_list[i].current_weapon = 0;
		wptr->player_list[i].current_car = 0;			
	}

	wptr->your_gun[0].sound_id = SND_FIRE_AR15;
	wptr->your_gun[1].sound_id = SND_FIRE_MP5;
	wptr->current_gun = 0;
	wptr->current_gun_model_id = wptr->your_gun[wptr->current_gun].model_id;
}

void LoadDebugAnimationSequenceList(HWND hwnd, char *filename, int model_id, world_ptr wptr)
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
			wptr->pmdata[model_id].sequence_start_frame[sequence_number] = start_frame;
			

			// read stop frame
			fscanf( fp, "%s", &p );
			if(strcmpi(p, "STOP_FRAME") != 0)
				return;

			fscanf( fp, "%d", &stop_frame );	
			wptr->pmdata[model_id].sequence_stop_frame[sequence_number] = stop_frame;

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


void LoadPlayerAnimationSequenceList(int model_id, world_ptr wptr)
{
	int i;

	i = model_id;

	wptr->pmdata[i].sequence_start_frame[0] =0;  // Stand
	wptr->pmdata[i].sequence_stop_frame [0] =39;

	wptr->pmdata[i].sequence_start_frame[1] =40; // run
	wptr->pmdata[i].sequence_stop_frame [1] =45;

	wptr->pmdata[i].sequence_start_frame[2] =46; // attack
	wptr->pmdata[i].sequence_stop_frame [2] =53;

	wptr->pmdata[i].sequence_start_frame[3] =54; // pain1
	wptr->pmdata[i].sequence_stop_frame [3] =57;

	wptr->pmdata[i].sequence_start_frame[4] =58; // pain2
	wptr->pmdata[i].sequence_stop_frame [4] =61;

	wptr->pmdata[i].sequence_start_frame[5] =62; // pain3
	wptr->pmdata[i].sequence_stop_frame [5] =65;

	wptr->pmdata[i].sequence_start_frame[6] =66; // jump
	wptr->pmdata[i].sequence_stop_frame [6] =71;

	wptr->pmdata[i].sequence_start_frame[7] =72; // flip
	wptr->pmdata[i].sequence_stop_frame [7] =83;

	wptr->pmdata[i].sequence_start_frame[8] =84; // Salute
	wptr->pmdata[i].sequence_stop_frame [8] =94;

	wptr->pmdata[i].sequence_start_frame[9] =95; // Taunt
	wptr->pmdata[i].sequence_stop_frame [9] =111;

	wptr->pmdata[i].sequence_start_frame[10] =112; // Wave
	wptr->pmdata[i].sequence_stop_frame [10] =122;

	wptr->pmdata[i].sequence_start_frame[11] =123; // Point
	wptr->pmdata[i].sequence_stop_frame [11] =134;
	
}

void LoadYourGunAnimationSequenceList(int model_id, world_ptr wptr)
{
	int i;

	i = model_id;

	wptr->your_gun[0].current_sequence = 2;
	wptr->your_gun[0].current_frame = 13;
	wptr->your_gun[1].current_sequence = 2;
	wptr->your_gun[1].current_frame = 13;
	wptr->your_gun[2].current_sequence = 0;
	wptr->your_gun[2].current_frame = 0;
		
	wptr->pmdata[i].sequence_start_frame[0] =0;  // Active1
	wptr->pmdata[i].sequence_stop_frame [0] =10;

	wptr->pmdata[i].sequence_start_frame[1] =11; // fire
	wptr->pmdata[i].sequence_stop_frame [1] =12;

	wptr->pmdata[i].sequence_start_frame[2] =13; // Idle
	wptr->pmdata[i].sequence_stop_frame [2] =39;

	wptr->pmdata[i].sequence_start_frame[3] =40; // putaway
	wptr->pmdata[i].sequence_stop_frame [3] =44;

	wptr->pmdata[i].sequence_start_frame[4] =45; // reload
	wptr->pmdata[i].sequence_stop_frame [4] =63;

	wptr->pmdata[i].sequence_start_frame[5] =64; // burstfire
	wptr->pmdata[i].sequence_stop_frame [5] =69;

	wptr->pmdata[i].sequence_start_frame[6] =70; // lastround
	wptr->pmdata[i].sequence_stop_frame [6] =71;

}

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
		PrintMessage(hwnd, "ERROR can't load ", filename, SCN_AND_FILE);
		MessageBox(hwnd, filename, "Error can't load file",MB_OK);
		return FALSE;
    }

	PrintMessage(hwnd, "Loading ", filename, SCN_AND_FILE);
	
	
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
			PrintMessage(hwnd, buffer, " Unrecognised command" ,SCN_AND_FILE);
			return FALSE;
		}
	}
	fclose(fp);	
	
	itoa(wptr->obdata_length, buffer, 10);	
	PrintMessage(hwnd, buffer, " DAT objects loaded" ,SCN_AND_FILE);

	memory_counter = memory_counter / 1024;
	itoa(memory_counter, buffer, 10);
	strcat(buffer, " KB\n");
	PrintMessage(hwnd, "memory allocated for DAT objects = ", buffer, LOGFILE_ONLY);
	
	return TRUE;
}


BOOL LoadWorldMap(HWND hwnd,char *filename,world_ptr wptr)
{
	FILE *fp;    
	char s[81];
	char p[100];
	char buffer[100];
	int y_count=30;
	int done=0;
	int object_count=0;
	int vert_count=0;
	int pv_count=0;
	int poly_count=0;
	int object_id;
	int lit_vert;
	int red,green,blue;
	int i;
	int num_lverts;
	int mem_counter = 0;
	BOOL lwm_start_flag = TRUE;
	

    fp = fopen(filename,"r");
    if(fp==NULL)
    {     
		PrintMessage(hwnd, "Error can't load file ", filename, SCN_AND_FILE);
        MessageBox(hwnd,"Error can't load file",NULL,MB_OK);
		return FALSE;
    }

	PrintMessage(hwnd, "Loading map ", filename, SCN_AND_FILE);

	while(done==0)
	{
		fscanf( fp, "%s", &s );

		if(strcmp(s,"OBJECT")==0)
		{
			fscanf( fp, "%s", &p );
			
			object_id = CheckObjectId(hwnd, (char *)&p, wptr);
			if(object_id == -1)
			{
				PrintMessage(hwnd, "Error Bad Object ID in: LoadWorld ", p, SCN_AND_FILE);
				MessageBox(hwnd,"Error Bad Object ID in: LoadWorld",NULL,MB_OK);
				return FALSE;
			}
			if(lwm_start_flag == FALSE)
				object_count++;

			wptr->oblist[object_count].type = object_id;
			
			num_lverts = wptr->num_vert_per_object[object_id];
			wptr->oblist[object_count].lit = new LIGHT[num_lverts];
			mem_counter += sizeof(LIGHT) * num_lverts;

			for(i = 0; i < num_lverts; i++)
			{
				wptr->oblist[object_count].lit[i].r = 0;
				wptr->oblist[object_count].lit[i].g = 0;
				wptr->oblist[object_count].lit[i].b = 0;
			}

			lwm_start_flag = FALSE;
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

		if(strcmp(s,"END_FILE")==0)
		{
			fscanf( fp, "%s", &p );

			wptr->oblist_length = object_count + 1;
			done = 1;
		}
	}
	fclose(fp);	
	
	itoa(wptr->oblist_length, buffer, 10);
	PrintMessage(hwnd, buffer, " map objects loaded", SCN_AND_FILE);

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
	PrintMessage(hwnd, buffer2, "ERROR bad ID in : CheckObjectId ",SCN_AND_FILE);
	MessageBox(hwnd,buffer2,"Bad ID in :CheckObjectId", MB_OK);
		
	return -1; //error
}

setupinfo_ptr LoadSetupInfo(HWND hwnd)
{
	FILE  *fp;    
	char  s[100];
	char  p[100];
	char  q[100];
	int done=0;
	BOOL mode_error			= TRUE;
	BOOL screen_error		= TRUE;
	BOOL control_error		= TRUE;
	BOOL sound_error		= TRUE;
	BOOL device_error		= TRUE;
	BOOL texture_size_error = TRUE;

	setupinfo.vmode.bpp = 16;
	setupinfo.screen = FULLSCREEN_ONLY;

    fp = fopen("rrsetup.ini","r");
    
	if(fp==NULL)    
	{
		PrintMessage(hwnd,"ERROR Can't load rrsetup.ini", NULL ,SCN_AND_FILE);
		Msg("error in rrsetup.ini");
		return NULL;
	}
       
	PrintMessage(hwnd, "Loading rrsetup.ini", NULL, LOGFILE_ONLY);

	while(done==0)
	{
		fscanf( fp, "%s", &s );

		if(strcmpi(s,"SCREEN")==0)
		{
			fscanf( fp, "%s", &p );
		
			if(strcmpi(p,"FULLSCREEN_ONLY")==0)	
			{
				setupinfo.screen = FULLSCREEN_ONLY;
				screen_error = FALSE;
			}
			
			if(strcmpi(p,"MAXIMISED_WINDOW")==0)
			{
				setupinfo.screen = MAXIMISED_WINDOW;
				screen_error = FALSE;
			}
			if(screen_error == TRUE)
				return NULL;
			
		}

		if(strcmpi(s,"MODE")==0)
		{
			fscanf( fp, "%s", &p );
			fscanf( fp, "%s", &q );	

			if( (strcmpi(p,"640")==0) && (strcmp(q,"480")==0) )
			{
				setupinfo.vmode.w = 640;
				setupinfo.vmode.h = 480;
				mode_error=FALSE;	
			}

			if( (strcmpi(p,"800")==0) && (strcmp(q,"600")) )
			{
				setupinfo.vmode.w = 800;
				setupinfo.vmode.h = 600;
				mode_error=FALSE;
			}

			if( (strcmpi(p,"320")==0) && (strcmp(q,"200")) )
			{
				setupinfo.vmode.w = 320;
				setupinfo.vmode.h = 200;
				mode_error=FALSE;
			}

			if(mode_error == TRUE)
			{
				Msg("ERROR in rrsetup.ini :  Mode");
				return NULL;
			}
		}


		if(strcmpi(s,"TEXTURE_SIZE")==0)
		{
			fscanf( fp, "%s", &p );

			if(strcmpi(p,"AUTO")==0)
			{
				setupinfo.TextureSize = 0;
				texture_size_error = FALSE;
			}

			if(strcmpi(p,"512")==0)
			{
				setupinfo.TextureSize = 512;
				texture_size_error = FALSE;
			}

			if(strcmpi(p,"256")==0)
			{
				setupinfo.TextureSize = 256;
				texture_size_error = FALSE;
			}

			if(strcmpi(p,"128")==0)	
			{
				setupinfo.TextureSize = 128;
				texture_size_error = FALSE;
			}

			if(strcmpi(p,"64")==0)	
			{
				setupinfo.TextureSize = 64;
				texture_size_error = FALSE;
			}

			if(strcmpi(p,"32")==0)	
			{
				setupinfo.TextureSize = 32;
				texture_size_error = FALSE;
			}

			if(texture_size_error == TRUE)
				return NULL;
		}

		if(strcmpi(s,"CONTROL")==0)
		{
			fscanf( fp, "%s", &p );

			if(strcmpi(p,"JOYSTICK")==0)
			{
				setupinfo.control = JOYSTICK;
				control_error = FALSE;
			}
			

			if(strcmpi(p,"KEYBOARD")==0)
			{
				setupinfo.control = KEYBOARD; 
				control_error = FALSE;
			}
			if(control_error == TRUE)
				return NULL;
		}
		
		if(strcmpi(s,"SOUND")==0)
		{
			fscanf( fp, "%s", &p );

			if(strcmpi(p,"ON")==0)
			{
				setupinfo.sound = TRUE; // sound On
				sound_error = FALSE;
			}

			if(strcmpi(p,"OFF")==0)
			{
				setupinfo.sound = FALSE; // Sound Off
				sound_error = FALSE;
			}
			if(sound_error == TRUE)
				return NULL;
		}


		if(strcmpi(s,"3DCARD") == 0)
		{
			fscanf( fp, "%s", &p );

			if(strcmpi(p,"PRIMARY") == 0)
			{
				d3dcard_driver_flag = USE_PRIMARY_D3DCARD;
				device_error = FALSE;
			}

			if(strcmpi(p,"SECONDARY") == 0)
			{
				d3dcard_driver_flag = USE_SECONDARY_D3DCARD;
				device_error = FALSE;
			}

			if(device_error == TRUE)
				return NULL;
		}

		if(strcmpi(s,"END_FILE")==0)
		{
			fscanf( fp, "%s", &p );				
			done = 1;
		}
	}
	fclose(fp);	
	return &setupinfo;
}


BOOL InitPreCompiledWorldMap(HWND hwnd, char *filename, world_ptr wptr)
{
	FILE *fp;
	int w;
	int done = 0;
	int count;
	int cell_x, cell_z;
	int mem_count;
	char  s[100], buffer[256];


	PrintMessage(hwnd, "InitPreCompiledWorldMap: starting", NULL, LOGFILE_ONLY);
		
	fp = fopen(filename,"r");
    if(fp==NULL)
    {     
		PrintMessage(hwnd,"Error can't load ", filename, SCN_AND_FILE);
        MessageBox(hwnd,"Error can't load file",NULL,MB_OK);
		return FALSE;
	}

	for(cell_z = 0; cell_z < MAX_NUM_Z_MAP_CELLS; cell_z++)
	{
		for(cell_x = 0; cell_x < MAX_NUM_X_MAP_CELLS; cell_x++)
		{
			wptr->cell[cell_x][cell_z] = NULL;
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
			PrintMessage(hwnd, "InitPreCompiledWorldMap: sucess", NULL, LOGFILE_ONLY);
			
			mem_count = mem_count / 1024;
			itoa(mem_count, buffer, 10);
			strcat(buffer, " KB\n");
			PrintMessage(hwnd, "memory allocated for map = ", buffer, LOGFILE_ONLY);
	
			return TRUE;
		}

		cell_x = atoi(s);

		if((cell_x >=0) && (cell_x <= MAX_NUM_X_MAP_CELLS))
		{
			fscanf( fp, "%s", &s );
			cell_z = atoi(s);

			if((cell_z >=0) && (cell_z <= MAX_NUM_Z_MAP_CELLS))
			{
				fscanf( fp, "%s", &s );
				count = atoi(s);
			
				wptr->cell[cell_x][cell_z] = new short[count];
				mem_count = mem_count + (count * sizeof(short));
							
				for(w=0;w<count;w++)
				{
					fscanf( fp, "%s", &s );
					wptr->cell[cell_x][cell_z][w] = atoi(s);						
				}
				
				wptr->cell_length[cell_x][cell_z] = count;
			}
		}
		
	} // end while		

	return FALSE;
}

// InitWorldMap - old map loading routine not used anymore
// doesn't make use of pre compiled map data
// but loads in the .mpa file and works out
// what cells every map objects is in.

BOOL InitWorldMap(HWND hwnd, char *filename, world_ptr wptr)
{
	FILE *fp;
	int i,j,w;
	int cnt=0;
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
	BOOL map[200][200];
	char buffer[50];

	float x,y,z;
	float wx,wy,wz;
	float tx,ty,tz;
	float sine,cosine;
	float last_rot_angle=(float)0;
	float k=(float)0.017453292;
	float sin_table[361];
	float cos_table[361];
	float fangle;


	fp = fopen(filename,"w");
    if(fp==NULL)
    {     
		PrintMessage(hwnd, "Error can't load ", filename, SCN_AND_FILE);
        MessageBox(hwnd,"Error can't load file",NULL,MB_OK);
		return FALSE;
    }

	PrintMessage(hwnd, "Processing Map Info", NULL, SCN_AND_FILE);

	for(i=0;i<360;i++)
	{
		fangle = (float)i * k;
		sin_table[i] = (float)sin(fangle);
		cos_table[i] = (float)cos(fangle);
	}
	
	poly_cnt=0;
	j=0;

	for(i=0;i<=wptr->oblist_length;i++)
	{	
		wx = wptr->oblist[i].x;
		wy = wptr->oblist[i].y;
		wz = wptr->oblist[i].z;
		
		angle = (int)wptr->oblist[i].rot_angle;

		cosine = cos_table[angle];
		sine   = sin_table[angle];
		
		ob_type = wptr->oblist[i].type;
		
		for(cz=0;cz<200;cz++)
		{
			for(cx=0;cx<200;cx++)
				map[cx][cz] = FALSE;
		}

		j=0;
		cell_count=0;
		cell_length[i] = cell_count;
		poly=wptr->num_polys_per_object[ob_type];

		for(w=0; w<poly; w++)
		{
			num_vert = wptr->obdata[ob_type].num_vert[w];

			for(vert_cnt=0; vert_cnt < num_vert; vert_cnt++)
			{
				x = wptr->obdata[ob_type].v[j].x; 
				y = wptr->obdata[ob_type].v[j].y;
				z = wptr->obdata[ob_type].v[j].z;
				
				j++;

				tx = wx + (x*cosine - z*sine); 
				ty = wy + y;
				tz = wz + (x*sine + z*cosine);

				cell_x = (int)(tx/(float)256);
				cell_z = (int)(tz/(float)256);
				
				if(map[cell_x][cell_z] == FALSE)
				{
					if(cell_count<MAX_NUM_OBJECTS_PER_CELL)
					{
						cell_xlist[i][cell_count] = cell_x;
						cell_zlist[i][cell_count] = cell_z;
						cell_count++;
						cell_length[i] = cell_count;
						map[cell_x][cell_z] = TRUE;
					}
				}
				
			}
			poly_cnt++;

		} // end for w
	} // end for i
	
	//////////////////////////////////////////////////////
	
	PrintMessage(hwnd, "Map size : 200 x 200 cells", NULL,SCN_AND_FILE);
	PrintMessage(hwnd, "Loading map data into cells", NULL,SCN_AND_FILE);

	for(cz=0;cz<200;cz++)
	{
		for(cx=0;cx<200;cx++)
		{
			count=0;

			for(i=0;i<=wptr->oblist_length;i++)
			{
				length = cell_length[i];

				for(j=0;j<length;j++)
				{
					if(cell_xlist[i][j] == cx)
					{
						if(cell_zlist[i][j] == cz)
						{
							if(count<MAX_NUM_OBJECTS_PER_CELL)
							{
								temp[count]=i;
								count++;
							}
						}
					}
				}
			}

			if(count>0)
			{
				wptr->cell[cx][cz] = new short[count];
				mem_count = mem_count + (count* sizeof(short));

				itoa(cx, buffer,10);
				fprintf( fp, "%s", buffer );

				itoa(cz, buffer,10);
				fprintf( fp, " %s", buffer );

				itoa(count, buffer,10);
				fprintf( fp, "  %s ", buffer );
								
				for(w=0;w<count;w++)
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

	return TRUE;
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



