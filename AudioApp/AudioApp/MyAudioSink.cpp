#include <mutex>           //Thread saftey

//Debug output
#include <iostream>

//Windows audio
#include "audioclient.h"
#include "mmdeviceapi.h"
#include "ksmedia.h"

//Printing data
#include <bitset>

//MyAudioSink
#include "MyAudioSink.h"

//Global Vars
#include "vars.h"

HRESULT MyAudioSink::CopyData(BYTE * pdata, UINT32 numFramesAvaliable, BOOL * bDone) {

	// parent loop ends when bDone is true, DoneRecording is set elsewhere
	mtx.lock();

	*bDone = DoneRecording;

	mtx.unlock();

	if (pdata != nullptr) {
		for (long long int i = 0; i < numFramesAvaliable; i++) {

			// mutex lock for thread sync
			this->NextBufferSize++;
			if (NextBufferSize > BUFFSIZE) {
				mtx.lock();
				float* ptemp = this->pNextSoundBuffer;
				this->pNextSoundBuffer = this->pCurrentSoundBuffer;
				this->pCurrentSoundBuffer = ptemp;
				this->NextBufferSize = 1;
				mtx.unlock();
			}
			memcpy(this->pNextSoundBuffer + this->NextBufferSize - 1, pdata + (i * 8), sizeof(float));
		}
	}
	else {
		std::cout << "pdata null" << std::endl;
	}

	return S_OK;

}

HRESULT MyAudioSink::SetFormat(WAVEFORMATEX* pwfx) {

	// save some data about stream type

	myWaveFormat.wBitsPerSample = pwfx->wBitsPerSample;
	myWaveFormat.nBlockAlign = pwfx->nBlockAlign;
	bytesPerSample = myWaveFormat.wBitsPerSample / 8;

	return S_OK;

}
