
// Road Rage version 1.1

// Written by William Chin
// Developed in MSDEV C++ v6.0 using the DirectX 6.0 SDK
// Date : 3/7/99
// File : Import3DS.cpp


#include "Import3DS.hpp"
#include "windows.h"
#include <stdio.h>
#include "world.hpp"
#include <math.h>


#define  MAX_NUM_3DS_TRIANGLES	5000
#define  MAX_NUM_3DS_VERTICES	5000
#define  MAX_NUM_3DS_FACES		3000
#define  MAX_NAME_LENGTH		256
#define  MAX_NUM_3DS_TEXTURES	100
#define  MAX_NUM_3DS_FRAMES		50
#define  MAX_NUM_3DS_OBJECTS    500


#define MAIN3DS							0x4D4D	// level 1
#define M3DS_VERSION					0x0002
#define MESH_VERSION					0x3D3E

#define	EDIT3DS							0x3D3D  // level 1
#define	  NAMED_OBJECT					0x4000	// level 2
#define		TRIANGLE_MESH				0x4100	// level 3
#define		  TRIANGLE_VERTEXLIST		0x4110	// level 4
#define		  TRIANGLE_VERTEXOPTIONS	0x4111	// level 4
#define		  TRIANGLE_MAPPINGCOORS		0x4140	// level 4
#define		  TRIANGLE_MAPPINGSTANDARD	0x4170  // level 4
#define		  TRIANGLE_FACELIST			0x4120	// level 4
#define			TRIANGLE_SMOOTH         0x4150  // level 5     
#define			TRIANGLE_MATERIAL       0x4130  // level 5 
#define		  TRI_LOCAL					0x4160	// level 4
#define		  TRI_VISIBLE				0x4165	// level 4
  
#define  	INT_PERCENTAGE				0x0030 
#define  	MASTER_SCALE				0x0100 

#define	  EDIT_MATERIAL					0xAFFF	// level 2
#define		MAT_NAME01					0xA000  // level 3

#define		  TEXTURE_MAP				0xA200  // level 4?
#define			MAPPING_NAME			0xA300  // level 5?

 
#define         MATERIAL_AMBIENT		0xA010
#define			MATERIAL_DIFFUSE		0xA020
#define			MATERIAL_SPECULAR		0xA030
#define			MATERIAL_SHININESS		0xA040
#define			MATERIAL_SHIN_STRENGTH	0xA041

#define			MAPPING_PARAMETERS		0xA351
#define			BLUR_PERCENTAGE			0xA353

#define			TRANS_PERCENT			0xA050
#define			TRANS_FALLOFF_PERCENT	0xA052
#define			REFLECTION_BLUR_PER		0xA053
#define			RENDER_TYPE				0xA100

#define			SELF_ILLUM				0xA084
#define			WIRE_THICKNESS			0xA087
#define			IN_TRANC				0xA08A
#define			SOFTEN					0xA08C
     
#define	KEYFRAME						0xB000	// level 1
#define	  KEYFRAME_MESH_INFO			0xB002
#define   KEYFRAME_START_AND_END		0xB008
#define	  KEYFRAME_HEADER				0xb00a



#define  	POS_TRACK_TAG	0xb020 
#define  	ROT_TRACK_TAG	0xb021 
#define  	SCL_TRACK_TAG	0xb022 
#define		FOV_TRACK_TAG	0xb023
#define		ROLL_TRACK_TAG	0xb024	
#define		COL_TRACK_TAG  	0xb025
#define		MORPH_TRACK_TAG 0xb026
#define		HOT_TRACK_TAG  	0xb027
#define		FALL_TRACK_TAG  0xb028
#define		HIDE_TRACK_TAG  0xb029
 	


#define  	PIVOT			0xb013 

#define  	NODE_HDR		0xb010 
#define  	NODE_ID			0xb030 
#define  	KFCURTIME		0xb009 


typedef struct 
{
	float x;
	float y;
	float z;
	
} VERT3DS;

typedef struct 
{
	short v[4];
	int tex;
	
} FACE3DS;

typedef struct 
{
	float x;
	float y;

} MAPPING_COORDINATES;


typedef struct 
{
	VERT3DS verts[3];
	MAPPING_COORDINATES texmapcoords[3];
	short material;
	short tex_alias;
	char mat_name[MAX_NAME_LENGTH];

} TRIANGLE3DS;



typedef struct 
{
	short	framenum;
	long	lunknown;	
	float	rotation_rad;
	float	axis_x;	 
	float	axis_y;	 
	float	axis_z;	

} ROT_TRACK3DS;

typedef struct 
{
	short	framenum;
	long	lunknown;	
	float	pos_x;	 
	float	pos_y;	 
	float	pos_z;	

} POS_TRACK3DS;

typedef struct 
{
	short framenum;
	long  lunknown;
	float scale_x; 
	float scale_y; 
	float scale_z;

} SCL_TRACK3DS;



typedef struct 
{
	int verts_start;
	int verts_end;
	short rotkeys;
	short poskeys;
	short sclkeys;
	VERT3DS pivot; 
	ROT_TRACK3DS rot_track[MAX_NUM_3DS_FRAMES];
	POS_TRACK3DS pos_track[MAX_NUM_3DS_FRAMES];
	SCL_TRACK3DS scl_track[MAX_NUM_3DS_FRAMES];

	float local_centre_x;
	float local_centre_y;
	float local_centre_z;

} OBJECT3DS;



OBJECT3DS oblist[MAX_NUM_3DS_OBJECTS];

int kf_count = 0;

short pt_flags;
short unknown;
unsigned long kfstart, kfend;
unsigned long kfcurframe;


int total_num_objects;
char object_names[MAX_NUM_3DS_OBJECTS][MAX_NAME_LENGTH];
BOOL command_found_flag;
BOOL loading_first_model_flag = TRUE;

int num_materials = 0;
int num_maps = 0;
int total_num_faces;
int total_num_verts;
int last_num_verts;
int last_num_faces;
int last_num_mcoords;
int total_num_mcoords;
int total_num_frames = 40;


FACE3DS				faces	 [MAX_NUM_3DS_FACES];
float				fverts	 [MAX_NUM_3DS_VERTICES][3];
MAPPING_COORDINATES mcoords	 [MAX_NUM_3DS_VERTICES]; 
short				object_texture[10000];

char material_list[MAX_NUM_3DS_TEXTURES][MAX_NAME_LENGTH];
char mapnames     [MAX_NUM_3DS_TEXTURES][MAX_NAME_LENGTH];

int					num_verts_in_object[10000];
int					num_faces_in_object[10000];


FILE *logfile;
FILE *fp_3dsmodel;

// function prototypes

BOOL ProcessVertexData(HWND hwnd, FILE *fp);
BOOL ProcessFaceData(HWND hwnd, FILE *fp);
void ProcessTriLocalData(FILE *fp);
void ProcessTriSmoothData(FILE *fp);
void ProcessMappingData(HWND hwnd, FILE *fp);
void ProcessMaterialData(HWND hwnd, FILE *fp, int pmodel_id, world_ptr wptr);

void AddMaterialName(HWND hwnd, FILE *fp);
void AddMapName(HWND hwnd, FILE *fp, int pmodel_id, world_ptr wptr);

void ProcessPositionTrack(HWND hwnd, FILE *fp);
void ProcessRotationTrack(HWND hwnd, FILE *fp);
void ProcessPivots(HWND hwnd, FILE *fp);
void ProcessScaleTrack(HWND hwnd, FILE *fp);

void ProcessNodeHeader(FILE *fp);
void ProcessNodeId(FILE *fp);

void ProcessMasterScale(FILE *fp);
void Process3DSVersion(FILE *fp);

void PrintLogFile(FILE *logfile, char *commmand);
void Write_pmdata_debugfile(HWND hwnd,  int pmodel_id, world_ptr wptr);
void ReleaseTempMemory();




BOOL Import3DS(HWND hwnd, char *filename, int pmodel_id, float scale, world_ptr wptr)
{
	FILE *fp; 
	FILE *fp_3dsmodel;
	FILE *rrlogfile;
	int done;
	int i,j,cnt;
	int length;
	int	data_length;
	int frame_num;
	int num_frames;
	int datfile_vert_cnt;
	int quad_cnt=0;
	unsigned short	command;
	char temp;
	float tx, ty, tz;
	float angle;
	float x,y,z;
	BOOL process_data_flag;
	char datfilename[1024];
	int file_ex_start = 0;


	total_num_objects = -1;
	kf_count = -1;
	total_num_faces = 0;
	total_num_verts = 0;
	last_num_faces = 0;
	last_num_verts = 0;
	num_maps = 0;
	num_materials = 0;
	last_num_mcoords = 0; 
	total_num_mcoords = 0;
	num_materials = 0;

	if ((fp = fopen(filename, "rb")) == 0) 
	{
		MessageBox(hwnd,"3DS File error : Can't open", filename, MB_OK);
		return FALSE;
	}


	if ((logfile = fopen("3dslogfile.txt", "w")) == 0) 
	{
		MessageBox(hwnd,"logfile error : Can't create", "3dslogfile.txt", MB_OK);
		return FALSE;
	}
	
	fprintf(logfile, "3DS Logfile generated by D3DS viewer\n");
	fprintf(logfile, "3DS Model filename %s\n\n", filename);


	done = 0;

	while (done == 0)
	{
		process_data_flag  = FALSE;
		command_found_flag = FALSE;

		fread(&command, sizeof(command), 1, fp);
		
		// Test for end of file

		if (feof(fp))
		{
			done = 1;
			break;
		}
		
		fread (&length, sizeof(length), 1, fp);
		data_length = length - 6;

		// Process 3DS file commands


		switch (command)
		{
			case TRIANGLE_MESH:
				PrintLogFile(logfile, "TRIANGLE_MESH");
				process_data_flag = TRUE;	
				break;

			case TRIANGLE_VERTEXLIST: 
				if(ProcessVertexData(hwnd, fp) == TRUE)
					process_data_flag = TRUE;
				else
				{
					MessageBox(hwnd,"Error : Too many verts", NULL, MB_OK);
					return FALSE;
				}
				break;

			case TRIANGLE_FACELIST: 
				if(ProcessFaceData(hwnd, fp) == TRUE)
					process_data_flag = TRUE;
				else
				{
					MessageBox(hwnd,"Error : Too many faces", NULL, MB_OK);
					return FALSE;
				}
				break;
			
			case TRIANGLE_MAPPINGCOORS:
				ProcessMappingData(hwnd, fp);
				process_data_flag = TRUE;
				break;

			case TRIANGLE_MATERIAL:
				ProcessMaterialData(hwnd, fp, pmodel_id, wptr);
				process_data_flag = TRUE;
				break;

			case EDIT_MATERIAL: 
				PrintLogFile(logfile, "\nEDIT_MATERIAL");	
				process_data_flag = TRUE;
				break;

			case MAT_NAME01:
				AddMaterialName(hwnd, fp);
				process_data_flag = TRUE;
				break;

			case TEXTURE_MAP:
				PrintLogFile(logfile, "TEXTURE_MAP");	
				process_data_flag = TRUE;
				break;
				
			case MAPPING_NAME:
				AddMapName(hwnd, fp, pmodel_id, wptr);
				process_data_flag = TRUE;
				break;
			
			case NAMED_OBJECT: 
				
				// read name and store
				
				total_num_objects++;

				for(i = 0; i < MAX_NAME_LENGTH; i++)
				{
					fread (&temp, 1, 1, fp); 
					
					if(total_num_objects >= MAX_NUM_3DS_OBJECTS )
						return FALSE;

					object_names[total_num_objects][i] = temp;
					data_length--;
				
					if(temp == 0)
					{
						fprintf(logfile, "\n\n%s %s\n", "NAMED_OBJECT ", 
							object_names[total_num_objects]);
		
						break;
					}
				}

				process_data_flag = TRUE;
				break;

			case MAIN3DS:
				PrintLogFile(logfile, "MAIN3DS");
				process_data_flag = TRUE;	
				break;

			case EDIT3DS:
				PrintLogFile(logfile, "EDIT3DS");
				process_data_flag = TRUE;	
				break;


			case KEYFRAME:
				PrintLogFile(logfile, "\n\nKEYFRAME");
				process_data_flag = TRUE;
				break;
			
			case KEYFRAME_MESH_INFO:
				PrintLogFile(logfile, "\nKEYFRAME_MESH_INFO");
				kf_count++;
				process_data_flag = TRUE;	
				break;

			case KEYFRAME_START_AND_END:
				PrintLogFile(logfile, "KEYFRAME_START_AND_END");
				fread(&kfstart, sizeof(unsigned long), 1, fp);
				fread(&kfend,	sizeof(unsigned long), 1, fp);
				process_data_flag = TRUE;	
				break;
				
			case KEYFRAME_HEADER:
				PrintLogFile(logfile, "KEYFRAME_HEADER");
				//process_data_flag = TRUE;	
				break;

			case KFCURTIME:
				fread(&kfcurframe, sizeof(long), 1, fp);
				fprintf(logfile, "KFCURTIME  %d\n", kfcurframe);
				process_data_flag = TRUE;	
				break;

			case PIVOT:
				ProcessPivots(hwnd, fp);
				process_data_flag = TRUE;	
				break;

			case POS_TRACK_TAG:  
				ProcessPositionTrack(hwnd, fp);
				process_data_flag = TRUE;	
				break;

			case ROT_TRACK_TAG:  
				ProcessRotationTrack(hwnd, fp);
				process_data_flag = TRUE;	
				break;
		
			case SCL_TRACK_TAG:
				ProcessScaleTrack(hwnd, fp);
				process_data_flag = TRUE;	
				break;
				
			case FOV_TRACK_TAG:	
				PrintLogFile(logfile, "PIVOT");
				//process_data_flag = TRUE;	
				break;

			case ROLL_TRACK_TAG:
				PrintLogFile(logfile, "PIVOT");
				//process_data_flag = TRUE;	
				break;

			case COL_TRACK_TAG:
				PrintLogFile(logfile, "PIVOT");
				//process_data_flag = TRUE;	
				break;

			case MORPH_TRACK_TAG:
				PrintLogFile(logfile, "PIVOT");
				//process_data_flag = TRUE;	
				break;

			case HOT_TRACK_TAG:
				PrintLogFile(logfile, "PIVOT");
				//process_data_flag = TRUE;	
				break;

			case FALL_TRACK_TAG:
				PrintLogFile(logfile, "PIVOT");
				//process_data_flag = TRUE;	
				break;

			case HIDE_TRACK_TAG:
				PrintLogFile(logfile, "PIVOT");
				//process_data_flag = TRUE;	
				break;

			
			case NODE_HDR:
				ProcessNodeHeader(fp);
				process_data_flag = TRUE;	
				break;

			case NODE_ID:
				ProcessNodeId(fp);
				process_data_flag = TRUE;	
				break;
				

			case M3DS_VERSION:
				Process3DSVersion(fp);
				process_data_flag = TRUE;
				break;

			case MESH_VERSION:
				PrintLogFile(logfile, "MESH_VERSION");
				break;
				
			case INT_PERCENTAGE:
				PrintLogFile(logfile, "INT_PERCENTAGE");	
				break;

			case MASTER_SCALE:
				ProcessMasterScale(fp);
				process_data_flag = TRUE;
				break;		

			case TRIANGLE_MAPPINGSTANDARD:
				PrintLogFile(logfile, "TRIANGLE_MAPPINGSTANDARD");
				break;	

			case TRIANGLE_VERTEXOPTIONS:
				PrintLogFile(logfile, "TRIANGLE_VERTEXOPTIONS");
				break;
	
			case TRIANGLE_SMOOTH:
				PrintLogFile(logfile, "TRIANGLE_SMOOTH");
				ProcessTriSmoothData(fp);
				process_data_flag = TRUE;	
				break;

			case TRI_LOCAL:
				PrintLogFile(logfile, "TRI_LOCAL");
				ProcessTriLocalData(fp);
				process_data_flag = TRUE;	
				break;


			// Skipping these commands / chunks


			case TRI_VISIBLE:
				PrintLogFile(logfile, "TRI_VISIBLE");
				break;

			case MATERIAL_AMBIENT:
				PrintLogFile(logfile, "MATERIAL_AMBIENT");
				break;

			case MATERIAL_DIFFUSE:
				PrintLogFile(logfile, "MATERIAL_DIFFUSE");
				break;

			case MATERIAL_SPECULAR:
				PrintLogFile(logfile, "MATERIAL_SPECULAR");
				break;

			case MATERIAL_SHININESS:
				PrintLogFile(logfile, "MATERIAL_SHININESS");
				break;

			case MATERIAL_SHIN_STRENGTH:
				PrintLogFile(logfile, "MATERIAL_SHIN_STRENGTH");
				break;
		
			case MAPPING_PARAMETERS:
				PrintLogFile(logfile, "MAPPING_PARAMETERS");
				break;
				
			case BLUR_PERCENTAGE:
				PrintLogFile(logfile, "BLUR_PERCENTAGE");
				break;

			case TRANS_PERCENT:
				PrintLogFile(logfile, "TRANS_PERCENT");
				break;
				
			case TRANS_FALLOFF_PERCENT:
				PrintLogFile(logfile, "TRANS_FALLOFF_PERCENT");
				break;

			case REFLECTION_BLUR_PER:
				PrintLogFile(logfile, "REFLECTION_BLUR_PER");
				break;
				
			case RENDER_TYPE:
				PrintLogFile(logfile, "RENDER_TYPE");
				break;
     
			case SELF_ILLUM:
				PrintLogFile(logfile, "SELF_ILLUM");
				break;
				
			case WIRE_THICKNESS:
				PrintLogFile(logfile, "WIRE_THICKNESS");
				break;

			case IN_TRANC:
				PrintLogFile(logfile, "IN_TRANC");
				break;
				
			case SOFTEN:
				PrintLogFile(logfile, "SOFTEN");
				break;

			break;
		} // end switch


		if(process_data_flag == FALSE)
		{
			if(command_found_flag == FALSE)
			{
				fprintf(logfile, "\n");
				fprintf(logfile, "%s  %x\n", "UNKNOWN COMMAND ", command);
			}

			// command was unrecognised, so skip it's data
			
			for(i = 0; i < data_length; i++)
			{
				fread (&temp, 1, 1, fp);
			}
			data_length = 0;
		}

	} // end while
	
	fclose (fp);
	

	total_num_objects++;


	// Transfer triangle list into the pmdata structure

	// allocate memory dynamically
	
	num_frames = MAX_NUM_3DS_FRAMES;

	wptr->pmdata[pmodel_id].w = new VERT*[num_frames];
	
	for(i = 0; i < num_frames; i++)
		wptr->pmdata[pmodel_id].w[i] = new VERT[total_num_verts];

	wptr->pmdata[pmodel_id].f = new  short[total_num_faces * 4];
	wptr->pmdata[pmodel_id].num_verts_per_object = new  short[total_num_objects];
	wptr->pmdata[pmodel_id].num_faces_per_object = new  short[total_num_objects];
	wptr->pmdata[pmodel_id].poly_cmd = new  short[total_num_objects];
	wptr->pmdata[pmodel_id].texture_list = new  short[total_num_objects];
	wptr->pmdata[pmodel_id].t = new VERT[total_num_faces * 4];

	int mem = 0;
	mem += (sizeof(VERT)  * total_num_verts * num_frames);
	mem += (sizeof(short) * total_num_faces * 4);
	mem += (sizeof(short) * total_num_objects);
	mem += (sizeof(short) * total_num_objects);
	mem += (sizeof(short) * total_num_objects);
	mem += (sizeof(short) * total_num_objects);
	mem += (sizeof(VERT)  * total_num_faces * 4);
	mem = mem /1024;

	rrlogfile = fopen("rrlogfile.txt","a");
	
	fprintf(rrlogfile, "%s%d", "objects = ",total_num_objects );
	fprintf(rrlogfile, "%s%d",     "  verts = ",total_num_verts ); 
	fprintf(rrlogfile, "%s%d\n",   "  faces = ",total_num_faces ); 	
	fprintf(rrlogfile, "memory allocated %d %s\n\n", mem, "KB");

	fclose(rrlogfile);

	fprintf(logfile, "\n\n%s %d\n", "num 3ds objects = ",total_num_objects);
	fprintf(logfile, "%s %d\n",     "total num verts = ",total_num_verts ); 
	fprintf(logfile, "%s %d\n",     "total num faces = ",total_num_faces ); 	
	fprintf(logfile, "memory allocated for 3ds model %d %s\n\n", mem, "KB");



	strcpy(datfilename, filename);

	for(i = 0; i < 1024; i++)
	{
		if(datfilename[i] == '.')
		{
			if(datfilename[i+1] == '.')
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

	if(file_ex_start == 0)
	{
		MessageBox(hwnd,"datfilename error :", NULL,  MB_OK);
		return FALSE;
	}

	strcpy(&datfilename[file_ex_start],".dat");


	if ((fp_3dsmodel = fopen(datfilename, "w+")) == 0) 
		MessageBox(hwnd,"logfile error : Can't open", "3dsmodel.txt", MB_OK);

	datfile_vert_cnt = 0;
	frame_num = 0;
	

	for(i = 0; i < total_num_verts; i++)
	{		
		// 3DS to DAT CONVERSION for Road Rage 1.1  --------------------
		// This piece of code is only intended to work with 3ds models
		// that are entirely composed of rectangles or boxes.
		// Also the "QuadTex" dat command is only supported in the latest
		// version of Road Rage 11dp (24th June)

		if(datfile_vert_cnt == 0)
		{
			fprintf(fp_3dsmodel,"TEXTURE %s\n", wptr->TexMap[faces[quad_cnt].tex].tex_alias_name); // mapnames[faces[quad_cnt].tex]);
			fprintf(fp_3dsmodel, "QUADTEX ");
		}

		fprintf(fp_3dsmodel, "%f %f %f", 
				fverts[i][0],
				fverts[i][2],
				fverts[i][1]);

		fprintf(fp_3dsmodel, " %f %f\n", 
				mcoords[i].x,
				mcoords[i].y);

		datfile_vert_cnt++;

		if(datfile_vert_cnt > 3)
		{
			quad_cnt+=2;
			datfile_vert_cnt = 0;
			fprintf(fp_3dsmodel, "\n"); 
		}
		// end of DAT conversion -------------------------------------
	}
	
	int v, pos_keys, v_start, v_end;

	for(i = 0; i < total_num_objects; i++)
	{
		v_start  = oblist[i].verts_start;
		v_end    = oblist[i].verts_end;

		for(v = v_start; v <= v_end; v++)
		{
			fverts[v][0] -= oblist[i].pivot.x; 
			fverts[v][1] -= oblist[i].pivot.y;
			fverts[v][2] -= oblist[i].pivot.z;
		}
	}
	

	// copy verts into pmdata for all frames
	
	for(frame_num = 0; frame_num < total_num_frames; frame_num++)
	{
		for(i = 0; i < total_num_verts; i++)
		{
			wptr->pmdata[pmodel_id].w[frame_num][i].x  = fverts[i][0];
			wptr->pmdata[pmodel_id].w[frame_num][i].y  = fverts[i][1];
			wptr->pmdata[pmodel_id].w[frame_num][i].z  = fverts[i][2];
		}
	}



	// Apply position track data


	float pos_x, pos_y, pos_z;

	fprintf(logfile, "APPLYING POSITION TRACK DATA\n\n");

	for(i = 0; i < total_num_objects; i++)
	{
		v_start  = oblist[i].verts_start;
		v_end    = oblist[i].verts_end;
		pos_keys = oblist[i].poskeys;

		//fprintf(logfile, "i: %d  obs: %d  v_start: %d  v_end: %d  pos_keys: %d\n", 
		//				  i, total_num_objects, v_start, v_end, pos_keys);


		for(j = 0; j < pos_keys; j++)
		{
			frame_num = oblist[i].pos_track[j].framenum;

			//fprintf(logfile, "j: %d  frame_num: %d  total_num_frames: %d\n",
			//	j, frame_num, total_num_frames);
				
			if((j < total_num_frames) && (frame_num < total_num_frames))
			{
				pos_x = oblist[i].pos_track[j].pos_x - oblist[i].local_centre_x;
				pos_y = oblist[i].pos_track[j].pos_y - oblist[i].local_centre_y;
				pos_z = oblist[i].pos_track[j].pos_z - oblist[i].local_centre_z;

				//fprintf(logfile, "i: %d  v_start: %d  v_end: %d\n", i, v_start, v_end);

				for(v = v_start; v <= v_end; v++)
				{
					wptr->pmdata[pmodel_id].w[frame_num][v].x += pos_x;
					wptr->pmdata[pmodel_id].w[frame_num][v].y += pos_y;
					wptr->pmdata[pmodel_id].w[frame_num][v].z += pos_z;

					fprintf(logfile, "obj: %d  frame_num: %d  v: %d\n", i, frame_num, v);
				}
				fprintf(logfile, "\n");

			}
		}

	}

	fprintf(logfile, "\n\nAPPLYING ROTATION TRACK DATA\n\n");



	// Apply rotation track data

	int rot_keys;
	float axis_x, axis_y, axis_z, rot_angle;
	float rot_angle_x, rot_angle_y, rot_angle_z; 

	for(i = 0; i < total_num_objects; i++)
	{
		v_start  = oblist[i].verts_start;
		v_end    = oblist[i].verts_end;
		rot_keys = oblist[i].rotkeys;
		rot_angle = 0;

		for(j = 1; j < rot_keys; j++)
		{
			frame_num = oblist[i].rot_track[j].framenum;

			if((j < total_num_frames) && (frame_num < total_num_frames))
			{	
				axis_x = oblist[i].rot_track[j].axis_x; 
				axis_y = oblist[i].rot_track[j].axis_y; 
				axis_z = oblist[i].rot_track[j].axis_z;

				
				rot_angle += oblist[i].rot_track[j].rotation_rad;

				rot_angle_x = rot_angle * axis_x;
				rot_angle_y = rot_angle * axis_y;
				rot_angle_z = rot_angle * axis_z;
				
				fprintf(logfile, "local xyz: %f %f %f\n", 
					oblist[i].local_centre_x,
					oblist[i].local_centre_y,
					oblist[i].local_centre_z);

				for(v = v_start; v <= v_end; v++)
				{
					x = wptr->pmdata[pmodel_id].w[frame_num][v].x - oblist[i].local_centre_x; 
					y = wptr->pmdata[pmodel_id].w[frame_num][v].y - oblist[i].local_centre_y;
					z = wptr->pmdata[pmodel_id].w[frame_num][v].z - oblist[i].local_centre_z;

					tx = x * (float)cos(rot_angle_z) + y * (float)sin(rot_angle_z);
					ty = y * (float)cos(rot_angle_z) - x * (float)sin(rot_angle_z);
					tz = z;

					x = tx;
					y = ty;
					z = tz;

					tx = x;
					ty = y * (float)cos(rot_angle_x) + z * (float)sin(rot_angle_x);
					tz = z * (float)cos(rot_angle_x) - y * (float)sin(rot_angle_x);

					x = tx;
					y = ty;
					z = tz;

					tx = x * (float)cos(rot_angle_y) - z * (float)sin(rot_angle_y);
					ty = y; 
					tz = z * (float)cos(rot_angle_y) + x * (float)sin(rot_angle_y);

					
					

					fprintf(logfile, "i: %d  frame: %d  v: %d  angle: %f  ", i, frame_num, v, rot_angle);
					fprintf(logfile, "pxyz: %f %f %f  xyz: %f %f %f\n", x, y, z,
						wptr->pmdata[pmodel_id].w[frame_num][v].x,
						wptr->pmdata[pmodel_id].w[frame_num][v].y,
						wptr->pmdata[pmodel_id].w[frame_num][v].z);

					wptr->pmdata[pmodel_id].w[frame_num][v].x  = tx + oblist[i].local_centre_x;
					wptr->pmdata[pmodel_id].w[frame_num][v].y  = ty + oblist[i].local_centre_y;
					wptr->pmdata[pmodel_id].w[frame_num][v].z  = tz + oblist[i].local_centre_z;
				}
				fprintf(logfile, "\n");
				
			}
		}

	}


	fprintf(logfile, "\n\nAPPLYING SCALE TRACK DATA\n\n");


	// Scale and rotate verts for all frames

	for(frame_num = 0; frame_num < total_num_frames; frame_num++)
	{
		for(i = 0; i < total_num_verts; i++)
		{	
			x  =  scale * wptr->pmdata[pmodel_id].w[frame_num][i].x;
			y  =  scale * wptr->pmdata[pmodel_id].w[frame_num][i].y;
			z  =  scale * wptr->pmdata[pmodel_id].w[frame_num][i].z;

			angle = 0; 

			tx = x * (float)cos(angle) + z * (float)sin(angle);
			ty = y;
			tz = z * (float)cos(angle) - x * (float)sin(angle);

			wptr->pmdata[pmodel_id].w[frame_num][i].x  = tx;
			wptr->pmdata[pmodel_id].w[frame_num][i].y  = ty;
			wptr->pmdata[pmodel_id].w[frame_num][i].z  = tz;
		}
	}



	fclose(fp_3dsmodel);

	cnt = 0;


	for(i = 0; i < total_num_faces; i++)
	{
		for (j = 0; j < 3; j++)
		{
			wptr->pmdata[pmodel_id].f[cnt] = faces[i].v[j];	

			wptr->pmdata[pmodel_id].t[cnt].x = mcoords[cnt].x;
			wptr->pmdata[pmodel_id].t[cnt].y = mcoords[cnt].y;

			cnt++;
		}
	}
	
	for(i = 0; i < total_num_objects; i++)
	{
		wptr->pmdata[pmodel_id].poly_cmd[i] = (short)POLY_CMD_INDEXED_TRI;
		wptr->pmdata[pmodel_id].num_verts_per_object[i] = (short)num_verts_in_object[i];
		wptr->pmdata[pmodel_id].num_faces_per_object[i] = (short)num_faces_in_object[i];
		wptr->pmdata[pmodel_id].texture_list[i] = object_texture[i];
	}


	loading_first_model_flag = FALSE;

	fclose(logfile);

	wptr->pmdata[pmodel_id].num_polys_per_frame = total_num_objects;
	wptr->pmdata[pmodel_id].num_faces = total_num_faces;
	wptr->pmdata[pmodel_id].num_verts = total_num_verts; 
	wptr->pmdata[pmodel_id].num_frames = total_num_frames;

	wptr->pmdata[pmodel_id].skx = 1;
	wptr->pmdata[pmodel_id].sky = 1;
	wptr->pmdata[pmodel_id].tex_alias = 7;	
	wptr->pmdata[pmodel_id].use_indexed_primitive = TRUE;

	ReleaseTempMemory();
	
	return TRUE;
}



// PROCESS TRIANGLE DATA ROUTINES

BOOL ProcessVertexData(HWND hwnd, FILE *fp)
{
	int	i, j ;
	int temp_int;
	float p, temp_float;
	unsigned short num_vertices;

	last_num_verts = total_num_verts;
	oblist[total_num_objects].verts_start = total_num_verts;
	
	fread(&num_vertices, sizeof(num_vertices), 1, fp);

	fprintf(logfile, "%s %d\n", "TRIANGLE_VERTEXLIST", num_vertices);

	num_verts_in_object[total_num_objects] = (short)num_vertices;

	if((total_num_verts + num_vertices) >= MAX_NUM_3DS_VERTICES)
		return FALSE;
			
	for (i = 0; i < (int)num_vertices; i++)
	{
		for (j = 0; j < 3; j++)
		{
			fread(&p, sizeof(float), 1, fp);
			temp_int = (int)( ((p * (float)10000) + (float)0.5));
			temp_float = (float)temp_int / (float)10000;
			fverts[total_num_verts][j] = temp_float;
		}

		fprintf(logfile, " %f %f %f\n", 
			fverts[total_num_verts][0],
			fverts[total_num_verts][1],
			fverts[total_num_verts][2]);


		// set default mapping co-ordinates, in case none are defined in model
		mcoords[total_num_verts].x = 0;
		mcoords[total_num_verts].y = 0;

		total_num_verts++;
	}	
	
	oblist[total_num_objects].verts_end = total_num_verts - 1;
	
	return TRUE;
}

BOOL ProcessFaceData(HWND hwnd, FILE *fp)
{

	int	i, j, cnt = 0;
	unsigned short num_faces, face_index, ftemp;

	last_num_faces = total_num_faces;

	fread(&num_faces, sizeof(num_faces), 1, fp);

	
	fprintf(logfile, "%s %d\n", "TRIANGLE_FACELIST ", num_faces);

	num_faces_in_object[total_num_objects] = (short)num_faces;

	if((total_num_faces + num_faces) >= MAX_NUM_3DS_FACES)
		return FALSE;
			

	for (i = 0; i < (int)num_faces; i++)
	{
		for (j = 0; j < 4; j++)
		{
			fread(&face_index, sizeof(face_index), 1, fp);
			
			// note faces 1 to 3 are valid face indices, but the 4th one is NOT
			if(j < 3)
			{
				ftemp = (unsigned short)(face_index);// + last_num_verts);
				faces[total_num_faces].v[j] = ftemp;
				fprintf(logfile, " %d", face_index);
			}
			else
			{
				fprintf(logfile, "   flags: %d => ", face_index);

				if((face_index & 0x0001) == 0x0001) 
					fprintf(logfile, "  AC: 1");
				else
					fprintf(logfile, "  AC: 0");


				if((face_index & 0x0002) == 0x0002) 
					fprintf(logfile, "  BC: 1");
				else
					fprintf(logfile, "  BC: 0");


				if((face_index & 0x0004) == 0x0004) 
					fprintf(logfile, "  AB: 1");
				else
					fprintf(logfile, "  AB: 0");
			}
		}
		total_num_faces++;
		fprintf(logfile, "\n");
	}
	
	return TRUE;
}

void ProcessTriSmoothData(FILE *fp)
{
	int i, num_faces;
	BYTE a,b,c,d;


	num_faces = num_faces_in_object[total_num_objects];

	for(i = 0;  i < num_faces; i++)
	{
		fread(&a, sizeof(BYTE), 1, fp);
		fread(&b, sizeof(BYTE), 1, fp);
		fread(&c, sizeof(BYTE), 1, fp);
		fread(&d, sizeof(BYTE), 1, fp);

		fprintf(logfile, " a,b,c,d : %d %d %d %d\n", a, b, c, d);
	}
}

void ProcessTriLocalData(FILE *fp)
{
	float x,y,z;
	float local_centre_x, local_centre_y, local_centre_z;


	fread(&x, sizeof(float), 1, fp);
	fread(&y, sizeof(float), 1, fp);
	fread(&z, sizeof(float), 1, fp);

	fprintf(logfile, " x,y,z: %f %f %f\n", x, y, z);
		
	fread(&x, sizeof(float), 1, fp);
	fread(&y, sizeof(float), 1, fp);
	fread(&z, sizeof(float), 1, fp);

	fprintf(logfile, " x,y,z: %f %f %f\n", x, y, z);
	
	fread(&x, sizeof(float), 1, fp);
	fread(&y, sizeof(float), 1, fp);
	fread(&z, sizeof(float), 1, fp);

	fprintf(logfile, " x,y,z: %f %f %f\n", x, y, z);
	
	fread(&local_centre_x, sizeof(float), 1, fp);
	fread(&local_centre_y, sizeof(float), 1, fp);
	fread(&local_centre_z, sizeof(float), 1, fp);

	fprintf(logfile, " local_centre_x,y,z : %f %f %f\n", 
			local_centre_x, local_centre_y, local_centre_z);
	
	oblist[total_num_objects].local_centre_x = local_centre_x;
	oblist[total_num_objects].local_centre_y = local_centre_y;
	oblist[total_num_objects].local_centre_z = local_centre_z;
}



// PROCESS TEXTURE, MATERIAL, AND MAPPING DATA ROUTINES

void AddMapName(HWND hwnd, FILE *fp, int pmodel_id, world_ptr wptr)
{
	int	i; 
	BOOL error = TRUE;
	char map_name[256];

	// read in map name from file

	for (i = 0; i < 256; i++)
	{
		fread(&map_name[i], sizeof(char), 1, fp);
		if(map_name[i] == 0)
			break;
	}

	// remove file extention from string

	for (i = 0; i < 256; i++)
	{	
		if(map_name[i] == '.')
		{
			map_name[i] = 0;
			break;
		}
	}

		// lookup texture alias

	for (i = 0; i < 100; i++)
	{	
		if(strcmpi(map_name, wptr->TexMap[i].tex_alias_name) == 0)
		{
			wptr->pmdata[pmodel_id].texture_maps[num_maps] = i;
			error = FALSE;
			break;
		}
	}

	if(error == TRUE)
	{
		MessageBox(hwnd,"Error : AddMapName", map_name, MB_OK);
		strcpy(mapnames[num_maps], "error");
		return;
	}

	strcpy(mapnames[num_maps], map_name);
	fprintf(logfile, "%s %s\n", "MAPPING_NAME ", mapnames[num_maps]); 
	num_maps++;

}

void AddMaterialName(HWND hwnd, FILE *fp)
{	
	int	i; 
	BOOL error = TRUE;
	char mat_name[256];

	for (i = 0; i < 256; i++)
	{
		fread(&mat_name[i], sizeof(char), 1, fp);
		if(mat_name[i] == 0)
		{
			error = FALSE;
			break;
		}
	}

	if(error == TRUE)
	{
		MessageBox(hwnd,"Error : AddMaterialName", NULL, MB_OK);
		strcpy(material_list[num_materials], "error");
		return;
	}

	fprintf(logfile,"MAT_NAME01  %s\n",mat_name);
	strcpy(material_list[num_materials], mat_name) ;
	num_materials++;
}

void ProcessMaterialData(HWND hwnd, FILE *fp, int pmodel_id, world_ptr wptr)
{
	int	i; 
	short findex, current_texture;
	unsigned short num_faces;
	BOOL error = TRUE;
	char mat_name[256];


	for (i = 0; i < 256; i++)
	{
		fread(&mat_name[i], sizeof(char), 1, fp);
		if(mat_name[i] == 0)
			break;
	}

	for (i = 0; i < MAX_NUM_3DS_TEXTURES; i++)
	{
		if(strcmpi(mat_name, material_list[i]) == 0)
		{
			current_texture = wptr->pmdata[pmodel_id].texture_maps[i];
			error = FALSE;
			break;
		}
	}

	if(error == TRUE)
	{
		MessageBox(hwnd,"Error : ProcessMaterialData", NULL, MB_OK);
		strcpy(material_list[num_materials], "error");
		return;
	}

	fread(&num_faces, sizeof(num_faces), 1, fp);

	fprintf(logfile, "TRIANGLE_MATERIAL %d\n", num_faces);

	for (i = 0; i < num_faces; i++)
	{
		fread(&findex, sizeof(short), 1, fp);
		faces[last_num_faces + findex].tex = current_texture;
		object_texture[total_num_objects] = current_texture;
	}
	return;
}

void ProcessMappingData(HWND hwnd, FILE *fp)
{
	int	i; 
	unsigned short num_mapping_coords;
	
	total_num_mcoords = last_num_verts;

	fread(&num_mapping_coords, sizeof(num_mapping_coords), 1, fp);

	fprintf(logfile, "%s %d\n", "TRIANGLE_MAPPINGCOORS ",
		num_mapping_coords);

	for (i = 0; i < num_mapping_coords; i++)
	{
		fread(&mcoords[total_num_mcoords].x, sizeof(float), 1, fp);
		fread(&mcoords[total_num_mcoords].y, sizeof(float), 1, fp);

		fprintf(logfile, " %f %f\n", 
			mcoords[total_num_mcoords].x,
			mcoords[total_num_mcoords].y);

		total_num_mcoords++;
	}
	return;
}



// KEYFRAME - PROCESS ANIMATION DATA ROUTINES

void ProcessPivots(HWND hwnd, FILE *fp)
{
	float x,y,z;
	
	fread(&x, sizeof(float), 1, fp);
	fread(&y,  sizeof(float), 1, fp);
	fread(&z,  sizeof(float), 1, fp);

	oblist[kf_count].pivot.x = x;
	oblist[kf_count].pivot.y = y;
	oblist[kf_count].pivot.z = z;

	fprintf(logfile, "PIVOT: %f %f %f\n", x, y, z);	

	
}

void ProcessRotationTrack(HWND hwnd, FILE *fp)
{		
	int i;
	short framenum;
	long lunknown; 
	float rotation_rad;
	float axis_x;
	float axis_y;
	float axis_z;



	fread(&pt_flags, sizeof(short), 1, fp);

	fread(&unknown,  sizeof(short), 1, fp);
	fread(&unknown,  sizeof(short), 1, fp);
	fread(&unknown,  sizeof(short), 1, fp);
	fread(&unknown,  sizeof(short), 1, fp);

	fread(&oblist[kf_count].rotkeys,	 sizeof(short), 1, fp);
	fread(&unknown,  sizeof(short), 1, fp);
	
	fprintf(logfile, "ROT_TRACK_TAG %d\n", oblist[kf_count].rotkeys);
	
	for(i = 0; i < oblist[kf_count].rotkeys; i++)
	{
		fread(&framenum,  sizeof(short), 1, fp);
		fread(&lunknown,  sizeof(long), 1, fp);
		fread(&rotation_rad,  sizeof(float), 1, fp);
		fread(&axis_x,  sizeof(float), 1, fp);
		fread(&axis_y,  sizeof(float), 1, fp);
		fread(&axis_z,  sizeof(float), 1, fp);

		fprintf(logfile, " framenum = %d  rot_angle/rads = %f  axis_x,y,z : %f %f %f\n", 
			              framenum, rotation_rad, axis_x, axis_y, axis_z);	

		if(i < total_num_frames)
		{
			oblist[kf_count].rot_track[i].framenum = framenum;
			oblist[kf_count].rot_track[i].lunknown = lunknown;
			oblist[kf_count].rot_track[i].rotation_rad = rotation_rad;
			oblist[kf_count].rot_track[i].axis_x = axis_x;
			oblist[kf_count].rot_track[i].axis_y = axis_y;
			oblist[kf_count].rot_track[i].axis_z = axis_z;
		}
	}

}

void ProcessPositionTrack(HWND hwnd, FILE *fp)
{	
	
	int i;
	short framenum;
	long  lunknown;
	float pos_x;
	float pos_y;
	float pos_z;


	
	fread(&pt_flags, sizeof(short), 1, fp);

	fread(&unknown,  sizeof(short), 1, fp);
	fread(&unknown,  sizeof(short), 1, fp);
	fread(&unknown,  sizeof(short), 1, fp);
	fread(&unknown,  sizeof(short), 1, fp);

	fread(&oblist[kf_count].poskeys,	 sizeof(short), 1, fp);
	fread(&unknown,  sizeof(short), 1, fp);
	
	fprintf(logfile, "POS_TRACK_TAG %d\n", oblist[kf_count].poskeys);
	
	for(i = 0; i < oblist[kf_count].poskeys; i++)
	{
		fread(&framenum,  sizeof(short), 1, fp);
		fread(&lunknown,  sizeof(long), 1, fp);
		fread(&pos_x,  sizeof(float), 1, fp);
		fread(&pos_y,  sizeof(float), 1, fp);
		fread(&pos_z,  sizeof(float), 1, fp);

		fprintf(logfile, " framenum = %d   pos_x,y,z : %f %f %f\n", 
			              framenum, pos_x, pos_y, pos_z);	

		if(i < total_num_frames)
		{
			//fprintf(logfile, "i: %d   total_num_frames: %d\n", 
			  //                i, total_num_frames);	

			oblist[kf_count].pos_track[i].framenum = framenum;
			oblist[kf_count].pos_track[i].lunknown = lunknown;
			oblist[kf_count].pos_track[i].pos_x = pos_x;
			oblist[kf_count].pos_track[i].pos_y = pos_y;
			oblist[kf_count].pos_track[i].pos_z = pos_z;
		}
	}
}
	
void ProcessScaleTrack(HWND hwnd, FILE *fp)
{	
	
	int i;
	short framenum;
	long lunknown;		
	float scale_x;
	float scale_y;
	float scale_z;


	fread(&pt_flags, sizeof(short), 1, fp);

	fread(&unknown,  sizeof(short), 1, fp);
	fread(&unknown,  sizeof(short), 1, fp);
	fread(&unknown,  sizeof(short), 1, fp);
	fread(&unknown,  sizeof(short), 1, fp);

	fread(&oblist[kf_count].sclkeys,	 sizeof(short), 1, fp);
	fread(&unknown,  sizeof(short), 1, fp);
	
	fprintf(logfile, "SCL_TRACK_TAG %d\n", oblist[kf_count].sclkeys);
	
	for(i = 0; i < oblist[kf_count].sclkeys; i++)
	{
		fread(&framenum,  sizeof(short), 1, fp);
		fread(&lunknown,  sizeof(long), 1, fp);
		
		fread(&scale_x,  sizeof(float), 1, fp);
		fread(&scale_y,  sizeof(float), 1, fp);
		fread(&scale_z,  sizeof(float), 1, fp);

		fprintf(logfile, " framenum = %d   x,y,z : %f %f %f\n", 
			              framenum, scale_x, scale_y, scale_z);	

	}
}

void ProcessNodeId(FILE *fp)
{
	short node_id;


	fread(&node_id, sizeof(short), 1, fp);
	fprintf(logfile, "NODE_ID  %d\n", node_id); 
}

void ProcessNodeHeader(FILE *fp)
{
	int i;
	short flags1, flags2, heirarchy;
	char node_name[256];


	// read in node name from file

	for (i = 0; i < 256; i++)
	{
		fread(&node_name[i], sizeof(char), 1, fp);
		if(node_name[i] == 0)
			break;
	}

	fread(&flags1, sizeof(short), 1, fp);
	fread(&flags2, sizeof(short), 1, fp);
	fread(&heirarchy, sizeof(short), 1, fp);

	fprintf(logfile, "NODE_HDR %s\n", node_name);
	fprintf(logfile, " flags1 %d\n", flags1);
	fprintf(logfile, " flags2 %d\n", flags2);
	fprintf(logfile, " heirarchy %d\n", heirarchy);

}

void Process3DSVersion(FILE *fp)
{
	short version;

	fread(&version, sizeof(short), 1, fp);
	fprintf(logfile, "3DS VERSION %d\n",version);
	fread(&version, sizeof(short), 1, fp);
}

void ProcessMasterScale(FILE *fp)
{
	float master_scale;

	fread(&master_scale, sizeof(float), 1, fp);
	fprintf(logfile, "MASTER_SCALE %f\n",master_scale);	
}

// RELEASE AND DEBUG ROUTINES

void ReleaseTempMemory()
{
	
}

void PrintLogFile(FILE *logfile, char *commmand)
{
	fprintf(logfile, commmand);
	fprintf(logfile,"\n");
	command_found_flag = TRUE;
}


void Write_pmdata_debugfile(HWND hwnd, int pmodel_id, world_ptr wptr)
{
	FILE *fp;
	int i;
	int frame_num;
	float x,y,z;
	int face;
	int num_verts;
	int num_faces;
	int tex;


	if ((fp = fopen("pmdata.txt", "w+")) == 0) 
	{
		MessageBox(hwnd,"logfile error : Can't open", "pmdata.txt", MB_OK);
		return;
	}

	////////////////////////////////////////////////////

	frame_num = 0;
	num_verts = wptr->pmdata[pmodel_id].num_verts;

	fprintf(fp, "model id  =  %d\n\n", pmodel_id);	
	fprintf(fp, "num verts =  %d\n\n", num_verts);	

	for(i = 0; i < num_verts; i++)
	{
		x = wptr->pmdata[pmodel_id].w[frame_num][i].x;
		y = wptr->pmdata[pmodel_id].w[frame_num][i].y;
		z = wptr->pmdata[pmodel_id].w[frame_num][i].z;
		fprintf(fp, "%d   %f %f %f\n", i, x, y, z);		
	}

	fprintf(fp, "\n");	

	////////////////////////////////////////////////////


	num_faces = wptr->pmdata[pmodel_id].num_faces;

	
	fprintf(fp, "num_faces =  %d\n\n", num_faces);	

	for(i = 0; i < num_faces*3; i++)
	{
		face = wptr->pmdata[pmodel_id].f[i];
		
		fprintf(fp, "%d   %d\n", i, face);		
	}

	fprintf(fp, "\n");	

	////////////////////////////////////////////////////


	num_faces = wptr->pmdata[pmodel_id].num_faces;

	
	fprintf(fp, "num_faces =  %d tex\n\n", num_faces);	

	for(i = 0; i < num_faces*3; i++)
	{
		x = wptr->pmdata[pmodel_id].t[i].x; 
		y = wptr->pmdata[pmodel_id].t[i].y;
		
		fprintf(fp, "%d   %f %f\n", i, x, y);		
	}

	fprintf(fp, "\n");	
	
	////////////////////////////////////////////////////

	num_faces = wptr->pmdata[pmodel_id].num_faces;

	for(i = 0; i < num_faces; i++)
	{
		tex = wptr->pmdata[pmodel_id].texture_list[i];
			
		fprintf(fp, "%d   %d\n", i, tex);		
	}

	fprintf(fp, "\n");	

	////////////////////////////////////////////////////


	fclose (fp);
}

