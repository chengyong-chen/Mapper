#pragma once


// this structure holds all the tooltip information
struct ButtonInfo : public CObject
{
	UINT			nControlID;		// ID of the control
	UINT			nInfoSize;		// number of lines in the info
	CStringArray	nControlInfo;	// container of the information
	COLORREF		nTextColor;		// text color
	COLORREF		nBackColor;		// background color
};



class AFX_EXT_CLASS CButtonTag : public CWnd
{
// Construction
public:
	CButtonTag();  
	virtual ~CButtonTag();	// destructor

	void Create(CWnd* pWnd);
	void ErasePreviousToolTagDisplay(UINT);
	void ShowTag(UINT);		// explicitly shown the tooltip given the control ID

	// NOTE: the user must override the PreTranslateMessage in the calling window in order
	// to handle mousemovent. 
	void		ShowTag(CPoint&);		// called only during Mousemovement

	// tooltip functions
	BOOL		AddControlInfo(UINT, CStringArray&, COLORREF back=RGB(255, 255, 220), COLORREF text=RGB(0, 0, 0));
	BOOL		RemoveControlInfo(UINT);

	// inline functions
	void		SetFontSize(int size) { m_nFontSize = size; }

// Implementation
protected:
	CWnd*			m_pParentWnd;
	int				m_nHeight;
	int				m_nWidth;
	int				m_nFontSize;

	int				m_nTotalLine;
	int				m_maxCharInLine;   

	virtual void	CalculateInfoBoxRect(UINT nControlID, CRect* pInfoRect);
	virtual void	CalculateHeightAndWidth(CStringArray& straInfos);

private:
	ButtonInfo*	IsControlIDExisting(UINT);
	void			DisplayInfo(ButtonInfo*);

private:
	CObArray		m_aControlInfo;
	UINT			m_currentControlID;
};