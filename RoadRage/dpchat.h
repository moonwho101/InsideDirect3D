/*==========================================================================
 *
 *  Copyright (C) 1996-1997 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       dpchat.h
 *  Content:	Header file for dpchat.cpp
 *
 ***************************************************************************/

#ifndef __DPCHAT_H__
#define __DPCHAT_H__

#define IDIRECTPLAY2_OR_GREATER
#include <dplay.h>

// constants
const DWORD MAXPLAYERS = 10;			// max no. players in the session

const DWORD APPMSG_CHATSTRING	    = 0;
const DWORD APPMSG_POSITION			= 1;
const DWORD APPMSG_FIRE				= 2;
const DWORD APPMSG_WALK_OR_DRIVE    = 3;
const DWORD APPMSG_GESTURE			= 4;
const DWORD APPMSG_JUMP				= 5;
const DWORD APPMSG_CROUCH			= 6;
const DWORD APPMSG_CHANGE_WEAPON_TO	= 7;
const DWORD APPMSG_HIT				= 8;
const APPMSG_PLAYER_LIST			= 9;
		


// structure used to store DirectPlay information
typedef struct {
	LPDIRECTPLAY4A	lpDirectPlay4A;		// IDirectPlay4A interface pointer
	HANDLE			hPlayerEvent;		// player event to use
	DPID			dpidPlayer;			// ID of player created
	BOOL			bIsHost;			// TRUE if we are hosting the session
} DPLAYINFO, *LPDPLAYINFO;



// guid for this application
// {5BFDB060-06A4-11d0-9C4F-00A0C905425E}
DEFINE_GUID(DPCHAT_GUID, 0x5bfdb060, 0x6a4, 0x11d0, 0x9c, 0x4f, 0x0, 0xa0, 0xc9, 0x5, 0x42, 0x5e);


// prototypes
extern HRESULT	ConnectUsingLobby(LPDPLAYINFO lpDPInfo);
extern HRESULT	ConnectUsingDialog(HINSTANCE hInstance, LPDPLAYINFO lpDPInfo);
extern void		ErrorBox(LPSTR lpszErrorStr, HRESULT hr);

// prototypes
BOOL CALLBACK	ChatWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT			SetupConnection(HINSTANCE hInstance, LPDPLAYINFO lpDPInfo);
HRESULT			ShutdownConnection(LPDPLAYINFO lpDPInfo);
DWORD WINAPI	ReceiveThread(LPVOID lpThreadParameter);
HRESULT			ReceiveMessage(LPDPLAYINFO lpDPInfo);
void			HandleApplicationMessage(LPDPLAYINFO lpDPInfo, LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize,
										 DPID idFrom, DPID idTo);
void			HandleSystemMessage(LPDPLAYINFO lpDPInfo, LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize,
									DPID idFrom, DPID idTo);
HRESULT			GetChatPlayerName(LPDIRECTPLAY4A lpDirectPlay4A, DPID dpidPlayer,
								  LPDPNAME *lplpName);
HRESULT			SendChatMessage(HWND hWnd, LPDPLAYINFO lpDPInfo);
HRESULT			NewChatString(LPDIRECTPLAY4A lpDirectPlay4A, DPID dpidPlayer,
							  LPSTR lpszMsg, LPSTR *lplpszStr);
char *			GetDirectPlayErrStr(HRESULT hr);

HRESULT			SendPositionMessage(DWORD RRnetID, float x, float y, float z, float view_angle, LPDPLAYINFO lpDPInfo);
HRESULT			SendActionFlagMessage(HWND hWnd, BOOL action_flag, DWORD dwType, LPDPLAYINFO lpDPInfo);
HRESULT			SendActionWordMessage(int action_word, DWORD dwType, LPDPLAYINFO lpDPInfo);
HRESULT			SendPlayerInfoMessage(DWORD player_id, BYTE player_num, char *player_name, DWORD player_name_length,
					  BYTE num_players, DWORD dwType, LPDPLAYINFO lpDPInfo);

int GetPlayerNumber(DWORD dpid);


#endif // __DPCHAT_H__