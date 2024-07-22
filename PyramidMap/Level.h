#pragma once

class CLevel
{
public:
	CLevel(void);
	~CLevel(void);

private:
	int m_Width;		/* map widths in pixels for each zoom level */
	int m_Height;		/* map heights in pixels for each zoom level */

	int m_xTiles;
	int m_yTiles;

	double m_pixelsPerLonDegree;
	double m_pixelsPerLonRadian;
	double m_bitmapOrigo;
};
