/*==========================================================================
 *
 *  Copyright (C) 1995-1997 Microsoft Corporation. All Rights Reserved.
 *
 *  File:       input.h
 *
 *
 ***************************************************************************/

#ifndef _INPUT_H
#define _INPUT_H

#include <dinput.h>

//--------------------------------------------------------------------------


// keyboard buffer size
#define KEYBUFSIZE 32


// keyboard commands
 

#define KEY_MOVE_FORWARD		0x00000001l // 1
#define KEY_MOVE_BACKWARD		0x00000002l // 2
#define KEY_LEFT				0x00000004l // 4
#define KEY_RIGHT				0x00000008l // 8

#define KEY_FIRE				0x00000010l // 16
#define KEY_MOVE_UP				0x00000020l // 32
#define KEY_MOVE_DOWN			0x00000040l // 64

#define KEY_SCREENSHOT			0x00000400l // 1024


#define JOY_BUTTON_1	0
#define JOY_BUTTON_2	1
#define JOY_BUTTON_3	2
#define JOY_BUTTON_4	3


#define IDC_DEVICES     300 

#ifdef __cplusplus
extern "C" {
#endif

//--------------------------------------------------------------------------

// external variables
extern BOOL bKeyboardAcquired;

extern DWORD (*ReadGameInput)(void);

//--------------------------------------------------------------------------

// prototypes
BOOL InitInput(HINSTANCE hInst, HWND hWnd);
void CleanupInput(void);
BOOL ReacquireInput(void);
BOOL PickInputDevice(int);

DWORD ReadKeyboardInput(void);
LPDIJOYSTATE ReadJoystickInput(void);



//--------------------------------------------------------------------------
#ifdef __cplusplus
};
#endif

#endif // _INPUT_H








