typedef struct {
	LPDIRECTPLAY4A	lpDirectPlay4A;		// IDirectPlay4A interface pointer
	HANDLE			hPlayerEvent;		// player event to use
	DPID			dpidPlayer;			// ID of player created
	BOOL			bIsHost;			// TRUE if we are hosting the session
} DPLAYINFO, *LPDPLAYINFO;


