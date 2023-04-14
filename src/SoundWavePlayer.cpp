#include "SoundWavePlayer.h"
#include "constants.h"
#include "logger.h"

DisplayBuffer::~DisplayBuffer()
{
	delete[] m_pBuffer;
	m_pBuffer = nullptr;
}

DisplayBuffer::DisplayBuffer(Uint32 sampleCount, SDL_AudioFormat type)
{
	setFormat(type);
	m_size = sampleCount * m_bytes;
	m_pBuffer = new Uint8[m_size];
	clear();
}

void DisplayBuffer::setFormat(SDL_AudioFormat type)
{
	m_type = type;
	if (m_type == AUDIO_S8 || m_type == AUDIO_U8)
	{
		m_bytes = 1;
	}
	else if (m_type == AUDIO_S16 || m_type == AUDIO_U16)
	{
		m_bytes = 2;
	}
	else if (m_type == AUDIO_S32 || m_type == AUDIO_F32)
	{
		m_bytes = 4;
	}
	else
	{
		ns_Util::Logger::LOG_ERROR("Unkown format!");
	}
}

void DisplayBuffer::clear()
{
	memset(m_pBuffer, 0, m_size * sizeof(Uint8));
	m_pos = 0;
}

void DisplayBuffer::print(const std::string & prefix) const
{
	using ns_Util::Logger;
	Logger::LOG_MSG(prefix, "DisplayBuffer: \n");
	Logger::LOG_MSG(prefix, "    Format             : ", audioFormat2String(m_type), '\n');
	Logger::LOG_MSG(prefix, "    Size(# of samples) : ", m_size, '\n');
	Logger::LOG_MSG(prefix, "    Bytes(per sample)  : ", m_bytes, '\n');
	Logger::LOG_MSG(prefix, "    Curr position      : ", m_pos, '\n');
}



SoundWavePlayer::SoundWavePlayer(int f, int a, SoundWave::value_type phase, WaveForm w, 
	int sampleRate, SDL_AudioFormat format, Uint8 channels, Uint16 sampleCount, SDL_AudioCallback callback, 
	int displayWidth, int displayHeight)
	: m_soundWave(f, a, w, phase)
	, m_audioCallback(callback)
	, m_graphBuffer(sampleCount, format)
	, m_displayWidth(displayWidth)
	, m_displayHeight(displayHeight)
{
	SDL_zero(m_desiredSpec);
	m_desiredSpec.freq = sampleRate;
	m_desiredSpec.format = format;
	m_desiredSpec.channels = channels;
	m_desiredSpec.samples = sampleCount;
	// Chartreuse
	m_waveColor = { 127, 255, 0, SDL_ALPHA_OPAQUE };
}

bool SoundWavePlayer::init()
{
	m_soundWave.reEquateValues();

	SDL_zero(m_deviceSpec);

	// NOTE :: ms = (sampleframes * 1000) / freq
	m_desiredSpec.callback = m_audioCallback;
	m_desiredSpec.userdata = this;

	m_deviceId = SDL_OpenAudioDevice(NULL, 0, &m_desiredSpec, &m_deviceSpec, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);
	if (m_deviceId == 0)
	{
		ns_Util::Logger::LOG_SDL_ERROR("Failed to open audio");
		return false;
	}
	m_audioPos = 0;
	m_audioLength = getSampleRate();
	graphBufferClear();
	return true;
}

void SoundWavePlayer::start()
{
	play();
}

void SoundWavePlayer::update(uint64_t elapsedTimeInMs, const std::vector<SDL_Event>& events)
{
	(void)elapsedTimeInMs;
	for (size_t i = 0, size = events.size(); i < size; ++i)
	{
		if (events[i].type == SDL_KEYDOWN)
		{
			handleKeyEvent(events[i].key.keysym.scancode);
		}
	}
}

void SoundWavePlayer::draw(SDL_Renderer* pRenderer)
{
	SDL_SetRenderDrawColor(pRenderer, m_waveColor.r, m_waveColor.g, m_waveColor.b, m_waveColor.a);
	SDL_RenderSetScale(pRenderer, m_waveSF, m_waveSF);

	const int yNormalizer = m_displayHeight;

	switch (getAudioFormat())
	{
		case AUDIO_S8:
			drawHelper<Sint8>((Sint8*)getGraphBuffer(), pRenderer, 0, m_displayWidth, yNormalizer);
			break;

		case AUDIO_U8:
			drawHelper<Uint8>((Uint8*)getGraphBuffer(), pRenderer, 0, m_displayWidth, yNormalizer);
			break;

		case AUDIO_S16:
			drawHelper<Sint16>((Sint16*)getGraphBuffer(), pRenderer, 0, m_displayWidth, yNormalizer);
			break;

		case AUDIO_U16:
			drawHelper<Uint16>((Uint16*)getGraphBuffer(), pRenderer, 0, m_displayWidth, yNormalizer);
			break;

		case AUDIO_S32:
			drawHelper<Sint32>((Sint32*)getGraphBuffer(), pRenderer, 0, m_displayWidth, yNormalizer);
			break;

		case AUDIO_F32:
			drawHelper<float>((float*)getGraphBuffer(), pRenderer, 0, m_displayWidth, yNormalizer);
			break;

		default:
			ns_Util::Logger::LOG_ERROR("Unkown format!");
			break;
	}
}

void SoundWavePlayer::play()
{
	m_bPaused = false;
	SDL_PauseAudioDevice(m_deviceId, 0);
}

void SoundWavePlayer::pause()
{
	m_bPaused = true;
	SDL_PauseAudioDevice(m_deviceId, 1);
}

void SoundWavePlayer::stop()
{
	pause();
	exit();
}

void SoundWavePlayer::print(const std::string& prefix) const
{
	using ns_Util::Logger;
	Logger::LOG_MSG(prefix, "SoundWavePlayer: \n");
	Logger::LOG_MSG(prefix, "    Device Id              : ", m_deviceId, '\n');
	Logger::LOG_MSG(prefix, "    Audio Position         : ", m_audioPos, '\n');
	Logger::LOG_MSG(prefix, "    Audio Length           : ", m_audioLength, '\n');
	Logger::LOG_MSG(prefix, "    Volume                 : ", m_volume, '\n');
	Logger::LOG_MSG(prefix, "    WaveColor(R, G, B, A)  : (", (int)m_waveColor.r, ',', (int)m_waveColor.g, ',', (int)m_waveColor.b, ',', (int)m_waveColor.a, ")\n");
	Logger::LOG_MSG(prefix, "    Wave point scaling     : ", m_waveSF, '\n');
	Logger::LOG_MSG(prefix, "    Display Width          : ", m_displayWidth, '\n');
	Logger::LOG_MSG(prefix, "    Display Height         : ", m_displayHeight, '\n');
	Logger::LOG_MSG(prefix, "    Desired Audio Specs    : \n");
	printAudioSpec(m_desiredSpec, prefix + "          ");
	Logger::LOG_MSG(prefix, "    Device Audio Specs     : \n");
	printAudioSpec(m_deviceSpec, prefix + "          ");
	m_soundWave.print(prefix + "          ");
	m_graphBuffer.print(prefix + "          ");
}

template<typename T>
void SoundWavePlayer::drawHelper(T *arr, SDL_Renderer* pRenderer, int xStart, int xEnd, int yNormalizer)
{
	for (int x2 = xStart + 1, x1 = xStart, y2 = 0, y1 = 0; x2 < xEnd; ++x2, ++x1)
	{
		y2 = static_cast<int>((yNormalizer - arr[x2]) / m_waveSF);
		y1 = static_cast<int>((yNormalizer - arr[x1]) / m_waveSF);
		SDL_RenderDrawLine(pRenderer, int(x2 / m_waveSF), y2, int(x1 / m_waveSF), y1);
	}
}

void SoundWavePlayer::exit()
{
	SDL_CloseAudioDevice(m_deviceId);
	SDL_zero(m_desiredSpec);
	SDL_zero(m_deviceSpec);
	m_graphBuffer.clear();
}

void SoundWavePlayer::handleKeyEvent(SDL_Scancode keyCode)
{
	switch (keyCode)
	{
		case SDL_SCANCODE_C:
			m_soundWave.setNextWaveForm();
			m_graphBuffer.clear();
			m_audioPos = 0;
			break;

		case SDL_SCANCODE_RIGHT:
			m_soundWave.changeFrequency(1);
			break;
		case SDL_SCANCODE_LEFT:
			m_soundWave.changeFrequency(-1);
			break;
		
		case SDL_SCANCODE_UP:
			m_soundWave.changeAmplitude(1);
			break;
		case SDL_SCANCODE_DOWN:
			m_soundWave.changeAmplitude(-1);
			break;

		case SDL_SCANCODE_SPACE:
			m_bPaused ? play() : pause();

		default:
			break;
	}
}

void SoundWavePlayer::printAudioSpec(const SDL_AudioSpec& spec, const std::string& prefix) const
{
	using ns_Util::Logger;
	Logger::LOG_MSG(prefix, "Frequency[sample rate] : ", spec.freq, '\n');
	Logger::LOG_MSG(prefix, "Format                 : ", audioFormat2String(spec.format), '\n');
	Logger::LOG_MSG(prefix, "Channels               : ", channelsToString(spec.channels), ", (", (int)spec.channels, ")\n");
	Logger::LOG_MSG(prefix, "Silence                : ", (int)spec.silence, '\n');
	Logger::LOG_MSG(prefix, "Samples                : ", spec.samples, '\n');
	Logger::LOG_MSG(prefix, "Padding                : ", spec.padding, '\n');
	Logger::LOG_MSG(prefix, "Size                   : ", spec.size, '\n');
}


template <typename T>
void SDLAudioCBHelper(SoundWavePlayer* pSoundWavePlayer, T* pStream, T* pGraphBuffer, int len)
{
	const float yOffset = pSoundWavePlayer->getDisplayHeight() / 2.0f;
	const int numOfChannels = pSoundWavePlayer->getAudioChannels();
	for (int i = 0, max = len / (pSoundWavePlayer->graphBufferBytes() * numOfChannels); i < max; ++i)
	{
		if (pSoundWavePlayer->getAudioLength() <= 0)
		{
			pStream[i] = pSoundWavePlayer->getDeviceSpecs()->silence;
		}
		else
		{
			SoundWave::value_type y = pSoundWavePlayer->getSample() * pSoundWavePlayer->getVolume();
			pStream[numOfChannels * i] = static_cast<T>(y + yOffset);

			// Same audio data for all channels
			if (numOfChannels > 1)
			{
				for (int j = 1; j <= numOfChannels; ++j)
				{
					pStream[numOfChannels * i + j] = pStream[numOfChannels * i];
				}
			}
			pSoundWavePlayer->incrementAudioPosition(1);

			// Copy data to display buffer for plotting
			Uint32& graphPos = pSoundWavePlayer->getGraphBufferPosition();
			if (graphPos >= pSoundWavePlayer->graphBufferSize())
			{
				graphPos = 0;
			}
			pGraphBuffer[graphPos++] = pStream[numOfChannels * i];
		}
	}
}

void SDLAudioCallback(void* pUserData, Uint8* pStream, int pStreamLengthInBytes)
{
	SoundWavePlayer* player = (SoundWavePlayer *)pUserData;
	Uint8* pGraphBuffer = player->getGraphBuffer();
	
	switch (player->getAudioFormat())
	{
		case AUDIO_S8:
			SDLAudioCBHelper<Sint8>(player, (Sint8*)pStream, (Sint8*)pGraphBuffer, pStreamLengthInBytes);
			break;

		case AUDIO_U8:
			SDLAudioCBHelper<Uint8>(player, (Uint8*)pStream, (Uint8*)pGraphBuffer, pStreamLengthInBytes);
			break;

		case AUDIO_S16:
			SDLAudioCBHelper<Sint16>(player, (Sint16*)pStream, (Sint16*)pGraphBuffer, pStreamLengthInBytes);
			break;

		case AUDIO_U16:
			SDLAudioCBHelper<Uint16>(player, (Uint16*)pStream, (Uint16*)pGraphBuffer, pStreamLengthInBytes);
			break;

		case AUDIO_S32:
			SDLAudioCBHelper<Sint32>(player, (Sint32*)pStream, (Sint32*)pGraphBuffer, pStreamLengthInBytes);
			break;

		case AUDIO_F32:
			SDLAudioCBHelper<float>(player, (float*)pStream, (float*)pGraphBuffer, pStreamLengthInBytes);
			break;

		default:
			ns_Util::Logger::LOG_ERROR("Unkown format!");
			break;
	}
}

std::string audioFormat2String(SDL_AudioFormat format)
{
	std::string strFormat;
	switch (format)
	{
		case AUDIO_S8:
			strFormat = "AUDIO_S8";
			break;

		case AUDIO_U8:
			strFormat = "AUDIO_U8";
			break;

		case AUDIO_S16:
			strFormat = "AUDIO_S16";
			break;

		case AUDIO_U16:
			strFormat = "AUDIO_U16";
			break;

		case AUDIO_S32:
			strFormat = "AUDIO_S32";
			break;

		case AUDIO_F32:
			strFormat = "AUDIO_F32";
			break;

		default:
			strFormat = "Unkown";
			break;
	}
	return strFormat;
}

std::string channelsToString(Uint8 channels)
{
	std::string strChannel;
	switch (channels)
	{
		case MONO:
			strChannel = "MONO";
			break;

		case STEREO:
			strChannel = "STEREO";
			break;

		case QUAD:
			strChannel = "QUAD";
			break;

		case HEXA:
			strChannel = "HEXA";
			break;

		default:
			strChannel = "Unkown";
			break;
	}
	return strChannel;
}