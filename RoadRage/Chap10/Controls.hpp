
#include <windows.h>

#define ASC_APOSTROPHE		39
#define ASC_COMMA			44
#define ASC_PERIOD			46
#define ASC_GRAVE			96

#define ASC_BACK_SPACE		0
#define ASC_FORWARD_SLASH	0 
#define ASC_BACK_SLASH		0
#define ASC_LEFT_BRACKET	0  
#define ASC_RIGHT_BRACKET	0
#define ASC_ESCAPE			0
#define ASC_TAB				0

#define ASC_RETURN			0
#define ASC_LEFT_CONTROL	0
#define ASC_LEFT_SHIFT		0
#define ASC_RIGHT_SHIFT		0
#define ASC_TAB				0
#define ASC_LEFT_MENU		0
#define ASC_MULTIPLY		0
#define ASC_SPACE_BAR		32

#define ASC_SHIFT_1			33
#define ASC_SHIFT_2			34
#define ASC_SHIFT_3			35
#define ASC_SHIFT_4			36
#define ASC_SHIFT_5			37
#define ASC_SHIFT_6			38
#define ASC_SHIFT_7			0
#define ASC_SHIFT_8			42
#define ASC_SHIFT_9			40
#define ASC_SHIFT_0			41



static char dik_to_char_upper_case[] = { 0, ASC_ESCAPE,

	ASC_SHIFT_1, ASC_SHIFT_2, ASC_SHIFT_3, ASC_SHIFT_4,
	ASC_SHIFT_5, ASC_SHIFT_6, ASC_SHIFT_7, ASC_SHIFT_8,
	ASC_SHIFT_9, ASC_SHIFT_0,

	'_','+', ASC_BACK_SPACE, ASC_TAB,

	'Q','W','E','R','T','Y','U','I','O','P',

	ASC_LEFT_BRACKET, ASC_RIGHT_BRACKET, ASC_RETURN, ASC_LEFT_CONTROL, 

	'A','S','D','F','G','H','J','K','L',':', '@',
	
	ASC_GRAVE, ASC_LEFT_SHIFT, ASC_BACK_SLASH,
	
	'Z','X','C','V','B','N','M', '<', '>', '?',
	ASC_RIGHT_SHIFT, ASC_MULTIPLY, ASC_LEFT_MENU, ASC_SPACE_BAR, 0};


static char dik_to_char_lower_case[] = { 0, ASC_ESCAPE,

	'1','2','3','4','5','6','7','8','9','0',

	'-','=', ASC_BACK_SPACE, ASC_TAB,

	'q','w','e','r','t','y','u','i','o','p',

	ASC_LEFT_BRACKET, ASC_RIGHT_BRACKET, ASC_RETURN, ASC_LEFT_CONTROL, 

	'a','s','d','f','g','h','j','k','l',';', ASC_APOSTROPHE,
	
	ASC_GRAVE, ASC_LEFT_SHIFT, ASC_BACK_SLASH,
	
	'z','x','c','v','b','n','m', ASC_COMMA, ASC_PERIOD, ASC_FORWARD_SLASH,
	ASC_RIGHT_SHIFT, ASC_MULTIPLY, ASC_LEFT_MENU, ASC_SPACE_BAR, 0};

typedef struct Controls_typ
{
	BOOL bLeft,  bRight; 
	BOOL bForward,  bBackward;
	BOOL bUp,  bDown;
	BOOL bHeadUp,  bHeadDown;
	BOOL bStepLeft,  bStepRight; 
	BOOL bFire,  bScores;
	BOOL bPrevWeap,  bNextWeap;
	BOOL bTravelMode;
	
} CONTROLS, *Controls_ptr;

VOID MovePlayer(CONTROLS *Controls);
