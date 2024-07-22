#include "stdafx.h"

#include "Submap.h"
#include "GrfDoc.h"
#include "../DataView/viewinfo.h"
#include "../Geometry/Holder.h"
#include "../Topology/Topo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSubmap::CSubmap(Holder::Submap* holder)
	: CPRect(), m_pDocument(nullptr)
{
	m_bType = 41;
	holder->CPRect::CopyTo(this);

	m_strFile = holder->m_strFile;
	if(_taccess(m_strFile, 0)!=-1)
	{
		const CWinApp* pApp = AfxGetApp();
		CDocTemplate* pTemplate = pApp->m_pDocManager->GetBestTemplate(m_strFile); 
		if(pTemplate != nullptr)
		{
			CDocument* pDocument = pTemplate->CreateNewDocument();
			if(pDocument->OnOpenDocument(m_strFile) == TRUE)
			{
				m_pDocument = (CGrfDoc*)pDocument;
				pTemplate->RemoveDocument(pDocument);
			}
			else
			{
				delete pDocument;
				pDocument = nullptr;
			}
		}
	}
}
CSubmap::CSubmap(CString strFile, CGrfDoc* pDocument)
	: m_strFile(strFile), m_pDocument(pDocument), CPRect()
{
	if(pDocument!=nullptr)
	{
		m_Rect = pDocument->m_Datainfo.GetDocRect();
		m_Rect.OffsetRect(-m_Rect.left, -m_Rect.top);
	}
	m_bType = 41;
}
CSubmap::~CSubmap()
{
	if(m_pDocument!= nullptr)
	{
		CDocTemplate* pDocTemplate = m_pDocument->GetDocTemplate();
		if(pDocTemplate != nullptr)
		{
			pDocTemplate->RemoveDocument(m_pDocument);
		}
		m_pDocument->OnCloseDocument();
	//	delete m_pDocument;
		m_pDocument = nullptr;
	}
}
void CSubmap::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CPRect::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		CString strFile = m_strFile;
		if(strFile.IsEmpty()==FALSE)
			AfxToRelatedPath(ar.m_strFileName, strFile);
		ar<<strFile;
	}
	else
	{		
		ar>>m_strFile;
		AfxToFullPath(ar.m_strFileName, m_strFile);	
	}
}
void CSubmap::Draw(Gdiplus::Graphics& g, const Gdiplus::Matrix& matrix, Context& context, const CSize& dpi) const
{
}

void CSubmap::Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const float& ratio) const
{
	if(m_pDocument == nullptr)
		return;

	CRect docRect = m_pDocument->m_Datainfo.GetDocRect();
	if(m_bRegular==true)
	{	
		int left;
		int top;
		viewinfo.DocToClient<int>(m_Rect.left, m_Rect.bottom,left, top);
		Gdiplus::GraphicsState status = g.Save();
		g.TranslateTransform(left,top);
		CViewinfo viewinfo2(m_pDocument->m_Datainfo);
		viewinfo2.FactorTo(viewinfo.m_xFactorMapToView*m_Rect.Width()/docRect.Width(), viewinfo.m_yFactorMapToView*m_Rect.Height()/docRect.Height());
		viewinfo2.m_scaleCurrent = viewinfo.m_scaleCurrent;
		m_pDocument->Draw(nullptr, viewinfo2, g, m_pDocument->m_Datainfo.GetDocRect());
		g.Restore(status);
	}
	else if(m_pPoints!=nullptr)
	{
		const double fSin = this->GetSin();
		const double fCos = this->GetCos();
		const double fTg = this->GetTg();
		
		int cx1 = m_pPoints[3].x-m_pPoints[0].x;
		int cy1 = m_pPoints[3].y-m_pPoints[0].y;
		int cx2 = m_pPoints[1].x-m_pPoints[0].x;
		int cy2 = m_pPoints[1].y-m_pPoints[0].y;		
		int cx = sqrt(((double)cx1*cx1+(double)cy1*cy1));
		int cy = -cx2*fSin + cy2*fCos;	//after rotated

		int left;
		int top;
		viewinfo.DocToClient<int>(m_pPoints[1].x, m_pPoints[1].y, left, top);
		const Gdiplus::GraphicsState state = g.Save();

		const Gdiplus::Matrix matrix(fCos, -fSin, -fTg*fCos+fSin, fTg*fSin+fCos, left, top);
		g.MultiplyTransform(&matrix);
		CViewinfo viewinfo2(m_pDocument->m_Datainfo);
		viewinfo2.FactorTo(viewinfo.m_xFactorMapToView*cx/docRect.Width(), viewinfo.m_yFactorMapToView*cy/docRect.Height());
		viewinfo2.m_scaleCurrent = viewinfo.m_scaleCurrent;
		m_pDocument->Draw(nullptr, viewinfo2, g, m_pDocument->m_Datainfo.GetDocRect());
		g.Restore(state);
	}
}

void CSubmap::Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const
{
	return CSubmap::Draw(g, viewinfo, 1.f);
}
void CSubmap::ReleaseWeb(pbf::writer& writer, const CSize offset) const
{
	CPRect::ReleaseWeb(writer, offset);

	BYTE minMapLevel;
	BYTE maxMapLevel;
	float srcMapLevel;
	std::string cdata1;
	pbf::writer child1(cdata1);

	m_pDocument->m_Datainfo.ReleaseWeb(child1, (pbf::tag)1, minMapLevel, maxMapLevel, srcMapLevel);
	m_pDocument->m_frame.ReleaseWeb(child1, (pbf::tag)2);
	m_pDocument->m_layertree.ReleaseWeb(child1, (pbf::tag)3, m_pDocument->m_Datainfo, minMapLevel, maxMapLevel, true);
	if(m_pDocument->m_topolist.size() > 0)
	{
		std::string cdata2;
		pbf::writer child2(cdata2);
		for(std::list<CTopology*>::iterator it = m_pDocument->m_topolist.begin(); it != m_pDocument->m_topolist.end(); it++)
		{
			CTopology* pTopology = *it;
			pTopology->ReleaseWeb(child2);
		}
		child1.add_block((pbf::tag)4, cdata2.c_str(), cdata2.length());
	}	
	
	writer.add_bytes(cdata1.c_str(), cdata1.length());
}
void CSubmap::ExportPs3(FILE* file, const Gdiplus::Matrix& matrix, CPsboard& aiKey) const
{
}
void CSubmap::ExportPs3(FILE* file, const CViewinfo& viewinfo, CPsboard& aiKey) const
{
}
void CSubmap::ExportPdf(HPDF_Doc pdf, HPDF_Page page, const Gdiplus::Matrix& matrix) const
{
}
void CSubmap::ExportPdf(HPDF_Doc pdf, HPDF_Page page, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const
{
}