#include "oscilloscope.h"
#include "logger.h"

Oscilloscope::Oscilloscope(int w, int h, int numOfCols)
	: m_width(w)
	, m_height(h)
	, m_grid(w, h, numOfCols)
	, m_soundWavePlayer(FREQUENCY, AMPLITUDE, PHASE, WAVEFORM, SAMPLE_RATE, FORMAT, CHANNELS, SAMPLE_COUNT, CB, m_width, m_height)
{
}

bool Oscilloscope::init()
{
	return m_soundWavePlayer.init();
}

void Oscilloscope::start()
{
	m_soundWavePlayer.start();
}

void Oscilloscope::stop()
{
	m_soundWavePlayer.stop();
}

void Oscilloscope::update(uint64_t elapsedTimeInMs, const std::vector<SDL_Event>& events)
{
	m_soundWavePlayer.update(elapsedTimeInMs, events);
}

void Oscilloscope::draw(SDL_Renderer* pRenderer)
{
	m_grid.draw(pRenderer);
	m_soundWavePlayer.draw(pRenderer);
}

void Oscilloscope::print(const std::string &prefix) const
{
	using ns_Util::Logger;
	Logger::LOG_MSG(prefix, "Oscilloscope: \n");
	m_soundWavePlayer.print(prefix + "          ");
}