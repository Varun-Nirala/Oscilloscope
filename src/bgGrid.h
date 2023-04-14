#pragma once

#include <SDL.h>
#include <vector>

class BgGrid
{
public:
	BgGrid(int screenWidth, int screenHeight, int numOfCols);

	void draw(SDL_Renderer *pRenderer);
private:
	void calculateGridPoints();
	void drawFineLine(SDL_Renderer* pRenderer, int x1, int y1, int x2, int y2);
	void drawGridLine(SDL_Renderer* pRenderer, int x1, int y1, int x2, int y2);
	void drawAxisLine(SDL_Renderer* pRenderer, int x1, int y1, int x2, int y2);
private:
	const int			m_numOfCols{};
	const int			m_screenWidth{};
	const int			m_screenHeight{};
	const int			m_centreX{};
	const int			m_centreY{};

	float				m_axisLineSF{6.0f};		// Axis line scale factor
	float				m_gridLineSF{3.0f};		// Grid line scale factor
	float				m_fineLineSF{1.0f};		// In between grid line scale factor

	SDL_Color			m_backGround{ 34, 139, 34 };		// Forest Green
	SDL_Color			m_axisColor{ 0, 0, 0 };				// Black
	SDL_Color			m_gridLineColor{ 0, 100, 0 };		// Green
	SDL_Color			m_fineLineColor{ 0, 100, 0 };		// Green

	std::vector<int>	m_vecX;
	std::vector<int>	m_vecY;
};
