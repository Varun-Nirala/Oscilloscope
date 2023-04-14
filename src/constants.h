#pragma once

#include <iostream>
#include <cmath>
#include <string>
#include <unordered_map>

#include <SDL.h>

#define INLINE inline

#define M_PI_2 1.57079632679489661923

INLINE long double cot(long double angle)
{
	return std::sin(angle) / std::cos(angle);
}

// Grid +ve column count
#define NUM_OF_COLUMNS	6

// Sound channels related
#define MONO			1
#define STEREO			2
#define QUAD			4
#define HEXA			6

// SDL Window related
#define WINDOW_NAME	   "Oscilloscope"
#define WINDOW_WIDTH   1024
#define WINDOW_HEIGHT  256

// Sound wave related
#define FREQUENCY      440
#define AMPLITUDE      120
#define PHASE          0
#define WAVEFORM       WaveForm::SINE

// SDL_AudioSpec related
#define SAMPLE_RATE    44100
#define FORMAT         AUDIO_U8
#define CHANNELS       STEREO
#define SAMPLE_COUNT   1024
#define CB             SDLAudioCallback