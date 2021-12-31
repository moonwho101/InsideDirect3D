
// D3D Road Rage version 1.1
// Written by William Chin
// Developed in MSDEV C++ v4.2 with DirectX 5 SDK
// Date : 12/10/98
// File : Models.cpp

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


VERT temp_verts[10000];

float   temp_float[1000];
int		temp_int[1000];

char Q2M_Name[50][50];
char Q2M_File[50][50];
char Q2S_File[50][50];

int  Q2M_Object_ID[50];
int  Q2M_counter = 0;


char Q2M_Anim_Name[50][50];
char Q2M_Anim_File[50][50];
char Q2S_Anim_File[50][50];

int  Q2M_Anim_Object_ID[50];
int  Q2M_Anim_Counter = 0;



BOOL ImportASC_Q2AnimModel(HWND hwnd, char *filename, int object_id, world_ptr wptr)
{
	int i;
	int hundreds,tens,units;
	char tfilename[50];
	char buffer[50];
	
	
	for(i=0;i<198;i++) // 198
	{			
		// player001.asc

		hundreds = i / 100;
		tens	 = ( i - (hundreds * 100) ) / 10;	
		units	 = i - (tens * 10) - (hundreds * 100);

		strcpy(tfilename, filename);

		itoa(hundreds, buffer,10);
		strcat(tfilename,(char *)buffer);

		itoa(tens, buffer,10);
		strcat(tfilename,(char *)buffer);

		itoa(units, buffer,10);
		strcat(tfilename,(char *)buffer);

		strcat(tfilename,".asc");

		//PrintMessage(hwnd, tfilename);
	
		ImportASC_Q2AnimFrame(hwnd, tfilename, i , object_id, wptr);
	}

	return 0;
}

BOOL ImportASC_Q2AnimFrame(HWND hwnd, char *filename, int frame_num, int pmodel_id, world_ptr wptr)
{
	FILE *fp;    
	char c[100];
	char p[100];
	char *q;
	int done=0;
	int number_of_vertices;
	int number_of_faces;
	int vert_count;
	int vert_num;
	int poly_count;
	int v_count;
	int i;
	BOOL start_face_list_flag = TRUE;


    fp = fopen(filename,"r");
    if(fp==NULL)
    {     
        MessageBox(hwnd,"Error can't load file",NULL,MB_OK);
		return 1;
    }

	//PrintMessage(hwnd, "Q2Model");
	
	vert_count   = 0;
	vert_num     = 0;
	poly_count   = 0;
	v_count      = 0;

	
	// LINE #1 ------------------------------
	
	fscanf( fp, "%s", &c ); // get command
	if(strcmp(c,"Named")!=0)
		return FALSE;
		
	fscanf( fp, "%s", &c ); // get command
	if(strcmp(c,"Object:")!=0)
		return FALSE;
			
	fscanf( fp, "%s", &p );	// ignore skin name	
	
	
	// LINE #2 ------------------------------

	fscanf( fp, "%s", &c ); // get command
	if(strcmp(c,"Tri-mesh,")!=0) 
		return FALSE;
				
	fscanf( fp, "%s", &c ); // get command
	if(strcmp(c,"Vertices:")!=0)
		return FALSE;
	
	fscanf( fp, "%s", &p );
	number_of_vertices = atoi(p);
		
	fscanf( fp, "%s", &c ); // get command
	if(strcmp(c,"Faces:")!=0)
		return FALSE;
		
	fscanf( fp, "%s", &p );
	number_of_faces = atoi(p);
		

	// LINE #3 ------------------------------

	fscanf( fp, "%s", &c ); // get command
	if(strcmp(c,"Vertex")!=0)
		return FALSE;

	fscanf( fp, "%s", &c ); // get command
	if(strcmp(c,"List:")!=0)
		return FALSE;


	// LINE #4 ------------------------------

	// read vertex list

	for(i=0; i < number_of_vertices; i++)
	{
		fscanf( fp, "%s", &c ); // get command
		if(strcmp(c,"Vertex")!=0)
		{
			MessageBox(hwnd,"Error 1",NULL,MB_OK);
			return FALSE;
		}

		fscanf( fp, "%s", &p ); // ignore vertex number
			
			
		fscanf( fp, "%s", &p ); // ignore "X:"
		fscanf( fp, "%s", &p );
		temp_verts[vert_count].x = (float)atof(p) * (float).4;
			
		fscanf( fp, "%s", &p ); // ignore "Y:"
		fscanf( fp, "%s", &p );
		temp_verts[vert_count].y = (float)atof(p) * (float).4 ;
			
		fscanf( fp, "%s", &p ); // ignore "Z:"
		fscanf( fp, "%s", &p );
		temp_verts[vert_count].z = ((float)atof(p)+24) * (float).4;

		vert_count++;
		
	}

	// read face list

	fscanf( fp, "%s", &c ); // get command
	if(strcmp(c,"Face")!=0)
	{
		MessageBox(hwnd,"Error 2",NULL,MB_OK);
		return FALSE;
	}
	
	fscanf( fp, "%s", &c ); // get command
	if(strcmp(c,"list:Face")!=0)
	{
		MessageBox(hwnd,"Error 3",NULL,MB_OK);
		return FALSE;
	}
	
	fscanf( fp, "%s", &p ); // ignore "0:"



	for(i=0; i < number_of_faces; i++)
	{
		if(start_face_list_flag == FALSE)
		{
			fscanf( fp, "%s", &c ); // get command
			if(strcmp(c,"Face")!=0)
			{
				MessageBox(hwnd,"Error 5",c,MB_OK);
				return FALSE;
			}
		
			fscanf( fp, "%s", &p ); // ignore face number	
		}

		start_face_list_flag = FALSE;

		fscanf( fp, "%s", &p); 
		q = (char *)p;
		q = _strinc(q);	q = _strinc(q); // ignore "A:"
	
		vert_num = atoi(q);
		wptr->pmdata[pmodel_id].v[frame_num][v_count].x = temp_verts[vert_num].x ;
		wptr->pmdata[pmodel_id].v[frame_num][v_count].y = temp_verts[vert_num].z ;
		wptr->pmdata[pmodel_id].v[frame_num][v_count].z = temp_verts[vert_num].y ;
		v_count++;

		fscanf( fp, "%s", &p); 
		q = (char *)p;
		q = _strinc(q);	q = _strinc(q); // ignore "B:"
		vert_num = atoi(q);
		wptr->pmdata[pmodel_id].v[frame_num][v_count].x = temp_verts[vert_num].x ;
		wptr->pmdata[pmodel_id].v[frame_num][v_count].y = temp_verts[vert_num].z ;
		wptr->pmdata[pmodel_id].v[frame_num][v_count].z = temp_verts[vert_num].y ;
		v_count++;

		fscanf( fp, "%s", &p); 
		q = (char *)p;
		q = _strinc(q);	q = _strinc(q); // ignore "C:"
		vert_num = atoi(q);
		wptr->pmdata[pmodel_id].v[frame_num][v_count].x = temp_verts[vert_num].x ;
		wptr->pmdata[pmodel_id].v[frame_num][v_count].y = temp_verts[vert_num].z ;
		wptr->pmdata[pmodel_id].v[frame_num][v_count].z = temp_verts[vert_num].y ;
		v_count++;


		fscanf( fp, "%s", &p); // ignore "AB:1"
		fscanf( fp, "%s", &p); // ignore "BC:1"
		fscanf( fp, "%s", &p); // ignore "CA:1"

		fscanf( fp, "%s", &c ); // get command
		if(strcmp(c,"Smoothing:")!=0)
		{
			MessageBox(hwnd,"Error 4",c,MB_OK);
			return FALSE;
		}

		fscanf( fp, "%s", &p ); // ignore "1"		

		poly_count++;
	}	
		
	fclose(fp);	
	
	wptr->pmdata[pmodel_id].num_polys_per_frame = number_of_faces;
	wptr->pmdata[pmodel_id].num_verts = v_count;
	return TRUE;
}


BOOL ImportASC_Q2Model(HWND hwnd, char *filename, int object_id, world_ptr wptr)
{
	FILE *fp;    
	char c[100];
	char p[100];
	char *q;
	int done=0;
	int number_of_vertices;
	int number_of_faces;
	int vert_count;
	int vert_num;
	int poly_count;
	int v_count;
	int i;
	BOOL start_face_list_flag = TRUE;


    fp = fopen(filename,"r");
    if(fp==NULL)
    {     
        MessageBox(hwnd,"Error can't load file",NULL,MB_OK);
		return 1;
    }

	//PrintMessage(hwnd, "Q2Model");
	
	vert_count   = 0;
	vert_num     = 0;
	poly_count   = 0;
	v_count      = 0;

	
	// LINE #1 ------------------------------
	
	fscanf( fp, "%s", &c ); // get command
	if(strcmp(c,"Named")!=0)
		return FALSE;
		
	fscanf( fp, "%s", &c ); // get command
	if(strcmp(c,"Object:")!=0)
		return FALSE;
			
	fscanf( fp, "%s", &p );	// ignore skin name	
	
	
	// LINE #2 ------------------------------

	fscanf( fp, "%s", &c ); // get command
	if(strcmp(c,"Tri-mesh,")!=0) 
		return FALSE;
				
	fscanf( fp, "%s", &c ); // get command
	if(strcmp(c,"Vertices:")!=0)
		return FALSE;
	
	fscanf( fp, "%s", &p );
	number_of_vertices = atoi(p);
		
	fscanf( fp, "%s", &c ); // get command
	if(strcmp(c,"Faces:")!=0)
		return FALSE;
		
	fscanf( fp, "%s", &p );
	number_of_faces = atoi(p);
		

	// LINE #3 ------------------------------

	fscanf( fp, "%s", &c ); // get command
	if(strcmp(c,"Vertex")!=0)
		return FALSE;

	fscanf( fp, "%s", &c ); // get command
	if(strcmp(c,"List:")!=0)
		return FALSE;


	// LINE #4 ------------------------------

	// read vertex list

	for(i=0; i < number_of_vertices; i++)
	{
		fscanf( fp, "%s", &c ); // get command
		if(strcmp(c,"Vertex")!=0)
		{
			MessageBox(hwnd,"Error 1",NULL,MB_OK);
			return FALSE;
		}

		fscanf( fp, "%s", &p ); // ignore vertex number
			
			
		fscanf( fp, "%s", &p ); // ignore "X:"
		fscanf( fp, "%s", &p );
		temp_verts[vert_count].x = (float)atof(p);
			
		fscanf( fp, "%s", &p ); // ignore "Y:"
		fscanf( fp, "%s", &p );
		temp_verts[vert_count].y = (float)atof(p);
			
		fscanf( fp, "%s", &p ); // ignore "Z:"
		fscanf( fp, "%s", &p );
		temp_verts[vert_count].z = (float)atof(p);

		vert_count++;
		
	}

	// read face list

	fscanf( fp, "%s", &c ); // get command
	if(strcmp(c,"Face")!=0)
	{
		MessageBox(hwnd,"Error 2",NULL,MB_OK);
		return FALSE;
	}
	
	fscanf( fp, "%s", &c ); // get command
	if(strcmp(c,"list:Face")!=0)
	{
		MessageBox(hwnd,"Error 3",NULL,MB_OK);
		return FALSE;
	}
	
	fscanf( fp, "%s", &p ); // ignore "0:"



	for(i=0; i < number_of_faces; i++)
	{
		if(start_face_list_flag == FALSE)
		{
			fscanf( fp, "%s", &c ); // get command
			if(strcmp(c,"Face")!=0)
			{
				MessageBox(hwnd,"Error 5",c,MB_OK);
				return FALSE;
			}
		
			fscanf( fp, "%s", &p ); // ignore face number	
		}

		start_face_list_flag = FALSE;

		fscanf( fp, "%s", &p); 
		q = (char *)p;
		q = _strinc(q);	q = _strinc(q); // ignore "A:"
		//PrintMessage(hwnd, q);
		vert_num = atoi(q);
		wptr->obdata[object_id].v[v_count].x = temp_verts[vert_num].x ;
		wptr->obdata[object_id].v[v_count].y = temp_verts[vert_num].z ;
		wptr->obdata[object_id].v[v_count].z = temp_verts[vert_num].y ;
		v_count++;

		fscanf( fp, "%s", &p); 
		q = (char *)p;
		q = _strinc(q);	q = _strinc(q); // ignore "B:"
		vert_num = atoi(q);
		wptr->obdata[object_id].v[v_count].x = temp_verts[vert_num].x ;
		wptr->obdata[object_id].v[v_count].y = temp_verts[vert_num].z ;
		wptr->obdata[object_id].v[v_count].z = temp_verts[vert_num].y ;
		v_count++;

		fscanf( fp, "%s", &p); 
		q = (char *)p;
		q = _strinc(q);	q = _strinc(q); // ignore "C:"
		vert_num = atoi(q);
		wptr->obdata[object_id].v[v_count].x = temp_verts[vert_num].x ;
		wptr->obdata[object_id].v[v_count].y = temp_verts[vert_num].z ;
		wptr->obdata[object_id].v[v_count].z = temp_verts[vert_num].y ;
		v_count++;

		wptr->obdata[object_id].tex[poly_count]=23;
		wptr->obdata[object_id].num_vert[poly_count]=3;

		fscanf( fp, "%s", &p); // ignore "AB:1"
		fscanf( fp, "%s", &p); // ignore "BC:1"
		fscanf( fp, "%s", &p); // ignore "CA:1"

		fscanf( fp, "%s", &c ); // get command
		if(strcmp(c,"Smoothing:")!=0)
		{
			MessageBox(hwnd,"Error 4",c,MB_OK);
			return FALSE;
		}

		fscanf( fp, "%s", &p ); // ignore "1"		

		poly_count++;
	}	
	
	wptr->num_vert_per_object [object_id]=v_count;
	wptr->num_polys_per_object[object_id]=poly_count;
	wptr->obdata_length++;

	
	fclose(fp);		

	return TRUE;
}


BOOL ImportASC_Q2skin(HWND hwnd, char *filename, int object_id, world_ptr wptr)
{
	FILE *fp;    
	char c[100];
	char p[100];
	char *q;
	int done=0;
	int number_of_vertices;
	int number_of_faces;
	int vert_count;
	int vert_num;
	int poly_count;
	int v_count;
	int i;
	//int frame_num;
	BOOL start_face_list_flag = TRUE;


    fp = fopen(filename,"r");
    if(fp==NULL)
    {     
        MessageBox(hwnd,"Error can't load file",NULL,MB_OK);
		return 1;
    }

	PrintMessage(hwnd, filename);
	
	vert_count   = 0;
	vert_num     = 0;
	poly_count   = 0;
	v_count      = 0;

	
	// LINE #1 ------------------------------
	
	fscanf( fp, "%s", &c ); // get command
	if(strcmp(c,"Named")!=0)
		return FALSE;
		
	fscanf( fp, "%s", &c ); // get command
	if(strcmp(c,"Object:")!=0)
		return FALSE;
			
	fscanf( fp, "%s", &p );	// ignore skin name	
	
	
	// LINE #2 ------------------------------

	fscanf( fp, "%s", &c ); // get command
	if(strcmp(c,"Tri-mesh,")!=0) 
		return FALSE;
				
	fscanf( fp, "%s", &c ); // get command
	if(strcmp(c,"Vertices:")!=0)
		return FALSE;
	
	fscanf( fp, "%s", &p );
	number_of_vertices = atoi(p);
		
	fscanf( fp, "%s", &c ); // get command
	if(strcmp(c,"Faces:")!=0)
		return FALSE;
		
	fscanf( fp, "%s", &p );
	number_of_faces = atoi(p);
		

	// LINE #3 ------------------------------

	fscanf( fp, "%s", &c ); // get command
	if(strcmp(c,"Vertex")!=0)
		return FALSE;

	fscanf( fp, "%s", &c ); // get command
	if(strcmp(c,"List:")!=0)
		return FALSE;


	// LINE #4 ------------------------------

	// read vertex list

	for(i=0; i < number_of_vertices; i++)
	{
		fscanf( fp, "%s", &c ); // get command
		if(strcmp(c,"Vertex")!=0)
		{
			MessageBox(hwnd,"Error 1",NULL,MB_OK);
			return FALSE;
		}

		fscanf( fp, "%s", &p ); // ignore vertex number
			
			
		fscanf( fp, "%s", &p ); // ignore "X:"
		fscanf( fp, "%s", &p );
		temp_verts[vert_count].x = (float)atof(p);
			
		fscanf( fp, "%s", &p ); // ignore "Y:"
		fscanf( fp, "%s", &p );
		temp_verts[vert_count].y = (float)atof(p);
			
		fscanf( fp, "%s", &p ); // ignore "Z:"
		fscanf( fp, "%s", &p );
		temp_verts[vert_count].z = (float)atof(p);

		vert_count++;
		
	}

	// read face list

	fscanf( fp, "%s", &c ); // get command
	if(strcmp(c,"Face")!=0)
	{
		MessageBox(hwnd,"Error 2",NULL,MB_OK);
		return FALSE;
	}
	
	fscanf( fp, "%s", &c ); // get command
	if(strcmp(c,"list:Face")!=0)
	{
		MessageBox(hwnd,"Error 3",NULL,MB_OK);
		return FALSE;
	}
	
	fscanf( fp, "%s", &p ); // ignore "0:"



	for(i=0; i < number_of_faces; i++)
	{
		if(start_face_list_flag == FALSE)
		{
			fscanf( fp, "%s", &c ); // get command
			if(strcmp(c,"Face")!=0)
			{
				MessageBox(hwnd,"Error 5",c,MB_OK);
				return FALSE;
			}
		
			fscanf( fp, "%s", &p ); // ignore face number	
		}

		start_face_list_flag = FALSE;

		fscanf( fp, "%s", &p); 
		q = (char *)p;
		q = _strinc(q);	q = _strinc(q); // ignore "A:"
		//PrintMessage(hwnd, q);
		vert_num = atoi(q);
		wptr->obdata[object_id].t[v_count].x = temp_verts[vert_num].x ;
		wptr->obdata[object_id].t[v_count].y = temp_verts[vert_num].y ;
		wptr->obdata[object_id].t[v_count].z = (float)0;
		v_count++;

		fscanf( fp, "%s", &p); 
		q = (char *)p;
		q = _strinc(q);	q = _strinc(q); // ignore "B:"
		vert_num = atoi(q);
		wptr->obdata[object_id].t[v_count].x = temp_verts[vert_num].x ;
		wptr->obdata[object_id].t[v_count].y = temp_verts[vert_num].y ;
		wptr->obdata[object_id].t[v_count].z = (float)0;
		v_count++;

		fscanf( fp, "%s", &p); 
		q = (char *)p;
		q = _strinc(q);	q = _strinc(q); // ignore "C:"
		vert_num = atoi(q);
		wptr->obdata[object_id].t[v_count].x = temp_verts[vert_num].x ;
		wptr->obdata[object_id].t[v_count].y = temp_verts[vert_num].y ;
		wptr->obdata[object_id].t[v_count].z = (float)0;
		v_count++;

		//wptr->obdata[object_id].tex[poly_count]=23;
		//wptr->obdata[object_id].num_vert[poly_count]=3;

		fscanf( fp, "%s", &p); // ignore "AB:1"
		fscanf( fp, "%s", &p); // ignore "BC:1"
		fscanf( fp, "%s", &p); // ignore "CA:1"

		fscanf( fp, "%s", &c ); // get command
		if(strcmp(c,"Smoothing:")!=0)
		{
			MessageBox(hwnd,"Error 4",c,MB_OK);
			return FALSE;
		}

		fscanf( fp, "%s", &p ); // ignore "1"		

		poly_count++;
	}	
		
	//wptr->num_vert_per_object [object_id]=v_count;
	//wptr->num_polys_per_object[object_id]=poly_count;
	//wptr->obdata_length++;

	
	fclose(fp);		

	return TRUE;
}




BOOL ImportASC_Q2Animskin(HWND hwnd, char *filename, int pmodel_id, world_ptr wptr)
{
	FILE *fp;    
	char c[100];
	char p[100];
	char *q;
	int done=0;
	int number_of_vertices;
	int number_of_faces;
	int vert_count;
	int vert_num;
	int poly_count;
	int v_count;
	int i;
	BOOL start_face_list_flag = TRUE;


    fp = fopen(filename,"r");
    if(fp==NULL)
    {     
        MessageBox(hwnd,"Error can't load file",NULL,MB_OK);
		return 1;
    }


	PrintMessage(hwnd, filename);
	
	vert_count   = 0;
	vert_num     = 0;
	poly_count   = 0;
	v_count      = 0;

	
	// LINE #1 ------------------------------
	
	fscanf( fp, "%s", &c ); // get command
	if(strcmp(c,"Named")!=0)
		return FALSE;
		
	fscanf( fp, "%s", &c ); // get command
	if(strcmp(c,"Object:")!=0)
		return FALSE;
			
	fscanf( fp, "%s", &p );	// ignore skin name	
	
	
	// LINE #2 ------------------------------

	fscanf( fp, "%s", &c ); // get command
	if(strcmp(c,"Tri-mesh,")!=0) 
		return FALSE;
				
	fscanf( fp, "%s", &c ); // get command
	if(strcmp(c,"Vertices:")!=0)
		return FALSE;
	
	fscanf( fp, "%s", &p );
	number_of_vertices = atoi(p);
		
	fscanf( fp, "%s", &c ); // get command
	if(strcmp(c,"Faces:")!=0)
		return FALSE;
		
	fscanf( fp, "%s", &p );
	number_of_faces = atoi(p);
		

	// LINE #3 ------------------------------

	fscanf( fp, "%s", &c ); // get command
	if(strcmp(c,"Vertex")!=0)
		return FALSE;

	fscanf( fp, "%s", &c ); // get command
	if(strcmp(c,"List:")!=0)
		return FALSE;


	// LINE #4 ------------------------------

	// read vertex list

	for(i=0; i < number_of_vertices; i++)
	{
		fscanf( fp, "%s", &c ); // get command
		if(strcmp(c,"Vertex")!=0)
		{
			MessageBox(hwnd,"Error 1",NULL,MB_OK);
			return FALSE;
		}

		fscanf( fp, "%s", &p ); // ignore vertex number
			
			
		fscanf( fp, "%s", &p ); // ignore "X:"
		fscanf( fp, "%s", &p );
		temp_verts[vert_count].x = (float)atof(p);
			
		fscanf( fp, "%s", &p ); // ignore "Y:"
		fscanf( fp, "%s", &p );
		temp_verts[vert_count].y = (float)atof(p);
			
		fscanf( fp, "%s", &p ); // ignore "Z:"
		fscanf( fp, "%s", &p );
		temp_verts[vert_count].z = (float)atof(p);

		vert_count++;
		
	}

	// read face list

	fscanf( fp, "%s", &c ); // get command
	if(strcmp(c,"Face")!=0)
	{
		MessageBox(hwnd,"Error 2",NULL,MB_OK);
		return FALSE;
	}
	
	fscanf( fp, "%s", &c ); // get command
	if(strcmp(c,"list:Face")!=0)
	{
		MessageBox(hwnd,"Error 3",NULL,MB_OK);
		return FALSE;
	}
	
	fscanf( fp, "%s", &p ); // ignore "0:"



	for(i=0; i < number_of_faces; i++)
	{
		if(start_face_list_flag == FALSE)
		{
			fscanf( fp, "%s", &c ); // get command
			if(strcmp(c,"Face")!=0)
			{
				MessageBox(hwnd,"Error 5",c,MB_OK);
				return FALSE;
			}
		
			fscanf( fp, "%s", &p ); // ignore face number	
		}

		start_face_list_flag = FALSE;

		fscanf( fp, "%s", &p); 
		q = (char *)p;
		q = _strinc(q);	q = _strinc(q); // ignore "A:"
		//PrintMessage(hwnd, q);
		vert_num = atoi(q);
		wptr->pmdata[pmodel_id].t[v_count].x = temp_verts[vert_num].x ;
		wptr->pmdata[pmodel_id].t[v_count].y = temp_verts[vert_num].y ;
		wptr->pmdata[pmodel_id].t[v_count].z = (float)0;
		v_count++;

		fscanf( fp, "%s", &p); 
		q = (char *)p;
		q = _strinc(q);	q = _strinc(q); // ignore "B:"
		vert_num = atoi(q);
		wptr->pmdata[pmodel_id].t[v_count].x = temp_verts[vert_num].x ;
		wptr->pmdata[pmodel_id].t[v_count].y = temp_verts[vert_num].y ;
		wptr->pmdata[pmodel_id].t[v_count].z = (float)0;
		v_count++;

		fscanf( fp, "%s", &p); 
		q = (char *)p;
		q = _strinc(q);	q = _strinc(q); // ignore "C:"
		vert_num = atoi(q);
		wptr->pmdata[pmodel_id].t[v_count].x = temp_verts[vert_num].x ;
		wptr->pmdata[pmodel_id].t[v_count].y = temp_verts[vert_num].y ;
		wptr->pmdata[pmodel_id].t[v_count].z = (float)0;
		v_count++;

		//wptr->obdata[object_id].tex[poly_count]=23;
		//wptr->obdata[object_id].num_vert[poly_count]=3;

		fscanf( fp, "%s", &p); // ignore "AB:1"
		fscanf( fp, "%s", &p); // ignore "BC:1"
		fscanf( fp, "%s", &p); // ignore "CA:1"

		fscanf( fp, "%s", &c ); // get command
		if(strcmp(c,"Smoothing:")!=0)
		{
			MessageBox(hwnd,"Error 4",c,MB_OK);
			return FALSE;
		}

		fscanf( fp, "%s", &p ); // ignore "1"		

		poly_count++;
	}	
		
	
	//wptr->num_vert_per_object [object_id]=v_count;
	//wptr->num_polys_per_object[object_id]=poly_count;
	//wptr->obdata_length++;

	
	fclose(fp);		

	return TRUE;
}


BOOL ConvertMultipleASCtoRRM(HWND hwnd, char *src_filename, char *dest_filename, world_ptr wptr)
{
	FILE *fp;  
	FILE *fp2;  
	int i,j;
	int v_count = 0;
	//int num_polys_per_frame;
	//int num_frames;
	short temp_i;
	float temp_f;

	int hundreds,tens,units;
	char tfilename[50];
	char buffer[50];

	char c[100];
	char p[100];
	char *q;
	int done=0;
	int number_of_frames;
	int number_of_vertices;
	int number_of_faces;
	int vert_count;
	int vert_num;
	int poly_count;
	int result;
	BOOL start_flag = TRUE;
	BOOL start_face_list_flag;
	
	// ConvertMultipleASCtoRRM(	hwnd, 
	//							"anim/crakhor/crakhor",  src_filename 
	//							"crakhor.rrm",			dest_filename
	//							wptr);

	
	fp2 = fopen(dest_filename,"w");
	if(fp2==NULL)
	{     
		MessageBox(hwnd,"Error can't load file",NULL,MB_OK);
		return 1;
	}

	result = _setmode( _fileno( fp2 ), _O_BINARY );
		
	if( result == -1 )
	{
		MessageBox(hwnd,"Cannot set mode",c,MB_OK);
		return FALSE;
	}

	number_of_frames = 60;//198;

	for(i = 0; i < number_of_frames; i++) 
	{			
		start_face_list_flag = TRUE;

		// player001.asc

		hundreds = i / 100;
		tens	 = ( i - (hundreds * 100) ) / 10;	
		units	 = i - (tens * 10) - (hundreds * 100);

		strcpy(tfilename, src_filename);//src_filename );

		itoa(hundreds, buffer,10);
		strcat(tfilename,(char *)buffer);

		itoa(tens, buffer,10);
		strcat(tfilename,(char *)buffer);

		itoa(units, buffer,10);
		strcat(tfilename,(char *)buffer);

		strcat(tfilename,".asc");

		//PrintMessage(hwnd, tfilename);

		fp = fopen(tfilename,"r");
		if(fp==NULL)
		{     
			MessageBox(hwnd,"Error2 can't load file",NULL,MB_OK);
			return 1;
		}
	
		vert_count   = 0;
		vert_num     = 0;
		poly_count   = 0;
		v_count      = 0;

	
		// LINE #1 ------------------------------
	
		fscanf( fp, "%s", &c ); // get command
		if(strcmp(c,"Named")!=0)
		{
			MessageBox(hwnd,"Error 1",NULL,MB_OK);
			return FALSE;
		}	
		
		fscanf( fp, "%s", &c ); // get command
		if(strcmp(c,"Object:")!=0)
		{
			MessageBox(hwnd,"Error 2",NULL,MB_OK);
			return FALSE;
		}	
			
		fscanf( fp, "%s", &p );	// ignore skin name	
	
	
		// LINE #2 ------------------------------

		fscanf( fp, "%s", &c ); // get command
		if(strcmp(c,"Tri-mesh,")!=0) 
		{
			MessageBox(hwnd,"Error 3",NULL,MB_OK);
			return FALSE;
		}	
				
		fscanf( fp, "%s", &c ); // get command
		if(strcmp(c,"Vertices:")!=0)
		{
			MessageBox(hwnd,"Error 4",NULL,MB_OK);
			return FALSE;
		}	
	
		fscanf( fp, "%s", &p );
		number_of_vertices = atoi(p);
		
		fscanf( fp, "%s", &c ); // get command
		if(strcmp(c,"Faces:")!=0)
		{
			MessageBox(hwnd,"Error 5",NULL,MB_OK);
			return FALSE;
		}	
		
		fscanf( fp, "%s", &p );
		number_of_faces = atoi(p);
		

		// LINE #3 ------------------------------

		fscanf( fp, "%s", &c ); // get command
		if(strcmp(c,"Vertex")!=0)
		{
			MessageBox(hwnd,"Error 6",NULL,MB_OK);
			return FALSE;
		}		

		fscanf( fp, "%s", &c ); // get command
		if(strcmp(c,"List:")!=0)
		{
			MessageBox(hwnd,"Error 7",NULL,MB_OK);
			return FALSE;
		}	

		// write header info to .rmm file

		if(start_flag == TRUE)
		{
			fwrite( &number_of_frames,   sizeof(short), 1, fp2 );
			fwrite( &number_of_vertices, sizeof(short), 1, fp2 );
			fwrite( &number_of_faces,    sizeof(short), 1, fp2 );
		
			itoa(number_of_frames,buffer,10);
			PrintMessage(hwnd, buffer );

			start_flag = FALSE;
		}
	
		// LINE #4 ------------------------------

		// read vertex list

		for(j=0; j < number_of_vertices; j++)
		{
			fscanf( fp, "%s", &c ); // get command
			if(strcmp(c,"Vertex")!=0)
			{
				MessageBox(hwnd,"Error 11",NULL,MB_OK);
				return FALSE;
			}	

			fscanf( fp, "%s", &p ); // ignore vertex number
			
			
			fscanf( fp, "%s", &p ); // ignore "X:"
			fscanf( fp, "%s", &p );
			temp_f = (float)atof(p);
			fwrite( &temp_f, sizeof(float), 1, fp2 );
			

			fscanf( fp, "%s", &p ); // ignore "Y:"
			fscanf( fp, "%s", &p );
			temp_f = (float)atof(p);
			fwrite( &temp_f, sizeof(float), 1, fp2 );


			fscanf( fp, "%s", &p ); // ignore "Z:"
			fscanf( fp, "%s", &p );
			temp_f = (float)atof(p);
			fwrite( &temp_f, sizeof(float), 1, fp2 );
			vert_count++;
		
		}

		// read face list

		fscanf( fp, "%s", &c ); // get command
		if(strcmp(c,"Face")!=0)
		{
			MessageBox(hwnd,"Error 12",NULL,MB_OK);
			return FALSE;
		}
	
		fscanf( fp, "%s", &c ); // get command
		if(strcmp(c,"list:Face")!=0)
		{
			MessageBox(hwnd,"Error 13",NULL,MB_OK);
			return FALSE;
		}
	
		fscanf( fp, "%s", &p ); // ignore "0:"



		for(j=0; j < number_of_faces; j++)
		{
			if(start_face_list_flag == FALSE)
			{
				fscanf( fp, "%s", &c ); // get command
				if(strcmp(c,"Face")!=0)
				{
					MessageBox(hwnd,"Error 15",c,MB_OK);
					return FALSE;
				}
		
				fscanf( fp, "%s", &p ); // ignore face number	
			}

			start_face_list_flag = FALSE;

			fscanf( fp, "%s", &p); 
			q = (char *)p;
			q = _strinc(q);	q = _strinc(q); // ignore "A:"
			temp_i = atoi(q);
			fwrite( &temp_i, sizeof(short), 1, fp2 );
			
			fscanf( fp, "%s", &p); 
			q = (char *)p;
			q = _strinc(q);	q = _strinc(q); // ignore "B:"
			temp_i = atoi(q);
			fwrite( &temp_i, sizeof(short), 1, fp2 );
		
			fscanf( fp, "%s", &p); 
			q = (char *)p;
			q = _strinc(q);	q = _strinc(q); // ignore "C:"
			temp_i = atoi(q);
			fwrite( &temp_i, sizeof(short), 1, fp2 );

			fscanf( fp, "%s", &p); // ignore "AB:1"
			fscanf( fp, "%s", &p); // ignore "BC:1"
			fscanf( fp, "%s", &p); // ignore "CA:1"


			fscanf( fp, "%s", &c ); // get command
			if(strcmp(c,"Smoothing:")!=0)
			{
				MessageBox(hwnd,"Error 14",c,MB_OK);
				return FALSE;
			}


			fscanf( fp, "%s", &p ); // ignore "1"		

			poly_count++;
		}	
		
		fclose(fp);	

	}
	 	
	fclose(fp2);


	// TEST ///////////////////////////////////////////////


	fp = fopen(dest_filename,"r");
	if(fp==NULL)
	{     
		MessageBox(hwnd,"Error can't load file",NULL,MB_OK);
		return FALSE;
	}

	result = _setmode( _fileno( fp ), _O_BINARY );
		
	if( result == -1 )
	{
		MessageBox(hwnd,"Cannot set mode",c,MB_OK);
		return FALSE;
	}

	number_of_frames = 0;
	number_of_vertices = 0;
	number_of_faces = 0;

	// load header info for .rrm file

	fread( &number_of_frames,	sizeof(short), 1, fp );
	fread( &number_of_vertices, sizeof(short), 1, fp );
	fread( &number_of_faces,    sizeof(short), 1, fp );

	itoa(number_of_frames,buffer,10);
	PrintMessage(hwnd, buffer );

	itoa(number_of_vertices,buffer,10);
	PrintMessage(hwnd, buffer );
	
	itoa(number_of_faces,buffer,10);
	PrintMessage(hwnd, buffer );

	fclose(fp);	

	return TRUE;

}



BOOL LoadQ2AnimRRM(HWND hwnd, char *filename, int pmodel_id, world_ptr wptr)
{
	FILE *fp;  

	int i;
	int frame_num;
	char c[100];
	//char buffer[50];
	short number_of_frames;
	short number_of_vertices;
	short number_of_faces;
	int vert_count;
	int vert_num;
	int v_count;
	int result;
	float temp_x, temp_y, temp_z;

	PrintMessage(hwnd, filename);

	fp = fopen(filename,"r");
	if(fp==NULL)
	{     
		MessageBox(hwnd,"Error can't load file",NULL,MB_OK);
		return FALSE;
	}

	result = _setmode( _fileno( fp ), _O_BINARY );
		
	if( result == -1 )
	{
		MessageBox(hwnd,"Cannot set mode",c,MB_OK);
		return FALSE;
	}

	// load header info for .rrm file

	fread( &number_of_frames,	sizeof(short), 1, fp );
	fread( &number_of_vertices, sizeof(short), 1, fp );
	fread( &number_of_faces,    sizeof(short), 1, fp );

	
	for(frame_num = 0; frame_num < number_of_frames; frame_num++) // 198
	{			
		// read vert list

		vert_count = 0;
		
		for(i=0; i < number_of_vertices; i++)
		{
			fread( &temp_x , sizeof(float), 1, fp );
			fread( &temp_y , sizeof(float), 1, fp );
			fread( &temp_z , sizeof(float), 1, fp );

			temp_verts[vert_count].x = temp_x * (float).4;
			temp_verts[vert_count].y = temp_y * (float).4;
			temp_verts[vert_count].z = (temp_z +(float)24) * (float).4;

			vert_count++;
		}
	

		// read face list

		v_count = 0;

		for(i=0; i < number_of_faces; i++)
		{	
			fread( &vert_num , sizeof(short), 1, fp );
			wptr->pmdata[pmodel_id].v[frame_num][v_count].x = temp_verts[vert_num].x ;
			wptr->pmdata[pmodel_id].v[frame_num][v_count].y = temp_verts[vert_num].z ;
			wptr->pmdata[pmodel_id].v[frame_num][v_count].z = temp_verts[vert_num].y ;
			v_count++;

			fread( &vert_num , sizeof(short), 1, fp );
			wptr->pmdata[pmodel_id].v[frame_num][v_count].x = temp_verts[vert_num].x ;
			wptr->pmdata[pmodel_id].v[frame_num][v_count].y = temp_verts[vert_num].z ;
			wptr->pmdata[pmodel_id].v[frame_num][v_count].z = temp_verts[vert_num].y ;
			v_count++;

		
			fread( &vert_num , sizeof(short), 1, fp );
			wptr->pmdata[pmodel_id].v[frame_num][v_count].x = temp_verts[vert_num].x ;
			wptr->pmdata[pmodel_id].v[frame_num][v_count].y = temp_verts[vert_num].z ;
			wptr->pmdata[pmodel_id].v[frame_num][v_count].z = temp_verts[vert_num].y ;
			v_count++;
		}
	}
	fclose(fp);

	wptr->pmdata[pmodel_id].num_polys_per_frame = number_of_faces;
	wptr->pmdata[pmodel_id].num_verts = v_count;

	return 0;
}