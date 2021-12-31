
// D3D Road Rage Editor version 1.1
// Written by William Chin
// Developed in MSDEV C++ v4.2 with DirectX 5 SDK
// Date : 5/10/98
// File : Sound.c


// Note : ".wav" files, with these parameters work fine 
// PCM 11025Hz 16bit Mono      
// PCM 22050HZ 8bit  Mono

// However these didn't work at all

// PCM 22050HZ 16bit  Mono

#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h> 
#include <dsound.h>
#include "dsutil.h"
#include "sound.h"

#define UNSET 0
#define SET 1    

HDC hdc;
HRESULT hr;
LPDIRECTSOUND           lpDS;
LPDIRECTSOUNDBUFFER		lpDSBStreamBuffer = NULL;
LPWAVEFORMATEX lpwfxFormat;
IDirectSoundBuffer *pDSB;

HSNDOBJ soundptrs[MAX_NUMBER_OF_SOUNDS];                 

BOOL sound_intialised_flag = UNSET;

HSNDOBJ temp_snd_ptr;

void SetFrequency(HWND hwnd,int snd_index, int frequency)
{
	if(sound_intialised_flag == UNSET)
		return;

	SndObjSetFrequency(soundptrs[snd_index],frequency);		
}

void InitialiseSound(HWND hwnd)
{
	if(sound_intialised_flag == SET)
		return;
	else
		sound_intialised_flag = SET;

	hdc=GetDC(hwnd);
	
	if (!SUCCEEDED(DirectSoundCreate(NULL, &lpDS, NULL)))			
	{
		TextOut(hdc,10, 10,"Create Failed",13);
		return;
	}

	if (!SUCCEEDED(lpDS->lpVtbl->SetCooperativeLevel(lpDS,hwnd,DSSCL_NORMAL)))					
	{
		TextOut(hdc,10, 30,"Set Co-op Failed",18);
		return;
	}
		
	temp_snd_ptr = SndObjCreate(lpDS, "colt", 1);
	if(temp_snd_ptr==NULL)
	{
		TextOut(hdc,10, 50,"Obj Failed",10);
		return;
	}
	else
		soundptrs[SND_FIRE_AR15] = temp_snd_ptr;


	temp_snd_ptr = SndObjCreate(lpDS, "engine", 1);
	if(temp_snd_ptr==NULL)
	{
		TextOut(hdc,10, 50,"Obj Failed",10);
		return;
	}
	else
		soundptrs[SND_ENGINE_REV] = temp_snd_ptr;

	ReleaseDC(hwnd,hdc);
}

void PlayDSound(int snd_index, int snd_mode)
{
	if(sound_intialised_flag == UNSET)
		return;
	SndObjPlay(soundptrs[snd_index], snd_mode);
}

void StopDSound(int snd_index)
{
	if(sound_intialised_flag == UNSET)
		return;
	SndObjStop(soundptrs[snd_index]);
}

//SndObjPlay(hsoEngineRev, DSBPLAY_LOOPING);	
