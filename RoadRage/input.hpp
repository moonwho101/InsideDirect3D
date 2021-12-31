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

/*
 * keyboard commands
 */
#define KEY_ACCELERATE			0x00000001l // 1
#define KEY_BRAKE				0x00000002l // 2
#define KEY_LEFT				0x00000004l // 4
#define KEY_RIGHT				0x00000008l // 8

#define KEY_FIRE				0x00000010l // 16
#define KEY_CHANGE_WEAPON_UP	0x00000020l // 32
#define KEY_CHANGE_WEAPON_DOWN  0x00000040l // 64
#define KEY_ENTER_OR_EXIT_CAR	0x00000080l // 128
#define KEY_CHANGE_GEAR_UP		0x00000100l // 256
#define KEY_CHANGE_GEAR_DOWN	0x00000200l // 512
#define FLIP					0x00000400l // 1024
#define SALUTE					0x00000800l // 2048
#define TAUNT					0x00001000l // 4096
#define WAVE					0x00002000l // 8192
#define POINT					0x00004000l // 16384

#define KEY_SCREENSHOT			0x00008000l // 32768

#define KEY_0					0x00010000l 
#define KEY_1					0x00020000l 
#define KEY_2					0x00040000l 
#define KEY_3					0x00080000l 

#define KEY_4					0x00100000l 
#define KEY_5					0x00200000l 
#define KEY_6					0x00400000l 
#define KEY_7					0x00800000l 

#define KEY_8					0x01000000l 
#define KEY_9					0x02000000l 
#define KEY_MINUS				0x04000000l 
#define KEY_PLUS				0x08000000l

#define KEY_M					0x10000000l
#define KEY_SHIFT				0x20000000l

#define JOY_BUTTON_1	0
#define JOY_BUTTON_2	1
#define JOY_BUTTON_3	2
#define JOY_BUTTON_4	3


#define IDC_DEVICES     300 

//extern "C" {

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
//};

#endif // _INPUT_H








