#pragma once

class CMap abstract: public CObject
{
public:
	CMap();
	virtual ~CMap();

private:
	int m_nLevel;
	int m_minLevel;
	int m_maxLevel;
	CArray<CLevel> m_arrLevel;
	CString m_strURL;
	CString m_Version;

private:
	bool m_bad;
	bool m_bAsync;

public:
	void SetLevel(int level) {m_nLevel=level;}
	int GetLevel(void) {return m_nLevel;}	
	int GetMaxLevel(void) {return m_maxLevel;}
	int GetMinLevel(void) {return m_minLevel;}

	void SetSize(int level,int width,int height);
	
	int GetWidth(void) {return m_mapwidths.GetEntry(m_Level);}		/* return width of map in pixels for current Level level */
	int GetHeight(void) {return m_mapheights.GetEntry(m_Level);}		/* return height of map in pixels for current Level level */
	int GetTileWidth(void) {return m_tilewidth;}
	int GetTileHeight(void) {return m_tileheight;}
	void SetLevelLevels(int minz,int maxz) {m_minLevel=minz;m_maxLevel=maxz;}
	void SetTileSize(int tilew,int tileh) {m_tilewidth=tilew;m_tileheight=tileh;}

	virtual void ResetOnline(void) override {}	/* only used in online based maps */
	virtual void ToMap(class GPXCoord *c,int *sx,int *sy) override=0;
	virtual void FromMap(int sx,int sy,class GPXCoord *c) override=0;
	virtual int DrawTile(int row,int col) override=0;	/* draw the tile to the current display */
	
	virtual int GetNumCopyrightLines(void) override=0;
	virtual CString GetCopyrightLine(int l) override=0;
	virtual CString GetStatusLine(void) override {return 0;}
};
