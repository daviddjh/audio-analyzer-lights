#pragma once
#ifndef EXTERN
#define EXTERN extern
#endif // !EXTERN

#include <mutex>

// EXTERN is only defined as the work extern
// in files that dont define it as ""
EXTERN std::mutex mtx;                  // mutex for sound and copying buffer

//#define BUFFSIZE 1024               	// Size of buffer sound captured and feed through fftw
//#define BUFFSIZE 2042               	// Size of buffer sound captured and feed through fftw
#define BUFFSIZE  3066              	// Size of buffer sound captured and feed through fftw
//#define BUFFSIZE 4096               	// Size of buffer sound captured and feed through fftw
//#define BUFFSIZE 8192               	// Size of buffer sound captured and feed through fftw
