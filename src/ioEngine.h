#pragma once

#include <SDL.h>
#include <string>
#include <iostream>
#include <vector>

#include "oscilloscope.h"

#undef main

class IO_Engine
{
public:
	~IO_Engine() { stop(); }
	IO_Engine(const std::string &sWindowName, const int w, const int h);

	void start();
	INLINE void update(uint64_t elapsedTimeInMs);
	void render();
	void stop();

	INLINE std::vector<SDL_Event>& getEvents() { return m_sdlEvents; }

	void print() const;
private:
	bool init();
	void loop();
	void handleInput();
	void exit();

private:
	SDL_Window				*m_pWindow{};
	SDL_Renderer			*m_pRenderer{};

	std::string				m_sWindowName;

	const int				m_width;
	const int				m_height;

	std::vector<SDL_Event>	m_sdlEvents;

	const int				m_framePerSecond{ 60 };
	const int				m_refreshIntervalInMs{ 1000 / m_framePerSecond };

	bool					m_bRunning{ false };

	Oscilloscope			m_oscilloscope;
};