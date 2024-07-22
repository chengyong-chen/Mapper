#pragma once

class CDib;

class CBmb : public CDib
{
	typedef struct tagBMBINFO
	{
		DWORD BeginPosition;
		DWORD BlockLength;
		WORD BlockWidth;
		WORD BlockHeight;
		WORD Originx;
		WORD Originy;
		HGLOBAL DatePoint;
	} BMBINFO;

public:
	CBmb();

	~CBmb() override;

protected:
	mutable CFile m_file;
	BMBINFO* m_pBmbInfo;
	unsigned short m_nHorzBlockNum;
	unsigned short m_nVertBlockNum;
public:
	bool Open(LPCTSTR lpszPathName) override;
	void Draw(Gdiplus::Graphics& g, const CRect& rect, CRect drawRect, int nAlpha) override;
	//virtual bool ReadOutBlockDate(CWnd* pWnd,CViewinfo& viewinfo,CDC* pDC,int IdleCount) override;
private:
	virtual void ReadBlock(long BlockNum);
	//void         ReleaseBlock(CWnd* pWnd,CRect rect);

public:
	CDib* Clone() const override;
	virtual void Copy(CBmb* BmbDen) const;

	CDib* GetSubDib(const CRect& rect) const override;
};
