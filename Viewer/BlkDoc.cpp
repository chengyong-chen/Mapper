#include "stdafx.h"
#include <afxpriv.h>
#include <io.h>

#include "Viewer.h"
#include "BlkView.h"
#include "BlkDoc.h"

#include "../Style/Block.h"



#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern __declspec( dllimport ) CHashtable* g_libPattern;

extern __declspec( dllimport ) CTagtable*  g_libLineSymbol;
extern __declspec( dllimport ) CTagtable*  g_libFillSymbol;
extern __declspec( dllimport ) CTagtable*  g_libSpotSymbol;

IMPLEMENT_DYNCREATE(CBlkDoc, CGisDoc)

BEGIN_MESSAGE_MAP(CBlkDoc, CGisDoc)
	//{{AFX_MSG_MAP(CBlkDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CBlkDoc::CBlkDoc()
{
	EnableCompoundFile(FALSE);
	
	m_nXCount = 0;
	m_nYCount = 0;
	m_nWidth  = 0;
	m_nHeight = 0;
}

CBlkDoc::~CBlkDoc()
{			
}		

void CBlkDoc::Draw(CView* pView,Graphics& g,DrawArgu& drawArgu)
{		
	CGisDoc::Draw(pView,g,drawArgu);
}

void CBlkDoc::DrawTag(CView* pView,Graphics& g,DrawArgu& drawArgu)
{

	CGisDoc::DrawTag(pView,g,drawArgu);
}

void CBlkDoc::Serialize(CArchive& ar, const DWORD& dwVersion)
{	
	CGisDoc::Serialize(ar, dwVersion);

	if(ar.IsLoading())
	{			
		ar >> m_nXCount;
		ar >> m_nYCount;
		ar >> m_nWidth;
		ar >> m_nHeight;
	}
	else
	{
	
	}
}

#ifdef _DEBUG
void CBlkDoc::AssertValid() const
{
	CGisDoc::AssertValid();
}

void CBlkDoc::Dump(CDumpContext& dc) const
{
	CGisDoc::Dump(dc);
}
#endif //_DEBUG