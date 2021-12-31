
#ifndef __DP_MESSAGES_H__
#define __DP_MESSAGES_H__

#include <dplobby.h>
#include <dplay.h>

const DWORD MAXPLAYERS = 10;			// max no. players in the session

const DWORD APPMSG_CHATSTRING	    = 10;
const DWORD APPMSG_POSITION			= 1;
const DWORD APPMSG_FIRE				= 2;
const DWORD APPMSG_WALK_OR_DRIVE    = 3;
const DWORD APPMSG_GESTURE			= 4;
const DWORD APPMSG_JUMP				= 5;
const DWORD APPMSG_CROUCH			= 6;
const DWORD APPMSG_CHANGE_WEAPON_TO	= 7;
const DWORD APPMSG_HIT				= 8;
const DWORD APPMSG_PLAYER_LIST	    = 9;
const DWORD APPMSG_PING_TO_CLIENT	= 11;
const DWORD APPMSG_PING_TO_SERVER	= 12;
const DWORD APPMSG_PING_TO_ALL		= 13;
const DWORD APPMSG_RESPAWN			= 14;


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
	DWORD RRnetID;
	char	name[1];
	
} MSG_PLAYER_INFO, *LPMSG_PLAYER_INFO;

typedef struct 
{
	DWORD	dwType;	
	BOOL IsRunning;
	float	x_pos;	
	float	y_pos;
	float	z_pos;
	WORD	view_angle;
	//DWORD RRnetID;
	
} MSG_POS, *LPMSG_POS;

typedef struct 
{
	DWORD dwType;	
	WORD  action_word;
	DWORD RRnetID;
	
} MSG_ACTIONWORDINFO, *LPMSG_ACTIONWORDINFO;

typedef struct 
{
	DWORD dwType;	
	DWORD action_dword;
	DWORD RRnetID;
	
} MSG_ACTIONDWORDINFO, *LPMSG_ACTIONDWORDINFO;

typedef struct 
{
	DWORD dwType;	
	float action_float;
	DWORD RRnetID;
	
} MSG_ACTIONFLOATINFO, *LPMSG_ACTIONFLOATINFO;

typedef struct 
{
	DWORD dwType;	
	BOOL  action_flag;
	DWORD RRnetID;
	
} MSG_ACTIONFLAGINFO, *LPMSG_ACTIONFLAGINFO;

typedef struct 
{
	DWORD dwType;	
	DWORD RRnetID;
	
} MSG_ACTIONNOINFO, *LPMSG_ACTIONNOINFO;

struct GAMEMSG_GENERIC
{
    DWORD dwType;
};


HRESULT HandleAppMessages( HWND hDlg, GAMEMSG_GENERIC* pMsg, DWORD dwMsgSize, 
                           DPID idFrom, DPID idTo );

HRESULT HandleSystemMessages( HWND hDlg, DPMSG_GENERIC* pMsg, DWORD dwMsgSize, 
                              DPID idFrom, DPID idTo );

HRESULT SendPositionMessage  (DWORD RRnetID, float x, float y, float z, float view_angle, BOOL IsRunning);
HRESULT SendActionFlagMessage(DWORD RRnetID, BOOL action_flag, DWORD dwType);
HRESULT SendActionWordMessage(DWORD RRnetID, int action_word, DWORD dwType);
HRESULT SendChatMessage(DWORD RRnetID, char *pChatStr);
HRESULT SendPlayerInfoMessage(DWORD player_id, BYTE player_num, 
				char *player_name, BYTE num_players, DWORD dwType);

HRESULT SendActionDWordMessage(DWORD RRnetID, DWORD action_dword, DWORD dwType, DWORD SendTo);
HRESULT SendActionFloatMessage(DWORD RRnetID, float action_float, DWORD dwType, DWORD SendTo);

VOID DisplayNumberPlayersInGame( HWND hDlg );
void ReadDpInfoFromRegistry();
void WriteRegisteryInfo();
BOOL CALLBACK UpdatePosCallBack(UINT uTimerID, 
		UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);

int GetPlayerNumber(DWORD dpid);
void AddDpChatMessageToDisplay(char *msg);
void DisplayIncomingDpChatMessages();

void RrDestroyPlayer();

#endif //__DP_MESSAGES_H__