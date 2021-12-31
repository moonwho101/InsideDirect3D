
#ifndef __ROADRAGE_H
#define __ROADRAGE_H

#include <dinput.h>
#include "D3DFrame.h"
#include "D3DEnum.h"
#include "D3DUtil.h"
#include "D3DRes.h"
#include "world.hpp"
#include "controls.hpp"


#define MAX_NUM_QUADS			20000
#define MAX_NUM_TRIANGLES		20000
#define MAX_NUM_FACE_INDICES	20000 
#define MAX_NUM_VERTICES		40000
#define MAX_NUM_TEXTURES		  100
#define MAX_NUM_PLAYERS			   10

#define RRnetID_ERROR			   -1

#define USE_DEFAULT_MODEL_TEX	0
#define USE_PLAYERS_SKIN		1

#define USE_INDEXED_DP			0
#define USE_NON_INDEXED_DP		1

#define KEY_DELAY_TIMER				1
#define ANIMATION_TIMER				2
#define RESPAWN_TIMER				3
#define DP_UPDATE_POSITION_TIMER	5
 

//-----------------------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: Application class. The base class provides just about all the
//       functionality we want, so we're just supplying stubs to interface with
//       the non-C++ functions of the app.
//-----------------------------------------------------------------------------

class CMyD3DApplication : public CD3DApplication
{

    D3DLIGHTTYPE m_dltType;
    VOID    Cleanup3DEnvironment();
	
protected:
    HRESULT OneTimeSceneInit();
    HRESULT InitDeviceObjects();
    HRESULT DeleteDeviceObjects();
    HRESULT Render();
    HRESULT FrameMove( FLOAT fTimeKey );
    HRESULT FinalCleanup();
	
public:
	
    CMyD3DApplication();

	BOOL RrBltCrossHair(LPDIRECTDRAWSURFACE7 lpddsTexturePtr);
    D3DTLVERTEX m_pBackground[4];

	VOID	DriveMode(CONTROLS *Controls);
	VOID	WalkMode(CONTROLS *Controls);
	VOID	MovePlayer(CONTROLS *Controls);
	BOOL	m_bBufferPaused;    
    BOOL    m_bWindowed;
    BOOL    m_bToggleFullscreen;
	BOOL	m_toggleFog;
	BOOL	IsRenderingOk;
	void	IsPlayerHit();
	void	DisplayScores();
	void	DisplayCredits(HWND hwnd);
	void	DisplayControls(HWND hwnd);
	void	DisplayRRStats(HWND hwnd);
	void	DisplayLegalInfo(HWND hwnd);

    VOID    OutputText( DWORD x, DWORD y, TCHAR* str );
	HRESULT Render3DEnvironment();

	HRESULT CALLBACK EnumTextureFormatsCallback(LPDDPIXELFORMAT pddpf,
												   LPVOID lpContext);
	LRESULT MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam,
                                  LPARAM lParam );
    // Miscellaneous functions
    VOID    ShowStats();
    BOOL    m_bShowStats;
	
	VOID	UpdateControls();
	BOOL	LoadRR_Resources();
	void	InitRRvariables();
	HRESULT AnimateCharacters();

	void PlayerToD3DVertList(int pmodel_id, int curr_frame, int angle, int texture_alias, int tex_flag);
	void PlayerToD3DIndexedVertList(int pmodel_id, int curr_frame, int angle, int texture_alias, int tex_flag);
	void ObjectToD3DVertList(int ob_type, int angle, int oblist_index);
	void SetMapLights(int ob_type, int angle, int oblist_index); 

	void AddPlayer(float x, float y, float z, float rot_angle);
	
	BOOL LoadRRTextures(HWND hwnd, char *filename);
	int CheckValidTextureAlias(HWND hwnd, char *alias);
	void AddPlayerLightSource(int player_num, float x, float y, float z);
	setupinfo_ptr setupinfo;

	void SetPlayerAnimationSequence(int player_number, int sequence_number);
	HRESULT			CreateInputDevice( HWND hWnd, 
								   LPDIRECTINPUT7       pDI,
								   LPDIRECTINPUTDEVICE2 pDIdDevice, 
								   GUID guidDevice,
								   const DIDATAFORMAT* pdidDataFormat, DWORD dwFlags );

	VOID			OpenSoundFile(TCHAR *strFileName, int bufferNum);
	HRESULT			PlaySound(int bufferNum, BOOL bLooped); 
	HRESULT			InitDirectSound( HWND hDlg );
	HRESULT			FreeDirectSound();
	VOID			LoadWaveFile( TCHAR* strFileName, int bufferNum );

	VOID DestroyInputDevice();
	HRESULT	CreateDInput( HWND hWnd );
	VOID DestroyDInput();

	LPDIRECTDRAWSURFACE7 lpds_crosshair;
	LPDIRECTDRAWSURFACE7 lpds_background;
	RECT rc_crosshair;
	HRESULT			CreateStaticBuffer( TCHAR* strFileName );
	HRESULT			FillBuffer(int bufferNum);
	BOOL			IsBufferPlaying(int bufferNum); 
	HRESULT			PlayBuffer( BOOL bLooped, int bufferNum );
	VOID			StopBuffer( BOOL bResetPosition, int bufferNum ); 
	BOOL			IsSoundPlaying(int bufferNum);
	HRESULT			RestoreBuffers();
	HRESULT			SetFrequency(int bufferNum, int frequency );
	
	float k;
	char buffer[256];
	char buffer2[256];
	char buffer3[256];
	char buffer4[256];

	BOOL DelayKey2[256];
	BOOL RRAppActive;
	BOOL firing_gun_flag;
	BOOL have_i_moved_flag;
	BOOL delay_key_press_flag;
	BOOL multiplay_flag;
	BOOL bInWalkMode;
	int	m_num_sounds;

	static HRESULT hr;

	D3DVALUE		angx, angy, angz; 
	D3DVALUE		look_up_ang;
    D3DVECTOR       m_vEyePt;
    D3DVECTOR       m_vLookatPt;
    D3DVECTOR       m_vUpVec;
    FLOAT           m_fEyeDistance; // Offset of each eye from m_vEyePt
    BOOL            m_bUseViewPoints;


	int debug_test;
	int keycode;
	int car_gear;
	int editor_insert_mode;
	int MouseX,MouseY;
	int xPos,yPos;
	int current_mode;
	int object_rot_angle;	
	int old_xPos, old_yPos;
	int toggle_delay ;
	int current_screen_status; // default, in case rrsetup.ini 
			
	int mod_id;	   
	int curr_frame;
	int stop_frame;  
					
	float temp_view_angle;
	
	int curr_seq;
	char szDevice_debug[256];
	
	OBJECTLIST	*oblist; 
	int oblist_length;
	
	GUNLIST *your_gun; 
	GUNLIST *other_players_guns; 
	int gunlist_length;
	int current_gun;
	int current_gun_model_id;
	int current_car;
	int num_your_guns;
	int num_op_guns;
	

	PLAYER *player_list; 
	PLAYER *car_list; 
	PLAYER *debug; 

	int num_players;
	int num_cars;
	int num_debug_models;
	int current_frame;
	int current_sequence;

	PLAYERMODELDATA *pmdata; 

	OBJECTDATA	*obdata; 
	
	int *num_vert_per_object;
	int num_polys_per_object[100];
	int obdata_length;
	int model_format;

	int *cell[200][200];
	int **cell_length; 
	
	BOOL **draw_flags; 
	BOOL m_tableFog;

	TEXTUREMAPPING TexMap[100]; 
	int number_of_tex_aliases;

	int num_polys_per_frame;
	int num_verts_per_frame;

	BOOL walk_mode_enabled;
	BOOL rendering_first_frame;

	D3DVERTEX *src_v; 

	LPDIRECT3DMATERIAL3 lpMat[MAX_NUM_TEXTURES];

	D3DMATERIALHANDLE  hMat[MAX_NUM_TEXTURES];

	BOOL *dp_command_index_mode;    
	D3DPRIMITIVETYPE *dp_commands;  

	int *verts_per_poly; 
	int *faces_per_poly; 
	int *src_f;			

	float pi;
	float piover2;

	D3DMATRIX matWorld, matProj;

	D3DMATRIX trans, roty;
	D3DMATRIX WorldMatrix, ViewMatrix, ProjMatrix;
	D3DVALUE posx, posy, posz; //Player position
	//D3DVALUE angx, angz; //Player angles
	D3DVECTOR from, at, up; // Used to set view matrix
	D3DVALUE Z_NEAR;
	D3DVALUE Z_FAR;
	D3DVALUE FOV; 
	D3DVALUE ASPECT_RATIO;

	int cell_flag;
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
	int cnt_f;

	int cnt;
	float sine, cosine;
	float wx,wy,wz;
	float x_off, y_off, z_off;

	int number_of_polys_per_frame;

	float sin_table[361];
	float cos_table[361];
	float gradient;
	float c;
	float inv_gradient;
	float ncp_angle;

	float px[100],py[100],pz[100],pw[100];
	float mx[100],my[100],mz[100],mw[100];
	float cx[100],cy[100],cz[100],cw[100];
	float tx[100],ty[100];

	int mlr[100],mlg[100],mlb[100];

	int *texture_list_buffer;

	char ImageFile[100][256];
	LPDIRECTDRAWSURFACE7 lpddsImagePtr[MAX_NUM_TEXTURES];

	int ctext; 
	int NumTextures;

	int *poly_clip_flags; 
	int *poly_clip; 

	BOOL *oblist_overdraw_flags;
	BOOL *oblist_overlite_flags;

	float player_x, player_z;

	int num_light_sources_in_map;
	int num_triangles_in_scene;
	int num_verts_in_scene;
	int num_dp_commands_in_scene;

	int tx_rate_ps;
	int rx_rate_ps;

	DWORD intensity;
	BOOL display_scores;
	BOOL bEnableAlphaTransparency;
	BOOL bEnableLighting;
	D3DSHADEMODE ShadeMode;
	HINSTANCE hInstApp;
	DWORD MyRRnetID;
	DWORD FireWeaponTimer;
	DWORD UpdatePosTimer;
	int MaxTextureSize;
	int MyHealth;
	int MyPing;


};

BOOL CALLBACK FireWeapon(UINT uTimerID, 
		UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);

#endif //__ROADRAGE_H