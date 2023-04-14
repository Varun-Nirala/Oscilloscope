#include "bgGrid.h"

BgGrid::BgGrid(int screenWidth, int screenHeight, int numOfCols)
	: m_numOfCols(numOfCols)
    , m_screenWidth(screenWidth)
    , m_screenHeight(screenHeight)
	, m_centreX{ screenWidth / 2 }
	, m_centreY{ screenHeight / 2 }
{
    calculateGridPoints();
}

void BgGrid::draw(SDL_Renderer* pRenderer)
{
    // Fill background
    SDL_SetRenderDrawColor(pRenderer, m_backGround.r, m_backGround.g, m_backGround.b, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(pRenderer);

    for (size_t i = 0, size = m_vecY.size(); i < size; ++i)
    {
        drawFineLine(pRenderer, 0, m_vecY[i], m_screenWidth, m_vecY[i]);
    }

    for (size_t i = 0, size = m_vecX.size(); i < size; ++i)
    {
        drawFineLine(pRenderer, m_vecX[i], 0, m_vecX[i], m_screenHeight);
    }

    drawAxisLine(pRenderer, m_centreX, 0, m_centreX, m_screenHeight);
    drawAxisLine(pRenderer, 0, m_centreY, m_screenWidth, m_centreY);
}

void BgGrid::calculateGridPoints()
{
    m_vecX.clear();
    m_vecY.clear();

    const int lineGap = m_screenWidth / (2 * m_numOfCols * 10);

    m_vecX.push_back(m_centreX);
    for (int rt = m_centreX + lineGap, lt = m_centreX - lineGap; lt > 0 && rt < m_screenWidth; rt += lineGap, lt -= lineGap)
    {
        m_vecX.push_back(rt);
        m_vecX.push_back(lt);
    }

    m_vecY.push_back(m_centreY);
    for (int down = m_centreY + lineGap, up = m_centreY - lineGap; up > 0 && down < m_screenHeight; down += lineGap, up -= lineGap)
    {
        m_vecY.push_back(down);
        m_vecY.push_back(up);
    }
}

void BgGrid::drawFineLine(SDL_Renderer* pRenderer, int x1, int y1, int x2, int y2)
{
    SDL_SetRenderDrawColor(pRenderer, m_fineLineColor.r, m_fineLineColor.g, m_fineLineColor.b, SDL_ALPHA_OPAQUE);
    SDL_RenderSetScale(pRenderer, m_fineLineSF, m_fineLineSF);
    SDL_RenderDrawLine(pRenderer, x1, y1, x2, y2);
}

void BgGrid::drawGridLine(SDL_Renderer* pRenderer, int x1, int y1, int x2, int y2)
{
    SDL_SetRenderDrawColor(pRenderer, m_gridLineColor.r, m_gridLineColor.g, m_gridLineColor.b, SDL_ALPHA_OPAQUE);
    SDL_RenderSetScale(pRenderer, m_gridLineSF, m_gridLineSF);
    SDL_RenderDrawLine(pRenderer, x1, y1, x2, y2);
}

void BgGrid::drawAxisLine(SDL_Renderer* pRenderer, int x1, int y1, int x2, int y2)
{
    SDL_SetRenderDrawColor(pRenderer, m_axisColor.r, m_axisColor.g, m_axisColor.b, SDL_ALPHA_OPAQUE);
    SDL_RenderSetScale(pRenderer, m_axisLineSF, m_axisLineSF);
    SDL_RenderDrawLine(pRenderer, x1, y1, x2, y2);
}