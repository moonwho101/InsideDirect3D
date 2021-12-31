//-----------------------------------------------------------------------------
// File: RoadRage.cpp
//
// Desc: The main RoadRage code.  The CMyD3DApplication class handles
//		 most of the RoadRage functionality.
//
// Copyright (c) 1999 William Chin and Peter Kovach. All rights reserved.
//-----------------------------------------------------------------------------
#define STRICT
#define D3D_OVERLOADS
#include "resource.h"
#include <math.h>
#include <time.h>
#include <stdio.h>
#include "D3DApp.h"
#include <dsound.h>
#include "D3DUtil.h"
#include "D3DEnum.h"
#include "world.hpp"
#include "3Dengine.hpp"
#include "RoadRage.hpp"
#include "Import3DS.hpp"
#include "LoadWorld.hpp"
#include <tchar.h>

#define SND_FIRE_AR15  0
#define SND_FIRE_MP5   1

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

#define MAX_SOUND_BUFFERS 30
#define MAX_PLAYER_NAME      14
#define MAX_SESSION_NAME     256
#define MAX_NUM_OBJECTS_PER_CELL 50

#define EXITCODE_FORWARD     1  // Dialog success, so go forward
#define EXITCODE_BACKUP      2  // Dialog canceled, show previous dialog
#define EXITCODE_QUIT        3  // Dialog quit, close app
#define EXITCODE_ERROR       4  // Dialog error, terminate app


// App specific DirectPlay messages and structures 
//-----------------------------------------------------------------------------
#define GAMEMSG_WAVE        1

enum APPMSGTYPE { MSG_NONE, MSGERR_APPMUSTEXIT, MSGWARN_SWITCHEDTOSOFTWARE };

extern BOOL FAR PASCAL AppAbout(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK AboutProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM );
extern LPDIRECTSOUNDBUFFER g_pDSBuffer[MAX_SOUND_BUFFERS];

FLOAT                g_fCurrentTime;
extern GUID                 g_AppGUID; 
extern TCHAR                g_strAppName[256];
		
extern C3DS* pC3DS;

BOOL bIsFlashlightOn    = FALSE;
BOOL logfile_start_flag = TRUE;
BOOL RR_exit_debug_flag = FALSE;
SETUPINFO setupinfo;
HDC hdc;

int PrintMsgX = 10;
int PrintMsgY = 10;

int num_op_guns = 0;
int num_cars;

int total_allocated_memory_count = 0;
int num_light_sources;
float angle_deg = 0.0f; // debug
float temp_f	= 0.0f; // debug

float weapon_flash_r;
float weapon_flash_g;

CMyD3DApplication* pCMyApp;
extern CLoadWorld* pCWorld;

void PrintMemAllocated(int mem, char *message);
VOID DisplayError( CHAR* strMessage );

BOOL bSkipThisCell;


//-----------------------------------------------------------------------------
// Name: AboutProc()
// Desc: Minimal message proc function for the about box
//-----------------------------------------------------------------------------
BOOL CALLBACK AboutProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM )
{
    if( WM_COMMAND == uMsg )
        if( IDOK == LOWORD(wParam) || IDCANCEL == LOWORD(wParam) )
            EndDialog (hWnd, TRUE);

    return ( WM_INITDIALOG == uMsg ) ? TRUE : FALSE;
}


//-----------------------------------------------------------------------------
// Name: CMyD3DApplication()
// Desc: Application constructor. Sets attributes for the app.
//-----------------------------------------------------------------------------
CMyD3DApplication::CMyD3DApplication()                
{
    m_strWindowTitle  = TEXT( "Chapter 6" );
    m_bAppUseZBuffer  = TRUE;
    m_fnConfirmDevice = NULL;

    m_dltType = D3DLIGHT_SPOT;
	k = (float)0.017453292;
	
	m_vUpVec = D3DVECTOR(0, 1, 0);
	
	rendering_first_frame = TRUE;
	RRAppActive			  = FALSE;
	firing_gun_flag		  = FALSE;
	delay_key_press_flag  = FALSE;
	display_scores		  = FALSE;
	bInWalkMode			  = TRUE;
    m_bShowStats      = FALSE;
    m_bWindowed    = TRUE;
    m_bToggleFullscreen = FALSE;
    m_bUseViewPoints = FALSE;

	m_num_sounds = 0;
	num_players			= 0;
	num_your_guns		= 0;
	num_cars			= 0;
	num_debug_models	= 0;
	current_frame		= 0;
	current_sequence	= 0;
	current_frame		= 0;		
	current_sequence	= 0;

	car_gear			= 1;
	editor_insert_mode  = 0;
	object_rot_angle	= 0;	
	toggle_delay		= 0;

	tx_rate_ps = 50;
	rx_rate_ps = 50;

	current_screen_status = FULLSCREEN_ONLY; // default, in case rrsetup.ini 
	
	pi = 3.141592654f;
	piover2 = (pi / 2.0f);

	Z_NEAR = D3DVAL(6.0);
	Z_FAR = D3DVAL(10000.0);
	FOV = D3DVAL(pi / 2.0f); // Field Of View set to 90 degrees 

    // Move the camera position around
    D3DVECTOR vFrom( 700.0f, 22.0f, 700.0f );
    D3DVECTOR vAt(850.0f, 25.0f, 480.0f);
    D3DVECTOR vUp(0.0f, 1.0f, 0.0f);
    SetViewParams( &vFrom, &vAt, &vUp, 0.1f);

	IsRenderingOk = FALSE;
	bEnableLighting = TRUE;
	ShadeMode = D3DSHADE_GOURAUD;


	pCMyApp = this;
}


//-----------------------------------------------------------------------------
// Name: OutputText()
// Desc: Draws text on the window.
//-----------------------------------------------------------------------------
VOID CMyD3DApplication::OutputText( DWORD x, DWORD y, TCHAR* str )
{
    HDC hDC;

    // Get a DC for the surface. Then, write out the buffer
    if( m_pddsRenderTarget )
    {
        if( SUCCEEDED( m_pddsRenderTarget->GetDC(&hDC) ) )
        {
            SetTextColor( hDC, RGB(255,255,0) );
            SetBkMode( hDC, TRANSPARENT );
            ExtTextOut( hDC, x, y, 0, NULL, str, lstrlen(str), NULL );
            m_pddsRenderTarget->ReleaseDC(hDC);
        }
    }

    // Do the same for the left surface (in case of stereoscopic viewing).
    if( m_pddsRenderTargetLeft )
    {
        if( SUCCEEDED( m_pddsRenderTargetLeft->GetDC( &hDC ) ) )
        {
            // Use a different color to help distinguish left eye view
            SetTextColor( hDC, RGB(255,0,255) );
            SetBkMode( hDC, TRANSPARENT );
            ExtTextOut( hDC, x, y, 0, NULL, str, lstrlen(str), NULL );
            m_pddsRenderTargetLeft->ReleaseDC(hDC);
        }
    }
}


//-----------------------------------------------------------------------------
// Name: ShowStats()
// Desc: Shows frame rate and dimensions of the rendering device.
//-----------------------------------------------------------------------------

VOID CMyD3DApplication::ShowStats()
{
    static FLOAT fFPS      = 0.0f;
    static FLOAT fLastTime = 0.0f;
    static DWORD dwFrames  = 0L;

    // Keep track of the time lapse and frame count
    FLOAT fTime = timeGetTime() * 0.001f; // Get current time in seconds
    ++dwFrames;

    // Update the frame rate once per second
    if( fTime - fLastTime > 1.0f )
    {
        fFPS      = dwFrames / (fTime - fLastTime);
        fLastTime = fTime;
        dwFrames  = 0L;
    }

    // Setup the text buffer to write out dimensions
    CHAR           buffer[80];
    DDSURFACEDESC2 ddsd;
    ddsd.dwSize = sizeof(DDSURFACEDESC2);
    m_pddsRenderTarget->GetSurfaceDesc( &ddsd );

    sprintf( buffer, "%7.02f fps (%dx%dx%d)", fFPS, ddsd.dwWidth,
             ddsd.dwHeight, ddsd.ddpfPixelFormat.dwRGBBitCount );
    OutputText( 0, 0, buffer );

	sprintf( buffer, "health : %d", MyHealth);
	OutputText( 300, 0, buffer );
}


//-----------------------------------------------------------------------------
// Name: Cleanup3DEnvironment()
// Desc: Cleanup scene objects
//-----------------------------------------------------------------------------
VOID CMyD3DApplication::Cleanup3DEnvironment()
{
    SetbActive(FALSE);
    SetbReady(FALSE);

    if( GetFramework() )
    {
        DeleteDeviceObjects();
        DeleteFramework();

        FinalCleanup();
    }

    D3DEnum_FreeResources();
}


LRESULT CMyD3DApplication::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam,
                                  LPARAM lParam )
{
	HMENU hMenu;

	m_hWnd = hWnd;
		
	hMenu = GetMenu( hWnd );

    switch( uMsg )
    {
		case WM_ACTIVATEAPP:
			RRAppActive = (BOOL)wParam;
            break;

		case WM_INITMENUPOPUP:
			CheckMenuItem (hMenu, MENU_LIGHT_SOURCES, bEnableLighting ? MF_CHECKED : MF_UNCHECKED);
			
			if(ShadeMode == D3DSHADE_FLAT)
			{
				CheckMenuItem (hMenu, MENU_FLAT,	MF_CHECKED);
				CheckMenuItem (hMenu, MENU_GOURAUD, MF_UNCHECKED);
			}
			if(ShadeMode == D3DSHADE_GOURAUD)
			{
				CheckMenuItem (hMenu, MENU_FLAT,	MF_UNCHECKED);
				CheckMenuItem (hMenu, MENU_GOURAUD, MF_CHECKED);
			}

			break;

        case WM_COMMAND:
            switch( LOWORD(wParam) )
            {
				case MENU_ABOUT:
					Pause(TRUE);
					DialogBox(hInstApp, MAKEINTRESOURCE(IDD_ABOUT), hWnd, (DLGPROC)AppAbout);
					Pause(FALSE);
                break;

				// Shading selection
				case MENU_FLAT_SHADING:
					CheckMenuItem (hMenu, MENU_FLAT,	MF_CHECKED);
					CheckMenuItem (hMenu, MENU_GOURAUD, MF_UNCHECKED);
					ShadeMode = D3DSHADE_FLAT;
					m_pd3dDevice->SetRenderState( D3DRENDERSTATE_SHADEMODE,
												D3DSHADE_FLAT );
					break;

				case MENU_GOURAUD_SHADING:
					CheckMenuItem (hMenu, MENU_FLAT,	MF_UNCHECKED);
					CheckMenuItem (hMenu, MENU_GOURAUD, MF_CHECKED);
					ShadeMode = D3DSHADE_GOURAUD;
					m_pd3dDevice->SetRenderState( D3DRENDERSTATE_SHADEMODE,
												D3DSHADE_GOURAUD );
					break;

           
				case MENU_LIGHT_SOURCES:
					bEnableLighting = !bEnableLighting;
					CheckMenuItem (hMenu, MENU_LIGHT_SOURCES, bEnableLighting ? MF_CHECKED : MF_UNCHECKED);
					break;

                case IDM_EXIT:
                    PrintMessage(NULL, "MsgProc - IDM_EXIT", NULL, LOGFILE_ONLY);
					Cleanup3DEnvironment();
                    SendMessage( hWnd, WM_CLOSE, 0, 0 );
					DestroyWindow( hWnd );
					PostQuitMessage(0);
                    exit(0);

            	default:
					return CD3DApplication::MsgProc( hWnd, uMsg, wParam,
														 lParam );
			}
            break;

        case WM_GETMINMAXINFO:
            ((MINMAXINFO*)lParam)->ptMinTrackSize.x = 100;
            ((MINMAXINFO*)lParam)->ptMinTrackSize.y = 100;
            break;

        case WM_CLOSE:
			PrintMessage(NULL, "MsgProc - WM_CLOSE", NULL, LOGFILE_ONLY);
			Cleanup3DEnvironment();
            DestroyWindow( hWnd );
            PostQuitMessage(0);
            return 0;

		default:
			return CD3DApplication::MsgProc( hWnd, uMsg, wParam,
											 lParam );
    }

    return DefWindowProc( hWnd, uMsg, wParam, lParam );
}
  
  

void CMyD3DApplication::AddPlayerLightSource(int player_num, float x, float y, float z)
{ 
	float pos_x, pos_y, pos_z;
	float dir_x, dir_y, dir_z;
    D3DLIGHT7 light;


	if(bEnableLighting == FALSE)
		return;

    ZeroMemory( &light, sizeof(D3DLIGHT7) );
    
    light.dcvDiffuse.r  = 0.0f;
    light.dcvDiffuse.g  = 0.0f;
    light.dcvDiffuse.b  = 0.0f;

	weapon_flash_r -= 0.05f; 
	weapon_flash_g = weapon_flash_r / 2.0f;

	light.dcvAmbient.r  = weapon_flash_r; //0.4f;
	light.dcvAmbient.g  = weapon_flash_g; //0.2f;
	light.dcvAmbient.b  = 0.0f;

	if(weapon_flash_r < 0)
	{
		weapon_flash_r = 0.4f;
		weapon_flash_r = 0.2f;
		player_list[player_num].bIsFiring = FALSE;
	}
	light.dcvSpecular.r  = 1.0f;
	light.dcvSpecular.g  = 1.0f;
	light.dcvSpecular.b  = 1.0f;
	
    light.dvRange       = 200.0f;

	pos_x = x;
	pos_y = -100;
	pos_z = z;

	dir_x = 0;
	dir_y = 100;
	dir_z = 0;


	light.dvPosition	  = D3DVECTOR( pos_x, pos_y, pos_z );
    light.dvDirection     = D3DVECTOR( dir_x, dir_y, dir_z );
    light.dvFalloff       = 100.0f;

	// angle dvPhi   must be within the range 0 to pi	 radians
	// angle dvTheta must be within the range 0 to dvPhi radians

    light.dvTheta         = 1.4f;	// spotlight's inner cone
    light.dvPhi           = 2.1f;	// spotlight's outer cone
    light.dvAttenuation0  = 1.0f;
	light.dltType		  = D3DLIGHT_SPOT;
		
	// Set the light
	
	m_pd3dDevice->SetLight( num_light_sources, &light );
	m_pd3dDevice->LightEnable( (DWORD)num_light_sources, TRUE );
	num_light_sources++;
}


void CMyD3DApplication::PlayerToD3DVertList(int pmodel_id, int curr_frame, int angle, int texture_alias, int tex_flag)
{ 
	int i,j;
	int num_verts_per_poly;
	int num_poly;
	int i_count;
	short v_index;
	float x,y,z;
	float rx,ry,rz;
	vert_ptr tp;	
	DWORD r,g,b;
	D3DPRIMITIVETYPE p_command;
	BOOL error = TRUE;
	

	if(angle >= 360)
		angle = angle - 360;
	if(angle < 0)
		angle += 360;

	if(pmdata[pmodel_id].use_indexed_primitive == TRUE)
	{
		PlayerToD3DIndexedVertList(pmodel_id, curr_frame, angle, texture_alias, tex_flag);
		return;
	}

	cosine = cos_table[angle];
	sine   = sin_table[angle];

	if(curr_frame >= pmdata[pmodel_id].num_frames)
		curr_frame = 0;

	i_count = 0;

	num_poly = pmdata[pmodel_id].num_polys_per_frame;
	
	for(i = 0; i < num_poly; i++)
	{
		p_command	   = pmdata[pmodel_id].poly_cmd[i];
		num_verts_per_poly = pmdata[pmodel_id].num_vert[i];
		
		for(j = 0; j < num_verts_per_poly; j++)
		{
			v_index = pmdata[pmodel_id].f[i_count];

			tp = &pmdata[pmodel_id].w[curr_frame][v_index];
			x = tp->x + x_off;
			z = tp->y + y_off;
			y = tp->z + z_off;

			rx = wx + (x*cosine - z*sine); 
			ry = wy + y;
			rz = wz + (x*sine + z*cosine);
				
			r=0;
			g=0;
			b=0;

			src_v[cnt].x = D3DVAL(rx);
			src_v[cnt].y = D3DVAL(ry);
			src_v[cnt].z = D3DVAL(rz);

			cnt++;
			i_count++;

		}
	
		verts_per_poly[number_of_polys_per_frame] = num_verts_per_poly;
		dp_command_index_mode[number_of_polys_per_frame] = USE_NON_INDEXED_DP;
		dp_commands[number_of_polys_per_frame] = p_command;

		if((p_command == D3DPT_TRIANGLESTRIP) || (p_command == D3DPT_TRIANGLEFAN))
			num_triangles_in_scene += ( num_verts_per_poly - 2 );

		if(p_command == D3DPT_TRIANGLELIST)
			num_triangles_in_scene += ( num_verts_per_poly / 3 );


		num_verts_in_scene +=	num_verts_per_poly;
		num_dp_commands_in_scene ++;
						
		number_of_polys_per_frame++;

	} // end for vert_cnt
		
	return;
}

void CMyD3DApplication::PlayerToD3DIndexedVertList(int pmodel_id, int curr_frame, int angle, int texture_alias, int tex_flag)
{ 
	int i,j;
	int num_verts_per_poly;
	int num_faces_per_poly;
	int num_poly;
	int poly_command;
	int i_count, face_i_count;
	float x,y,z;
	float rx,ry,rz;
	FILE *fp;

	if(curr_frame >= pmdata[pmodel_id].num_frames)
		curr_frame = 0;

	curr_frame = 0;
	cosine = cos_table[angle];
	sine   = sin_table[angle];

	i_count = 0;
	face_i_count = 0;

	if(rendering_first_frame == TRUE)
		fp = fopen("rrlogfile.txt","a");

	// process and transfer the model data from the pmdata structure 
	// to the array of D3DVERTEX structures, src_v 
	
	num_poly = pmdata[pmodel_id].num_polys_per_frame;
	
	for(i = 0; i < num_poly; i++)
	{
		poly_command	   = pmdata[pmodel_id].poly_cmd[i];
		num_verts_per_poly = pmdata[pmodel_id].num_verts_per_object[i];
		num_faces_per_poly = pmdata[pmodel_id].num_faces_per_object[i];
		
		for(j = 0; j < num_verts_per_poly; j++)
		{
			
			x = pmdata[pmodel_id].w[curr_frame][i_count].x + x_off;
			z = pmdata[pmodel_id].w[curr_frame][i_count].y + y_off;
			y = pmdata[pmodel_id].w[curr_frame][i_count].z + z_off;

			rx = wx + (x*cosine - z*sine); 
			ry = wy + y;
			rz = wz + (x*sine + z*cosine);
				
			src_v[cnt].x =  D3DVAL(rx);
			src_v[cnt].y =  D3DVAL(ry);
			src_v[cnt].z =  D3DVAL(rz);

			if(rendering_first_frame == TRUE)
			{
				fprintf( fp, "%f %f %f, ", 
				//cnt,
				src_v[cnt].x,
				src_v[cnt].y,
				src_v[cnt].z);
			}

			cnt++;
			i_count++;

		} // end for j
	
		for(j = 0; j < num_faces_per_poly*3; j++)
		{
			src_f[cnt_f] = pmdata[pmodel_id].f[face_i_count];

			if(rendering_first_frame == TRUE)
			{
				//fprintf( fp, "%d ", src_f[cnt_f] );
			}

			cnt_f++;
			face_i_count++;
		}

		verts_per_poly[number_of_polys_per_frame] = num_verts_per_poly;
		faces_per_poly[number_of_polys_per_frame] = num_faces_per_poly;

		dp_command_index_mode[number_of_polys_per_frame] = USE_INDEXED_DP;
		dp_commands			 [number_of_polys_per_frame] = D3DPT_TRIANGLELIST;
		
		num_triangles_in_scene += num_faces_per_poly;
		num_verts_in_scene	   += num_verts_per_poly;
		num_dp_commands_in_scene ++;
								
		number_of_polys_per_frame++;

	} // end for vert_cnt	
	
	if(rendering_first_frame == TRUE)
	{
	fprintf( fp, " \n\n"); 
	fclose(fp);
	}

	return;
}


void CMyD3DApplication::ObjectToD3DVertList(int ob_type, int angle, int oblist_index)
{ 
	int ob_vert_count = 0;
	int poly;
	int num_vert;
	int vert_cnt;
	int w, i;
	float x,y,z;
	float pos_x, pos_y, pos_z;
	float dir_x, dir_y, dir_z;
	D3DPRIMITIVETYPE poly_command; 
	BOOL poly_command_error = TRUE;
 	
	
	if( (oblist[oblist_index].light_source->command != 0 ) &&
		(oblist_overlite_flags[oblist_index] == FALSE)     &&
		(bEnableLighting) )
						
	{

		oblist_overlite_flags[oblist_index] = TRUE;

		// Set up the light structure
		D3DLIGHT7 light;
		ZeroMemory( &light, sizeof(D3DLIGHT7) );
    
		light.dcvDiffuse.r  = 0.0f;
		light.dcvDiffuse.g  = 0.0f;
		light.dcvDiffuse.b  = 0.0f;

		light.dcvAmbient.r  = 0.4f;
		light.dcvAmbient.g  = 0.2f;
		light.dcvAmbient.b  = 0.0f;

		light.dcvSpecular.r  = 1.0f;
		light.dcvSpecular.g  = 1.0f;
		light.dcvSpecular.b  = 1.0f;
	 
		//light.dvRange     = D3DLIGHT_RANGE_MAX;
		light.dvRange       = 200.0f;

		pos_x = oblist[oblist_index].light_source->position_x;
		pos_y = oblist[oblist_index].light_source->position_y;
		pos_z = oblist[oblist_index].light_source->position_z;

		dir_x = oblist[oblist_index].light_source->direction_x;
		dir_y = oblist[oblist_index].light_source->direction_y;
		dir_z = oblist[oblist_index].light_source->direction_z;


		switch( oblist[oblist_index].light_source->command )
		{
		
			case POINT_LIGHT_SOURCE:
				light.dvPosition      = D3DVECTOR( pos_x, pos_y, pos_z ); 
				light.dvAttenuation1  = 1.0f;
				light.dltType		  = D3DLIGHT_POINT; 
				break;

			case DIRECTIONAL_LIGHT_SOURCE:
				light.dvDirection     = D3DVECTOR( dir_x, dir_y, dir_z );
				light.dltType		  = D3DLIGHT_DIRECTIONAL; 
				break;

			case SPOT_LIGHT_SOURCE:
				light.dvPosition	  = D3DVECTOR( pos_x, pos_y, pos_z );
				light.dvDirection     = D3DVECTOR( dir_x, dir_y, dir_z );
				light.dvFalloff       = 100.0f;

				// angle dvPhi   must be within the range 0 to pi	 radians
				// angle dvTheta must be within the range 0 to dvPhi radians

				light.dvTheta         = 1.4f;	// spotlight's inner cone
				light.dvPhi           = 2.1f;	// spotlight's outer cone
				light.dvAttenuation0  = 1.0f;
				light.dltType		  = D3DLIGHT_SPOT;
				break;
		}

		// Set the light
	
		m_pd3dDevice->SetLight( num_light_sources, &light );
		m_pd3dDevice->LightEnable( (DWORD)num_light_sources, TRUE );
		num_light_sources++;
	}

	if(bSkipThisCell == TRUE)
		return;

	cosine = cos_table[angle];
	sine   = sin_table[angle];

	ob_vert_count = 0;
	poly = num_polys_per_object[ob_type];

	for(w=0; w<poly; w++)
	{
		num_vert = obdata[ob_type].num_vert[w];

		for(vert_cnt = 0; vert_cnt < num_vert; vert_cnt++)
		{
			x = obdata[ob_type].v[ob_vert_count].x;
			y = obdata[ob_type].v[ob_vert_count].y;
			z = obdata[ob_type].v[ob_vert_count].z;

			tx[vert_cnt] = obdata[ob_type].t[ob_vert_count].x; 
			ty[vert_cnt] = obdata[ob_type].t[ob_vert_count].y; 

			mx[vert_cnt] = wx + (x*cosine - z*sine); 
			my[vert_cnt] = wy + y;
			mz[vert_cnt] = wz + (x*sine + z*cosine);

			ob_vert_count++;

		} // end for vert_cnt
		
		verts_per_poly[number_of_polys_per_frame] = num_vert;
		poly_command = obdata[ob_type].poly_cmd[w];

			for(i = 0; i < verts_per_poly[number_of_polys_per_frame]; i++)
			{
			
				src_v[cnt].x  = D3DVAL(mx[i]);
				src_v[cnt].y  = D3DVAL(my[i]);
				src_v[cnt].z  = D3DVAL(mz[i]);
				src_v[cnt].tu = D3DVAL(tx[i]); 
				src_v[cnt].tv = D3DVAL(ty[i]);
				cnt++;
			}
			
		dp_commands[number_of_polys_per_frame] = poly_command;
		dp_command_index_mode[number_of_polys_per_frame] = USE_NON_INDEXED_DP;
		number_of_polys_per_frame++;
		
		if((poly_command == D3DPT_TRIANGLESTRIP) || (poly_command == D3DPT_TRIANGLEFAN))
			num_triangles_in_scene += ( num_vert - 2 );

		if(poly_command == D3DPT_TRIANGLELIST)
			num_triangles_in_scene += ( num_vert / 3 );

		num_verts_in_scene +=	num_vert;
		num_dp_commands_in_scene ++;

		if((poly_command < 0) || (poly_command > 6))
			PrintMessage(NULL, "CMyD3DApplication::ObjectToD3DVertList -  ERROR UNRECOGNISED COMMAND", NULL , LOGFILE_ONLY);
	
	} // end for w

	return;
}


//-----------------------------------------------------------------------------
// Name: Render3DEnvironment()
// Desc: Draws the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::Render3DEnvironment()
{
    HRESULT hr;

    // Check the cooperative level before rendering
    if( FAILED( hr = m_pDD->TestCooperativeLevel() ) )
    {
        switch( hr )
        {
            case DDERR_EXCLUSIVEMODEALREADYSET:
            case DDERR_NOEXCLUSIVEMODE:
                // Do nothing because some other app has exclusive mode
                return S_OK;

            case DDERR_WRONGMODE:
                // The display mode changed on us. Resize accordingly
                if( m_pDeviceInfo->bWindowed )
                    return Change3DEnvironment();
                break;
        }
        return hr;
    }

    // Get the relative time, in seconds
    FLOAT fTime = ( timeGetTime() - GetBaseTime() ) * 0.001f;

    // FrameMove (animate) the scene
    if( GetbFrameMoving() || GetSingleStep() )
    {
        if( FAILED( hr = FrameMove( fTime ) ) )
            return hr;

        SetSingleStep(FALSE);
    }

    // If the display is in a stereo mode, render the scene from the left eye
    // first, then the right eye.
    if( m_bAppUseStereo && m_pDeviceInfo->bStereo && !m_pDeviceInfo->bWindowed )
    {
        // Render the scene from the left eye
        m_pd3dDevice->SetTransform( D3DTRANSFORMSTATE_VIEW, &m_matLeftView );
        if( FAILED( hr = m_pd3dDevice->SetRenderTarget( m_pddsRenderTargetLeft, 0 ) ) )
            return hr;
        if( FAILED( hr = Render() ) )
            return hr;

        //Render the scene from the right eye
        m_pd3dDevice->SetTransform( D3DTRANSFORMSTATE_VIEW, &m_matRightView );
        if( FAILED( hr = m_pd3dDevice->SetRenderTarget( m_pddsRenderTarget, 0 ) ) )
            return hr;
        if( FAILED( hr = Render() ) )
            return hr;
    } 
    else 
    {
        // Set center viewing matrix if app is stereo-enabled
        if( m_bAppUseStereo )
            m_pd3dDevice->SetTransform( D3DTRANSFORMSTATE_VIEW, &m_matView );

        // Render the scene as normal
        if( FAILED( hr = Render() ) )
            return hr;
    }

    // Show the frame rate, etc.
    if( m_bShowStats )
        ShowStats();

    // Show the frame on the primary surface.
    if( FAILED( hr = (GetFramework())->ShowFrame() ) )
    {
        if( DDERR_SURFACELOST != hr )
            return hr;

        (GetFramework())->RestoreSurfaces();
        RestoreSurfaces();
    }

    return S_OK;
}


void CMyD3DApplication::InitRRvariables()
{
	int i;
	float fangle;
	float dx, dy;
	RECT szClient;

	GetClientRect(m_hWnd, &szClient);

	dx = (float)(szClient.right - szClient.left);
	dy = (float)(szClient.bottom - szClient.top);
	ASPECT_RATIO = 1.0f;

	PrintMessage(NULL, "CMyD3DApplication::InitRRvariables - starting", NULL, LOGFILE_ONLY);

	walk_mode_enabled = TRUE;
	current_gun = 0;

	car_speed = (float)0;
	angy = 0; 
	look_up_ang = 0;
			
	oblist = new OBJECTLIST[10000]; 
	obdata = new OBJECTDATA[100]; 

	src_v = new D3DVERTEX[MAX_NUM_VERTICES]; 

	player_list				= new PLAYER[10];
	car_list				= new PLAYER[10];
	debug					= new PLAYER[10];
	your_gun				= new GUNLIST[10];
	other_players_guns		= new GUNLIST[10];
	pmdata					= new PLAYERMODELDATA[10];

	poly_clip_flags			= new int[MAX_NUM_QUADS]; 
	poly_clip				= new int[MAX_NUM_QUADS];

	dp_command_index_mode	= new BOOL[MAX_NUM_QUADS];   
	dp_commands				= new D3DPRIMITIVETYPE[MAX_NUM_QUADS]; 

	verts_per_poly			= new int[MAX_NUM_QUADS]; 
	faces_per_poly			= new int[MAX_NUM_QUADS];
	src_f					= new int[MAX_NUM_FACE_INDICES];

	oblist_overdraw_flags   = new BOOL[MAX_NUM_QUADS];
	oblist_overlite_flags   = new BOOL[MAX_NUM_QUADS];
	num_vert_per_object		= new int[100];

	cell_length = new int*[200];

	for(i=0;i<200;i++)
		cell_length[i] = new int[200];


	draw_flags = new BOOL*[200];

	for(i=0;i<200;i++)
		draw_flags[i] = new BOOL[200];

	for(i=0;i<256;i++)
		DelayKey2[i] = FALSE;

	for(i = 0; i < 10; i++)
	{
		player_list[i].frags = 0;	
		player_list[i].x = 0;
		player_list[i].y = 0;
		player_list[i].z = 0;
		player_list[i].rot_angle = 0;
		player_list[i].model_id = 0;
		player_list[i].bIsFiring = FALSE;
		player_list[i].health = 100;
		player_list[i].bIsPlayerAlive = TRUE;
		player_list[i].bStopAnimating = FALSE;
		player_list[i].current_weapon = 0;
		player_list[i].current_car = 0;
		player_list[i].current_frame = 0;
		player_list[i].current_sequence = 0;
		player_list[i].bIsPlayerInWalkMode = TRUE;
		player_list[i].bIsPlayerValid = FALSE;
		strcpy(player_list[i].name, "");
	}

	MyHealth = 100;

	for(i=0;i<256;i++) 
		DelayKey2[i] = FALSE;

	for(i=0;i<=360;i++) 
	{
		fangle = (float)i * k;
		sin_table[i] = (float)sin(fangle);
		cos_table[i] = (float)cos(fangle);
	}

	m_vEyePt.x = 700;
	m_vEyePt.y = 22;
	m_vEyePt.z = 700;


	your_gun[0].sound_id = SND_FIRE_AR15;
	your_gun[1].sound_id = SND_FIRE_MP5;
	current_gun = 0;
	
	PrintMemAllocated(sizeof(OBJECTLIST[10000]), "oblist"); 
	PrintMemAllocated(sizeof(OBJECTDATA[100]), "obdata"); 

	PrintMemAllocated(sizeof(D3DVERTEX[MAX_NUM_VERTICES]), "src_v"); 

	PrintMemAllocated(sizeof(PLAYER[10]), "player_list"); 
	PrintMemAllocated(sizeof(PLAYER[10]), "car_list"); 
	PrintMemAllocated(sizeof(PLAYER[10]), "debug"); 
	PrintMemAllocated(sizeof(GUNLIST[10]), "your_gun"); 
	PrintMemAllocated(sizeof(GUNLIST[10]), "other_players_guns"); 
	PrintMemAllocated(sizeof(PLAYERMODELDATA[10]),"pmdata"); 

	PrintMemAllocated(sizeof(int[MAX_NUM_QUADS]), "poly_clip_flags"); 
	PrintMemAllocated(sizeof(int[MAX_NUM_QUADS]), "poly_clip"); 
	PrintMemAllocated(sizeof(int[MAX_NUM_QUADS]), "dp_command_index_mode"); 
	PrintMemAllocated(sizeof(int[MAX_NUM_QUADS]), "dp_commands"); 
	PrintMemAllocated(sizeof(int[MAX_NUM_QUADS]), "verts_per_poly"); 
	PrintMemAllocated(sizeof(int[MAX_NUM_QUADS]), "faces_per_poly"); 
	PrintMemAllocated(sizeof(int[MAX_NUM_FACE_INDICES]), "src_f"); 
	
	PrintMemAllocated(sizeof(BOOL[MAX_NUM_QUADS]), "oblist_overdraw_flags");
	PrintMemAllocated(sizeof(BOOL[MAX_NUM_QUADS]), "oblist_overlite_flags");

	PrintMemAllocated(total_allocated_memory_count, "TOTAL"); 

}


//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------

BOOL CMyD3DApplication::LoadRR_Resources()
{
	if (!pCWorld->LoadObjectData(m_hWnd,"objects.dat"))
	{
		PrintMessage(m_hWnd, "LoadObjectData failed", NULL, LOGFILE_ONLY);
		return FALSE;
	}
	
	if (!pCWorld->LoadWorldMap  (m_hWnd,"level1.map"))
	{
		PrintMessage(m_hWnd, "LoadWorldMap failed", NULL, LOGFILE_ONLY);
		return FALSE;
	}

	if (!pCWorld->InitPreCompiledWorldMap(m_hWnd,"level1.cmp"))
	{
		PrintMessage(m_hWnd, "InitPreCompiledWorldMap failed", NULL, LOGFILE_ONLY);
		return FALSE;
	}	
	
	if (!pCWorld->LoadImportedModelList(m_hWnd,"modellist.dat"))
	{
		PrintMessage(m_hWnd, "LoadImportedModelList failed", NULL, LOGFILE_ONLY);
		return FALSE;
	}

	OpenSoundFile("colt.wav", 0);
	OpenSoundFile("engine.wav", 1);
	
	return TRUE;
}




//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
{
	HRESULT hr;
	CMyD3DApplication d3dApp;

	d3dApp.hInstApp = hInst;

    if( FAILED( d3dApp.Create( hInst, strCmdLine ) ) )
        return 0;

    // Init DirectSound
    if( FAILED( d3dApp.InitDirectSound( d3dApp.Get_hWnd() ) ) )
    {
        MessageBox( NULL, "Error initializing DirectSound.  Sample will now exit.", 
                            "DirectSound Sample", MB_OK | MB_ICONERROR );
        return FALSE;
    }

    pCMyApp->m_bBufferPaused = FALSE;

	for (int ii = 0; ii < MAX_SOUND_BUFFERS; ii++)
		g_pDSBuffer[ii] = NULL;

	if (FAILED(hr = CoInitialize( NULL ) ) )
		return FALSE;

	d3dApp.InitRRvariables();
	d3dApp.LoadRR_Resources();
    d3dApp.Run();

	PrintMessage(NULL, "Quitting", NULL, LOGFILE_ONLY);
		
    d3dApp.FreeDirectSound();
	
	CoUninitialize();
	return TRUE;
}


//-----------------------------------------------------------------------------
// Name: OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::OneTimeSceneInit()
{
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FrameMove( FLOAT fTimeKey )
{
    D3DMATRIX matView;
	float r = 100.0f; 
	float pos_x, pos_y, pos_z;
	float dir_x, dir_y, dir_z;
	float lx, ly, lz;
	float radius = 50.0f; // used for flashlight
	float gun_angle;
	int i;

	// check that angy is within limits for sin and cos table
	if(angy >= 360)
		angy = angy - 360;

	if(angy < 0)
		angy += 360;

	gun_angle = -angy + (float)90;
	
	if(gun_angle >=360)
		gun_angle = gun_angle -360;
	if(gun_angle <0)
		gun_angle = gun_angle +360;
	
	i = current_gun;
	your_gun[i].rot_angle = gun_angle;
	your_gun[i].x = m_vEyePt.x;
	your_gun[i].y = (float)9+ m_vEyePt.y - 22;
	your_gun[i].z = m_vEyePt.z;


	for(i = 0; i < num_light_sources_in_map; i++)
		m_pd3dDevice->LightEnable( (DWORD)i, FALSE );

	num_light_sources = 0;

	// Set up the light structure
    D3DLIGHT7 light;
    ZeroMemory( &light, sizeof(D3DLIGHT7) );
    
    light.dcvDiffuse.r  = 0.0f;
    light.dcvDiffuse.g  = 0.0f;
    light.dcvDiffuse.b  = 0.0f;

	light.dcvAmbient.r  = 0.3f;
	light.dcvAmbient.g  = 0.3f;
	light.dcvAmbient.b  = 0.3f;

	light.dcvSpecular.r  = 0.0f;
	light.dcvSpecular.g  = 0.0f;
	light.dcvSpecular.b  = 0.0f;
	 
    light.dvRange       = 500.0f; // D3DLIGHT_RANGE_MAX

	// Calculate the flashlight's lookat point, from 
	// the player's view direction angy.

	lx = m_vEyePt.x + radius * sinf(angy * k); 
	ly = 0;
	lz = m_vEyePt.z + radius * cosf(angy * k);

	// Calculate direction vector for flashlight
	dir_x = lx - m_vEyePt.x;
	dir_y = 0; //ly - m_vEyePt.y;
	dir_z = lz - m_vEyePt.z;

	// set flashlight's position to player's position
	pos_x = m_vEyePt.x;
	pos_y = 10; //m_vEyePt.y;
	pos_z = m_vEyePt.z;

	light.dvPosition	  = D3DVECTOR( pos_x, pos_y, pos_z );
    light.dvDirection     = D3DVECTOR( dir_x, dir_y, dir_z );
    light.dvFalloff       = 1.0f;
    light.dvTheta         = 0.1f;	// spotlight's inner cone
    light.dvPhi           = 0.3f;	// spotlight's outer cone
    light.dvAttenuation0  = 0.3f;
	light.dltType		  = D3DLIGHT_SPOT;
		
    // Set the light
	
	if(bIsFlashlightOn == TRUE)
	{
		m_pd3dDevice->SetLight( num_light_sources, &light );
		m_pd3dDevice->LightEnable( (DWORD)num_light_sources, TRUE );
		num_light_sources++;
	}
	

	// Calculate the players lookat point, from 
	// the player's view direction angy.

	m_vLookatPt.x = m_vEyePt.x + r * sinf(angy * k); 
	m_vLookatPt.y = m_vEyePt.y + r * sinf(look_up_ang * k);
	m_vLookatPt.z = m_vEyePt.z + r * cosf(angy * k);

	// set World, View, and Projection matrices
	D3DUtil_SetIdentityMatrix( matWorld );
    D3DUtil_SetViewMatrix( matView, m_vEyePt, m_vLookatPt, m_vUpVec );
	D3DUtil_SetProjectionMatrix( matProj, FOV, ASPECT_RATIO, Z_NEAR, Z_FAR );

    m_pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD,      &matWorld );
    m_pd3dDevice->SetTransform( D3DTRANSFORMSTATE_VIEW,       &matView );
    m_pd3dDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &matProj );


    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::Render()
{
	int i, t;
	int angle;
	int oblist_index;
	int ob_type;
	int vert_index;
	int face_index;	
	int cell_x,cell_z;
	int f_index;
	int vi,q,ix,iz,icnt;
	int vol_length;
	int vol_x[MAX_NUM_QUADS];
	int vol_z[MAX_NUM_QUADS];

	DWORD dwIndexCount;  
	DWORD dwVertexCount; 
	DWORD color_key=0;
	BOOL use_player_skins_flag = TRUE;

	float skx, sky;
	D3DPRIMITIVETYPE command;
	D3DVERTEX temp_v[2000]; // debug
	HRESULT hr;
	int ap_cnt = 0;

	IsRenderingOk = TRUE;

	if(rendering_first_frame == TRUE)
	{
		RRAppActive = TRUE;
		PrintMessage(m_hWnd, "RenderScene : rendering first frame", NULL, LOGFILE_ONLY);
	}


	m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
		               0x00000000, 1.0f, 0L );	

    D3DCLIPSTATUS status={D3DCLIPSTATUS_EXTENTS2, 0, 2048.0f, 0.0f, 2048.0f, 0.0f, 0.0f, 0.0f};
	
	number_of_polys_per_frame = 0;
	num_triangles_in_scene = 0 ;
	num_verts_in_scene = 0;
	num_dp_commands_in_scene = 0 ;

	poly_cnt	= 0;
	cnt			= 0;
	icnt		= 0;  
	face_index  = 0; 
	cnt_f		= 0;  
	
	cell_x = (int)(m_vEyePt.x / 256);
	cell_z = (int)(m_vEyePt.z / 256);

	player_x = m_vEyePt.x;
	player_z = m_vEyePt.z;


	for(i = 0; i < MAX_NUM_QUADS; i++)
	{
		oblist_overdraw_flags[i] = FALSE;
		oblist_overlite_flags[i] = FALSE;
	}

	if(angy>=360)
		angy=angy-360;

	if(angy<0)
		angy+=360;


	// Work out which direction player is facing in
	// i.e north, east, south, or west
	// and store the numbers of the cells which are in view.	
	// Roadrage works out what cells are visible by simply
	// looking at a rectangular 5 x 6 block of cells directly 
	// infront of the player. 

	if((angy >= 315) || (angy <= 45)) // player facing North
	{
		for(ix=-2;ix<=2;ix++)
		{
			vol_x[icnt]   = cell_x + ix, vol_z[icnt]   = cell_z-1;
			vol_x[icnt+1] = cell_x + ix, vol_z[icnt+1] = cell_z;
			vol_x[icnt+2] = cell_x + ix, vol_z[icnt+2] = cell_z+1;
			vol_x[icnt+3] = cell_x + ix, vol_z[icnt+3] = cell_z+2;
			vol_x[icnt+4] = cell_x + ix, vol_z[icnt+4] = cell_z+3;
			vol_x[icnt+5] = cell_x + ix, vol_z[icnt+5] = cell_z+4;

			icnt+=6;
		}
	}

	if( (angy > 225) && (angy < 315) ) // player facing West
	{
		for(iz=-2;iz<=2;iz++)
		{
			vol_x[icnt]   = cell_x +1,  vol_z[icnt]   = cell_z + iz;
			vol_x[icnt+1] = cell_x ,	vol_z[icnt+1] = cell_z + iz;
			vol_x[icnt+2] = cell_x - 1, vol_z[icnt+2] = cell_z + iz;
			vol_x[icnt+3] = cell_x - 2, vol_z[icnt+3] = cell_z + iz;
			vol_x[icnt+4] = cell_x - 3, vol_z[icnt+4] = cell_z + iz;
			vol_x[icnt+5] = cell_x - 4, vol_z[icnt+5] = cell_z + iz;

			icnt+=6;
		}
	}

	if( (angy >= 135) && (angy <= 225)) // player facing South
	{
		for(ix=-2;ix<=2;ix++)
		{
			vol_x[icnt]   = cell_x + ix, vol_z[icnt]   = cell_z+1;
			vol_x[icnt+1] = cell_x + ix, vol_z[icnt+1] = cell_z;
			vol_x[icnt+2] = cell_x + ix, vol_z[icnt+2] = cell_z-1;
			vol_x[icnt+3] = cell_x + ix, vol_z[icnt+3] = cell_z-2;
			vol_x[icnt+4] = cell_x + ix, vol_z[icnt+4] = cell_z-3;
			vol_x[icnt+5] = cell_x + ix, vol_z[icnt+5] = cell_z-4;

			icnt+=6;
		}
	}

	if((angy > 45) && (angy < 135)) // player facing East
	{
		for(iz=-2;iz<=2;iz++)
		{
			vol_x[icnt]   = cell_x-1,	vol_z[icnt]   = cell_z + iz;
			vol_x[icnt+1] = cell_x,		vol_z[icnt+1] = cell_z + iz;
			vol_x[icnt+2] = cell_x + 1, vol_z[icnt+2] = cell_z + iz;
			vol_x[icnt+3] = cell_x + 2, vol_z[icnt+3] = cell_z + iz;
			vol_x[icnt+4] = cell_x + 3, vol_z[icnt+4] = cell_z + iz;
			vol_x[icnt+5] = cell_x + 4, vol_z[icnt+5] = cell_z + iz;

			icnt+=6;
		}
	}

	number_of_polys_per_frame = 0;
	cnt = 0;

	vol_length = icnt;

	
	for(vi=0;vi<vol_length;vi++)
	{		
		cell_x = vol_x[vi];
		cell_z = vol_z[vi];
	
		if( (vi == 0)  || (vi == 6)  || (vi == 12) || 
			(vi == 18) || (vi == 24) || (vi == 30) )
		{
			bSkipThisCell = TRUE;
		}
		else
			bSkipThisCell = FALSE;

		// check that this cell is within the map limits
		if( ((cell_x >=  0) && (cell_z >=  0)) &&  
			((cell_x < 200) && (cell_z < 200)) )
		{
			// check if there is a list of map objects allocated to this cell
			if(cell[cell_x][cell_z] != NULL) 
			{					
				// get the number of objects which need to be drawn 
				// for this cell
				clength = cell_length[cell_x][cell_z];
				
				for(q=0;q<clength;q++)
				{	
					// get the object's map ID
					oblist_index = cell[cell_x][cell_z][q];

					if( (oblist_index >= 0) && (oblist_index < oblist_length) )
					{
						// check map object list and prevent the same 
						// objects being drawn more than once

						if(oblist_overdraw_flags[oblist_index] == FALSE)
						{
							if(bSkipThisCell == FALSE)
								oblist_overdraw_flags[oblist_index] = TRUE;

							wx = oblist[oblist_index].x;
							wy = oblist[oblist_index].y;
							wz = oblist[oblist_index].z;
		
							angle = (int)oblist[oblist_index].rot_angle;
							ob_type = oblist[oblist_index].type;

							ObjectToD3DVertList(ob_type, angle, oblist_index);

						}
					}
				} // end for q
			} // end if
		} // end if
	} // end for vi


	// Todo :  Load D3D Vertex array here for characters and guns etc 

	// DRAW YOUR GUN ///////////////////////////////////////////
	
	if(bInWalkMode )
	{
		skx = (float)0.40000000 / (float)256;
		sky = (float)1.28000000 / (float)256;
		use_player_skins_flag = 1;

		i = current_gun;

		if(current_gun < num_your_guns)
		{
			wx = your_gun[i].x; 
			wy = your_gun[i].y;
			wz = your_gun[i].z; 

			x_off = your_gun[i].x_offset;
			y_off = your_gun[i].y_offset;
			z_off = your_gun[i].z_offset; 

			ob_type = your_gun[i].model_id;
			current_frame = your_gun[i].current_frame;
			angle = (int)your_gun[i].rot_angle;
		
			PlayerToD3DVertList(ob_type, 
								current_frame, 
								angle, 
								NULL, 
								NULL); 
		}
	}

	// DRAW DEBUG MODELS
	
	x_off = 0;
	y_off = 0;
	z_off = 0;

	for(i = 0; i < num_debug_models; i++)
	{
		wx = debug[i].x; 
		wy = debug[i].y;		
		wz = debug[i].z;
		angle = (int)debug[i].rot_angle;

		if( (abs((int)(wx - player_x)) < 512) && (abs((int)(wz - player_z)) < 512) ) 
		{
			use_player_skins_flag = 1;
			current_frame = debug[i].current_frame;

			PlayerToD3DVertList(debug[i].model_id, 
						current_frame,
						angle, 
						0,
						NULL);
		}
	}

	if(RR_exit_debug_flag == TRUE)
		RR_exit_debug_flag = TRUE;
	

	if(number_of_polys_per_frame == 0)
	{
		PrintMessage(NULL, "number_of_polys_per_frame = 0", NULL, LOGFILE_ONLY);
		return TRUE;
	}
    car_speed=(float)0;

 
    hr = m_pd3dDevice->BeginScene();
	if (hr != D3D_OK)
	{
		PrintMessage(NULL, "BeginScene : FAILED", NULL, LOGFILE_ONLY);
        return FALSE;
	}
	
	if (m_pd3dDevice->SetClipStatus(&status) != D3D_OK)
	{
		PrintMessage(NULL, "SetClipStatus : FAILED", NULL, LOGFILE_ONLY);
        return FALSE; 
	}

	if (m_pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, FALSE)!= D3D_OK) 
		return FALSE;

	vert_index = 0;
	face_index = 0;
	ap_cnt = 0;

	num_polys_per_frame = number_of_polys_per_frame;

	
	for(i = 0; i < number_of_polys_per_frame; i++)
	{

			if(dp_command_index_mode[i] == USE_NON_INDEXED_DP) 
			{
				if (m_pd3dDevice->DrawPrimitive(dp_commands[i], D3DFVF_VERTEX, 
					(LPVOID)&src_v[vert_index], verts_per_poly[i], NULL) != D3D_OK) 
				{
					PrintMessage(NULL, "CMyD3DApplication::Render - DrawPrimitive FAILED", NULL, LOGFILE_ONLY);		
					return FALSE;
				}
				vert_index += verts_per_poly[i];
			}

			if(dp_command_index_mode[i] == USE_INDEXED_DP) 	
			{
				dwIndexCount  = faces_per_poly[i] * 3;
				dwVertexCount = verts_per_poly[i];
				command		  = dp_commands[i];


				for(t = 0; t < (int)dwIndexCount; t++)
				{
					f_index = src_f[face_index + t];

					memset(&temp_v[t], 0, sizeof(D3DVERTEX));
					memcpy(&temp_v[t], &src_v[vert_index + f_index], 
						sizeof(D3DVERTEX));
				}

				if (m_pd3dDevice->DrawPrimitive(command, 
								D3DFVF_VERTEX, 
								(LPVOID)&temp_v[0], 
								dwIndexCount, 
								NULL) != D3D_OK) 
				{
					PrintMessage(NULL, "CMyD3DApplication::Render - DrawPrimitive FAILED", NULL, LOGFILE_ONLY);		
					return FALSE;
				}

				face_index += dwIndexCount;
				vert_index += dwVertexCount;
			}
	} // end for i

	num_verts_per_frame = vert_index;

	if(rendering_first_frame == TRUE)
	{
		itoa(number_of_polys_per_frame, buffer, 10);
				PrintMessage(NULL, "num_poly = ", buffer, LOGFILE_ONLY);	
	}

    if (m_pd3dDevice->GetClipStatus(&status) != D3D_OK)
	{
		PrintMessage(NULL, "GetClipStatus : FAILED", NULL, LOGFILE_ONLY);
        return FALSE; 
	}
		
	hr = m_pd3dDevice->EndScene();
	if (hr != D3D_OK)
	{
		PrintMessage(NULL, "EndScene : FAILED", NULL, LOGFILE_ONLY);
        return FALSE;
	}

	rendering_first_frame = FALSE;

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InitDeviceObjects()
{
    // Create and set up the object material
    D3DMATERIAL7 mtrl;
    D3DUtil_InitMaterial( mtrl, 1.0f, 1.0f, 1.0f, 0.0f );
    mtrl.power = 40.0f;
    m_pd3dDevice->SetMaterial( &mtrl );
    m_pd3dDevice->SetRenderState( D3DRENDERSTATE_AMBIENT, 0x00505050 );
    
	// Set the transform matrices
    D3DUtil_SetIdentityMatrix( matWorld );
    D3DUtil_SetProjectionMatrix( matProj, 1.57f, 1.0f, 1.0f, 100.0f );
    m_pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD,      &matWorld );
    m_pd3dDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &matProj );

    // Turn on lighting. Light will be set during FrameMove() call
	m_pd3dDevice->SetRenderState( D3DRENDERSTATE_LIGHTING, bEnableLighting );

    // Set miscellaneous renderstates
    m_pd3dDevice->SetRenderState(D3DRENDERSTATE_DITHERENABLE, TRUE); 
	m_pd3dDevice->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE); 	
	m_pd3dDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_TRUE);
	m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FILLMODE, D3DFILL_SOLID);
	m_pd3dDevice->SetRenderState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD);
		
    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: FinalCleanup()
// Desc: Called before the app exits, this function gives the app the chance
//       to cleanup after itself.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FinalCleanup()
{
    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Called when the app is exitting, or the device is being changed,
//       this function deletes any device dependant objects.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::DeleteDeviceObjects()
{
    return S_OK;
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



void PrintMemAllocated(int mem, char *message)
{
	FILE *fp;
	char buffer[100];
	char buffer2[100];
	int mem_kb;

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
        MessageBox(NULL,"Can't load logfile","File Error",MB_OK);
		fclose(fp);
		return;
	}

	strcpy(buffer, "memory allocated for ");
	strcat(buffer, message);
	strcat(buffer, " = ");

	mem_kb = mem / 1024;
	itoa(mem_kb, buffer2, 10);
	strcat(buffer, buffer2);
	strcat(buffer, " KB");

	fprintf( fp, " %s\n", buffer );

	total_allocated_memory_count += mem;
	
	fclose(fp);
	
}




BOOL CALLBACK FireWeapon(UINT uTimerID, 
		UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)         
{   
	pCMyApp->firing_gun_flag = TRUE;
		
	pCMyApp->StopBuffer(TRUE,0);
	pCMyApp->PlaySound(0, FALSE);

	return TRUE;
}