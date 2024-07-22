#pragma once

#include "../Framework/gfx.h"

#include "SymbolView.hpp"
#include "TabletView.hpp"

#include "../Dataview/Datainfo.h"
#include "../Action/ActionStack.h"
#include "../Layer/Layer.h"
#include "../Layer/LayerTree.h"

#include "../Style/Line.h"
#include "../Tool/Tool.h"

#include "PoleDlg.h"

#include "../Tool/Global.h"
#include "../Tool/ZoomTool.h"
#include "../Geometry/Global.h"
#include "../Geometry/Pole.h"

extern UNIT   a_UnitArray[3];
extern BYTE   a_nUnitIndex;

extern __declspec(dllimport) BYTE d_nUnitIndex;
extern __declspec(dllimport) CZoomTool zoominTool;

template<class T>
class CSymbolDoc : public COleDocument
{
	DECLARE_DYNCREATE_T(CSymbolDoc, T)
protected:
	CSymbolDoc();

public:
	CLIPFORMAT m_ClipboardFormat; // custom clipboard format

	// Attributes
public:
	CView* m_pSymbolView;
	CView* m_pTabletView;

public:
	CDatainfo m_Datainfo;
	CLayerTree m_layertree;
	CTool* m_pTool;
	CTool* m_oldTool;

public:
	Undoredo::CActionStack m_actionStack;
	// Operations
public:
	void Draw(CViewinfo& Viewinfo, Gdiplus::Graphics& g, const CRect& inRect);
	void Capture(CView* pView, CViewinfo& Viewinfo, Gdiplus::Graphics& g, const CRect& invalidRect);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSymbolDoc)
public:
	void Serialize(CArchive& ar) override;
	virtual void Serialize(CArchive& ar, const DWORD& dwVersion);
	BOOL OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) override;
	//}}AFX_VIRTUAL

	// Implementation
public:
#ifdef _DEBUG
	void AssertValid() const override;
	void Dump(CDumpContext& dc) const override;
#endif

protected:

	// Generated message map functions
protected:
	//{{AFX_MSG(CSymbolDoc)
	afx_msg void OnFileExportce();
	afx_msg void OnEditUndo();
	afx_msg void OnEditRedo();
	afx_msg void OnEditClone();
	afx_msg void OnEditClear();
	afx_msg void OnEditCut();
	afx_msg void OnEditDublicate();
	afx_msg void OnEditPaste();
	afx_msg void OnEditCopy();
	afx_msg void OnPoleMake();
	afx_msg void OnPoleUndo();
	afx_msg void OnPoleSetup();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

IMPLEMENT_DYNCREATE_T(CSymbolDoc, T, COleDocument)

BEGIN_TEMPLATE_MESSAGE_MAP(CSymbolDoc, T, COleDocument)
	//{{AFX_MSG_MAP(CSymbolDoc)
	ON_COMMAND(ID_FILE_EXPORTCE, OnFileExportce)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_COMMAND(ID_EDIT_CLONE, OnEditClone)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_DUBLICATE, OnEditDublicate)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_POLE_MAKE, OnPoleMake)
	ON_COMMAND(ID_POLE_UNDO, OnPoleUndo)
	ON_COMMAND(ID_POLE_SETUP, OnPoleSetup)
	//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

template<class T>
CSymbolDoc<T>::CSymbolDoc()
	:m_actionStack(*this)
{
	m_ClipboardFormat = (CLIPFORMAT)::RegisterClipboardFormat(_T("Diwatu Mapper Grf"));

	EnableCompoundFile(FALSE);
	m_layertree.m_root.m_bDynamic = 0XFF;
	m_Datainfo.m_mapCanvas = CSizeF(200, 200);
	m_Datainfo.m_mapOrigin = CPointF(-100, -100);

	m_Datainfo.m_scaleMaximum = 0.01;
	m_Datainfo.m_scaleSource = 1.f;
	m_Datainfo.m_scaleMinimum = 10;
	m_Datainfo.m_pProjection = nullptr;


	m_pTool = &zoominTool;
	m_oldTool = nullptr;
}


template<class T>
void CSymbolDoc<T>::Serialize(CArchive& ar)
{
	DWORD dwCurrent = AfxGetCurrentArVersion();
	DWORD dwVersion = dwCurrent;
	if(ar.IsStoring())
	{
		ar << dwCurrent;
	}
	else
	{
		ar >> dwVersion;
	}
	CSymbolDoc<T>::Serialize(ar, dwVersion);
}


template<class T>
void CSymbolDoc<T>::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CTabletView<T>* pTabletView = (CTabletView<T>*)m_pTabletView;
	CListCtrl& pList = (CListCtrl&)pTabletView->GetListCtrl();
	if(ar.IsStoring())
	{
		pTabletView->FetchGeomlist();
		ar << (int)pList.GetItemCount();
		for(int index = 0; index < pList.GetItemCount(); index++)
		{
			T* pSymbol = (T*)pList.GetItemData(index);
			if(pSymbol != nullptr)
			{
				pSymbol->Serialize(ar, dwVersion);
			}
		}
	}
	else
	{
		pList.DeleteAllItems();

		int nCount;
		ar >> nCount;
		for(int index = 0; index < nCount; index++)
		{
			T* pSymbol = new T();//CSymbol::Apply(type);
			if(pSymbol != nullptr)
			{
				pSymbol->Serialize(ar, dwVersion);
				pList.InsertItem(LVIF_TEXT | LVIF_PARAM, pList.GetItemCount(), pSymbol->m_strName, 0, 0, -1, (DWORD)pSymbol);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSymbolDoc diagnostics

#ifdef _DEBUG
template<class T>
void CSymbolDoc<T>::AssertValid() const
{
	COleDocument::AssertValid();
}
template<class T>
void CSymbolDoc<T>::Dump(CDumpContext& dc) const
{
	COleDocument::Dump(dc);
}
#endif //_DEBUG
template<class T>
BOOL CSymbolDoc<T>::OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if(pHandlerInfo != nullptr)
		return COleDocument::OnCmdMsg(nId, nCode, pExtra, pHandlerInfo);
	if(nCode != CN_UPDATE_COMMAND_UI)
		return COleDocument::OnCmdMsg(nId, nCode, pExtra, pHandlerInfo);
	CCmdUI* pCmdUI = (CCmdUI*)pExtra;
	if(pCmdUI->m_bContinueRouting == TRUE)
		return COleDocument::OnCmdMsg(nId, nCode, pExtra, pHandlerInfo);

	const unsigned long nActiveCount = m_layertree.m_root.m_nActiveCount;
	ASSERT(!pCmdUI->m_bContinueRouting);
	switch(nId)
	{
		case ID_EDIT_UNDO:
			break;
		case ID_EDIT_REDO:
			break;
		case ID_EDIT_CUT:
		case ID_EDIT_COPY:
			pCmdUI->Enable(nActiveCount == 0 ? FALSE : TRUE);
			return TRUE;
			break;
		case ID_EDIT_PASTE:
			{
				COleDataObject dataObject;
				const BOOL bEnable = dataObject.AttachClipboard() && (dataObject.IsDataAvailable(m_ClipboardFormat) || COleClientItem::CanCreateFromData(&dataObject));
				pCmdUI->Enable(bEnable);
				return TRUE;
			}
			break;
		case ID_POLE_MAKE:
			if(this->IsKindOf(RUNTIME_CLASS_T(CSymbolDoc, CSymbolLine)) == TRUE)
			{
				CTypedPtrList<CObList, CGeom*>& geomlist = m_layertree.m_root.GetGeomList();
				POSITION pos = geomlist.GetHeadPosition();
				while(pos != nullptr)
				{
					CGeom* geom = geomlist.GetNext(pos);
					if(geom->m_bActive == false)
						continue;
					if(geom->IsKindOf(RUNTIME_CLASS(CPole)) == TRUE)
						continue;

					pCmdUI->Enable(TRUE);
					return TRUE;
				}
				pCmdUI->Enable(FALSE);
			}
			else
			{
				pCmdUI->Enable(FALSE);
			}
			return TRUE;
			break;
		case ID_POLE_SETUP:
		case ID_POLE_UNDO:
			if(this->IsKindOf(RUNTIME_CLASS_T(CSymbolDoc, CSymbolLine)) == TRUE)
			{
				CTypedPtrList<CObList, CGeom*>& geomlist = m_layertree.m_root.GetGeomList();
				POSITION pos = geomlist.GetTailPosition();
				while(pos != nullptr)
				{
					CGeom* geom = geomlist.GetPrev(pos);
					if(geom->m_bActive == false)
						continue;

					if(geom->IsKindOf(RUNTIME_CLASS(CPole)) == TRUE)
					{
						pCmdUI->Enable(TRUE);
						return TRUE;
					}
				}
				pCmdUI->Enable(FALSE);
			}
			else
			{
				pCmdUI->Enable(FALSE);
			}
			return TRUE;
			break;
		case ID_FILE_SAVE:
			pCmdUI->Enable(IsModified());
			return TRUE;
			break;
	}

	pCmdUI->m_bContinueRouting = FALSE;
	return COleDocument::OnCmdMsg(nId, nCode, pExtra, pHandlerInfo);
}
/////////////////////////////////////////////////////////////////////////////
// CSymbolDoc commands
template<class T>
void CSymbolDoc<T>::OnPoleMake()
{
	CTabletView<T>* pTabletView = (CTabletView<T>*)m_pTabletView;
	CSymbolView<T>* pSymbolView = (CSymbolView<T>*)m_pSymbolView;
	pSymbolView->m_editgeom.UnFocusAll(pSymbolView);

	CPole* pole = new CPole();
	pole->m_bDirection = true;
	pole->m_bActive = true;

	CRect mRect;
	mRect.SetRectEmpty();

	CTypedPtrList<CObList, CGeom*>& geomlist = m_layertree.m_root.GetGeomList();
	POSITION oPos = geomlist.GetHeadPosition();
	POSITION poPos;

	while((poPos = oPos) != nullptr)
	{
		CGeom* geom = geomlist.GetNext(oPos);
		if(geom->m_bActive == false)
			continue;

		if(geom->IsKindOf(RUNTIME_CLASS(CPole)) == TRUE)
		{
			AfxMessageBox(_T("The Selected geometry is already a Pole!"));
			continue;
		}

		geom->m_bActive = false;
		//		geom->DrawBlackAnchor(this,0,0);
		m_layertree.m_root.m_nActiveCount--;

		geomlist.RemoveAt(poPos);

		pole->m_geomlist.AddTail(geom);

		const unsigned long inflate = ((CLine*)(geom->m_pLine))->m_nWidth/2 + 1;

		CRect rect = geom->m_Rect;
		rect.InflateRect(inflate, inflate);

		mRect = UnionRect(mRect, rect);
	}

	if(pole->m_geomlist.IsEmpty() == false)
	{
		const CPoint center = mRect.CenterPoint();
		pole->m_Center = CPoint(center.x, center.y);
		pole->m_Rect = mRect;

		oPos = pole->m_geomlist.GetHeadPosition();
		while(oPos != nullptr)
		{
			CGeom* geom = pole->m_geomlist.GetNext(oPos);
			CSize Δ = CSize(-pole->m_Center.x, -pole->m_Center.y);
			geom->Move(Δ);
		}

		geomlist.AddTail(pole);
		m_layertree.m_root.m_nActiveCount++;
	}
	else
	{
		delete pole;
		pole = nullptr;
	}
	pSymbolView->Invalidate();
}
template<class T>
void CSymbolDoc<T>::OnPoleUndo()
{
	CTypedPtrList<CObList, CGeom*>& geomlist = m_layertree.m_root.GetGeomList();
	POSITION oPos = geomlist.GetTailPosition();
	POSITION poPos;

	while((poPos = oPos) != nullptr)
	{
		CGeom* geom = geomlist.GetPrev(oPos);
		if(geom->m_bActive == false)
			continue;

		if(geom->IsKindOf(RUNTIME_CLASS(CPole)) == false)
		{
			AfxMessageBox(_T("The selected geometry isn't a pole!"));
			continue;
		}

		geomlist.RemoveAt(poPos);

		CPole* pole = (CPole*)geom;
		POSITION pos = pole->m_geomlist.GetHeadPosition();
		while(pos != nullptr)
		{
			CGeom* mObj = pole->m_geomlist.GetNext(pos);
			CSize Δ = CSize(pole->m_Center.x, pole->m_Center.y);
			mObj->Move(Δ);
			mObj->m_bActive = true;
			geomlist.AddTail(mObj);
			m_layertree.m_root.m_nActiveCount++;
		}

		pole->m_geomlist.RemoveAll();
		delete pole;
		pole = nullptr;
		m_layertree.m_root.m_nActiveCount--;
	}

	CTabletView<T>* pTabletView = (CTabletView<T>*)m_pTabletView;
	CSymbolView<T>* pSymbolView = (CSymbolView<T>*)m_pSymbolView;
	pSymbolView->Invalidate();
}
template<class T>
void CSymbolDoc<T>::Draw(CViewinfo& Viewinfo, Gdiplus::Graphics& g, const CRect& inRect)
{
	CTypedPtrList<CObList, CGeom*>& geomlist = m_layertree.m_root.GetGeomList();
	POSITION pos = geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = geomlist.GetNext(pos);
		pGeom->Draw(g, Viewinfo, Viewinfo.CurrentRatio());
	}
}
template<class T>
void CSymbolDoc<T>::Capture(CView* pView, CViewinfo& Viewinfo, Gdiplus::Graphics& g, const CRect& invalidRect)
{
	if(invalidRect.IsRectNull())
		return;

	AfxGetApp()->BeginWaitCursor();
	m_layertree.m_root.Capture(g, Viewinfo, invalidRect);
	AfxGetApp()->EndWaitCursor();
}
template<class T>
void CSymbolDoc<T>::OnEditClone()
{
	CTabletView<T>* pTabletView = (CTabletView<T>*)m_pTabletView;
	CSymbolView<T>* pSymbolView = (CSymbolView<T>*)m_pSymbolView;
	m_layertree.m_root.Duplicate(pSymbolView, m_Datainfo, pSymbolView->m_viewMonitor, CSize(0, 0), m_actionStack);
}

template<class T>
void CSymbolDoc<T>::OnEditClear()
{
	CTabletView<T>* pTabletView = (CTabletView<T>*)m_pTabletView;
	CSymbolView<T>* pSymbolView = (CSymbolView<T>*)m_pSymbolView;
	if(pSymbolView->m_editgeom.Clear(pSymbolView))
		return;

	m_layertree.m_root.Clear(pSymbolView, pSymbolView->m_viewMonitor, m_actionStack);
	SetModifiedFlag(TRUE);
}
template<class T>
void CSymbolDoc<T>::OnEditCut()
{
	// TODO: Add your command handler code here
	OnEditCopy();
	OnEditClear();
}
template<class T>
void CSymbolDoc<T>::OnEditDublicate()
{
	CTabletView<T>* pTabletView = (CTabletView<T>*)m_pTabletView;
	CSymbolView<T>* pSymbolView = (CSymbolView<T>*)m_pSymbolView;
	if(pSymbolView == nullptr)
	{
		return;
	}

	m_layertree.m_root.Duplicate(pSymbolView, m_Datainfo, pSymbolView->m_viewMonitor, CSize(50, 50), m_actionStack);
	SetModifiedFlag(TRUE);
}
template<class T>
void CSymbolDoc<T>::OnEditCopy()
{
	ASSERT_VALID(this);
	ASSERT(m_ClipboardFormat != NULL);

	CSharedFile file;
	CArchive ar(&file, CArchive::store);

	CLayerTree layertree;
	CLayer* mylayer = new CLayer(layertree, m_Datainfo.m_scaleMinimum, m_Datainfo.m_scaleMaximum, m_Datainfo.m_scaleMinimum, m_Datainfo.m_scaleMaximum);
	m_layertree.m_root.CopyTo(mylayer);

	CTabletView<T>* pTabletView = (CTabletView<T>*)m_pTabletView;
	CSymbolView<T>* pSymbolView = (CSymbolView<T>*)m_pSymbolView;
	const CRect* rect = (CRect*)(pSymbolView->SendMessage(WM_GETACTIVERECT, 0, 0));
	CPoint objcenter = rect->CenterPoint();
	objcenter.x = rect->left + rect->Width()/2;//µ±zoomRectµÄÖµ¹ý´óÊ±£¬CenterPoint£¨£©º¯ÊýÖ´ÐÐÓÐÎó
	objcenter.y = rect->top + rect->Height()/2;
	DWORD dwVersion = AfxGetCurrentArVersion();
	ar << dwVersion;
	ar << objcenter;
	CDatainfo datainfo;
	datainfo.Serialize(ar, dwVersion);
	layertree.Serialize(ar, dwVersion);
	ar.Close();

	mylayer->m_geomlist.RemoveAll();
	mylayer->m_pSpot = nullptr;
	mylayer->m_pLine = nullptr;
	mylayer->m_pFill = nullptr;
	mylayer->m_pType = nullptr;
	delete mylayer;
	mylayer = nullptr;

	COleDataSource* pDataSource = nullptr;
	try
	{
		pDataSource = new COleDataSource;
		pDataSource->CacheGlobalData(m_ClipboardFormat, file.Detach());
		pDataSource->SetClipboard();
	}
	catch(CException* ex)
	{
		delete pDataSource;
		THROW_LAST();
	}
}

template<class T>
void CSymbolDoc<T>::OnEditPaste()
{
	CTabletView<T>* pTabletView = (CTabletView<T>*)m_pTabletView;
	CSymbolView<T>* pSymbolView = (CSymbolView<T>*)m_pSymbolView;

	pSymbolView->InactivateAll();

	COleDataObject dataObject;
	dataObject.AttachClipboard();

	if(dataObject.IsDataAvailable(m_ClipboardFormat))
	{
		CFile* pFile = dataObject.GetFileData(m_ClipboardFormat);
		if(pFile == nullptr)
			return;

		CArchive ar(pFile, CArchive::load);
		try
		{
			ar.m_pDocument = this;

			DWORD dwVersion;
			ar >> dwVersion;
			CPoint objcenter;
			ar >> objcenter;

			CRect rect;
			pSymbolView->GetClientRect(rect);
			CPoint cliPoint = rect.CenterPoint();
			const CPoint docPoint = pSymbolView->m_viewMonitor.ClientToDoc(cliPoint, false);

			CSize Δ = docPoint - objcenter;

			CLayerTree layertree;
			CDatainfo datainfo;
			datainfo.Serialize(ar, dwVersion);
			layertree.Serialize(ar, dwVersion);

			for(auto it = layertree.forwbegin();it!=layertree.forwend();++it)
			{
				CLayer* source = *it;
				POSITION pos = source->m_geomlist.GetTailPosition();
				while(pos!=NULL)
				{
					CGeom* pGeom = source->m_geomlist.GetPrev(pos);
					pGeom->Move(Δ);					
					pGeom->m_bActive = true;
					
					m_layertree.m_root.m_geomlist.AddTail(pGeom);
					m_layertree.m_root.m_nActiveCount++;
					m_layertree.m_root.SetModifiedFlag(true);

					m_layertree.m_root.Invalidate(pSymbolView, pSymbolView->m_viewMonitor, pGeom);
				}
				source->m_geomlist.RemoveAll();
			}
		}
		catch(CException* ex)
		{
			ar.Close();
			delete pFile;
			THROW_LAST();
		}

		ar.Close();
		delete pFile;
	}
	SetModifiedFlag(TRUE);
	//	CCommand* command = new CCommand("None");
	//	m_hubCommand.Submit(command);
}
template<class T>
void CSymbolDoc<T>::OnEditUndo()
{
	// TODO: Add your command handler code here

}

template<class T>
void CSymbolDoc<T>::OnEditRedo()
{
	// TODO: Add your command handler code here
}

template<class T>
void CSymbolDoc<T>::OnPoleSetup()
{
	CTypedPtrList<CObList, CGeom*>& geomlist = m_layertree.m_root.GetGeomList();
	POSITION oPos = geomlist.GetHeadPosition();
	while(oPos != nullptr)
	{
		CGeom* pGeom = geomlist.GetNext(oPos);
		if(pGeom->m_bActive == false)
			continue;

		if(pGeom->IsKindOf(RUNTIME_CLASS(CPole)) == false)
		{
			AfxMessageBox(_T("The selected geometry isn't a pole!"));
			continue;
		}

		CPole* pole = (CPole*)pGeom;
		CPoleDlg dlg(NULL, pole->m_Center.x, pole->m_Center.y, pole->m_bDirection);
		if(dlg.DoModal() == IDOK)
		{
			pole->m_bDirection = dlg.m_bDirection;

			const long x = round(dlg.m_fX*a_UnitArray[d_nUnitIndex].pointpro*10000);
			const long y = round(dlg.m_fY*a_UnitArray[d_nUnitIndex].pointpro*10000);

			CSize Δ = CSize(pole->m_Center.x - x, pole->m_Center.y - y);

			if(Δ == CSize(0, 0))
				break;

			POSITION oPos = pole->m_geomlist.GetHeadPosition();
			while(oPos != nullptr)
			{
				CGeom* geom = pole->m_geomlist.GetNext(oPos);
				geom->Move(Δ);
			}
			pole->m_Center.x = x;
			pole->m_Center.y = y;
		}
	}
	SetModifiedFlag(TRUE);
}

template<class T>
void CSymbolDoc<T>::OnFileExportce()
{
	CTabletView<T>* pTabletView = (CTabletView<T>*)m_pTabletView;
	CString strPath = this->GetPathName();
	if(strPath.IsEmpty() == FALSE)
	{
		strPath.Replace(_T(".lym"), _T(".lce"));
		strPath.Replace(_T(".mym"), _T(".mce"));
		strPath.Replace(_T(".fym"), _T(".fce"));

		CFile  file;
		if(file.Open(strPath, CFile::modeCreate | CFile::modeWrite) == TRUE)
		{
			CArchive ar(&file, CArchive::store);
			pTabletView->FetchGeomlist();
			const float fVersion = AfxGetSoftVersion();
			ar << fVersion;
			const CListCtrl& pList = (CListCtrl&)pTabletView->GetListCtrl();
			const int nCount = pList.GetItemCount();
			ar << nCount;
			for(int index = 0; index < nCount; index++)
			{
				T* pSymbol = (T*)pList.GetItemData(index);
				if(pSymbol != nullptr)
				{
					const BYTE type = pSymbol->Gettype();
					ar << type;
					pSymbol->ReleaseCE(ar);
				}
			}
			ar.Close();
			file.Close();
		}
	}
};
