//-----------------------------------------------------------------------------
// File: PlaySound.cpp
//
// Desc: DirectSound support for how to load a wave file and play it using a 
//       static DirectSound buffer.
//
//-----------------------------------------------------------------------------
#define STRICT
#include <objbase.h>
#include <commdlg.h>
#include <mmreg.h>
#include <dsound.h>
#include "resource.h"
#include "d3dapp.h"
#include "WavRead.h"
#include "world.hpp"
#include "roadrage.hpp"
#define MAX_SOUND_BUFFERS 30

//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

LPDIRECTSOUND       g_pDS            = NULL;
LPDIRECTSOUNDBUFFER g_pDSBuffer[MAX_SOUND_BUFFERS];
LPDIRECTSOUNDNOTIFY g_pDSNotify      = NULL;
CWaveSoundRead*     g_pWaveSoundRead = NULL;
DWORD               g_dwBufferBytes;


//-----------------------------------------------------------------------------
// Name: LoadWaveFile()
// Desc: Loads the wave file into a secondary static DirectSound buffer
//-----------------------------------------------------------------------------
VOID CMyD3DApplication::LoadWaveFile( TCHAR* strFileName, int bufferNum )
{
	PrintMessage(NULL, "Loading sound ", strFileName, LOGFILE_ONLY);
    
    // Create the sound buffer object from the wave file data
    if( FAILED( CreateStaticBuffer( strFileName ) ) )
    {        
		PrintMessage(NULL, "LoadWaveFile() - FAILED", NULL, LOGFILE_ONLY);
    }
    else // The sound buffer was successfully created
    {
        // Fill the buffer with wav data
        FillBuffer( bufferNum);
    }
}

//-----------------------------------------------------------------------------
// Name: OpenSoundFile()
// Desc: Called when the user requests to open a sound file
//-----------------------------------------------------------------------------
VOID CMyD3DApplication::OpenSoundFile(TCHAR *strFileName, int bufferNum)
{
    LoadWaveFile(strFileName, bufferNum);
}


//-----------------------------------------------------------------------------
// Name: PlaySound()
// Desc: Play the sound
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::PlaySound(int bufferNum, BOOL bLooped) 
{
    HRESULT hr;

    if( m_bBufferPaused )
    {
        // Play the buffer since it is currently paused
        if( FAILED( hr = PlayBuffer( bLooped, bufferNum ) ) )
            return hr;

        // Update the UI controls to show the sound as playing
        m_bBufferPaused = FALSE;
    }
    else
    {
        if( IsBufferPlaying(bufferNum) )
        {
            // To pause, just stop the buffer, but don't reset the position
            StopBuffer( FALSE, bufferNum );
            m_bBufferPaused = TRUE;
        }
        else
        {
            // The buffer is not playing, so play it again
            if( FAILED( hr = PlayBuffer( bLooped, bufferNum ) ) )
                return hr;

            // Show the sound as playing
            m_bBufferPaused = FALSE;

        }
    }

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: InitDirectSound()
// Desc: Initilizes DirectSound
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InitDirectSound( HWND hWnd )
{
    HRESULT             hr;
    LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

    // Initialize COM
    if( hr = CoInitialize( NULL ) )
        return hr;

    // Create IDirectSound using the primary sound device
    if( FAILED( hr = DirectSoundCreate( NULL, &g_pDS, NULL ) ) )
        return hr;

    // Set coop level to DSSCL_PRIORITY
    if( FAILED( hr = g_pDS->SetCooperativeLevel( hWnd, DSSCL_PRIORITY ) ) )
        return hr;

    // Get the primary buffer 
    DSBUFFERDESC dsbd;
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize        = sizeof(DSBUFFERDESC);
    dsbd.dwFlags       = DSBCAPS_PRIMARYBUFFER;
    dsbd.dwBufferBytes = 0;
    dsbd.lpwfxFormat   = NULL;
       
    if( FAILED( hr = g_pDS->CreateSoundBuffer( &dsbd, &pDSBPrimary, NULL ) ) )
        return hr;

    // Set primary buffer format to 22kHz and 16-bit output.
    WAVEFORMATEX wfx;
    ZeroMemory( &wfx, sizeof(WAVEFORMATEX) ); 
    wfx.wFormatTag      = WAVE_FORMAT_PCM; 
    wfx.nChannels       = 2; 
    wfx.nSamplesPerSec  = 22050; 
    wfx.wBitsPerSample  = 16; 
    wfx.nBlockAlign     = wfx.wBitsPerSample / 8 * wfx.nChannels;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

    if( FAILED( hr = pDSBPrimary->SetFormat(&wfx) ) )
        return hr;

    SAFE_RELEASE( pDSBPrimary );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FreeDirectSound()
// Desc: Releases DirectSound 
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FreeDirectSound()
{
    SAFE_DELETE( g_pWaveSoundRead );

    // Release DirectSound interfaces
    for (int ii = 0; ii < m_num_sounds; ii++)
		SAFE_RELEASE( g_pDSBuffer[ii] );
    SAFE_RELEASE( g_pDS ); 

    // Release COM
    CoUninitialize();

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: CreateStaticBuffer()
// Desc: Creates a wave file, sound buffer and notification events 
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::CreateStaticBuffer( TCHAR* strFileName )
{
    HRESULT hr; 

    // Create a new wave file class
    g_pWaveSoundRead = new CWaveSoundRead();

    // Load the wave file
    if( FAILED( g_pWaveSoundRead->Open( strFileName ) ) )
    {
		PrintMessage(NULL, "Bad wave file", NULL, LOGFILE_ONLY);
    }

    // Set up the direct sound buffer, and only request the flags needed
    // since each requires some overhead and limits if the buffer can 
    // be hardware accelerated
    DSBUFFERDESC dsbd;
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize        = sizeof(DSBUFFERDESC);
    dsbd.dwFlags       = DSBCAPS_STATIC | DSBCAPS_CTRLFREQUENCY ;
    dsbd.dwBufferBytes = g_pWaveSoundRead->m_ckIn.cksize;
    dsbd.lpwfxFormat   = g_pWaveSoundRead->m_pwfx;

    // Create the static DirectSound buffer 
    if( FAILED( hr = g_pDS->CreateSoundBuffer( &dsbd, &g_pDSBuffer[m_num_sounds++], NULL ) ) )
        return hr;

    // Remember how big the buffer is
    g_dwBufferBytes = dsbd.dwBufferBytes;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FillBuffer()
// Desc: Fill the DirectSound buffer with data from the wav file
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FillBuffer(int bufferNum)
{
    HRESULT hr; 
    BYTE*   pbWavData; // Pointer to actual wav data 
    UINT    cbWavSize; // Size of data
    VOID*   pbData  = NULL;
    VOID*   pbData2 = NULL;
    DWORD   dwLength;
    DWORD   dwLength2;

    // The size of wave data is in pWaveFileSound->m_ckIn
    INT nWaveFileSize = g_pWaveSoundRead->m_ckIn.cksize;

    // Allocate that buffer.
    pbWavData = new BYTE[ nWaveFileSize ];
    if( NULL == pbWavData )
        return E_OUTOFMEMORY;

    if( FAILED( hr = g_pWaveSoundRead->Read( nWaveFileSize, 
                                           pbWavData, 
                                           &cbWavSize ) ) )           
        return hr;

    // Reset the file to the beginning 
    g_pWaveSoundRead->Reset();

    // Lock the buffer down
    if( FAILED( hr = g_pDSBuffer[bufferNum]->Lock( 0, g_dwBufferBytes, &pbData, &dwLength, 
                                   &pbData2, &dwLength2, 0L ) ) )
        return hr;

    // Copy the memory to it.
    memcpy( pbData, pbWavData, g_dwBufferBytes );

    // Unlock the buffer, we don't need it anymore.
    g_pDSBuffer[bufferNum]->Unlock( pbData, g_dwBufferBytes, NULL, 0 );
    pbData = NULL;

    // We dont need the wav file data buffer anymore, so delete it 
    SAFE_DELETE( pbWavData );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: PlayBuffer()
// Desc: User hit the "Play" button, so play the DirectSound buffer
//-----------------------------------------------------------------------------
BOOL CMyD3DApplication::IsBufferPlaying(int bufferNum) 
{
    DWORD dwStatus = 0;

    if( NULL == g_pDSBuffer[bufferNum] )
        return E_FAIL;

    g_pDSBuffer[bufferNum]->GetStatus( &dwStatus );

    if( dwStatus & DSBSTATUS_PLAYING )
        return TRUE;
    else 
        return FALSE;
}




//-----------------------------------------------------------------------------
// Name: PlayBuffer()
// Desc: User hit the "Play" button, so play the DirectSound buffer
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::PlayBuffer( BOOL bLooped, int bufferNum )
{
    HRESULT hr;

    if( NULL == g_pDSBuffer[bufferNum] )
        return E_FAIL;

    // Restore the buffers if they are lost
    if( FAILED( hr = RestoreBuffers() ) )
        return hr;

    // Play buffer 
    DWORD dwLooped = bLooped ? DSBPLAY_LOOPING : 0L;
    if( FAILED( hr = g_pDSBuffer[bufferNum]->Play( 0, 0, dwLooped ) ) )
        return hr;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: StopBuffer()
// Desc: Stop the DirectSound buffer from playing 
//-----------------------------------------------------------------------------
VOID CMyD3DApplication::StopBuffer( BOOL bResetPosition, int bufferNum ) 
{
    if( NULL == g_pDSBuffer[bufferNum] )
        return;

    g_pDSBuffer[bufferNum]->Stop();

    if( bResetPosition )
        g_pDSBuffer[bufferNum]->SetCurrentPosition( 0L );    
}




//-----------------------------------------------------------------------------
// Name: IsSoundPlaying()
// Desc: Checks to see if a sound is playing and returns TRUE if it is.
//-----------------------------------------------------------------------------
BOOL CMyD3DApplication::IsSoundPlaying(int bufferNum)
{
    if( g_pDSBuffer[bufferNum] )
    {  
        DWORD dwStatus = 0;
        g_pDSBuffer[bufferNum]->GetStatus( &dwStatus );
        return( ( dwStatus & DSBSTATUS_PLAYING ) != 0 );
    }
    else
    {
        return FALSE;
    }
}


//-----------------------------------------------------------------------------
// Name: SetFrequency()
// Desc: Set playback frequency
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::SetFrequency(int bufferNum, int frequency )
{
    HRESULT hr;

    if( NULL == g_pDSBuffer[bufferNum] )
        return E_FAIL;

    // Play buffer 
 
    if( FAILED( hr = g_pDSBuffer[1]->SetFrequency( (DWORD)frequency ) ) )
    {
		PrintMessage(NULL, "CD3DApplication::SetFrequency - FAILED", NULL, LOGFILE_ONLY);
		return hr;
	}

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: RestoreBuffers()
// Desc: Restore lost buffers and fill them up with sound if possible
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RestoreBuffers()
{
    HRESULT hr;

    for (int ii = 0; ii < MAX_SOUND_BUFFERS; ii++)
	{
		if( NULL == g_pDSBuffer[ii] )
		    return S_OK;

		DWORD dwStatus;
		if( FAILED( hr = g_pDSBuffer[ii]->GetStatus( &dwStatus ) ) )
		    return hr;
	
		if( dwStatus & DSBSTATUS_BUFFERLOST )
		{
		    // Since the app could have just been activated, then
		    // DirectSound may not be giving us control yet, so 
		    // the restoring the buffer may fail.  
		    // If it does, sleep until DirectSound gives us control.
		    do 
		    {
		        hr = g_pDSBuffer[ii]->Restore();
		        if( hr == DSERR_BUFFERLOST )
		            Sleep( 10 );
		    }
		    while( hr = g_pDSBuffer[ii]->Restore() );
	
	        if( FAILED( hr = FillBuffer(ii) ) )
	            return hr;
	    }
	}	
    return S_OK;
}

