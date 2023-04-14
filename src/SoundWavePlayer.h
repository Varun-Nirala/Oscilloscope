#pragma once

#include <vector>
#include <SDL.h>
#include <SDL_audio.h>
#include "soundWave.h"

extern void SDLAudioCallback(void* pUserData, Uint8* pStream, int pStreamLengthInBytes);
extern std::string audioFormat2String(SDL_AudioFormat format);
extern std::string channelsToString(Uint8 channels);

class SoundWavePlayer;
struct DisplayBuffer
{
	friend class SoundWavePlayer;

	~DisplayBuffer();

	DisplayBuffer(Uint32 sampleCount, SDL_AudioFormat type);

	void setFormat(SDL_AudioFormat type);

	// size is in sample not in byte
	INLINE Uint32 size() const { return m_size / m_bytes; }

	// size of each sample
	INLINE Uint32 bytes() const { return m_bytes; }

	void clear();

	void print(const std::string& prefix = "") const;
private:
	Uint8					*m_pBuffer{};
	Uint32					m_size{};
	Uint32					m_pos{};
	Uint32					m_bytes{};
	SDL_AudioFormat			m_type{};
};

class SoundWavePlayer
{
public:
	~SoundWavePlayer() { stop(); }

	SoundWavePlayer(
		int f, int a, SoundWave::value_type phase, WaveForm w,
		int sampleRate, SDL_AudioFormat format, Uint8 channels, Uint16 sampleCount, SDL_AudioCallback callback,
		int displayWidth, int displayHeight);

	INLINE void setFrequency(int f) { m_soundWave.setFrequency(f); }
	INLINE void setAmplitude(int a) { m_soundWave.setAmplitude(a); }
	INLINE void setPhase(SoundWave::value_type phase) { m_soundWave.setPhase(phase); }
	INLINE void setWaveForm(WaveForm w) { m_soundWave.setWaveForm(w); }
	INLINE void setAudioPosition(int pos) { m_audioPos = pos; }
	INLINE void setAudioLength(int len) { m_audioLength = len; }
	INLINE void setSampleRate(int s) { m_desiredSpec.freq = s; }
	INLINE void setAudioFormat(SDL_AudioFormat format) { m_desiredSpec.format = format; }
	INLINE void setAudioChannels(Uint8 channels) { m_desiredSpec.channels = channels; }
	INLINE void setSampleCount(Uint16 samples) { m_desiredSpec.samples = samples; }
	INLINE void setAudioCallback(SDL_AudioCallback callback) { m_audioCallback = callback; }
	INLINE void setVolume(float volume) { m_volume = volume; }

	INLINE int getFrequency() const { return m_soundWave.getFrequency(); }
	INLINE int getAmplitude() const { return m_soundWave.getAmplitude(); }
	INLINE SoundWave::value_type getPhase() const { return m_soundWave.getPhase(); }
	INLINE WaveForm getWaveForm() const { return m_soundWave.getWaveForm(); }
	INLINE int getAudioPosition() const { return m_audioPos; }
	INLINE int getAudioLength() const { return m_audioLength; }
	INLINE int getSampleRate() const { return m_deviceSpec.freq; }
	INLINE SDL_AudioFormat getAudioFormat() const { return m_deviceSpec.format; }
	INLINE Uint8 getAudioChannels() const { return m_deviceSpec.channels; }
	INLINE Uint16 getSampleCount() const { return m_deviceSpec.samples; }
	INLINE SDL_AudioCallback getAudioCallback() const { return m_audioCallback; }
	INLINE float getVolume() const { return m_volume; }

	INLINE SoundWave& getSoundWave() { return m_soundWave; }
	INLINE const SoundWave& getSoundWave() const { return m_soundWave; }

	INLINE SDL_AudioSpec* getDeviceSpecs() { return &m_deviceSpec; }

	INLINE int getDisplayWidth() const { return m_displayWidth; }
	INLINE int getDisplayHeight() const { return m_displayHeight; }

	// Display Buffer API
	INLINE Uint8* getGraphBuffer() { return m_graphBuffer.m_pBuffer; }
	INLINE const Uint8* getGraphBuffer() const { return m_graphBuffer.m_pBuffer; }

	INLINE void setGraphBufferFormat(SDL_AudioFormat format) { m_graphBuffer.setFormat(format); }
	INLINE SDL_AudioFormat getGraphBufferFormat() const { return m_graphBuffer.m_type; }

	INLINE Uint32 graphBufferSize() const { return m_graphBuffer.size(); }

	INLINE Uint32 graphBufferBytes() const { return m_graphBuffer.bytes(); }

	INLINE void graphBufferClear() { return m_graphBuffer.clear(); }

	INLINE void setGraphBufferPosition(int pos) { m_graphBuffer.m_pos = pos; }
	INLINE Uint32& getGraphBufferPosition() { return m_graphBuffer.m_pos; }
	INLINE const Uint32& getGraphBufferPosition() const { return m_graphBuffer.m_pos; }

	INLINE void incrementGraphBufferPosition(Uint32 inc = 1) { m_graphBuffer.m_pos += inc; }
	// Display Buffer API

	INLINE void incrementAudioPosition(Uint32 inc = 1) { m_audioPos += inc; }
	
	// audio_pos / SAMPLE_RATE
	INLINE SoundWave::value_type getSample() const { return m_soundWave.getSample(m_audioPos / (SoundWave::value_type)getSampleRate()); }

	bool init();

	void start();
	void update(uint64_t elapsedTimeInMs, const std::vector<SDL_Event> &events);
	void draw(SDL_Renderer* pRenderer);
	void play();
	void pause();
	void stop();

	template <typename T>
	INLINE T* castUint8BufferAs(Uint8* pBuf) { return static_cast<T*>(pBuf); }

	void print(const std::string& prefix = "") const;
private:
	template<typename T>
	void drawHelper(T* arr, SDL_Renderer* pRenderer, int xStart, int xEnd, int yNormalizer);
	void exit();
	void handleKeyEvent(SDL_Scancode keyCode);
	void printAudioSpec(const SDL_AudioSpec& spec, const std::string &prefix = "") const;

private:
	SDL_AudioDeviceID		m_deviceId{};
	SDL_AudioSpec			m_desiredSpec{};
	SDL_AudioSpec			m_deviceSpec{};
	SDL_AudioCallback		m_audioCallback{};

	SoundWave				m_soundWave;

	int						m_audioPos{};
	int						m_audioLength{};

	bool					m_bPaused{ true };

	DisplayBuffer			m_graphBuffer;

	float					m_volume{ 1.0f };
	SDL_Color				m_waveColor;
	float					m_waveSF{ 2.0f };

	const int				m_displayWidth{};
	const int				m_displayHeight{};
};