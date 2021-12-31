//-----------------------------------------------------------------------------
// File: Collision.cpp
//
// Desc: Code for importing Quake 2 models
//
// Copyright (c) 1999 William Chin. All rights reserved.
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <windows.h>

#include "ImportMD2.hpp"
#include "D3DApp.h"
#include "roadrage.hpp"

extern CMyD3DApplication* pCMyApp;


CMD2* pCMD2;

CMD2::CMD2()
{
	pCMD2 = this;
}

BOOL CMD2::ImportMD2_GLCMD( HWND hwnd, char *filename, int texture_alias, 
					 int pmodel_id, float scale)
{
	FILE *fp;
	MD2HEADER header;
	MD2VERTEX bverts;
	float bscale[3];		
	float translate[3]; 
	char name[16];		
	int i,j;
	int frame_num;
	int cnt;
	int N;
	int id = 1;
	int glnum_verts;
	int num_glverts_per_command;
	float f;
	char buf[256], buffer[256], buffer2[256];
	GLVERT glv[2000];
	int	   glc[2000];
	

	

	int glv_cnt;
	int glc_cnt;

	fp = fopen(filename,"rb");
	if(fp == NULL) 
	{
		MessageBox(hwnd,"Can't open md2",NULL,MB_OK);
		return FALSE;
	}

	
	// read file header into MD2HEADER structure
	fread(&header, sizeof(MD2HEADER), 1, fp); 


	// the glcmd format:
	// a positive integer starts a tristrip command, followed by that many
	// vertex structures.
	// a negative integer starts a trifan command, followed by -x vertexes
	// a zero indicates the end of the command list.
	// a vertex consists of a floating point s, a floating point t,
	// and an integer vertex index.

	fseek(fp, (UINT)header.offset_glcmds, SEEK_SET);
	
	if(header.offset_end == header.offset_glcmds)
	{
		MessageBox(hwnd,"NO GL Commands in this md2",NULL,MB_OK);
		//PrintMessage(NULL, "ERROR : NO GL Commands in this md2", NULL, LOGFILE_ONLY);
		return FALSE;
	}

	glv_cnt = 0;
	glc_cnt = 0;
	
	while(id!=0)
	{	
		fread(&id,sizeof(int),1,fp);
		
		if(id !=0)
		{
			glc[glc_cnt] = id;
			glc_cnt ++;

			num_glverts_per_command = abs(id);

			for(j = 0; j < num_glverts_per_command; j++)
			{
				fread(&f,sizeof(float),1,fp);
				glv[glv_cnt].s = f;
				fread(&f,sizeof(float),1,fp);
				glv[glv_cnt].t = f;
				fread(&N,sizeof(int),1,fp);
				glv[glv_cnt].index = N;

				glv_cnt++;
			}
		}
		
	}
	
	
	strcpy(buffer, "GL commands = ");
	itoa(glc_cnt, buf, 10);
	strcat(buffer, buf);

	strcpy(buffer2, "    GL face indices = ");
	itoa(glv_cnt, buf, 10);
	strcat(buffer2, buf);

	//PrintMessage(NULL, buffer, buffer2, LOGFILE_ONLY);
			
	itoa(header.num_verts, buffer, 10);
	//PrintMessage(NULL, "verts = ", buffer, LOGFILE_ONLY);


	// allocate memory dynamically

	pCMyApp->pmdata[pmodel_id].w = new VERT*[header.num_frames];
	
	for(i = 0; i < header.num_frames; i++)
		pCMyApp->pmdata[pmodel_id].w[i] = new VERT[header.num_verts];

	pCMyApp->pmdata[pmodel_id].f = new int[glv_cnt];
	pCMyApp->pmdata[pmodel_id].num_vert = new int[header.num_verts];
	pCMyApp->pmdata[pmodel_id].poly_cmd = new D3DPRIMITIVETYPE[glc_cnt];
	pCMyApp->pmdata[pmodel_id].texture_list = new int[glc_cnt];
	pCMyApp->pmdata[pmodel_id].t = new VERT[glv_cnt];

	int mem = 0;
	mem += (sizeof(VERT)  * header.num_verts * header.num_frames);
	mem += (sizeof(short) * glv_cnt);
	mem += (sizeof(short) * header.num_verts);
	mem += (sizeof(short) * glc_cnt);
	mem += (sizeof(short) * glc_cnt);
	mem += (sizeof(VERT)  * glv_cnt);

	mem = mem / 1024;

	itoa(mem, buffer, 10);
	strcat(buffer, " KB\n");
	PrintMessage(NULL, "Memory allocated = ", buffer, LOGFILE_ONLY);
	


	// load GL Commands into pmdata structure

	cnt = 0;

	for(i = 0; i < glc_cnt; i++)
	{
		if(glc[i] == 0)
		{
			PrintMessage(NULL, "ERROR MD2 GL COMMAND DATA", NULL, LOGFILE_ONLY);
			return FALSE;
		}

		if(glc[i] < 0 )
			pCMyApp->pmdata[pmodel_id].poly_cmd[i] = D3DPT_TRIANGLEFAN;

		if(glc[i] > 0 )
			pCMyApp->pmdata[pmodel_id].poly_cmd[i] = D3DPT_TRIANGLESTRIP;

		pCMyApp->pmdata[pmodel_id].texture_list[i] = texture_alias;

		glnum_verts = abs(glc[i]); 
		pCMyApp->pmdata[pmodel_id].num_vert[i] = glnum_verts;

		for(j = 0; j < glnum_verts; j++)
		{
			pCMyApp->pmdata[pmodel_id].f[cnt]   = glv[cnt].index;  
			
			pCMyApp->pmdata[pmodel_id].t[cnt].x = glv[cnt].s * header.skinwidth;
			pCMyApp->pmdata[pmodel_id].t[cnt].y = glv[cnt].t * header.skinheight;	
	
			cnt++;
		}
	}

	// read vertices for all frames

	fseek(fp, (UINT)header.offset_frames, SEEK_SET);

	for(frame_num = 0; frame_num < header.num_frames; frame_num++)
	{
		fread(bscale,sizeof(float),3,fp);		
		fread(translate,sizeof(float),3,fp);
		fread(name,1,16,fp);				
		
		//strcpy(frame_list[frame_num].framename, name );
		

		for(j = 0; j < header.num_verts; j++) // VERTS
		{
			fread(&bverts,sizeof(MD2VERTEX),1,fp); 

			pCMyApp->pmdata[pmodel_id].w[frame_num][j].x = scale * (bscale[0] * bverts.v[0] + translate[0]);
			pCMyApp->pmdata[pmodel_id].w[frame_num][j].y = scale * (bscale[1] * bverts.v[1] + translate[1]);
			pCMyApp->pmdata[pmodel_id].w[frame_num][j].z = scale * (bscale[2] * bverts.v[2] + translate[2]);
		}
	}

	pCMyApp->pmdata[pmodel_id].num_polys_per_frame = glc_cnt;
	pCMyApp->pmdata[pmodel_id].num_faces = glc_cnt;
	pCMyApp->pmdata[pmodel_id].num_verts = cnt; //glv_cnt; // cnt; 
	pCMyApp->pmdata[pmodel_id].scale = scale;
	fclose(fp);

	

	pCMyApp->pmdata[pmodel_id].tex_alias = texture_alias;	
	
	pCMyApp->pmdata[pmodel_id].skx = (float)1 / header.skinwidth; 
	pCMyApp->pmdata[pmodel_id].sky = (float)1 / header.skinheight; 
	pCMyApp->pmdata[pmodel_id].num_frames = header.num_frames;
	pCMyApp->pmdata[pmodel_id].use_indexed_primitive = FALSE;

	return TRUE;
}


