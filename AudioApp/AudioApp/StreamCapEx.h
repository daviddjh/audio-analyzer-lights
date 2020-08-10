#include "mmdeviceapi.h"
#include "MyAudioSink.h"
#pragma once

HRESULT RecordAudioStream(MyAudioSink* pMySink);

HRESULT StartRecord(MyAudioSink* pMySink);
