

#define SND_ENGINE_REV			1
#define SND_FIRE_AR15			2
#define SND_ENGINE_START		3
#define SND_ENGINE_IDLE			4

#define SND_SKID_TO_STOP		5
#define SND_SKID_ROUND_CORNER	6    	 
#define SND_FIRE_MP5			7	
	    
#define MAX_NUMBER_OF_SOUNDS	20  
#define NUMBER_OF_LOADED_SOUNDS 2        

#ifdef __cplusplus
extern "C" {
#endif

void SetFrequency(HWND hwnd,int snd_index, int frequency);
void InitialiseSound(HWND hwnd);
void PlayDSound(int snd_index, int snd_mode);
void StopDSound(int snd_index);

#ifdef __cplusplus
}
#endif