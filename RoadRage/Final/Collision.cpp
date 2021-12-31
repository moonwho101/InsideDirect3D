//-----------------------------------------------------------------------------
// File: Collision.cpp
//
// Desc: Code for determining if another player is hit
//
// Copyright (c) 1999 William Chin and Peter Kovach. All rights reserved.
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <io.h>
#include <math.h>
#include <windows.h>
#include "D3DApp.h"
#include "D3DMath.h"
#include "roadrage.hpp"
#include "collision.hpp"
#include "dpmessages.hpp"
#include "world.hpp"

float gv_gradient;		
float gv_dz, gv_dx, gv_da; 
float gv_target_angle;
float gv_distance;	
float gv_hit_width;	
float gv_temp_angle;
float gv_quadrant;


//-----------------------------------------------------------------------------
// Name: CMyD3DApplication::IsPlayerHit()
// Desc: This routine determines if one of the network players
//		 is hit when you fire your weapon.
//-----------------------------------------------------------------------------
void CMyD3DApplication::IsPlayerHit()
{
	int i;
	int player_num;
	DWORD player_id;


	if(num_players == 0)
		return;

	for(i = 0; i < num_players; i++)
	{
		player_id = player_list[i].RRnetID;
		
		if( (player_list[i].RRnetID != MyRRnetID) &&
			(player_list[i].bIsPlayerValid == TRUE) &&
			(player_id > 0) )
		{	
			gv_dx = player_list[i].x -m_vEyePt.x;
			gv_dz = player_list[i].z -m_vEyePt.z;

			gv_gradient		= gv_dz / gv_dx; 
			gv_target_angle = -1 * ((float)180 / (float)3.14159) * (float)atan(gv_gradient);
			gv_distance		= (float)sqrt(gv_dx * gv_dx + gv_dz * gv_dz);
	
			gv_temp_angle	= (float)angy;
	
			if((gv_dx >= 0) && (gv_dz >= 0))
			{
				gv_quadrant = 0; //quadrant 1    0 to 90
				gv_target_angle = 90 + gv_target_angle; 
			}

			if((gv_dx >= 0) && (gv_dz < 0))
				gv_quadrant = 90; //quadrant 2   90 to 180 

			if((gv_dx < 0) && (gv_dz < 0))
			{
				gv_quadrant = 180; //quadrant 3  180 to 270
				gv_target_angle = 90 + gv_target_angle; 
			}

			if((gv_dx < 0) && (gv_dz >= 0))
				gv_quadrant = 270; //quadrant 4  270 to 360
		
			gv_target_angle += gv_quadrant;


			if (gv_temp_angle >= 360)
				gv_temp_angle = gv_temp_angle - 360;

			if (gv_temp_angle < 0)
				gv_temp_angle = gv_temp_angle + 360;

			if (gv_target_angle >= 360)
				gv_target_angle = gv_target_angle - 360;

			if (gv_target_angle < 0)
				gv_target_angle = gv_target_angle + 360;

			gv_da			= gv_temp_angle - gv_target_angle;
			gv_hit_width	= (float)(abs((int)((float)tan(k * gv_da) * gv_distance))); 

			if(gv_hit_width < 10)
			{
				player_list[i].health -= 20;

				if(player_list[i].health >= 0)
				{
					player_num = GetPlayerNumber(MyRRnetID);
					if((player_num >= 0) && (player_num < MAX_NUM_PLAYERS))
					{
						SendActionDWordMessage(MyRRnetID, 
								  player_list[i].RRnetID, 
								  APPMSG_HIT,
								  DPID_ALLPLAYERS);
					}
					else
						PrintMessage(NULL,"IsPlayerHit - GetPlayerNumber FAILED", NULL, LOGFILE_ONLY);	
				}
				
				if(player_list[i].health <= 0)
				{
					if(player_list[i].bIsPlayerAlive == TRUE)
					{
						SetPlayerAnimationSequence(i, 6);	
						player_list[i].bIsPlayerAlive = FALSE;
						player_list[player_num].frags++;							
					}
				}
				else
					SetPlayerAnimationSequence(i, 4);
			}
		}
			
	} // end for i loop	
}
