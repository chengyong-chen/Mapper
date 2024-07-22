#pragma once

#include "Resource.h"
#include "PRect.h"

class CDib;

class AFX_EXT_CLASS CImager sealed : public CPRect
{
protected:
	DECLARE_SERIAL(CImager);
	CImager();

public:
	CImager(const CPoint& origin, const CDatainfo* pDatainfo, CString lpszPathName);
	CImager(CRect rect, CDib* pDib);

	unsigned char m_bEmbeded;
	CString m_strFile;

	unsigned char m_nAlpha;
	CDib* m_pDib;

public:
	void Sha1(boost::uuids::detail::sha1& sha1) const override;	
	void Sha1(boost::uuids::detail::sha1& sha1, const CSize offset) const override;
	// Implementation
public:
	float GetInflation(const CLine* pLine, const CType* pType, const float& ratioLine, const float& ratioType) const override { return 0.f; };
	void Draw(Gdiplus::Graphics& g, const Gdiplus::Matrix& matrix, Context& context, const CSize& dpi) const override;
	void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const float& ratio) const override;
	void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const override;
	
public:
	BOOL operator ==(const CGeom& geom) const override;
	void CopyTo(CGeom* pGeom, bool ignore = false) const override;
	void Swap(CGeom* pGeom) override;
	bool UnGroup(CTypedPtrList<CObList, CGeom*>& geomlist) override { return false; };
	CGeom* Clip(const CRect& bound, Clipper2Lib::Paths64& clips, bool in, int tolerance, bool bStroke) override;
	bool PickOn(const CPoint& point, const unsigned long& gap) const override { return PickIn(point); }
	void Attribute(CWnd* pWnd, const CViewinfo& viewinfo) override;
public:
	~CImager() override;
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
	void ReleaseCE(CArchive& ar) const override;
	void ReleaseDCOM(CArchive& ar) override;
	void ReleaseWeb(boost::json::object& json) const override;
	void ReleaseWeb(pbf::writer& writer, const CSize offset = CSize(0, 0)) const override;
	DWORD PackPC(CFile* pFile, BYTE*& bytes) override;
	DWORD ReleaseCE(CFile& file, const BYTE& type) const override;
	void ExportPs3(FILE* file, const Gdiplus::Matrix& matrix, CPsboard& aiKey) const override;
	void ExportPs3(FILE* file, const CViewinfo& viewinfo, CPsboard& aiKey) const override;
	void ExportPdf(HPDF_Doc pdf, HPDF_Page page, const Gdiplus::Matrix& matrix) const override;
	void ExportPdf(HPDF_Doc pdf, HPDF_Page page, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const override;
};

class CImagerDlg : public CDialog
{
	DECLARE_DYNAMIC(CImagerDlg)

public:
	CImagerDlg(CWnd* pParent, CImager* imager); 
	~CImagerDlg() override;

	enum
	{
		IDD = IDD_IMAGE
	};

	CImager* m_imager;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImagerDlg)
protected:
	void OnOK() override;
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV ֧��
	//}}AFX_VIRTUAL

	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CImagerDlg)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChangeEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	BOOL OnInitDialog() override;
	afx_msg void OnBnClickedEmbeded();
	afx_msg void OnBnClickedLinked();
};
