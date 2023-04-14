#pragma once

#include "bgGrid.h"
#include "SoundWavePlayer.h"

class Oscilloscope
{
public:
	Oscilloscope(int w, int h, int numOfCols);

	bool init();
	void start();
	void stop();
	void update(uint64_t elapsedTimeInMs, const std::vector<SDL_Event>& events);
	void draw(SDL_Renderer *pRenderer);
	void print(const std::string& prefix = "") const;

private:
	const int			m_width{};
	const int			m_height{};
	BgGrid				m_grid;
	SoundWavePlayer		m_soundWavePlayer;
};
