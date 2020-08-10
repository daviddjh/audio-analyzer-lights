#pragma once
#ifndef MYAUDIOSINK
#define MYAUDIOSINK
#include <deque>

class MyAudioSink {

public:
	bool DoneRecording = false;
	float SoundBuffer[500];

	WAVEFORMATEX myWaveFormat;

	int bytesPerSample = 0;

	HRESULT CopyData(BYTE* pdata, UINT32 numFramesAvaliable, BOOL * bDone);

	HRESULT SetFormat(WAVEFORMATEX* pwfx);

};

#endif // !MYAUDIOSINK
