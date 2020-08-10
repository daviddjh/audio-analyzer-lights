#pragma once
#ifndef MYAUDIOSINK
#define MYAUDIOSINK

//Global Vars
#include "vars.h"

class MyAudioSink {

public:
	bool DoneRecording = false;
	int NextBufferSize = 0;

	float SoundBufferA[BUFFSIZE];
	float SoundBufferB[BUFFSIZE];
	float * pCurrentSoundBuffer = SoundBufferA;
	float * pNextSoundBuffer = SoundBufferB;

	WAVEFORMATEX myWaveFormat;

	int bytesPerSample = 0;

	HRESULT CopyData(BYTE* pdata, UINT32 numFramesAvaliable, BOOL * bDone);

	HRESULT SetFormat(WAVEFORMATEX* pwfx);

};

#endif // !MYAUDIOSINK
