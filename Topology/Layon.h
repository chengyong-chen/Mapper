#pragma once

#include "../Layer/Layer.h"

class CRegionTopoer;

class __declspec(dllexport) CLayon : public CLayer
{
public:
	CLayon(CTree<CLayer>& tree);
	CLayon(CTree<CLayer>& tree, float minLevelObj, float maxLevelObj, float minLevelTag, float maxLevelTag);

	~CLayon() override;

public:
	BYTE Gettype() const override { return 11; };

public:	
	CRegionTopoer* m_pRegionTopoer;

public:
	bool IsControllable() const override { return false; }
	bool Copyable() const override { return false; };

public:	
	bool CopyGeomTo(CLayer* layerSrc, function<void(CString)> dbcopied) const override { return false; };
	void CopyTo(CLayer* pLayer) const override {};
	void Group(CWnd* pWnd, const CViewinfo& viewinfo, Undoredo::CActionStack& actionstack) override {};
	void Align(CWnd* pWnd, const CViewinfo& viewinfo, Undoredo::CActionStack& actionstack, CRect rect, ALIGN align) override {};
	bool Join(CWnd* pWnd, const CViewinfo& viewinfo, const unsigned int& tolerance, Undoredo::CActionStack& actionstack, std::function<bool(WORD wLayer, DWORD dwPoly1, DWORD dwPoly2, const BYTE& mode)>& joinable, std::function<void(WORD wLayer, DWORD dwPoly1, DWORD dwPoly2, const BYTE& mode)>& joined) override { return false; };
	bool Combine(CWnd* pWnd, const CDatainfo& datainfo, const CViewinfo& viewinfo, Undoredo::CActionStack& actionstack) override { return false; };
	bool Split(CWnd* pWnd, const CViewinfo& viewinfo, const CPoint& local, Undoredo::CActionStack& actionstack, std::function<void(CLayer* pLayer, DWORD dwPoly, const CPoint& point, DWORD dwBred)>& splitted) override { return false; };
	bool Replace(CWnd* pWnd, const CViewinfo& viewinfo, CGeom* pGeom, std::list<CGeom*> geoms, Undoredo::CActionStack& actionstack) override { return false; };
	void Move(CWnd* pWnd, const CViewinfo& viewinfo, Undoredo::CActionStack& actionstack, const CSize& Δ) override {};
	void Arrange(CWnd* pWnd, const CViewinfo& viewinfo, Undoredo::CActionStack& actionstack, ARRANGE arrange) override {};
	void Close(CWnd* pWnd, const CViewinfo& viewinfo, const unsigned int& tolerence, Undoredo::CActionStack& actionstack) override {};
public:
	void Serializelist(CArchive& ar, const DWORD& dwVersion) override;
};
