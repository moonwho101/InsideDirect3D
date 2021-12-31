//-----------------------------------------------------------------------------
// File: DpMessages.cpp
//
// Desc: Code for handling all DirectPlay messaging
//
// Copyright (c) 1999 William Chin and Peter Kovach. All rights reserved.
//-----------------------------------------------------------------------------

#include <dplobby.h>
#include <dplay.h>
#include "D3DApp.h"
#include "D3DUtil.h"
#include "D3DMath.h"
#include "world.hpp"
#include "3Dengine.hpp"
#include "RoadRage.hpp"
#include "D3DTextr.h" 
#include "LoadWorld.hpp"
#include "DpMessages.hpp"

#define MAX_PLAYER_NAME      14
#define MAX_SESSION_NAME     256
#define DPLAY_SAMPLE_KEY     TEXT("Software\\Microsoft\\DirectX DirectPlay Samples")

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }



LPDIRECTPLAY4A       g_pDP = NULL;
LPDPLCONNECTION      g_pDPLConnection;
LPDIRECTPLAYLOBBY3   g_pDPLobby;
GUID                 g_AppGUID; 
HANDLE               g_hDPMessageEvent;
HKEY                 g_hDPlaySampleRegKey                      = NULL;
TCHAR                g_strPreferredProvider[MAX_SESSION_NAME];
TCHAR                g_strSessionName[MAX_SESSION_NAME];
TCHAR                g_strLocalPlayerName[MAX_PLAYER_NAME];
DPID                 g_LocalPlayerDPID; 
BOOL                 g_bHostPlayer;
TCHAR                g_strAppName[256];
BOOL                 g_bUseProtocol;
VOID*              g_pvDPMsgBuffer                           = NULL;
DWORD              g_dwDPMsgBufferSize                       = 0;
DWORD              g_dwNumberOfActivePlayers;


int net_debug_receive = 0;
int net_debug_send = 0;
int time_out_period = 0;//1000;
int DpChatMessage_counter = 0;
char DpChatMessages[12][1024];

int GetPlayerNumber(DWORD dpid);
char buffer [256];
char buffer2[256];

extern CMyD3DApplication* pCMyApp;
extern CD3DApplication* g_pD3DApp;
//extern HWND g_hWnd;

HRESULT ReadRegKey( HKEY hKey, TCHAR* strName, TCHAR* strValue, 
                    DWORD dwLength, TCHAR* strDefault );
HRESULT WriteRegKey( HKEY hKey, TCHAR* strName, TCHAR* strValue );

//-----------------------------------------------------------------------------
// Name: ProcessDirectPlayMessages()
// Desc: Processes for DirectPlay messages
//-----------------------------------------------------------------------------

void RrDestroyPlayer()
{
	if(pCMyApp->multiplay_flag == FALSE)
		return;

	timeKillEvent(pCMyApp->UpdatePosTimer);
	timeKillEvent(pCMyApp->FireWeaponTimer);

	if(g_pDP == NULL)
	{
		PrintMessage(NULL, "RrDestroyPlayer FAILED - g_pDP = NULL", NULL, LOGFILE_ONLY);
		return;
	}
	else
		PrintMessage(NULL, "RrDestroyPlayer  - g_pDP ok ", NULL, LOGFILE_ONLY);

	if(g_pDP->DestroyPlayer( g_LocalPlayerDPID ) != DP_OK)
	{
		PrintMessage(NULL, "RrDestroyPlayer FAILED - DestroyPlayer", NULL, LOGFILE_ONLY);
		return;
	}
	else
		PrintMessage(NULL, "RrDestroyPlayer - DestroyPlayer ok", NULL, LOGFILE_ONLY);

    if(g_pDP->Close() != DP_OK)
	{
		PrintMessage(NULL, "RrDestroyPlayer FAILED - Close", NULL, LOGFILE_ONLY);
		return;
	}
	else
		PrintMessage(NULL, "RrDestroyPlayer - Close ok", NULL, LOGFILE_ONLY);


    SAFE_DELETE_ARRAY( g_pDPLConnection );
    SAFE_RELEASE( g_pDPLobby );
    //SAFE_RELEASE( g_pDP );

	PrintMessage(NULL, "RrDestroyPlayer - disconnected ok", NULL, LOGFILE_ONLY);
       
	pCMyApp->multiplay_flag = FALSE;
	pCMyApp->num_players = 0;
}

HRESULT ProcessDirectPlayMessages( HWND hDlg )
{
    DPID    idFrom;
    DPID    idTo;
    LPVOID  pvMsgBuffer;
    DWORD   dwMsgBufferSize;
    HRESULT hr;


	if(g_pDP == NULL)
		return FALSE;

    // Read all messages in queue
    dwMsgBufferSize = g_dwDPMsgBufferSize;
    pvMsgBuffer     = g_pvDPMsgBuffer;
    
    while( TRUE )
    {
        // See what's out there
        idFrom = 0;
        idTo   = 0;

        hr = g_pDP->Receive( &idFrom, &idTo, DPRECEIVE_ALL, 
                             pvMsgBuffer, &dwMsgBufferSize );

        if( hr == DPERR_BUFFERTOOSMALL )
        {
            // The current buffer was too small, 
            // so reallocate it and try again
            SAFE_DELETE_ARRAY( pvMsgBuffer );

            pvMsgBuffer = new BYTE[ dwMsgBufferSize ];
            if( pvMsgBuffer == NULL )
                return E_OUTOFMEMORY;

            // Save new buffer in globals
            g_pvDPMsgBuffer     = pvMsgBuffer;
            g_dwDPMsgBufferSize = dwMsgBufferSize;

            continue; // Now that the buffer is bigger, try again
        }

        if( DPERR_NOMESSAGES == hr )
            return S_OK;

        if( FAILED(hr) )
            return hr;

        // Handle the messages. If its from DPID_SYSMSG, its a system message, 
        // otherwise its an application message. 
        if( idFrom == DPID_SYSMSG )
        {
            hr = HandleSystemMessages( hDlg, (DPMSG_GENERIC*)pvMsgBuffer, 
                                       dwMsgBufferSize, idFrom, idTo );
            if( FAILED(hr) )
                return hr;
        }
        else
        {
            hr = HandleAppMessages( hDlg, (GAMEMSG_GENERIC*)pvMsgBuffer, 
                                    dwMsgBufferSize, idFrom, idTo );
            if( FAILED(hr) )
                return hr;
        }
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: HandleAppMessages()
// Desc: Evaluates application messages and performs appropriate actions
//-----------------------------------------------------------------------------
HRESULT HandleAppMessages( HWND hDlg, GAMEMSG_GENERIC* pMsg, DWORD dwMsgSize, 
                           DPID idFrom, DPID idTo )
{
    LPSTR				  lpszStr	    = NULL;
	LPMSG_POS			  lpPos		    = NULL;
	LPMSG_ACTIONWORDINFO  lpActionWord  = NULL;
	LPMSG_ACTIONDWORDINFO lpActionDWord = NULL;
	LPMSG_ACTIONFLAGINFO  lpActionFlag  = NULL;
	LPMSG_PLAYER_INFO     lpPlayerInfo  = NULL;
	LPMSG_ACTIONFLOATINFO lpActionFloat = NULL;
	int player_num = 0;
	int i;
	int ping;
	int MyPlayerNum;
	int mod_id;
	int curr_seq;
	int curr_wep;
	int start_frame;
	int scoring_player;
	float curr_time;
	float fTime;
	DWORD from;
	FILE *fp;

	
	switch (pMsg->dwType)
	{	
		
		case APPMSG_PING_TO_SERVER:
			lpActionFloat = (LPMSG_ACTIONFLOATINFO)pMsg;
			fTime = lpActionFloat->action_float;
			from  = lpActionFloat->RRnetID;
			pCMyApp->num_packets_received++;

			if(g_bHostPlayer == TRUE)	
			{
				//if(idTo == DPID_SERVERPLAYER)
					SendActionFloatMessage(pCMyApp->MyRRnetID, fTime, APPMSG_PING_TO_CLIENT, from);
			}
			break;

		case APPMSG_PING_TO_CLIENT:
			lpActionFloat = (LPMSG_ACTIONFLOATINFO)pMsg;
			fTime = lpActionFloat->action_float;
			from  = lpActionFloat->RRnetID;
			pCMyApp->num_packets_received++;
			
			if(idTo == pCMyApp->MyRRnetID)
			{
				curr_time = timeGetTime() * 0.001f;
				ping = (int)( (curr_time - fTime) *1000.0f );
				ping = ping / 2;
				if(ping > 99999)
					ping = 99999;

				pCMyApp->MyPing = ping;

				MyPlayerNum = GetPlayerNumber(pCMyApp->MyRRnetID);
				if(MyPlayerNum == RRnetID_ERROR)
				{
					PrintMessage(NULL,"IsPlayerHit - APPMSG_PING_TO_CLIENT FAILED", NULL, LOGFILE_ONLY);
					break;
				}

				pCMyApp->player_list[MyPlayerNum].ping = ping;

				SendActionFloatMessage(pCMyApp->MyRRnetID, 
									   (float)ping, 
									   APPMSG_PING_TO_ALL, 
									   DPID_ALLPLAYERS);
									   
			}
			break;

		case APPMSG_PING_TO_ALL:
			lpActionFloat = (LPMSG_ACTIONFLOATINFO)pMsg;
			ping = (int)lpActionFloat->action_float;
			from  = lpActionFloat->RRnetID;
			pCMyApp->num_packets_received++;
			
			player_num = GetPlayerNumber(from);
			if(player_num == RRnetID_ERROR)
			{
				PrintMessage(NULL,"IsPlayerHit - APPMSG_PING_TO_ALL FAILED", NULL, LOGFILE_ONLY);
				break;
			}
			pCMyApp->player_list[player_num].ping = ping;
			break;

		case APPMSG_POSITION:
			
			lpPos = (LPMSG_POS)pMsg;	
			player_num = GetPlayerNumber(idFrom); //lpPos->RRnetID);
			if(player_num == RRnetID_ERROR)
			{
				PrintMessage(NULL,"IsPlayerHit - APPMSG_POSITION FAILED", NULL, LOGFILE_ONLY);
				break;
			}
			pCMyApp->player_list[player_num].x = lpPos->x_pos;	
			pCMyApp->player_list[player_num].y = lpPos->y_pos+12;
			pCMyApp->player_list[player_num].z = lpPos->z_pos;
			pCMyApp->player_list[player_num].rot_angle = lpPos->view_angle;
			pCMyApp->player_list[player_num].bIsRunning = lpPos->IsRunning;
			
			if(lpPos->IsRunning == TRUE)
			{	
				if(pCMyApp->player_list[player_num].current_sequence != 1)
				{
					if(pCMyApp->player_list[player_num].bIsPlayerAlive == TRUE)
						pCMyApp->SetPlayerAnimationSequence(player_num, 1);	
				}
			}
			pCMyApp->num_packets_received++;
			break;

		case APPMSG_PLAYER_LIST:
			lpPlayerInfo = (LPMSG_PLAYER_INFO)pMsg;	
			i = lpPlayerInfo->player_number;
			pCMyApp->num_players = lpPlayerInfo->num_players;
			pCMyApp->player_list[i].RRnetID = lpPlayerInfo->RRnetID;
			lstrcpy(pCMyApp->player_list[i].name, lpPlayerInfo->name);
			pCMyApp->num_packets_received++;

			pCMyApp->player_list[i].x = 500;
			pCMyApp->player_list[i].y = 12;
			pCMyApp->player_list[i].z = 500;
			pCMyApp->player_list[i].rot_angle = 0;
			pCMyApp->player_list[i].model_id = 0;
			pCMyApp->player_list[i].skin_tex_id = 27;
			pCMyApp->player_list[i].ping = 0;
			pCMyApp->player_list[i].health = 100;
			pCMyApp->player_list[i].bIsPlayerAlive = TRUE;
			pCMyApp->player_list[i].bStopAnimating = FALSE;
			pCMyApp->player_list[i].current_weapon = 0;
			pCMyApp->player_list[i].current_car = 0;
			pCMyApp->player_list[i].current_frame = 0;
			pCMyApp->player_list[i].current_sequence = 0;
			pCMyApp->player_list[i].bIsPlayerInWalkMode = TRUE;
			pCMyApp->player_list[i].RRnetID = lpPlayerInfo->RRnetID;
			pCMyApp->player_list[i].bIsPlayerValid = TRUE;

			fp = fopen("rrlogfile.txt","a");
			fprintf( fp, "\n APPMSG_PLAYER_LIST -----\n");
			fprintf( fp, "player_num : %d\n", i);
			fprintf( fp, "Player's name  : %s\n", pCMyApp->player_list[i].name);
			fprintf( fp, "Player's id    : %d\n", lpPlayerInfo->RRnetID);
			fprintf( fp, "Player's idFrom: %d\n", idFrom);
			fprintf( fp, "-------------------\n\n");
			fclose(fp);

			break;

		case APPMSG_GESTURE:	
			
			lpActionWord = (LPMSG_ACTIONWORDINFO)pMsg;		
			curr_seq = lpActionWord->action_word;
			player_num = GetPlayerNumber(idFrom); //lpActionWord->RRnetID);
			if(player_num == RRnetID_ERROR)
			{
				PrintMessage(NULL,"IsPlayerHit - APPMSG_GESTURE FAILED", NULL, LOGFILE_ONLY);
				break;
			}
			pCMyApp->player_list[player_num].current_sequence = curr_seq;		
			mod_id	    = pCMyApp->player_list[player_num].model_id;
			start_frame = pCMyApp->pmdata[mod_id].sequence_start_frame[curr_seq];
			pCMyApp->player_list[player_num].current_frame = start_frame;
			pCMyApp->num_packets_received++;
			break;

		case APPMSG_CHANGE_WEAPON_TO:
			lpActionWord = (LPMSG_ACTIONWORDINFO)pMsg;
			player_num = GetPlayerNumber(idFrom);//lpActionWord->RRnetID);
			if(player_num == RRnetID_ERROR)
			{
				PrintMessage(NULL,"IsPlayerHit - APPMSG_CHANGE_WEAPON_TO FAILED", NULL, LOGFILE_ONLY);
				break;
			}
			pCMyApp->player_list[player_num].current_weapon = lpActionWord->action_word;		
			pCMyApp->num_packets_received++;
			break;

		case APPMSG_HIT:
			lpActionDWord = (LPMSG_ACTIONDWORDINFO)pMsg;
			player_num = GetPlayerNumber(lpActionDWord->action_dword);
			if(player_num == RRnetID_ERROR)
			{
				PrintMessage(NULL,"IsPlayerHit - APPMSG_HIT FAILED", NULL, LOGFILE_ONLY);
				break;
			}
			pCMyApp->player_list[player_num].health -= 20;
			if(pCMyApp->player_list[player_num].health <= 0)
			{
				pCMyApp->player_list[player_num].health = 0;

				if(pCMyApp->player_list[player_num].bIsPlayerAlive == TRUE)
				{
					pCMyApp->SetPlayerAnimationSequence(player_num, 6);	
					pCMyApp->player_list[player_num].bIsPlayerAlive = FALSE;

					scoring_player = GetPlayerNumber(lpActionDWord->RRnetID);
					if(scoring_player == RRnetID_ERROR)
					{
						PrintMessage(NULL,"IsPlayerHit - APPMSG_HIT (scoring_player) FAILED", NULL, LOGFILE_ONLY);
						break;
					}
					pCMyApp->player_list[scoring_player].frags++;
				}
			}

			if(lpActionDWord->action_dword == pCMyApp->MyRRnetID)
				pCMyApp->MyHealth = pCMyApp->player_list[player_num].health;

			if(pCMyApp->MyHealth <= 0)
			{
				pCMyApp->m_vEyePt.y = 0;
				SetTimer(g_pD3DApp->Get_hWnd() , RESPAWN_TIMER, 3000, NULL);
				timeKillEvent(pCMyApp->FireWeaponTimer);
				PrintMessage(NULL,"received RESPAWN_TIMER ok",NULL, LOGFILE_ONLY);
			}

			pCMyApp->num_packets_received++;
			//PrintMessage(NULL,"received APPMSG_HIT ok",NULL, LOGFILE_ONLY);
	
			break;

		case APPMSG_FIRE:
			lpActionWord = (LPMSG_ACTIONWORDINFO)pMsg;
			player_num   = GetPlayerNumber(lpActionWord->RRnetID);
			if(player_num == RRnetID_ERROR)
			{
				PrintMessage(NULL,"IsPlayerHit - APPMSG_FIRE FAILED", NULL, LOGFILE_ONLY);
				break;
			}
			curr_wep = lpActionWord->action_word;	
				
			pCMyApp->player_list[player_num].bIsFiring = TRUE;

			if(pCMyApp->player_list[player_num].current_sequence != 2)
			{
				if(pCMyApp->player_list[player_num].bIsPlayerAlive == TRUE)
					pCMyApp->SetPlayerAnimationSequence(player_num, 2);
				
			}
			pCMyApp->StopBuffer(TRUE, 0);
			pCMyApp->PlaySound(0, NULL);

			pCMyApp->num_packets_received++;
			break;
		
		case APPMSG_WALK_OR_DRIVE:
			lpActionFlag = (LPMSG_ACTIONFLAGINFO)pMsg;
			player_num = GetPlayerNumber(lpActionFlag->RRnetID);
			if(player_num == RRnetID_ERROR)
			{
				PrintMessage(NULL,"IsPlayerHit - APPMSG_WALK_OR_DRIVE FAILED", NULL, LOGFILE_ONLY);
				break;
			}
			pCMyApp->player_list[player_num].bIsPlayerInWalkMode = lpActionFlag->action_flag;
			pCMyApp->num_packets_received++;
			break;	

		case APPMSG_RESPAWN:
			lpActionFlag = (LPMSG_ACTIONFLAGINFO)pMsg;
			player_num = GetPlayerNumber(lpActionFlag->RRnetID);
			if(player_num == RRnetID_ERROR)
			{
				PrintMessage(NULL,"IsPlayerHit - APPMSG_RESPAWN FAILED", NULL, LOGFILE_ONLY);
				break;
			}
			pCMyApp->player_list[player_num].bIsPlayerAlive = TRUE;
			pCMyApp->player_list[player_num].bStopAnimating = FALSE;
			pCMyApp->player_list[player_num].current_weapon = 0;
			pCMyApp->player_list[player_num].current_car = 0;
			pCMyApp->player_list[player_num].current_frame = 0;
			pCMyApp->player_list[player_num].current_sequence = 0;
			pCMyApp->player_list[player_num].bIsPlayerInWalkMode = TRUE;
			pCMyApp->player_list[player_num].health = 100;

			pCMyApp->num_packets_received++;
			//PrintMessage(NULL,"received APPMSG_RESPAWN ok",NULL, LOGFILE_ONLY);
	
			break;	

		case APPMSG_JUMP:
			pCMyApp->num_packets_received++;
			break;
			
		case APPMSG_CROUCH:
			pCMyApp->num_packets_received++;
			break;

		case APPMSG_CHATSTRING:
			LPMSG_CHATSTRING   lpszStr = (LPMSG_CHATSTRING) pMsg;

			// post string to chat window
			if (lpszStr)
			{
				if (lpszStr->szMsg[0] != 13 )
				{
					AddDpChatMessageToDisplay(lpszStr->szMsg);
					pCMyApp->num_packets_received++;
				}
			}
			break;
	}
    return S_OK;
}

void DisplayIncomingDpChatMessages()
{
	int i;

	for(i = 0; i < 4; i++)
	{
		if(DpChatMessages[i])
			pCMyApp->OutputText(20,60+i*20, DpChatMessages[i]);
	}
}

void AddDpChatMessageToDisplay(char *msg)
{
	int i;


	if(DpChatMessage_counter < 4)
	{
		strcpy(DpChatMessages[DpChatMessage_counter], msg);
		DpChatMessage_counter++;
	}
	else
	{
		for(i = 0; i < 3; i++)
			strcpy(DpChatMessages[i], DpChatMessages[i+1] );

		strcpy(DpChatMessages[3], msg);
			
	}

}

int GetPlayerNumber(DWORD dpid)
{
	int i;

	for(i = 0; i < pCMyApp->num_players; i++)
	{
		if(pCMyApp->player_list[i].bIsPlayerValid == TRUE)
		{
			if(pCMyApp->player_list[i].RRnetID == dpid) 
				return i;
			
		}
	}
	
	PrintMessage(NULL, "GetPlayerNumber: Failed to get player ID", NULL, LOGFILE_ONLY);
	return RRnetID_ERROR;
}


//-----------------------------------------------------------------------------
// Name: HandleSystemMessages()
// Desc: Evaluates system messages and performs appropriate actions
//-----------------------------------------------------------------------------
HRESULT HandleSystemMessages( HWND hDlg, DPMSG_GENERIC* pMsg, DWORD dwMsgSize, 
                              DPID idFrom, DPID idTo )
{  
	LPSTR	lpszStr = NULL;
	LPDPMSG_CREATEPLAYERORGROUP 	lp = (LPDPMSG_CREATEPLAYERORGROUP) pMsg;
	LPSTR							lpszPlayerName;
	LPSTR							szDisplayFormat = "%s has joined";
			
	FILE *fp;
	int i;
	int player;

	
    switch( pMsg->dwType )
    {
        case DPSYS_SESSIONLOST:
            // Non-host message.  This message is sent to all players 
            // when the host exits the game.

			PrintMessage(NULL, "DPSYS_SESSIONLOST - host has quit", NULL, LOGFILE_ONLY);
            if( g_bHostPlayer )
                return E_FAIL; // Sanity check
			AddDpChatMessageToDisplay( "Host has quit the game" );
            PostQuitMessage( DPERR_SESSIONLOST );
            break;

		case DPSYS_HOST:
			PrintMessage(NULL, "DPSYS_HOST - you are now the host", NULL, LOGFILE_ONLY);
			g_bHostPlayer = TRUE;
			pCMyApp->MyPing = 0;
			AddDpChatMessageToDisplay( "You are now the host" );
			break;

        case DPSYS_CREATEPLAYERORGROUP:
            DPMSG_CREATEPLAYERORGROUP* pCreateMsg;
            pCreateMsg = (DPMSG_CREATEPLAYERORGROUP*) pMsg;

            // Update the number of active players
            g_dwNumberOfActivePlayers++;

			// get pointer to player name
			if (lp->dpnName.lpszShortNameA)
				lpszPlayerName = lp->dpnName.lpszShortNameA;
			else
				lpszPlayerName = "unknown";
			
			for(i = 0; i < MAX_NUM_PLAYERS; i++)
			{
				if( pCMyApp->player_list[i].bIsPlayerValid == FALSE )
				{
					player = i;
					pCMyApp->player_list[i].bIsPlayerValid = TRUE;
					break;
				}
			}
		
			fp = fopen("rrlogfile.txt","a");
			fprintf( fp, "\n receive DPSYS_CREATEPLAYERORGROUP \n");
			fprintf( fp, "player_id     : %d\n", lp->dpId);
			fprintf( fp, "player_number : %d\n", player);
			fprintf( fp, "-------------------\n\n");
			fclose(fp);

			pCMyApp->player_list[player].x = 500;
			pCMyApp->player_list[player].y = 12;
			pCMyApp->player_list[player].z = 500;
			pCMyApp->player_list[player].rot_angle = 0;
			pCMyApp->player_list[player].model_id = 0;
			pCMyApp->player_list[player].skin_tex_id = 27;
			pCMyApp->player_list[player].ping = 0;
			pCMyApp->player_list[player].health = 100;
			pCMyApp->player_list[player].bIsPlayerAlive = TRUE;
			pCMyApp->player_list[player].bStopAnimating = FALSE;
			pCMyApp->player_list[player].current_weapon = 0;
			pCMyApp->player_list[player].current_car = 0;
			pCMyApp->player_list[player].current_frame = 0;
			pCMyApp->player_list[player].current_sequence = 0;
			pCMyApp->player_list[player].bIsPlayerInWalkMode = TRUE;
			pCMyApp->player_list[player].RRnetID = lp->dpId;
			pCMyApp->num_players++;

			strcpy(pCMyApp->player_list[player].name, lpszPlayerName);
			
		
			if(g_bHostPlayer == TRUE)
			{
				for(i = 0; i < MAX_NUM_PLAYERS; i++)
				{
					if(pCMyApp->player_list[i].bIsPlayerValid == TRUE)
					{
					
						SendPlayerInfoMessage(
							pCMyApp->player_list[i].RRnetID,
							i,
							pCMyApp->player_list[i].name,	
							pCMyApp->num_players, 
							APPMSG_PLAYER_LIST );
					}
				}											  
			
				strcpy(buffer, lpszPlayerName);
				strcat(buffer, " has joined the game");
				AddDpChatMessageToDisplay( buffer );
			}

            DisplayNumberPlayersInGame( hDlg );
            break;

        case DPSYS_DESTROYPLAYERORGROUP:
            DPMSG_DESTROYPLAYERORGROUP* pDeleteMsg;
            pDeleteMsg = (DPMSG_DESTROYPLAYERORGROUP*) pMsg;

			fp = fopen("rrlogfile.txt","a");
			fprintf( fp, "\n receive DPSYS_DESTROYPLAYERORGROUP \n");

			player = GetPlayerNumber(pDeleteMsg->dpId);
			if(player == RRnetID_ERROR)
			{
				PrintMessage(NULL,"IsPlayerHit - DPSYS_DESTROYPLAYERORGROUP FAILED", NULL, LOGFILE_ONLY);
				break;
			}
			pCMyApp->player_list[player].bIsPlayerValid = FALSE;
			
			fprintf( fp, "player_id     : %d\n", pDeleteMsg->dpId);
			fprintf( fp, "player_number : %d\n", player);
			fprintf( fp, "-------------------\n\n");
			fclose(fp);


			strcpy(buffer, pCMyApp->player_list[player].name);
			strcat(buffer, " has left the game");
			AddDpChatMessageToDisplay( buffer );

            // Update the number of active players
            g_dwNumberOfActivePlayers--;

            DisplayNumberPlayersInGame( hDlg );
            break;
    }

    return S_OK;
}


HRESULT SendPlayerInfoMessage(DWORD player_id, BYTE player_num, 
				char *player_name, BYTE num_players, DWORD dwType)
{
	LPMSG_PLAYER_INFO lpPIMessage = NULL;
	DWORD dwPIMessageSize = NULL;
	HRESULT	hr;
	FILE *fp;
	DWORD player_name_length;


	if(pCMyApp->multiplay_flag == FALSE)
		return NULL;
	
	player_name_length = lstrlen(player_name);

	// create space for message plus 
	dwPIMessageSize = sizeof(MSG_PLAYER_INFO) + player_name_length;
	lpPIMessage = (LPMSG_PLAYER_INFO) GlobalAllocPtr(GHND, dwPIMessageSize);
	if (lpPIMessage == NULL)
	{
		hr = DPERR_OUTOFMEMORY;
		goto FAILURE;
	}


	// build message	
	lpPIMessage->dwType  = dwType;
	lpPIMessage->player_number = player_num;
	lpPIMessage->num_players = num_players;
	lpPIMessage->RRnetID = player_id;
	lstrcpy(lpPIMessage->name, player_name);


	fp = fopen("rrlogfile.txt","a");
	
	fprintf( fp, "\n Send APPMSG_PLAYER_LIST -----\n");
	fprintf( fp, "player_id     : %d\n", player_id);
	fprintf( fp, "player_number : %d\n", player_num);
	fprintf( fp, "num_players   : %d\n", num_players);
	fprintf( fp, "-------------------\n\n");
	
	fclose(fp);
		

	// send this string to all other players if your the server

	hr = g_pDP->SendEx(pCMyApp->MyRRnetID, DPID_ALLPLAYERS, DPSEND_GUARANTEED, 
		         lpPIMessage, dwPIMessageSize , 1, time_out_period, NULL, NULL);

	
	if(hr == DP_OK || DPERR_PENDING)
		pCMyApp->num_packets_sent++;
	else
		goto FAILURE;

	return (hr);

FAILURE:

	if (lpPIMessage)
		GlobalFreePtr(lpPIMessage);

	return (hr);
}


HRESULT SendPositionMessage(DWORD RRnetID, float x, float y, float z, float view_angle, BOOL IsRunning)
{
	LPMSG_POS lpPosMessage     = NULL;
	DWORD	  dwPosMessageSize = NULL;
	HRESULT	  hr;
	


	if(pCMyApp->multiplay_flag == FALSE)
		return NULL;
	
	// create space for message plus 
	dwPosMessageSize = sizeof(MSG_POS);
	lpPosMessage = (LPMSG_POS) GlobalAllocPtr(GHND, dwPosMessageSize);
	if (lpPosMessage == NULL)
	{
		hr = DPERR_OUTOFMEMORY;
		goto FAILURE;
	}

	// build message	
	lpPosMessage->dwType  = APPMSG_POSITION;
	//lpPosMessage->RRnetID = RRnetID;
	lpPosMessage->x_pos = x;
	lpPosMessage->y_pos = 0;
	lpPosMessage->z_pos = z;
	lpPosMessage->view_angle = (WORD)view_angle;
	lpPosMessage->IsRunning  = IsRunning; 

	hr = g_pDP->SendEx(RRnetID, DPID_ALLPLAYERS, DPSEND_ASYNC, 
		         lpPosMessage, sizeof(MSG_POS) , 1, time_out_period, NULL, NULL);

	if(hr == DP_OK || DPERR_PENDING)
		pCMyApp->num_packets_sent++;
	else
		goto FAILURE;

	return (hr);

FAILURE:

	if (lpPosMessage)
		GlobalFreePtr(lpPosMessage);

	return (hr);
}

HRESULT SendActionFlagMessage(DWORD RRnetID, BOOL action_flag, DWORD dwType)
{
	LPMSG_ACTIONFLAGINFO lpAFMessage = NULL;
	DWORD dwAFMessageSize = NULL;
	HRESULT	hr;
	

	if(pCMyApp->multiplay_flag == FALSE)
		return NULL;
	
	// create space for message plus 
	dwAFMessageSize = sizeof(MSG_ACTIONFLAGINFO);

	lpAFMessage = (LPMSG_ACTIONFLAGINFO) GlobalAllocPtr(GHND, dwAFMessageSize);
	if (lpAFMessage == NULL)
	{
		hr = DPERR_OUTOFMEMORY;
		goto FAILURE;
	}

	// build message	
	lpAFMessage->dwType = dwType;
	lpAFMessage->RRnetID = pCMyApp->MyRRnetID;
	lpAFMessage->action_flag = action_flag;
	

	// send this string to all other players if your the server

	hr = g_pDP->SendEx(RRnetID, DPID_ALLPLAYERS, DPSEND_GUARANTEED, 
		        lpAFMessage, sizeof(MSG_ACTIONFLAGINFO) , 3, time_out_period, NULL, NULL);

	if(hr == DP_OK || DPERR_PENDING)
		pCMyApp->num_packets_sent++;
	else
		goto FAILURE;

	return (hr);

FAILURE:

	if (lpAFMessage)
		GlobalFreePtr(lpAFMessage);

	return (hr);
}

HRESULT SendActionWordMessage(DWORD RRnetID, int action_word, DWORD dwType)
{
	LPMSG_ACTIONWORDINFO lpAWMessage = NULL;
	DWORD dwAWMessageSize = 0;
	HRESULT	hr;


	if(pCMyApp->multiplay_flag == FALSE)
		return NULL;
	
	// create space for message plus 
	dwAWMessageSize = sizeof(MSG_ACTIONWORDINFO);
	lpAWMessage = (LPMSG_ACTIONWORDINFO) GlobalAllocPtr(GHND, dwAWMessageSize);
	if (lpAWMessage == NULL)
	{
		hr = DPERR_OUTOFMEMORY;
		goto FAILURE;
	}

	// build message	
	lpAWMessage->dwType  = dwType;
	lpAWMessage->RRnetID = pCMyApp->MyRRnetID;
	lpAWMessage->action_word = action_word;

	// send this string to all other players

	hr = g_pDP->SendEx(RRnetID, DPID_ALLPLAYERS, DPSEND_ASYNC, lpAWMessage, 
		sizeof(MSG_ACTIONWORDINFO) , 2, time_out_period, NULL, NULL);

	if(hr == DP_OK || DPERR_PENDING)
		pCMyApp->num_packets_sent++;
	else
		goto FAILURE;

	return (hr);

FAILURE:

	if (lpAWMessage)
		GlobalFreePtr(lpAWMessage);

	return (hr);
}

HRESULT SendActionDWordMessage(DWORD RRnetID, DWORD action_dword, DWORD dwType, DWORD SendTo)
{
	LPMSG_ACTIONDWORDINFO lpADWMessage = NULL;
	DWORD dwADWMessageSize = 0;
	HRESULT	hr;


	if(pCMyApp->multiplay_flag == FALSE)
		return NULL;
	
	// create space for message plus 
	dwADWMessageSize = sizeof(MSG_ACTIONDWORDINFO);
	lpADWMessage = (LPMSG_ACTIONDWORDINFO) GlobalAllocPtr(GHND, dwADWMessageSize);
	if (lpADWMessage == NULL)
	{
		hr = DPERR_OUTOFMEMORY;
		goto FAILURE;
	}

	// build message	
	lpADWMessage->dwType  = dwType;
	lpADWMessage->RRnetID = pCMyApp->MyRRnetID;
	lpADWMessage->action_dword = action_dword;

	// send this string to all other players

	hr = g_pDP->SendEx(RRnetID, SendTo, DPSEND_GUARANTEED, lpADWMessage, 
		sizeof(MSG_ACTIONDWORDINFO) , 1, 0, NULL, NULL);

	if(hr == DP_OK || DPERR_PENDING)
		pCMyApp->num_packets_sent++;
	else
		goto FAILURE;

	//PrintMessage(NULL,"SendActionDWordMessage ok",NULL, LOGFILE_ONLY);
	return (hr);

FAILURE:

	if (lpADWMessage)
		GlobalFreePtr(lpADWMessage);

	return (hr);
}

HRESULT SendActionFloatMessage(DWORD RRnetID, float action_float, DWORD dwType, DWORD SendTo)
{
	LPMSG_ACTIONFLOATINFO lpADWMessage = NULL;
	DWORD dwADWMessageSize = 0;
	HRESULT	hr;


	if(pCMyApp->multiplay_flag == FALSE)
		return NULL;
	
	// create space for message plus 
	dwADWMessageSize = sizeof(MSG_ACTIONFLOATINFO);
	lpADWMessage = (LPMSG_ACTIONFLOATINFO) GlobalAllocPtr(GHND, dwADWMessageSize);
	if (lpADWMessage == NULL)
	{
		hr = DPERR_OUTOFMEMORY;
		goto FAILURE;
	}

	// build message	
	lpADWMessage->dwType  = dwType;
	lpADWMessage->RRnetID = RRnetID;
	lpADWMessage->action_float = action_float;

	// send this message to all other players

	hr = g_pDP->SendEx(RRnetID, SendTo, DPSEND_ASYNC, lpADWMessage, 
		sizeof(MSG_ACTIONFLOATINFO) , 1, time_out_period, NULL, NULL);

	if(hr == DP_OK || DPERR_PENDING)
		pCMyApp->num_packets_sent++;
	else
		goto FAILURE;

	return (hr);

FAILURE:

	if (lpADWMessage)
		GlobalFreePtr(lpADWMessage);

	PrintMessage(NULL, "SendActionFloatMessage : FAILED", NULL, LOGFILE_ONLY);
	return (hr);
}

HRESULT SendChatMessage(DWORD RRnetID, char *pChatStr)
{
	LPMSG_CHATSTRING	lpChatMessage	= NULL;
	DWORD				dwChatMessageSize;
	HRESULT				hr;


	if(pCMyApp->multiplay_flag == FALSE)
		return NULL;

	// create space for message plus string (string length included in message header)
	dwChatMessageSize = sizeof(MSG_CHATSTRING) + lstrlen(pChatStr);
	lpChatMessage = (LPMSG_CHATSTRING) GlobalAllocPtr(GHND, dwChatMessageSize);
	
	if (lpChatMessage == NULL)
	{
		hr = DPERR_OUTOFMEMORY;
		goto FAILURE;
	}

	// build message	
	lpChatMessage->dwType = APPMSG_CHATSTRING;
	lstrcpy(lpChatMessage->szMsg, pChatStr);
	
	// send this string to all other players
	hr = g_pDP->SendEx(RRnetID, DPID_ALLPLAYERS, DPSEND_ASYNC, lpChatMessage, 
		dwChatMessageSize , 1, time_out_period, NULL, NULL);

	if(hr == DP_OK || DPERR_PENDING)
		pCMyApp->num_packets_sent++;
	else
		goto FAILURE;

	return (hr);

FAILURE:

	if (lpChatMessage)
		GlobalFreePtr(lpChatMessage);

	return (hr);
}

BOOL CALLBACK UpdatePosCallBack(UINT uTimerID, 
		UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)

{
	int player_num;


	if(pCMyApp->multiplay_flag == FALSE)
		return FALSE;

	ProcessDirectPlayMessages( NULL );

	player_num = GetPlayerNumber(pCMyApp->MyRRnetID);
	if(player_num == RRnetID_ERROR)
	{
		PrintMessage(NULL,"IsPlayerHit - UpdatePosCallBack FAILED", NULL, LOGFILE_ONLY);
		return FALSE;
	}

	SendPositionMessage(pCMyApp->MyRRnetID, 
					    pCMyApp->m_vEyePt.x,
						pCMyApp->m_vEyePt.y,
						pCMyApp->m_vEyePt.z,
						pCMyApp->angy,
						pCMyApp->have_i_moved_flag );

	return TRUE;
}


//-----------------------------------------------------------------------------
// Name: DisplayNumberPlayersInGame()
// Desc: Displays the number of active players 
//-----------------------------------------------------------------------------
VOID DisplayNumberPlayersInGame( HWND hDlg )
{
    TCHAR strNumberPlayers[32];

    sprintf( strNumberPlayers, "%d", g_dwNumberOfActivePlayers );
    // Put this value somewhere are shown below
	//SetDlgItemText( hDlg, IDC_NUM_PLAYERS, strNumberPlayers );
}

void ReadDpInfoFromRegistry()
{

    RegCreateKeyEx( HKEY_CURRENT_USER, DPLAY_SAMPLE_KEY, 0, NULL,
                    REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, 
                    &g_hDPlaySampleRegKey, NULL );

    ReadRegKey( g_hDPlaySampleRegKey, "Player Name", 
                g_strLocalPlayerName, MAX_PLAYER_NAME, "" );
    ReadRegKey( g_hDPlaySampleRegKey, "Session Name", 
                g_strSessionName, MAX_SESSION_NAME, "" );
    ReadRegKey( g_hDPlaySampleRegKey, "Preferred Provider", 
                g_strPreferredProvider, MAX_SESSION_NAME, "" );
}


//-----------------------------------------------------------------------------
// Name: ReadRegKey()
// Desc: Read a registry key 
//-----------------------------------------------------------------------------
HRESULT ReadRegKey( HKEY hKey, TCHAR* strName, TCHAR* strValue, 
                    DWORD dwLength, TCHAR* strDefault )
{
	DWORD dwType;
	LONG bResult;

	bResult = RegQueryValueEx( hKey, strName, 0, &dwType, 
							 (LPBYTE) strValue, &dwLength );
	if ( bResult != ERROR_SUCCESS )
		strcpy( strValue, strDefault );

    return S_OK;
}

void WriteRegisteryInfo()
{
	WriteRegKey( g_hDPlaySampleRegKey, "Player Name", g_strLocalPlayerName );
    WriteRegKey( g_hDPlaySampleRegKey, "Session Name", g_strSessionName );
    WriteRegKey( g_hDPlaySampleRegKey, "Preferred Provider", g_strPreferredProvider );
}

//-----------------------------------------------------------------------------
// Name: WriteRegKey()
// Desc: Writes a registry key 
//-----------------------------------------------------------------------------
HRESULT WriteRegKey( HKEY hKey, TCHAR* strName, TCHAR* strValue )
{
	LONG bResult;

	bResult = RegSetValueEx( hKey, strName, 0, REG_SZ, 
							 (LPBYTE) strValue, strlen(strValue) + 1 );
	if ( bResult != ERROR_SUCCESS )
		return E_FAIL;

    return S_OK;
}

