#include "ioEngine.h"
#include "logger.h"

IO_Engine::IO_Engine(const std::string& sWindowName, const int w, const int h)
	: m_sWindowName(sWindowName)
	, m_width(w)
	, m_height(h)
	, m_oscilloscope(w, h, NUM_OF_COLUMNS)
{
}

void IO_Engine::start()
{
	if (init())
	{
		m_bRunning = true;
		print();
		m_oscilloscope.start();
		loop();
	}
}

void IO_Engine::stop()
{
	m_oscilloscope.stop();
	exit();
}

void IO_Engine::update(uint64_t elapsedTimeInMs)
{
	m_oscilloscope.update(elapsedTimeInMs, m_sdlEvents);
}

void IO_Engine::render()
{
	SDL_RenderClear(m_pRenderer);

	m_oscilloscope.draw(m_pRenderer);

	SDL_RenderPresent(m_pRenderer);
}

void IO_Engine::print() const
{
	using ns_Util::Logger;
	Logger::LOG_MSG("IO_Engine: \n");
	Logger::LOG_MSG("    Window Name      : ", m_sWindowName, ", Height = ", m_height, ", Width = ", m_width, '\n');
	Logger::LOG_MSG("    Refresh Interval : ", m_refreshIntervalInMs, " ms\n");
	Logger::LOG_MSG("    SDL Window is    : ", m_pWindow ? "Initialized\n" : "Uninitialized\n");
	Logger::LOG_MSG("    SDL Rendered is  : ", m_pRenderer ? "Initialized\n" : "Uninitialized\n");
	m_oscilloscope.print("          ");
}

bool IO_Engine::init()
{
	SDL_Init(SDL_INIT_EVERYTHING);

	m_pWindow = SDL_CreateWindow(m_sWindowName.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_width, m_height, SDL_WINDOW_SHOWN);
	if (!m_pWindow)
	{
		ns_Util::Logger::LOG_SDL_ERROR("Failed to create window");
		return false;
	}

	m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, SDL_RENDERER_ACCELERATED);
	if (!m_pRenderer)
	{
		ns_Util::Logger::LOG_SDL_ERROR("Failed to create renderer");
		return false;
	}

	if (!m_oscilloscope.init())
	{
		ns_Util::Logger::LOG_SDL_ERROR("Failed to init Oscilloscope.");
		return false;
	}
	return true;
}

void IO_Engine::loop()
{
	uint64_t lastTimeUpdate = SDL_GetTicks64();
	uint64_t currTimeUpdate = 0;
	uint64_t elapsedTime = 0;
	while (m_bRunning)
	{
		currTimeUpdate = SDL_GetTicks64();
		elapsedTime = currTimeUpdate - lastTimeUpdate;
		lastTimeUpdate = currTimeUpdate;

		handleInput();
		update(elapsedTime);
		render();

		if (elapsedTime < m_refreshIntervalInMs)
		{
			SDL_Delay(Uint32(m_refreshIntervalInMs - elapsedTime));
		}
	}
	stop();
}

void IO_Engine::handleInput()
{
	SDL_Event sdlEvent;
	m_sdlEvents.clear();
	while (SDL_PollEvent(&sdlEvent))
	{
		if (sdlEvent.type == SDL_QUIT || sdlEvent.type == SDL_KEYDOWN && sdlEvent.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
		{
			m_bRunning = false;
			break;
		}
		else
		{
			m_sdlEvents.push_back(sdlEvent);
		}
	}
}

void IO_Engine::exit()
{
	if (m_pRenderer)
	{
		SDL_DestroyRenderer(m_pRenderer);
		m_pRenderer = nullptr;
	}
	if (m_pWindow)
	{
		SDL_DestroyWindow(m_pWindow);
		m_pWindow = nullptr;
	}
	SDL_Quit();
}