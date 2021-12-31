//-----------------------------------------------------------------------------
// File: Controls.cpp
//
// Desc: Code for handling player input
//
// Copyright (c) 1999 William Chin and Peter Kovach. All rights reserved.
//-----------------------------------------------------------------------------
#include "dpmessages.hpp"
#include "d3dapp.h"
#include "roadrage.hpp"

#define FORWARD_GEAR  0
#define REVERSE_GEAR  1

#define SND_ENGINE_REV 1

VOID WalkMode (CONTROLS *Controls);
VOID DriveMode(CONTROLS *Controls);

BOOL set_firing_timer_flag = FALSE;

int   gear = FORWARD_GEAR;
int chat_msg_cnt = 0;
int frequency;
float car_speed = 0;
char chat_message[1024];
char dp_chat_msg[1024];
char rr_multiplay_chat_string[1024];

	
//-----------------------------------------------------------------------------
// Name: MovePlayer
// Desc: This method handles showing the scores for our game.
//		 It then moves the player as long as we are still alive.   
//		 sends a DirectPlay message to notify all other players that
//		 we are moving.  
//		 Finally, it handles input controls for driving or walking.
//-----------------------------------------------------------------------------
VOID CMyD3DApplication::MovePlayer(CONTROLS *Controls)
{
	// Update the variables for the player

	if((Controls->bScores == TRUE) && (DelayKey2[DIK_S] == FALSE) )
	{
		display_scores = !display_scores;
		DelayKey2[DIK_S] = TRUE;
	}

	if( MyHealth > 0 )
	{
		if(Controls->bTravelMode == TRUE && DelayKey2[DIK_SPACE] == FALSE)
		{
			bInWalkMode = !bInWalkMode;
			DelayKey2[DIK_SPACE] = TRUE;

			if(bInWalkMode)
			{
				StopBuffer(TRUE, 1);
				SendActionFlagMessage(MyRRnetID, TRUE, APPMSG_WALK_OR_DRIVE);
			}
			else
			{
				PlaySound(your_gun[1].sound_id, TRUE);
				SendActionFlagMessage(MyRRnetID, FALSE, APPMSG_WALK_OR_DRIVE);
			}
		}

		if(bInWalkMode)
			WalkMode(Controls);
		else
			DriveMode(Controls);
	}	
}

//-----------------------------------------------------------------------------
// Name: CMyD3DApplication::ResetChatString
// Desc: This method resets the chat count to 0 
//-----------------------------------------------------------------------------
VOID CMyD3DApplication::ResetChatString()
{
	chat_msg_cnt = 0;
}

//-----------------------------------------------------------------------------
// Name: CMyD3DApplication::HandleTalkMode
// Desc: This method handles chat messaging for multi-player games.
//		 The user can type anything at the 'say' prompt.  
//		 If the user type 'disconnect', they will exit from the gaming
//		 session.  If they type 'kill', they will be re-started at the
//		 starting position. 
//-----------------------------------------------------------------------------
VOID CMyD3DApplication::HandleTalkMode(BYTE *diks)
{
	int i;
	int player_num;
	char ch;
	BOOL shift = FALSE;

	if( (diks[DIK_LSHIFT] && 0x80) || (diks[DIK_RSHIFT] && 0x80) )
		shift = TRUE;


	for(i = 0; i < 256; i++)
	{
		if((diks[i] && 0x80) && (DelayKey2[i] == FALSE))
		{
			if( (i <= 58) && (i != DIK_RETURN) )
			{
				DelayKey2[i] = TRUE;

				if(shift == TRUE)
					ch = dik_to_char_upper_case[i];
				else
					ch = dik_to_char_lower_case[i];

				if( (diks[DIK_BACK] && 0x80) == FALSE )
				{
					if(ch != 0)
					{
						if( chat_msg_cnt < 1000 )
						{
							chat_message[chat_msg_cnt] = ch;
							chat_msg_cnt++;
						}
					}
				}
				else
				{
					chat_msg_cnt--;
					if(chat_msg_cnt < 0)
						chat_msg_cnt = 0;
				}
			}
		}
	}

	chat_message[chat_msg_cnt] = 0;
	strcpy( rr_multiplay_chat_string, "say: ");
	strcat( rr_multiplay_chat_string, chat_message);

	player_num = GetPlayerNumber(MyRRnetID);
	if(player_num == RRnetID_ERROR)
	{
		PrintMessage(NULL,"IsPlayerHit - HandleTalkMode FAILED", NULL, LOGFILE_ONLY);
		return;
	}

	strcpy( dp_chat_msg, player_list[player_num].name);
	strcat( dp_chat_msg, " : ");
	strcat( dp_chat_msg, chat_message);

	if( (diks[DIK_RETURN] && 0x80) && (DelayKey2[DIK_RETURN] == FALSE) )
	{
		DelayKey2[DIK_RETURN] = TRUE;

		if(strcmpi(chat_message, "disconnect") == 0 )
		{
			RrDestroyPlayer();
			chat_msg_cnt = 0;
			strcpy(dp_chat_msg, "");
			return;
		}
		if(strcmpi(chat_message, "kill") == 0 )
		{
			m_vEyePt.x = 700;
			m_vEyePt.y = 22;
			m_vEyePt.z = 700;
			chat_msg_cnt = 0;
			strcpy(dp_chat_msg, "");
			return;
		}
		SendChatMessage( MyRRnetID, dp_chat_msg );
		AddDpChatMessageToDisplay( dp_chat_msg );
		DelayKey2[DIK_RETURN] = TRUE;
		chat_msg_cnt = 0;
	}		
}


//-----------------------------------------------------------------------------
// Name: CMyD3DApplication::DriveMode()
// Desc: This method checks all controls when the user is in 'Driving'
//		 mode and adjusts the associated variables accordingly - e.g., 
//		 switching weapons, turning, moving, etc.
//-----------------------------------------------------------------------------
VOID CMyD3DApplication::DriveMode(CONTROLS *Controls)
{
	int i = 0;
	float dir = 0.0f;
	float step_left_angy;
	float step_right_angy;	

	have_i_moved_flag = FALSE;

	if( Controls->bPrevWeap )
	{
		current_gun--;
		if(current_gun < 0)
			current_gun = 0;

		SendActionFlagMessage(MyRRnetID, 
							  current_gun, 
							  APPMSG_CHANGE_WEAPON_TO);
	}

	if( Controls->bNextWeap )
	{
		current_gun++;
		if(current_gun >= num_your_guns)
			current_gun = num_your_guns-1;

		SendActionFlagMessage(MyRRnetID, 
							  current_gun, 
							  APPMSG_CHANGE_WEAPON_TO);
	}

	// turn left
    if( Controls->bLeft )	
	{	
		angy -= 3; 
		have_i_moved_flag = TRUE;
	}
		
	// turn right
    if( Controls->bRight )	
	{
		angy += 3; 
		have_i_moved_flag = TRUE;
	}
	
	// move forward
	if( Controls->bForward )		
	{
		car_speed += 1.0f;
		if(car_speed > 20 )
			car_speed = 20.0f;

		frequency = 12000 + (int)((float)car_speed * (float)250.0);
		SetFrequency(SND_ENGINE_REV, frequency);					

		have_i_moved_flag = TRUE;
	}
    
	// move backward
	if( Controls->bBackward )	
	{
		car_speed -= 1.0f;
		if(car_speed < 0 )
			car_speed = 0.0f;

		frequency = 12000 + (int)((float)car_speed * (float)250.0);
		SetFrequency(SND_ENGINE_REV, frequency);					

		have_i_moved_flag = TRUE;
	}

    
	if( gear == FORWARD_GEAR)		
		dir = 1.0f;
    
	if( gear == REVERSE_GEAR)
		dir = -1.0f;
		
	

	m_vEyePt.x += dir * car_speed * sinf(angy * k);
	m_vEyePt.z += dir * car_speed * cosf(angy * k);
	
	if(car_speed != 0 ) 
		have_i_moved_flag = TRUE;
	

	// Move vertically up towards sky
	if( Controls->bUp )		
		gear = FORWARD_GEAR;
	
    // Move vertically down towards ground
	if( Controls->bDown )	
		gear = REVERSE_GEAR;
	
	// fire gun 
    if( Controls->bFire  == TRUE)		
		firing_gun_flag = TRUE;
	else
		firing_gun_flag = FALSE;

	// tilt head upwards
	if( Controls->bHeadDown )		
	{	
		look_up_ang -= 3; 
		
		if(look_up_ang < -45)
			look_up_ang = -45;
	}
	
	// tilt head downward
    if( Controls->bHeadUp )		
	{
		look_up_ang += 3; 
		
		if(look_up_ang > 45)
			look_up_ang = 45;

	}

	// side step left
	if(Controls->bStepLeft )
	{
		step_left_angy = angy - 90;

		if(step_left_angy < 0)
			step_left_angy += 360;

		if(step_left_angy >= 360)
			step_left_angy = step_left_angy - 360;

		m_vEyePt.x += car_speed * sinf(step_left_angy * k);
		m_vEyePt.z += car_speed * cosf(step_left_angy * k);

		have_i_moved_flag = TRUE;
	}

	// side step right
	if(Controls->bStepRight )
	{
		step_right_angy = angy + 90;

		if(step_right_angy < 0)
			step_right_angy += 360;

		if(step_right_angy >= 360)
			step_right_angy = step_right_angy - 360;

		m_vEyePt.x += car_speed * sinf(step_right_angy * k);
		m_vEyePt.z += car_speed * cosf(step_right_angy * k);

		have_i_moved_flag = TRUE;
	}
}

//-----------------------------------------------------------------------------
// Name: CMyD3DApplication::WalkMode()
// Desc: This method checks all controls when the user is in 'Walking'
//		 mode and adjusts the associated variables accordingly - e.g., 
//		 switching weapons, turning, moving, etc.
//-----------------------------------------------------------------------------
VOID CMyD3DApplication::WalkMode(CONTROLS *Controls)
{
	int i = 0;
	float speed = 5;
	float step_left_angy;
	float step_right_angy;


	have_i_moved_flag = FALSE;

	if( Controls->bPrevWeap )
	{
		current_gun--;
		if(current_gun < 0)
			current_gun = 0;

		SendActionFlagMessage(MyRRnetID, 
							  current_gun, 
							  APPMSG_CHANGE_WEAPON_TO);
	}

	if( Controls->bNextWeap )
	{
		current_gun++;
		if(current_gun >= num_your_guns)
			current_gun = num_your_guns-1;

		SendActionFlagMessage(MyRRnetID, 
							  current_gun, 
							  APPMSG_CHANGE_WEAPON_TO);
	}

	// turn left
    if( Controls->bLeft )	
	{	
		angy -= 5; 
		have_i_moved_flag = TRUE;
	}
		
	// turn right
    if( Controls->bRight )	
	{
		angy += 5; 
		have_i_moved_flag = TRUE;
	}
	
    // move forward
	if( Controls->bForward )		
	{
		m_vEyePt.x += speed * sinf(angy * k);
		m_vEyePt.z += speed * cosf(angy * k);
		have_i_moved_flag = TRUE;
	}
    
	// move backward
	if( Controls->bBackward )	
	{
		m_vEyePt.x += -speed * sinf(angy * k);
		m_vEyePt.z += -speed * cosf(angy * k);
		have_i_moved_flag = TRUE;
	}

	// Move vertically up towards sky
	if( Controls->bUp )		
		m_vEyePt.y += 2; 
	
    // Move vertically down towards ground
	if( Controls->bDown )	
		m_vEyePt.y -= 2;

	// fire gun 
    if( (Controls->bFire == TRUE) && (MyHealth > 0 ) )		
	{
		firing_gun_flag = TRUE;
		
		if(set_firing_timer_flag == FALSE)
		{
			FireWeaponTimer = timeSetEvent(100, 0, 
				 (LPTIMECALLBACK)FireWeapon,
				 NULL, TIME_PERIODIC );

			set_firing_timer_flag = TRUE;
		}
	}
	else
	{
		firing_gun_flag = FALSE;

		if(set_firing_timer_flag == TRUE)
		{
			timeKillEvent(FireWeaponTimer);
			set_firing_timer_flag = FALSE;
		}
	}

	// tilt head upwards
	if( Controls->bHeadDown )		
	{	
		look_up_ang -= 3; 
		
		if(look_up_ang < -45)
			look_up_ang = -45;
	}
	
	// tilt head downward
    if( Controls->bHeadUp )		
	{
		look_up_ang += 3; 
		
		if(look_up_ang > 45)
			look_up_ang = 45;

	}

	// side step left
	if(Controls->bStepLeft )
	{
		step_left_angy = angy - 90;

		if(step_left_angy < 0)
			step_left_angy += 360;

		if(step_left_angy >= 360)
			step_left_angy = step_left_angy - 360;

		m_vEyePt.x += speed * sinf(step_left_angy * k);
		m_vEyePt.z += speed * cosf(step_left_angy * k);

		have_i_moved_flag = TRUE;
	}

	// side step right
	if(Controls->bStepRight )
	{
		step_right_angy = angy + 90;

		if(step_right_angy < 0)
			step_right_angy += 360;

		if(step_right_angy >= 360)
			step_right_angy = step_right_angy - 360;

		m_vEyePt.x += speed * sinf(step_right_angy * k);
		m_vEyePt.z += speed * cosf(step_right_angy * k);

		have_i_moved_flag = TRUE;
	}

}