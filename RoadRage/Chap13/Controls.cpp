//-----------------------------------------------------------------------------
// File: Controls.cpp
//
// Desc: Code for handling player input
//
// Copyright (c) 1999 William Chin and Peter Kovach. All rights reserved.
//-----------------------------------------------------------------------------
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
			}
			else
			{
				PlaySound(your_gun[1].sound_id, TRUE);
			}
		}

		if(bInWalkMode)
			WalkMode(Controls);
		else
			DriveMode(Controls);
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
	}

	if( Controls->bNextWeap )
	{
		current_gun++;
		if(current_gun >= num_your_guns)
			current_gun = num_your_guns-1;
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
	}

	if( Controls->bNextWeap )
	{
		current_gun++;
		if(current_gun >= num_your_guns)
			current_gun = num_your_guns-1;
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