
// Road Rage 1.1

// FILE : dpchat.cpp
// This file was originally part of the Microsoft DirectX5 SDK
// Edited by William Chin
// Edited with MSDEV C++ v6.0 using the DirectX 6.0 SDK
// Edited on:  3/7/99


#include <windows.h>
#include <windowsx.h>

#include "dpchat.h"
#include "d3dmain.hpp"
#include "resource.h"
#include "comm.h"
#include "sound.hpp"
#include "world.hpp"

#if defined(UNICODE) || defined(_UNICODE)
#error This app does not support UNICODE
#endif

// constants
			// message type for chat string
const UINT	WM_USER_ADDSTRING	= WM_USER+257;	// window message to add string to chat string list
const DWORD	MAXSTRLEN			= 200;			// max size of a temporary string

extern multiplay_flag;
extern have_i_moved_flag;
extern firing_gun_flag;
extern WORLD world;
extern DWORD MyRRnetID;
extern BOOL am_i_the_server;
extern DPLAYINFO DPInfo;

// structures

// message structure used to send a chat string to another player
typedef struct 
{
	DWORD	dwType;		// message type (APPMSG_CHATSTRING)
	char	szMsg[1];	// message string (variable length)
} MSG_CHATSTRING, *LPMSG_CHATSTRING;

typedef struct 
{
	DWORD dwType;	
	BYTE  player_number;
	BYTE  num_players;
	DWORD player_id;
	DWORD RRnetID;
	
} MSG_PLAYER_INFO, *LPMSG_PLAYER_INFO;

typedef struct 
{
	DWORD	dwType;	
	BOOL IsRunning;
	float	x_pos;	
	float	y_pos;
	float	z_pos;
	float	view_angle;
	DWORD RRnetID;
	
} MSG_POS, *LPMSG_POS;

typedef struct 
{
	DWORD	dwType;	
	int action_word;
	DWORD RRnetID;
	
} MSG_ACTIONWORDINFO, *LPMSG_ACTIONWORDINFO;

typedef struct 
{
	DWORD	dwType;	
	BOOL action_flag;
	DWORD RRnetID;
	
} MSG_ACTIONFLAGINFO, *LPMSG_ACTIONFLAGINFO;


// globals
HANDLE			ghReceiveThread = NULL;			// handle of receive thread
DWORD			gidReceiveThread = 0;			// id of receive thread
HANDLE			ghKillReceiveEvent = NULL;		// event used to kill receive thread
HWND			ghChatWnd = NULL;				// main chat window

static char buffer[256];
extern world_ptr wptr;
int num_packets_sent = 0;
int num_packets_received = 0;
int server_assigned_player_number = 0;


BOOL CALLBACK ChatWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LPDPLAYINFO	lpDPInfo;
	DWORD		dwTextLen;

	switch(uMsg)
	{
	case WM_INITDIALOG:
		// Save the connection info pointer
		lpDPInfo = (LPDPLAYINFO) lParam;

		// store global window
		ghChatWnd = hWnd;
		break;

	case WM_DESTROY:
		ghChatWnd = NULL;
		break;

	// this is a user-defined message used to add strings to the log window
	case WM_USER_ADDSTRING:
		// get length of text in log window
		dwTextLen = SendDlgItemMessage(hWnd, IDC_LOGEDIT, WM_GETTEXTLENGTH,
									   (WPARAM) 0, (LPARAM) 0);

		// put selection at end
		dwTextLen = SendDlgItemMessage(hWnd, IDC_LOGEDIT, EM_SETSEL,
									   (WPARAM) dwTextLen, (LPARAM) dwTextLen);

		// add string in lParam to log window
		SendDlgItemMessage(hWnd, IDC_LOGEDIT, EM_REPLACESEL,
							(WPARAM) FALSE, (LPARAM) lParam);
		GlobalFreePtr((LPVOID) lParam);
		break;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_SENDBUTTON:
			//SendChatMessage(hWnd, lpDPInfo);
			break;			

		case IDCANCEL:
			EndDialog(hWnd, FALSE);
			break;
		}
		break;
	}

	// Allow for default processing
	return FALSE;
}

HRESULT SetupConnection(HINSTANCE hInstance, LPDPLAYINFO lpDPInfo)
{
	HRESULT		hr;

	ZeroMemory(lpDPInfo, sizeof(DPLAYINFO));

	// create event used by DirectPlay to signal a message has arrived
	lpDPInfo->hPlayerEvent = CreateEvent(NULL,		// no security
										 FALSE,		// auto reset
										 FALSE,		// initial event reset
										 NULL);		// no name
	if (lpDPInfo->hPlayerEvent == NULL)
	{
		hr = DPERR_NOMEMORY;
		goto FAILURE;
	}

	// create event used to signal that the receive thread should exit
	ghKillReceiveEvent = CreateEvent(NULL,		// no security
									 FALSE,		// auto reset
									 FALSE,		// initial event reset
									 NULL);		// no name
	if (ghKillReceiveEvent == NULL)
	{
		hr = DPERR_NOMEMORY;
		goto FAILURE;
	}

	// create thread to receive player messages
	ghReceiveThread = CreateThread(NULL,			// default security
								   0,				// default stack size
								   ReceiveThread,	// pointer to thread routine
								   lpDPInfo,		// argument for thread
								   0,				// start it right away
								   &gidReceiveThread);
	if (ghReceiveThread == NULL)
	{
		hr = DPERR_NOMEMORY;
		goto FAILURE;
	}

	// try to connect using the lobby
	hr = ConnectUsingLobby(lpDPInfo);
	if FAILED(hr)
	{
		// if the error returned is DPERR_NOTLOBBIED, that means we
		// were not launched by a lobby and we should ask the user for
		// connection settings. If any other error is returned it means
		// we were launched by a lobby but there was an error making the
		// connection.

		if (hr != DPERR_NOTLOBBIED)
			ErrorBox("Could not connect using lobby because of error %s", hr);

		// if there is no lobby connection, ask the user for settings
		hr = ConnectUsingDialog(hInstance, lpDPInfo);
		if FAILED(hr)
			goto FAILURE;
	}

	return (DP_OK);	

FAILURE:
	ShutdownConnection(lpDPInfo);

	return (hr);
}

HRESULT ShutdownConnection(LPDPLAYINFO lpDPInfo)
{
	if (ghReceiveThread)
	{
		// wake up receive thread and wait for it to quit
		SetEvent(ghKillReceiveEvent);
		WaitForSingleObject(ghReceiveThread, INFINITE);

		CloseHandle(ghReceiveThread);
		ghReceiveThread = NULL;
	}

	if (ghKillReceiveEvent)
	{
		CloseHandle(ghKillReceiveEvent);
		ghKillReceiveEvent = NULL;
	}

	if (lpDPInfo->lpDirectPlay4A)
	{
		if (lpDPInfo->dpidPlayer)
		{
			lpDPInfo->lpDirectPlay4A->DestroyPlayer(lpDPInfo->dpidPlayer);
			lpDPInfo->dpidPlayer = 0;
		}
		lpDPInfo->lpDirectPlay4A->Close();
		lpDPInfo->lpDirectPlay4A->Release();
		lpDPInfo->lpDirectPlay4A = NULL;
	}

	if (lpDPInfo->hPlayerEvent)
	{
		CloseHandle(lpDPInfo->hPlayerEvent);
		lpDPInfo->hPlayerEvent = NULL;
	}

	return (DP_OK);
}

DWORD WINAPI ReceiveThread(LPVOID lpThreadParameter)
{
	LPDPLAYINFO	lpDPInfo = (LPDPLAYINFO) lpThreadParameter;
	HANDLE		eventHandles[2];

	eventHandles[0] = lpDPInfo->hPlayerEvent;
	eventHandles[1] = ghKillReceiveEvent;

	// loop waiting for player events. If the kill event is signaled
	// the thread will exit
	while (WaitForMultipleObjects(2, eventHandles, FALSE, INFINITE) == WAIT_OBJECT_0)
	{
		// receive any messages in the queue
		ReceiveMessage(lpDPInfo);
	}

	ExitThread(0);

	return (0);
}

HRESULT ReceiveMessage(LPDPLAYINFO lpDPInfo)
{
	DPID				idFrom, idTo;
	LPVOID				lpvMsgBuffer;
	DWORD				dwMsgBufferSize;
	HRESULT				hr;

	lpvMsgBuffer = NULL;
	dwMsgBufferSize = 0;

	// loop to read all messages in queue
	do
	{
		// loop until a single message is successfully read
		do
		{
			// read messages from any player, including system player
			idFrom = 0;
			idTo = 0;

			hr = DPlayReceive(lpDPInfo->lpDirectPlay4A, &idFrom, &idTo, DPRECEIVE_ALL, lpvMsgBuffer, &dwMsgBufferSize);

			// not enough room, so resize buffer
			if (hr == DPERR_BUFFERTOOSMALL)
			{
				if (lpvMsgBuffer)
					GlobalFreePtr(lpvMsgBuffer);
				lpvMsgBuffer = GlobalAllocPtr(GHND, dwMsgBufferSize);
				if (lpvMsgBuffer == NULL)
					hr = DPERR_OUTOFMEMORY;
			}
		} while (hr == DPERR_BUFFERTOOSMALL);

		if ((SUCCEEDED(hr)) &&							// successfully read a message
			(dwMsgBufferSize >= sizeof(DPMSG_GENERIC)))	// and it is big enough
		{
			// check for system message
			if (idFrom == DPID_SYSMSG)
			{
				HandleSystemMessage(lpDPInfo, (LPDPMSG_GENERIC) lpvMsgBuffer,
									dwMsgBufferSize, idFrom, idTo);
			
			}
			else
			{
				HandleApplicationMessage(lpDPInfo, (LPDPMSG_GENERIC) lpvMsgBuffer,
										 dwMsgBufferSize, idFrom, idTo);
				
			}
		}
	} while (SUCCEEDED(hr));

	// free any memory we created
	if (lpvMsgBuffer)
		GlobalFreePtr(lpvMsgBuffer);

	return (DP_OK);
}

int GetPlayerNumber(DWORD dpid)
{
	int i;

	for(i = 0; i < wptr->num_players; i++)
	{
		if(wptr->player_list[i].RRnetID == dpid)
			return i;	
	}
	PrintMessage(NULL, "GetPlayerNumber: Failed to get player ID", NULL, LOGFILE_ONLY);
	return 0;
}


void HandleApplicationMessage(LPDPLAYINFO lpDPInfo, LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize,
							  DPID idFrom, DPID idTo)
{
	LPSTR	lpszStr = NULL;
	LPMSG_POS lpPos = NULL;
	LPMSG_ACTIONWORDINFO lpActionWord = NULL;
	LPMSG_ACTIONFLAGINFO lpActionFlag = NULL;
	LPMSG_PLAYER_INFO    lpPlayerInfo = NULL;
	HRESULT	hr;
	int player_num = 0;
	int mod_id;
	int curr_seq;
	int curr_wep;
	int start_frame;
	char buffer[256];
	char buffer2[256];

	FILE *fp;
	
	switch (lpMsg->dwType)
	{	
		case APPMSG_PLAYER_LIST:
			lpPlayerInfo = (LPMSG_PLAYER_INFO)lpMsg;	
			wptr->num_players = lpPlayerInfo->num_players;
			wptr->player_list[lpPlayerInfo->player_number].RRnetID = lpPlayerInfo->player_id;
			
			fp = fopen("rrlogfile.txt","a");
	
			fprintf( fp, "\nAPPMSG_PLAYER_LIST-----\n");
			fprintf( fp, "player_id     : %d\n", lpPlayerInfo->player_id);
			fprintf( fp, "player_number : %d\n", lpPlayerInfo->player_number);
			fprintf( fp, "num_players   : %d\n", lpPlayerInfo->num_players);
			fprintf( fp, "-------------------\n\n");
	
			fclose(fp);
			
			break;

		case APPMSG_GESTURE:	
			lpActionWord = (LPMSG_ACTIONWORDINFO)lpMsg;		
			curr_seq = lpActionWord->action_word;
			player_num = GetPlayerNumber(lpActionWord->RRnetID);

			wptr->player_list[player_num].current_sequence = curr_seq;		
			mod_id	    = wptr->player_list[player_num].model_id;
			start_frame = wptr->pmdata[mod_id].sequence_start_frame[curr_seq];
			wptr->player_list[player_num].current_frame = start_frame;
			num_packets_received++;

			fp = fopen("rrlogfile.txt","a");
	
			fprintf( fp, "\nAPPMSG_GESTURE-----\n");
			fprintf( fp, "player_num : %d\n",player_num);
			fprintf( fp, "Player's name  : %s\n", wptr->player_list[player_num].name);
			fprintf( fp, "Player's id    : %d\n", lpActionWord->RRnetID);
			fprintf( fp, "Player's idFrom: %d\n", idFrom);
			fprintf( fp, "-------------------\n\n");
	
			fclose(fp);

			break;

		case APPMSG_CHANGE_WEAPON_TO:
			lpActionWord = (LPMSG_ACTIONWORDINFO)lpMsg;
			player_num = GetPlayerNumber(lpActionWord->RRnetID);
			wptr->player_list[player_num].current_weapon     = lpActionWord->action_word;		
			break;

		case APPMSG_HIT:
			break;

		case APPMSG_FIRE:
			lpActionWord = (LPMSG_ACTIONWORDINFO)lpMsg;
			player_num = GetPlayerNumber(lpActionWord->RRnetID);
			curr_wep     = lpActionWord->action_word;	
			//wptr->player_list[player_num].current_weapon = curr_wep;
				
			if(wptr->player_list[player_num].current_sequence != 2)
			{
				curr_seq = 2; // run	
				wptr->player_list[player_num].current_sequence = curr_seq;	
				mod_id	    = wptr->player_list[player_num].model_id;
				start_frame = wptr->pmdata[mod_id].sequence_start_frame[curr_seq];
				wptr->player_list[player_num].current_frame = start_frame;
			}
			StopDSound(wptr->your_gun[curr_wep].sound_id);
			PlayDSound(wptr->your_gun[curr_wep].sound_id, NULL);
			num_packets_received++;
			break;
		
		case APPMSG_WALK_OR_DRIVE:
			lpActionFlag = (LPMSG_ACTIONFLAGINFO)lpMsg;
			player_num = GetPlayerNumber(lpActionFlag->RRnetID);
			wptr->player_list[player_num].walk_or_drive_mode = lpActionFlag->action_flag;
			break;	

		case APPMSG_JUMP:
			break;
			
		case APPMSG_CROUCH:
			break;

		case APPMSG_POSITION:
			
			lpPos = (LPMSG_POS)lpMsg;	
			player_num = GetPlayerNumber(lpPos->RRnetID);

			itoa((int)lpPos->x_pos,buffer,10);
			strcpy(buffer2, buffer);
			strcat(buffer2, " ");
			itoa((int)lpPos->y_pos,buffer,10);
			strcat(buffer2, buffer);
			strcat(buffer2, " ");
			itoa((int)lpPos->z_pos,buffer,10);
			strcat(buffer2, buffer);

			wptr->player_list[player_num].x = lpPos->x_pos;	
			wptr->player_list[player_num].y = lpPos->y_pos+12;
			wptr->player_list[player_num].z = lpPos->z_pos;
			wptr->player_list[player_num].rot_angle = lpPos->view_angle;
			
			itoa(player_num, buffer,10);
			//PrintMessage(NULL, "APPMSG_POSITION ", buffer, LOGFILE_ONLY);
			
			if(lpPos->IsRunning == TRUE)
			{	
				if(wptr->player_list[player_num].current_sequence != 1)
				{
					curr_seq = 1; // run	
					wptr->player_list[player_num].current_sequence = curr_seq;	
					mod_id	    = wptr->player_list[player_num].model_id;
					start_frame = wptr->pmdata[mod_id].sequence_start_frame[curr_seq];
					wptr->player_list[player_num].current_frame = start_frame;
				}
			}
			
			num_packets_received++;
			break;

		case APPMSG_CHATSTRING:
			LPMSG_CHATSTRING   lp = (LPMSG_CHATSTRING) lpMsg;

			// create string to display
			hr = NewChatString(lpDPInfo->lpDirectPlay4A, idFrom, lp->szMsg, &lpszStr);
			if FAILED(hr)
				break;
			// post string to chat window
			if (lpszStr)
			{
				DDTextOut((char *)lpszStr);
				//MessageBeep(0);
			}
			break;
	}
	
	
	
}

void HandleSystemMessage(LPDPLAYINFO lpDPInfo, LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize,
						 DPID idFrom, DPID idTo)
{
	LPSTR		lpszStr = NULL;
	FILE *fp;
	DWORD *temp;
	int i;

	// The body of each case is there so you can set a breakpoint and examine
	// the contents of the message received.
	switch (lpMsg->dwType)
	{
		case DPSYS_CREATEPLAYERORGROUP:
		{
			LPDPMSG_CREATEPLAYERORGROUP 	lp = (LPDPMSG_CREATEPLAYERORGROUP) lpMsg;
			LPSTR							lpszPlayerName;
			LPSTR							szDisplayFormat = "%s has joined";
			
			// get pointer to player name
			if (lp->dpnName.lpszShortNameA)
				lpszPlayerName = lp->dpnName.lpszShortNameA;
			else
				lpszPlayerName = "unknown";

			// allocate space for string
			lpszStr = (LPSTR) GlobalAllocPtr(GHND, lstrlen(szDisplayFormat) +
												   lstrlen(lpszPlayerName) + 1);
			if (lpszStr == NULL)
				break;

			// build string
			wsprintf(lpszStr, szDisplayFormat, lpszPlayerName);

			temp =  (DWORD *)lp->lpData;
			
			world.player_list[world.num_players].x = 500;
			world.player_list[world.num_players].y = 12;
			world.player_list[world.num_players].z = 500;
			world.player_list[world.num_players].rot_angle = 0;
			world.player_list[world.num_players].model_id = 0;
			world.player_list[world.num_players].skin_tex_id = 0;
			world.player_list[world.num_players].current_weapon = 0;
			world.player_list[world.num_players].current_car = 0;
			world.player_list[world.num_players].current_frame = 0;
			world.player_list[world.num_players].current_sequence = 0;
			world.player_list[world.num_players].walk_or_drive_mode = 0;
			strcpy(world.player_list[world.num_players].name, lpszPlayerName);
			world.player_list[world.num_players].RRnetID = lp->dpId;
			world.player_list[world.num_players].dpid = lp->dpId;
			world.num_players++;
		
			fp = fopen("rrlogfile.txt","a");
	
			
			for(i = 0; i < wptr->num_players; i++)
			{
				fprintf( fp, "\nDPSYS_CREATEPLAYERORGROUP SENDING\n");
				fprintf( fp, "Players Number : %d\n",i);
				fprintf( fp, "Player's name  : %s\n", world.player_list[i].name);
				fprintf( fp, "Player's dpid  : %d\n", world.player_list[i].dpid);
				fprintf( fp, "Server's id    : %d\n", MyRRnetID);
				fprintf( fp, "---------------\n\n");
	

				SendPlayerInfoMessage(world.player_list[i].dpid,
									  i,
									  world.player_list[i].name,
									  strlen(world.player_list[i].name),		
									  world.num_players, 
								      APPMSG_PLAYER_LIST, 
								      &DPInfo);
			}
			fclose(fp);
			
		}
		break;

		case DPSYS_DESTROYPLAYERORGROUP:
		{
			LPDPMSG_DESTROYPLAYERORGROUP	lp = (LPDPMSG_DESTROYPLAYERORGROUP)lpMsg;
			LPSTR							lpszPlayerName;
			LPSTR							szDisplayFormat = "\"%s\" has left\r\n";
			
			// get pointer to player name
			if (lp->dpnName.lpszShortNameA)
				lpszPlayerName = lp->dpnName.lpszShortNameA;
			else
				lpszPlayerName = "unknown";

			// allocate space for string
			lpszStr = (LPSTR) GlobalAllocPtr(GHND, lstrlen(szDisplayFormat) +
												   lstrlen(lpszPlayerName) + 1);
			if (lpszStr == NULL)
				break;

			// build string
			wsprintf(lpszStr, szDisplayFormat, lpszPlayerName);
		}
		break;

		case DPSYS_ADDPLAYERTOGROUP:
		{
			LPDPMSG_ADDPLAYERTOGROUP lp = (LPDPMSG_ADDPLAYERTOGROUP)lpMsg;
		}
		break;

		case DPSYS_DELETEPLAYERFROMGROUP:
		{
			LPDPMSG_DELETEPLAYERFROMGROUP lp = (LPDPMSG_DELETEPLAYERFROMGROUP)lpMsg;
		}
		break;

		case DPSYS_SESSIONLOST:
		{
			LPDPMSG_SESSIONLOST lp = (LPDPMSG_SESSIONLOST)lpMsg;
		}
		break;

		case DPSYS_HOST:
		{
			LPDPMSG_HOST	lp = (LPDPMSG_HOST)lpMsg;
			LPSTR			szDisplayFormat = "You have become the host\r\n";

			// allocate space for string
			lpszStr = (LPSTR) GlobalAllocPtr(GHND, lstrlen(szDisplayFormat) + 1);
			if (lpszStr == NULL)
				break;

			// build string
			lstrcpy(lpszStr, szDisplayFormat);

			// we are now the host
			lpDPInfo->bIsHost = TRUE;
		}
		break;

		case DPSYS_SETPLAYERORGROUPDATA:
		{
			LPDPMSG_SETPLAYERORGROUPDATA lp = (LPDPMSG_SETPLAYERORGROUPDATA)lpMsg;
		}
		break;

		case DPSYS_SETPLAYERORGROUPNAME:
		{
			LPDPMSG_SETPLAYERORGROUPNAME lp = (LPDPMSG_SETPLAYERORGROUPNAME)lpMsg;
		}
		break;
	}

	// post string to chat window
	if (lpszStr)
	{	
		DDTextOut(lpszStr);
	}
}

HRESULT GetChatPlayerName(LPDIRECTPLAY4A lpDirectPlay4A, DPID dpidPlayer,
						  LPDPNAME *lplpName)
{
	LPDPNAME	lpName = NULL;
	DWORD		dwNameSize;
	HRESULT		hr;

	// get size of player name data
	hr = lpDirectPlay4A->GetPlayerName(dpidPlayer, NULL, &dwNameSize);
	if (hr != DPERR_BUFFERTOOSMALL)
		goto FAILURE;

	// make room for it
	lpName = (LPDPNAME) GlobalAllocPtr(GHND, dwNameSize);
	if (lpName == NULL)
	{
		hr = DPERR_OUTOFMEMORY;
		goto FAILURE;
	}

	// get player name data
	hr = lpDirectPlay4A->GetPlayerName(dpidPlayer, lpName, &dwNameSize);
	if FAILED(hr)
		goto FAILURE;

	// return pointer to name structure
	*lplpName = lpName;

	return (DP_OK);

FAILURE:
	if (lpName)
		GlobalFreePtr(lpName);

	return (hr);
}

HRESULT SendChatMessage(HWND hWnd, LPDPLAYINFO lpDPInfo)
{
	LPSTR				lpszChatStr = NULL;
	LPSTR				lpszStr = NULL;
	LPMSG_CHATSTRING	lpChatMessage = NULL;
	DWORD				dwChatMessageSize;
	HRESULT				hr;

	
	strcpy(buffer, "Test Message");
	lpszChatStr = (char *)&buffer;

	// create string to display this text
	hr = NewChatString(lpDPInfo->lpDirectPlay4A, lpDPInfo->dpidPlayer, lpszChatStr, &lpszStr);
	if FAILED(hr)
		goto FAILURE;

	
	lpszStr = NULL;						// set to NULL so we don't delete it below

	// create space for message plus string (string length included in message header)
	dwChatMessageSize = sizeof(MSG_CHATSTRING) + lstrlen(lpszChatStr);
	lpChatMessage = (LPMSG_CHATSTRING) GlobalAllocPtr(GHND, dwChatMessageSize);
	if (lpChatMessage == NULL)
	{
		hr = DPERR_OUTOFMEMORY;
		goto FAILURE;
	}

	// build message	
	lpChatMessage->dwType = APPMSG_CHATSTRING;
	lstrcpy(lpChatMessage->szMsg, lpszChatStr);
	
	// send this string to all other players
	hr = lpDPInfo->lpDirectPlay4A->Send(lpDPInfo->dpidPlayer, DPID_ALLPLAYERS,
			DPSEND_GUARANTEED, lpChatMessage, dwChatMessageSize);
	if FAILED(hr)
		goto FAILURE;

FAILURE:
	if (lpszChatStr)
		GlobalFreePtr(lpszChatStr);

	if (lpszStr)
		GlobalFreePtr(lpszStr);

	if (lpChatMessage)
		GlobalFreePtr(lpChatMessage);

	return (hr);
}

HRESULT NewChatString(LPDIRECTPLAY4A lpDirectPlay4A, DPID dpidPlayer,
					  LPSTR lpszMsg, LPSTR *lplpszStr)
{
	LPDPNAME	lpName = NULL;
	LPSTR		lpszStr = NULL;
	LPSTR		lpszPlayerName;
	LPSTR		szDisplayFormat = "%s>\t%s\r\n";
	HRESULT		hr;
	
	// get name of player
	hr = GetChatPlayerName(lpDirectPlay4A, dpidPlayer, &lpName);
	if FAILED(hr)
		goto FAILURE;

	if (lpName->lpszShortNameA)
		lpszPlayerName = lpName->lpszShortNameA;
	else
		lpszPlayerName = "unknown";

	// allocate space for display string
	lpszStr = (LPSTR) GlobalAllocPtr(GHND, lstrlen(szDisplayFormat) +
										   lstrlen(lpszPlayerName) +
										   lstrlen(lpszMsg) + 1);
	if (lpszStr == NULL)
	{
		hr = DPERR_OUTOFMEMORY;
		goto FAILURE;
	}

	// build string
	wsprintf(lpszStr, szDisplayFormat, lpszPlayerName, lpszMsg);

	*lplpszStr = lpszStr;
	lpszStr = NULL;

FAILURE:
	if (lpszStr)
		GlobalFreePtr(lpszStr);
	
	if (lpName)
		GlobalFreePtr(lpName);

	return (hr);
}

HRESULT SendPositionMessage(DWORD RRnetID, float x, float y, float z, float view_angle, LPDPLAYINFO lpDPInfo)
{
	LPMSG_POS		lpPosMessage = NULL;
	DWORD			dwPosMessageSize = NULL;
	HRESULT			hr;
	unsigned long sendto;


	if(multiplay_flag == FALSE)
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
	lpPosMessage->dwType = APPMSG_POSITION;
	lpPosMessage->RRnetID = RRnetID;
	lpPosMessage->x_pos = x;
	lpPosMessage->y_pos = 0;
	lpPosMessage->z_pos = z;
	lpPosMessage->view_angle = view_angle;
	lpPosMessage->IsRunning = have_i_moved_flag;

	if(am_i_the_server == TRUE)
		sendto = DPID_ALLPLAYERS;
	else
		sendto = DPID_SERVERPLAYER;

	hr = DPlaySend(lpDPInfo->lpDirectPlay4A, lpDPInfo->dpidPlayer, sendto,
			NULL, lpPosMessage, dwPosMessageSize);

	if(SUCCEEDED(hr))
		num_packets_sent++;

	if FAILED(hr)
		goto FAILURE;

FAILURE:

	if (lpPosMessage)
		GlobalFreePtr(lpPosMessage);

	return (hr);
}



HRESULT SendPlayerInfoMessage(DWORD player_id, BYTE player_num, char *player_name, DWORD player_name_length,
					  BYTE num_players, DWORD dwType, LPDPLAYINFO lpDPInfo)
{
	LPMSG_PLAYER_INFO lpPIMessage = NULL;
	DWORD dwPIMessageSize = NULL;
	HRESULT	hr;
	unsigned long sendto;
	FILE *fp;


	if(multiplay_flag == FALSE)
		return NULL;
	
	// create space for message plus 
	dwPIMessageSize = sizeof(MSG_PLAYER_INFO);
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
	lpPIMessage->player_id = player_id;
	lpPIMessage->RRnetID = MyRRnetID;
		
	fp = fopen("rrlogfile.txt","a");
	
	fprintf( fp, "\nAPPMSG_PLAYER_LIST-----\n");
	fprintf( fp, "player_id     : %d\n", player_id);
	fprintf( fp, "player_number : %d\n", player_num);
	fprintf( fp, "num_players   : %d\n", num_players);
	fprintf( fp, "-------------------\n\n");
	
	fclose(fp);
			
	if(am_i_the_server == TRUE)
		sendto = DPID_ALLPLAYERS;
	else
		sendto = DPID_SERVERPLAYER;

	// send this string to all other players if your the server

	hr = DPlaySend(lpDPInfo->lpDirectPlay4A, lpDPInfo->dpidPlayer, sendto,
			NULL, lpPIMessage, dwPIMessageSize);

	if(SUCCEEDED(hr))
		num_packets_sent++;

	if FAILED(hr)
		goto FAILURE;

FAILURE:

	if (lpPIMessage)
		GlobalFreePtr(lpPIMessage);

	return (hr);
}


HRESULT SendActionFlagMessage(HWND hWnd, BOOL action_flag, DWORD dwType, LPDPLAYINFO lpDPInfo)
{
	LPMSG_ACTIONFLAGINFO lpAFMessage = NULL;
	DWORD dwAFMessageSize = NULL;
	HRESULT	hr;
	unsigned long sendto;

	if(multiplay_flag == FALSE)
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
	lpAFMessage->RRnetID = MyRRnetID;
	lpAFMessage->action_flag = action_flag;
	
	if(am_i_the_server == TRUE)
		sendto = DPID_ALLPLAYERS;
	else
		sendto = DPID_SERVERPLAYER;

	// send this string to all other players if your the server

	hr = DPlaySend(lpDPInfo->lpDirectPlay4A, lpDPInfo->dpidPlayer, sendto,
			NULL, lpAFMessage, dwAFMessageSize);

	if(SUCCEEDED(hr))
		num_packets_sent++;

	if FAILED(hr)
		goto FAILURE;

FAILURE:

	if (lpAFMessage)
		GlobalFreePtr(lpAFMessage);

	return (hr);
}

HRESULT SendActionWordMessage(int action_word, DWORD dwType, LPDPLAYINFO lpDPInfo)
{
	LPMSG_ACTIONWORDINFO lpAWMessage = NULL;
	DWORD dwAWMessageSize = 0;
	HRESULT	hr;
	FILE *fp;

	if(multiplay_flag == FALSE)
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
	lpAWMessage->RRnetID = MyRRnetID;
	lpAWMessage->action_word = action_word;
	
	fp = fopen("rrlogfile.txt","a");
	
	fprintf( fp, "\nSendActionWordMessage-----\n");
	fprintf( fp, "Player's id    : %d\n", MyRRnetID);
	fprintf( fp, "-------------------\n\n");
	
	fclose(fp);

	//itoa((int)action_word, buffer, 10);
	//DDTextOut((char *)buffer);

	// send this string to all other players

	hr = DPlaySend(lpDPInfo->lpDirectPlay4A, lpDPInfo->dpidPlayer, DPID_SERVERPLAYER,
			NULL, lpAWMessage, dwAWMessageSize);

	if(SUCCEEDED(hr))
		num_packets_sent++;

	if FAILED(hr)
		goto FAILURE;

FAILURE:

	if (lpAWMessage)
		GlobalFreePtr(lpAWMessage);

	return (hr);
}



void ErrorBox(LPSTR lpszErrorStr, HRESULT hr)
{
	char	szStr[MAXSTRLEN];

	wsprintf(szStr, lpszErrorStr, GetDirectPlayErrStr(hr));

	MessageBox(NULL, szStr, "Error", MB_OK);
}

char * GetDirectPlayErrStr(HRESULT hr)
{
	static char		szTempStr[12];

	switch (hr)
	{
	case DP_OK: return ("DP_OK");
	case DPERR_ALREADYINITIALIZED: return ("DPERR_ALREADYINITIALIZED");
	case DPERR_ACCESSDENIED: return ("DPERR_ACCESSDENIED");
	case DPERR_ACTIVEPLAYERS: return ("DPERR_ACTIVEPLAYERS");
	case DPERR_BUFFERTOOSMALL: return ("DPERR_BUFFERTOOSMALL");
	case DPERR_CANTADDPLAYER: return ("DPERR_CANTADDPLAYER");
	case DPERR_CANTCREATEGROUP: return ("DPERR_CANTCREATEGROUP");
	case DPERR_CANTCREATEPLAYER: return ("DPERR_CANTCREATEPLAYER");
	case DPERR_CANTCREATESESSION: return ("DPERR_CANTCREATESESSION");
	case DPERR_CAPSNOTAVAILABLEYET: return ("DPERR_CAPSNOTAVAILABLEYET");
	case DPERR_EXCEPTION: return ("DPERR_EXCEPTION");
	case DPERR_GENERIC: return ("DPERR_GENERIC");
	case DPERR_INVALIDFLAGS: return ("DPERR_INVALIDFLAGS");
	case DPERR_INVALIDOBJECT: return ("DPERR_INVALIDOBJECT");
//	case DPERR_INVALIDPARAM: return ("DPERR_INVALIDPARAM");	 dup value
	case DPERR_INVALIDPARAMS: return ("DPERR_INVALIDPARAMS");
	case DPERR_INVALIDPLAYER: return ("DPERR_INVALIDPLAYER");
	case DPERR_INVALIDGROUP: return ("DPERR_INVALIDGROUP");
	case DPERR_NOCAPS: return ("DPERR_NOCAPS");
	case DPERR_NOCONNECTION: return ("DPERR_NOCONNECTION");
//	case DPERR_NOMEMORY: return ("DPERR_NOMEMORY");		dup value
	case DPERR_OUTOFMEMORY: return ("DPERR_OUTOFMEMORY");
	case DPERR_NOMESSAGES: return ("DPERR_NOMESSAGES");
	case DPERR_NONAMESERVERFOUND: return ("DPERR_NONAMESERVERFOUND");
	case DPERR_NOPLAYERS: return ("DPERR_NOPLAYERS");
	case DPERR_NOSESSIONS: return ("DPERR_NOSESSIONS");
	case DPERR_PENDING: return ("DPERR_PENDING");
	case DPERR_SENDTOOBIG: return ("DPERR_SENDTOOBIG");
	case DPERR_TIMEOUT: return ("DPERR_TIMEOUT");
	case DPERR_UNAVAILABLE: return ("DPERR_UNAVAILABLE");
	case DPERR_UNSUPPORTED: return ("DPERR_UNSUPPORTED");
	case DPERR_BUSY: return ("DPERR_BUSY");
	case DPERR_USERCANCEL: return ("DPERR_USERCANCEL");
	case DPERR_NOINTERFACE: return ("DPERR_NOINTERFACE");
	case DPERR_CANNOTCREATESERVER: return ("DPERR_CANNOTCREATESERVER");
	case DPERR_PLAYERLOST: return ("DPERR_PLAYERLOST");
	case DPERR_SESSIONLOST: return ("DPERR_SESSIONLOST");
	case DPERR_UNINITIALIZED: return ("DPERR_UNINITIALIZED");
	case DPERR_NONEWPLAYERS: return ("DPERR_NONEWPLAYERS");
	case DPERR_INVALIDPASSWORD: return ("DPERR_INVALIDPASSWORD");
	case DPERR_CONNECTING: return ("DPERR_CONNECTING");
	case DPERR_CONNECTIONLOST: return ("DPERR_CONNECTIONLOST");
	case DPERR_UNKNOWNMESSAGE: return ("DPERR_UNKNOWNMESSAGE");
	case DPERR_CANCELFAILED: return ("DPERR_CANCELFAILED");
	case DPERR_INVALIDPRIORITY: return ("DPERR_INVALIDPRIORITY");
	case DPERR_NOTHANDLED: return ("DPERR_NOTHANDLED");
	case DPERR_CANCELLED: return ("DPERR_CANCELLED");
	case DPERR_ABORTED: return ("DPERR_ABORTED");
	case DPERR_BUFFERTOOLARGE: return ("DPERR_BUFFERTOOLARGE");
	case DPERR_CANTCREATEPROCESS: return ("DPERR_CANTCREATEPROCESS");
	case DPERR_APPNOTSTARTED: return ("DPERR_APPNOTSTARTED");
	case DPERR_INVALIDINTERFACE: return ("DPERR_INVALIDINTERFACE");
	case DPERR_NOSERVICEPROVIDER: return ("DPERR_NOSERVICEPROVIDER");
	case DPERR_UNKNOWNAPPLICATION: return ("DPERR_UNKNOWNAPPLICATION");
	case DPERR_NOTLOBBIED: return ("DPERR_NOTLOBBIED");
	case DPERR_SERVICEPROVIDERLOADED: return ("DPERR_SERVICEPROVIDERLOADED");
	case DPERR_ALREADYREGISTERED: return ("DPERR_ALREADYREGISTERED");
	case DPERR_NOTREGISTERED: return ("DPERR_NOTREGISTERED");
	case DPERR_AUTHENTICATIONFAILED: return ("DPERR_AUTHENTICATIONFAILED");
	case DPERR_CANTLOADSSPI: return ("DPERR_CANTLOADSSPI");
	case DPERR_ENCRYPTIONFAILED: return ("DPERR_ENCRYPTIONFAILED");
	case DPERR_SIGNFAILED: return ("DPERR_SIGNFAILED");
	case DPERR_CANTLOADSECURITYPACKAGE: return ("DPERR_CANTLOADSECURITYPACKAGE");
	case DPERR_ENCRYPTIONNOTSUPPORTED: return ("DPERR_ENCRYPTIONNOTSUPPORTED");
	case DPERR_CANTLOADCAPI: return ("DPERR_CANTLOADCAPI");
	case DPERR_NOTLOGGEDIN: return ("DPERR_NOTLOGGEDIN");
	case DPERR_LOGONDENIED: return ("DPERR_LOGONDENIED");
	}

	// For errors not in the list, return HRESULT string
	wsprintf(szTempStr, "0x%08X", hr);
	return (szTempStr);
}

