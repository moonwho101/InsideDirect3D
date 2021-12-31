
// Road Rage version 1.1

// Written by William Chin
// This file was originally part of the Microsoft DirectX5 SDK
// Date : 3/7/99
// File : Controls.cpp


#include "d3dmain.hpp"
#include "d3dappi.hpp"
#include "world.hpp"
#include "bitmap.hpp"
#include "input.hpp"
#include "sound.hpp"
#include "texture.hpp"
#include "error.hpp"
#include <winbase.h>
#include <dinput.h>
#include <dsound.h>
#include <dplay.h>
#include <dplobby.h>
#include "dpchat.h"
#include <mmsystem.h>
#include "comm.h"


extern WORLD world;
extern CAMERA cam;
extern d3dmainglobals myglobs;
extern Joystick_Enabled_Flag;
extern update_player_position_flag;
extern delay_key_press_flag;
extern car_gear;
extern D3DAppInfo* d3dapp;
extern DPLAYINFO DPInfo;
extern BOOL RRAppActive;

static double k=0.017453292;
static int temp_view_angle;
BOOL have_i_moved_flag = FALSE;
void UpdateDebugFrames(int new_seq);


void UpdateControls()
{
	DWORD dwKeyState = 0;
	LPDIJOYSTATE js_ptr;
	int frequency;
	int i;
	//int mod_id;
	//int curr_frame;
	int shift;
	//int stop_frame;
	float gun_angle;
	char buffer[50];
	char buffer2[50];

	temp_view_angle = cam.view_angle;

	if(Joystick_Enabled_Flag == TRUE) 
	{		
		update_player_position_flag = TRUE;

		js_ptr = ReadJoystickInput();

		if(js_ptr == 0) // have we lost joystick input ?
			ReacquireInput();
		
		if(js_ptr != 0)
		{
			if(world.walk_mode_enabled == TRUE)
			{
				// JOYSTICK WALK MODE

				StopDSound(SND_ENGINE_REV);
				car_speed = (float)0;

				cam.view_point_y=(float)16;

				if (js_ptr->rgbButtons[JOY_BUTTON_1] & 0x80)
				{
					PlayDSound(SND_FIRE_AR15, NULL);	
				}
			
				if(delay_key_press_flag == FALSE)
				{
					if (js_ptr->rgbButtons[JOY_BUTTON_2] & 0x80)
					{
						world.walk_mode_enabled = FALSE;
						delay_key_press_flag = TRUE;
						SetTimer(myglobs.hWndMain,3, KEY_DELAY,NULL);
					}
				
					if (js_ptr->rgbButtons[JOY_BUTTON_3] & 0x80)
					{
						world.current_gun--;	
						if(world.current_gun < 0)
							world.current_gun++;
						delay_key_press_flag = TRUE;
						SetTimer(myglobs.hWndMain,3, KEY_DELAY,NULL);
					}

					if (js_ptr->rgbButtons[JOY_BUTTON_4] & 0x80)
					{
						world.current_gun++;
						if(world.current_gun > MAX_NUM_WEAPONS)
							world.current_gun--;
						delay_key_press_flag = TRUE;
						SetTimer(myglobs.hWndMain,3, KEY_DELAY,NULL);
					}
				}

				if(js_ptr->lX < -200) // left
					temp_view_angle -= 4;
			
				if(js_ptr->lX > 200) // right
					temp_view_angle += 4;
		
				if(js_ptr->lY < -200) // accelerate
					car_speed+=(float)4;
			
				if(js_ptr->lY > 200) // brake
					car_speed-=(float)4;
			
				if(cam.view_angle<0)
					temp_view_angle += 360;

				if(cam.view_angle>=360)
					temp_view_angle = temp_view_angle -360;

				cam.direction_x = (float)sin((float)6.28*((float)temp_view_angle/(float)360));
				cam.direction_y=(float)0;
				cam.direction_z = (float)cos((float)6.28*((float)temp_view_angle/(float)360));
       
				cam.view_point_x+=(float)cam.direction_x*(float)car_speed*(float)car_gear;
				cam.view_point_z+=(float)cam.direction_z*(float)car_speed*(float)car_gear;
		
			}
			else
			{
				// JOYSTICK DRIVE MODE

				PlayDSound(SND_ENGINE_REV, DSBPLAY_LOOPING);

				cam.view_point_y=(float)13;
				
				if(delay_key_press_flag == FALSE)
				{
					if (js_ptr->rgbButtons[JOY_BUTTON_2] & 0x80)
					{
						world.walk_mode_enabled = TRUE;
						delay_key_press_flag = TRUE;
						SetTimer(myglobs.hWndMain,3, KEY_DELAY,NULL);		
					}
				}

				if (js_ptr->rgbButtons[JOY_BUTTON_3] & 0x80)
				{
					if((car_speed > 0) && (car_gear == -1))
						car_speed-=(float).2;
					else
						car_gear = 1;
				}

				if (js_ptr->rgbButtons[JOY_BUTTON_4] & 0x80)
				{
					if((car_speed > 0) && (car_gear == 1))
						car_speed-=(float).2;
					else
						car_gear = -1;
				}

				if(js_ptr->lX < -200) // left
				{
					if(car_speed > 0) // cam.view_angle-=1
						temp_view_angle += js_ptr->lX / 500;
				}
		
				if(js_ptr->lX > 200) // right
				{
					if(car_speed > 0) // cam.view_angle+=1
						temp_view_angle += js_ptr->lX / 500;
				}
		

				if(js_ptr->lY < -200) // accelerate
				{
					frequency = 12000 + (int)((float)car_speed * (float)250.0);
					SetFrequency(myglobs.hWndMain, SND_ENGINE_REV, frequency);					
					car_speed+=(float).1;
				}

				if(js_ptr->lY > 200) // brake
				{
					frequency = 12000 + (int)((float)car_speed * (float)187.5);
					SetFrequency(myglobs.hWndMain, SND_ENGINE_REV, frequency);
					car_speed-=(float).2;
				}

				if(car_speed<0)
					car_speed=(float)0;
				
				if(car_speed>64)
					car_speed=(float)64;

				if(cam.view_angle<0)
					temp_view_angle += 360;

				if(cam.view_angle>=360)
					temp_view_angle = temp_view_angle -360;

				cam.direction_x = (float)sin((float)6.28*((float)temp_view_angle/(float)360));
				cam.direction_y = (float)0;
				cam.direction_z = (float)cos((float)6.28*((float)temp_view_angle/(float)360));
       
				cam.view_point_x+=(float)cam.direction_x*(float)car_speed*(float)car_gear;
				cam.view_point_z+=(float)cam.direction_z*(float)car_speed*(float)car_gear;
		
			}
		}
	}
	
	if(Joystick_Enabled_Flag == FALSE) 
	{	
		// KEYBOARD 
		if( RRAppActive == FALSE)
			return;

		dwKeyState = ReadKeyboardInput();

		if((dwKeyState & KEY_SCREENSHOT) == KEY_SCREENSHOT) 
		{
			DDBitmapScreenDump(myglobs.hWndMain, d3dapp);
		}

		update_player_position_flag = FALSE;

		i = 0;//world.current_debug_model;

		
		shift = 0;
		strcpy(buffer, "Play Sequence ");
		
		if((dwKeyState &KEY_SHIFT ) == KEY_SHIFT) 
			shift = 10;

		
		if((dwKeyState &KEY_1 ) == KEY_1) 
		{
			UpdateDebugFrames(shift + 1);
			itoa(shift + 1, buffer2, 10);
			strcat(buffer, buffer2);
			DDTextOut(buffer);
		}
		
		if((dwKeyState &KEY_2 ) == KEY_2) 
		{
			UpdateDebugFrames(shift + 2);
			itoa(shift + 2, buffer2, 10);
			strcat(buffer, buffer2);
			DDTextOut(buffer);
		}

		if((dwKeyState &KEY_3 ) == KEY_3) 
		{
			UpdateDebugFrames(shift + 3);
			itoa(shift + 3, buffer2, 10);
			strcat(buffer, buffer2);
			DDTextOut(buffer);
		}

		if((dwKeyState &KEY_4 ) == KEY_4)
		{
			UpdateDebugFrames(shift + 4);
			itoa(shift + 4, buffer2, 10);
			strcat(buffer, buffer2);
			DDTextOut(buffer);
		}

		if((dwKeyState &KEY_5 ) == KEY_5) 
		{
			UpdateDebugFrames(shift + 5);
			itoa(shift + 5, buffer2, 10);
			strcat(buffer, buffer2);
			DDTextOut(buffer);
		}

		if((dwKeyState &KEY_6 ) == KEY_6) 
		{
			UpdateDebugFrames(shift + 6);
			itoa(shift + 6, buffer2, 10);
			strcat(buffer, buffer2);
			DDTextOut(buffer);
		}

		if((dwKeyState &KEY_7 ) == KEY_7) 
		{
			UpdateDebugFrames(shift + 7);
			itoa(shift + 7, buffer2, 10);
			strcat(buffer, buffer2);
			DDTextOut(buffer);
		}

		if((dwKeyState &KEY_8 ) == KEY_8) 
		{
			UpdateDebugFrames(shift + 8);
			itoa(shift + 8, buffer2, 10);
			strcat(buffer, buffer2);
			DDTextOut(buffer);
		}

		if((dwKeyState &KEY_9 ) == KEY_9) 
		{
			UpdateDebugFrames(shift + 9);
			itoa(shift + 9, buffer2, 10);
			strcat(buffer, buffer2);
			DDTextOut(buffer);
		}

		if((dwKeyState &KEY_0 ) == KEY_0) 
		{
			UpdateDebugFrames(shift + 10);
			itoa(shift + 10, buffer2, 10);
			strcat(buffer, buffer2);
			DDTextOut(buffer);
		}

		if((dwKeyState &KEY_M ) == KEY_M) 
		{
			//SendChatMessage(myglobs.hWndMain, "RR Test Message", &DPInfo);
			world.display_scores =!world.display_scores;
		}
		

		if(world.walk_mode_enabled == TRUE)
		{
			// KEYBOARD WALK MODE
		
			StopDSound(SND_ENGINE_REV);
			car_speed = (float)0;

			cam.view_point_y=(float)20;

			if((dwKeyState &FLIP ) == FLIP) 
			{
				SendActionWordMessage(7,  APPMSG_GESTURE, &DPInfo);
			}
				
			if((dwKeyState &SALUTE ) == SALUTE) 
			{			
				SendActionWordMessage(8,  APPMSG_GESTURE, &DPInfo);
		
			}
			
			if((dwKeyState &TAUNT ) == TAUNT) 
			{
				SendActionWordMessage(9,  APPMSG_GESTURE, &DPInfo);
			}
			
			if((dwKeyState &WAVE ) == WAVE) 
			{
				SendActionWordMessage(10,  APPMSG_GESTURE, &DPInfo);
		
			}
				
			if((dwKeyState &POINT ) == POINT) 
			{
				SendActionWordMessage(11,  APPMSG_GESTURE, &DPInfo);
		
			}
			
				
			if((dwKeyState & KEY_FIRE) == KEY_FIRE) 
			{

			}

			if(delay_key_press_flag == FALSE)
			{
				if((dwKeyState &KEY_CHANGE_WEAPON_UP ) == KEY_CHANGE_WEAPON_UP) 
				{
					world.current_gun++;
					if(world.current_gun >= world.num_your_guns)
						world.current_gun--;
					delay_key_press_flag = TRUE;
					SetTimer(myglobs.hWndMain,3, KEY_DELAY,NULL);
					SendActionFlagMessage(myglobs.hWndMain,  world.current_gun,  APPMSG_CHANGE_WEAPON_TO,  &DPInfo);
				}	

				if((dwKeyState & KEY_CHANGE_WEAPON_DOWN) == KEY_CHANGE_WEAPON_DOWN) 
				{
					world.current_gun--;	
					if(world.current_gun < 0)
						world.current_gun++;
					delay_key_press_flag = TRUE;
					SetTimer(myglobs.hWndMain,3, KEY_DELAY,NULL);
					SendActionFlagMessage(myglobs.hWndMain,  world.current_gun,  APPMSG_CHANGE_WEAPON_TO,  &DPInfo);
				}

			
				if((dwKeyState & KEY_ENTER_OR_EXIT_CAR) == KEY_ENTER_OR_EXIT_CAR) 
				{
					world.walk_mode_enabled = FALSE;
					delay_key_press_flag = TRUE;
					SendActionFlagMessage(myglobs.hWndMain,  !world.walk_mode_enabled,  APPMSG_WALK_OR_DRIVE,  &DPInfo);
					SetTimer(myglobs.hWndMain,3, KEY_DELAY,NULL);
				}
			}

			if((dwKeyState & KEY_LEFT) == KEY_LEFT) 
				temp_view_angle-=4;
				
			if((dwKeyState & KEY_RIGHT) == KEY_RIGHT) 
				temp_view_angle+=4;
			
			if((dwKeyState & KEY_ACCELERATE) == KEY_ACCELERATE) 			
				car_speed = (float)4;
			
			if((dwKeyState & KEY_BRAKE) == KEY_BRAKE) 
				car_speed = (float)-4;
			
			if(cam.view_angle<0)
				temp_view_angle += 360;

			if(cam.view_angle>=360)
				temp_view_angle = temp_view_angle -360;

			cam.direction_x = (float)sin(k * temp_view_angle);
			cam.direction_y = (float)0;
			cam.direction_z = (float)cos(k * temp_view_angle);
       
			cam.view_point_x+=(float)cam.direction_x*(float)car_speed;
			cam.view_point_z+=(float)cam.direction_z*(float)car_speed;
		}
		else 
		{
			// KEYBOARD DRIVE MODE

			PlayDSound(SND_ENGINE_REV, DSBPLAY_LOOPING);

			cam.view_point_y=(float)21;


			if(delay_key_press_flag == FALSE)
			{
				if((dwKeyState & KEY_ENTER_OR_EXIT_CAR) == KEY_ENTER_OR_EXIT_CAR) 
				{
					world.walk_mode_enabled = TRUE;
					delay_key_press_flag = TRUE;
					SendActionFlagMessage(myglobs.hWndMain,  !world.walk_mode_enabled,  APPMSG_WALK_OR_DRIVE,  &DPInfo);
					SetTimer(myglobs.hWndMain,3, KEY_DELAY,NULL);
				}
			}

			if ((dwKeyState & KEY_CHANGE_GEAR_UP) == KEY_CHANGE_GEAR_UP)
			{
				if((car_speed > 0) && (car_gear == -1))
					car_speed-=(float).2;
				else
					car_gear = 1;
			}

			if ((dwKeyState & KEY_CHANGE_GEAR_DOWN) == KEY_CHANGE_GEAR_DOWN)
			{
				if((car_speed > 0) && (car_gear == 1))
					car_speed-=(float).2;
				else
					car_gear = -1;
			}

			if((dwKeyState & KEY_LEFT) == KEY_LEFT) 
			{
				if(car_speed > 0)
					temp_view_angle-=2;
			}
		
			if((dwKeyState & KEY_RIGHT) == KEY_RIGHT) 
			{
				if(car_speed > 0)
					temp_view_angle+=2;
			}

			if((dwKeyState & KEY_ACCELERATE) == KEY_ACCELERATE) 
			{
				frequency = 12000 + (int)((float)car_speed * (float)250.0);
				SetFrequency(myglobs.hWndMain, SND_ENGINE_REV, frequency);					
				car_speed+=(float).1;
			}

			if((dwKeyState & KEY_BRAKE) == KEY_BRAKE) 
			{
				frequency = 12000 + (int)((float)car_speed * (float)187.5);
				SetFrequency(myglobs.hWndMain, SND_ENGINE_REV, frequency);
				car_speed-=(float).2;
			}

			if(car_speed<0)
				car_speed=(float)0;
				
			if(car_speed>64)
				car_speed=(float)64;

			if(cam.view_angle<0)
				temp_view_angle += 360;

			if(cam.view_angle>=360)
				temp_view_angle = temp_view_angle -360;

			cam.direction_x = (float)sin((float)6.28*((float)temp_view_angle/(float)360));
			cam.direction_y = (float)0;
			cam.direction_z = (float)cos((float)6.28*((float)temp_view_angle/(float)360));
       
			cam.view_point_x+=(float)cam.direction_x*(float)car_speed*(float)car_gear;
			cam.view_point_z+=(float)cam.direction_z*(float)car_speed*(float)car_gear;
		}
	}
	
	
	cam.view_angle = (int)temp_view_angle;	

	gun_angle = -cam.view_angle + (float)90;
	
	if(gun_angle >=360)
		gun_angle = gun_angle -360;
	if(gun_angle <0)
		gun_angle = gun_angle +360;
	
	i = world.current_gun;
	world.your_gun[i].rot_angle = gun_angle;
	world.your_gun[i].x = cam.view_point_x;
	world.your_gun[i].y = (float)9;
	world.your_gun[i].z = cam.view_point_z;

	if((cam.last_view_point_x - cam.view_point_x == 0) && 
		(cam.last_view_point_y - cam.view_point_y == 0) &&
		(cam.last_view_point_z - cam.view_point_z == 0))
	{
		have_i_moved_flag = FALSE;
	}
	else
		have_i_moved_flag = TRUE;

	cam.last_view_point_x = cam.view_point_x;
	cam.last_view_point_y = cam.view_point_y;
	cam.last_view_point_z = cam.view_point_z;
}

void UpdateDebugFrames(int new_seq)
{
	int i;
	int mod_id;
	int curr_seq;
	int curr_frame;

	for(i = 0; i < world.num_debug_models; i++)
	{
		curr_frame = world.debug[i].current_frame;
		curr_seq   = world.debug[i].current_sequence;
		mod_id     = world.debug[i].model_id;
		
		if((curr_frame == 0) || (curr_seq == 0))
		{		
			world.debug[i].current_sequence = new_seq;
			world.debug[i].current_frame = world.pmdata[mod_id].sequence_start_frame[new_seq];
		}
	}		
}