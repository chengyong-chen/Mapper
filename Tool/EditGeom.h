#pragma once

class CGeom;
class CLayer;
class CViewinfo;
namespace Undoredo {
	class CActionStack;
};
using namespace std;
#include <set>

class __declspec(dllexport) CEditGeom
{
public:
	CEditGeom(const CViewinfo& viewinfo, Undoredo::CActionStack& actionstack);

	const CViewinfo& m_Viewinfo;
	Undoredo::CActionStack& m_actionstack;
	CLayer* m_pLayer;
	CGeom* m_pGeom;

	std::set<unsigned int> m_Anchors;
	std::pair<unsigned int, BYTE> m_Handle;

public:
	void SwitchAnchorsFocused(CDC* pDC) const;
	void SwitchAnchorsNormal(CDC* pDC) const;

	void MoveAnchor(CWnd* pWnd, const CSize& delta, bool bMatch);
	void ChangeAnchor(CWnd* pWnd, CDC* pDC, const CSize& delta, bool bMatch);
	void MoveContrl(CDC* pDC, const CSize& delta) const;
	void ChangeContrl(CWnd* pWnd, CDC* pDC, const CSize& delta);

	void AddAnchor(CWnd* pWnd, const CPoint& point, bool bTail);
	void AddAnchor(CWnd* pWnd, const CPoint point[], bool bTail);

	void NewGeom(CWnd* pWnd, CGeom* pGeom);
	void FocusAnchor(CWnd* pWnd, unsigned int nAnchor, bool isadd);

	void UnFocusAll(CWnd* pWnd);
	void UnFocusAnchors(CWnd* pWnd);

	bool Clear(CWnd* pWnd);
	bool Corner(CWnd* pWnd);
	bool Cornnect(CWnd* pWnd);
	bool IsBezierNull();
	bool IsTextPolyNull();

	bool PickControl(CWnd* pWnd, const CPoint& point);

public:
public:
	typedef enum
	{
		Nothing,
		FirstPolying,
		FirstBeziering,
		SecondPolying,
		SecondBeziering,
		Tailing,
		Heading,
		Inserting
	} Mode;

	Mode m_Mode;
};
