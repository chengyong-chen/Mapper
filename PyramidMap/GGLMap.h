#pragma once

class CGGLMap
{
public:
	CGGLMap();
	~CGGLMap();
	
	virtual int DrawTile(int tx,int ty) override;	/* draw the tile to the current display */
	void ResetOnline(void);
	void ToMap(class GPXCoord *c,int *sx,int *sy);
	void FromMap(int sx,int sy,class GPXCoord *c);\

	bool LoadTile(kGUIString *url,kGUIString *fn,int x,int y,int serverid);
	void AddBadURL(kGUIString *url) {m_badurls.Add(url->GetString(),0);}

	void BrowseMap(void) {}

private:
	CGGPTile m_activeTiles[MAXDLS];
	CList m_badurls;

	static kGUIThread m_checkThread;
};
