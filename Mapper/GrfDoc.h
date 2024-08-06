#pragma once

#include "Preference.h"

#include "../Dataview/Datainfo.h"

#include "../Style/Library.h"

#include "../Layer/LayerTree.h"
#include "../Layer/LayerTreeCtrl.h"

#include "../Action/ActionStack.h"

#include "../Rectify/Tin.h"

#include "../Tool/DatumTool.h"

#include "../Frame/Frame.h"

class CProjection;
class CTopoCtrl;
class CDeck;
class CTopology;
class CTool;

class CGrfDoc : public COleDocument
{
public:
	CGrfDoc() noexcept;
protected:
	DECLARE_DYNCREATE(CGrfDoc)
protected:
	CLIPFORMAT m_ClipboardFormat; // custom clipboard format

public:
	CDatainfo m_Datainfo;
	CLayerTree m_layertree;
	std::list<CTopology*> m_topolist;
	CTin m_Tin;
	CFrame m_frame;

public:
	CDatumTool m_datumTool;
	CTool* m_pTool = nullptr;
	CTool* m_oldTool = nullptr;

	CLayerTreeCtrl m_LayerTreeCtrl;
	CTopoCtrl* m_pTopoCtrl = nullptr;
	Preference preference;

public:
	DWORD m_dwMaxGeomId;
	Gdiplus::ARGB m_argbBackground = 0XFFFFFFFF;
	DWORD ApplyGeomId();
	void FreeGeomId(CGeom* pGeom);
	unsigned int TotalActivated();
	CGeom* GetTheActivatedGeom(CLayer*& pLayer);	

public:
	virtual void OnPublishPC(CDeck* pDeck, CDaoRecordset* rs);
	virtual void OnPublishCE(CDeck* pDeck);
	virtual BOOL Import(CString strFile);
	virtual void Paste(CLayerTree& layertree);
	virtual void ReleaseWEBCap(UINT nId, CString strFolder, CString strName);

public:
	Undoredo::CActionStack m_actionStack;
public:
	void PreRemoveGeom(CGeom* pGeom);

public:
	virtual void Draw(CView* pView, const CViewinfo& viewinfo, Gdiplus::Graphics& g, const CRect& invalidRect);
	virtual void Capture(CView* pView, const CViewinfo& viewinfo, Gdiplus::Graphics& g, const CRect& invalidRect);
	virtual void DrawTag(CView* pView, const CViewinfo& viewinfo, Gdiplus::Graphics& g, const CRect& invalidRect);

public:
	std::string Convert();
	void Transform(double m11, double m21, double m31, double m12, double m22, double m32);
	void ReleaseWEBVectorPage(CString strName, bool diwatu);

	int hiddenlevel = -1;
	Gdiplus::Bitmap* CreateThumbnail(int width, int height);

private:
	std::string CreateDiwatuEntity(bool bUpdate, CStringA maptype, CString strName, const char* blob, int minMapLevel, int maxMapLevel, float srcMapLevel, bool withLngLat, bool withGeoCode, CString strDescription, std::optional<int> minPartialLevel, std::optional<int> maxPartialLevel);
	CPath* GetUniqueClosedPath();
	void Replace(CTypedPtrList<CObList, CGeom*>& geomlist);
	

public:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGrfDoc)
public:
	BOOL OnNewDocument() override;
	BOOL OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) override;
	void PreCloseFrame(CFrameWnd* pFrameWnd) override;
	virtual void ReleaseCE(CArchive& ar) const;

protected:
	void Serialize(CArchive& ar) override;
	virtual void Serialize(CArchive& ar, const DWORD& dwVersion);
	//}}AFX_VIRTUAL

	// Implementation
public:
	~CGrfDoc() override;
#ifdef _DEBUG
	void AssertValid() const override;
	void Dump(CDumpContext& dc) const override;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CGrfDoc)	
	afx_msg void OnFileExport();
	afx_msg void OnFileMapinfo();
	afx_msg void OnProcessWipeout();
	afx_msg void OnProcessNormalize();
	afx_msg void OnProcessCleanDuplicated();
	afx_msg void OnProcessCleanOutofrange();
	afx_msg void OnProcessCleanOrphan();
	afx_msg void OnProcessAutoTagging();
	afx_msg void OnPolygonAutoColoring();
	afx_msg void OnEditPaste();
	afx_msg void OnEditCopy();
	afx_msg void OnEditMove();
	afx_msg void OnEditClear();
	afx_msg void OnEditCut();
	afx_msg void OnEditClone();
	afx_msg void OnEditDuplicate();
	
	afx_msg void OnEditTransform();
	afx_msg void OnEditUndo();
	afx_msg void OnEditRedo();
	afx_msg void OnEditClipIn();
	afx_msg void OnEditClipOut();
	afx_msg void OnEditClipRelease();
	afx_msg void OnEditMaskRelease();
	afx_msg void OnEditInsertGPSRoute();
	afx_msg void OnEditInsertSubmap();

	afx_msg void OnRectify();
	afx_msg void OnDatumLoad();
	afx_msg void OnDatumStore();
	afx_msg void OnDatumClear();

	afx_msg void OnGeomLock();
	afx_msg void OnGeomUnlock();
	afx_msg void OnGeomGroup();
	afx_msg void OnGeomUngroup();
	afx_msg void OnGeomCombine();
	afx_msg void OnGeomDisband();
	afx_msg void OnAlign(UINT nId);

	afx_msg void OnAnnoSetup();
	afx_msg void OnAnnoClear();
	afx_msg void OnAnnoReplace();
	afx_msg void OnAnnoToText();

	afx_msg void OnTagDelete();
	afx_msg void OnTagSetup();
	afx_msg void OnTagCreate();
	afx_msg void OnTagCreateOptimized();

	afx_msg void OnPathReverse();
	afx_msg void OnPathDensify();
	afx_msg void OnPathSimplify();
	afx_msg void OnPathBuffer();
	afx_msg void OnPathClose();

	afx_msg void OnCovertBezierToPoly();
	afx_msg void OnCovertPolyToBezier();
	afx_msg void OnPolygonSculpt(UINT nId);
	afx_msg void OnGeogroidReset();
	afx_msg void OnLabeloidReset();
	afx_msg void OnGeogroidPlant();
	afx_msg void OnLabeloidPlant();
	afx_msg void OnPointRemove();
	afx_msg void OnPointCorner();
	afx_msg void OnPointConnect();

	afx_msg void OnReplaceWithMark();
	afx_msg void OnMapReIDAll();

	afx_msg void OnTopoPickTool();
	afx_msg void OnTopoEroseTool();
	afx_msg void OnTopoSplitTool();
	afx_msg void OnTopoCrossTool();
	afx_msg void OnTopoCreateEdges();
	afx_msg void OnTopoRemoveEdges();
	afx_msg void OnTopoEdgesJoin();
	afx_msg void OnRectifyToolDatum();

	afx_msg void OnReleaseWEBVectorPage();
	afx_msg void OnReleaseWEBVectorTile();
	afx_msg void OnReleaseWEBRasterTile();
	//}}AFX_MSG	
	DECLARE_MESSAGE_MAP()
};