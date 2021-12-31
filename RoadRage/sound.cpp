
// Road Rage version 1.1

// Written by William Chin
// Developed in MSDEV C++ v6.0 using the DirectX 6.0 SDK
// Date : 3/7/99
// File : sound.cpp


#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h> 
#include <dsound.h>
#include "dsutil.hpp"
#include "sound.hpp"

#define UNSET 0
#define SET 1    

static HDC hdc;
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

	hdc=GetDC(hwnd);
	
	if (!SUCCEEDED(DirectSoundCreate(NULL, &lpDS, NULL)))			
	{
		TextOut(hdc,10, 10,"Create Failed",13);
		return;
	}

	if (!SUCCEEDED(lpDS->SetCooperativeLevel(hwnd,DSSCL_NORMAL)))					
	{
		TextOut(hdc,10, 30,"Set Co-op Failed",18);
		return;
	}
		
	temp_snd_ptr = SndObjCreate(lpDS, "m4fire", 1);
	if(temp_snd_ptr==NULL)
	{
		TextOut(hdc,10, 50,"Obj Failed : m4fire",19);
		return;
	}
	else 
		soundptrs[SND_FIRE_AR15] = temp_snd_ptr;

	temp_snd_ptr = SndObjCreate(lpDS, "mp5fire", 1);
	if(temp_snd_ptr==NULL)
	{
		TextOut(hdc,10, 50,"Obj Failed : mp5fire",20);
		return;
	}
	else
		soundptrs[SND_FIRE_MP5] = temp_snd_ptr;


	temp_snd_ptr = SndObjCreate(lpDS, "engine", 1);
	if(temp_snd_ptr==NULL)
	{
		TextOut(hdc,10, 50,"Obj Failed : engine",19);
		return;
	}
	else
		soundptrs[SND_ENGINE_REV] = temp_snd_ptr;

	sound_intialised_flag = SET;

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

//GetStatus (soundptrs[snd_index], DSBSTATUS_PLAYING );

