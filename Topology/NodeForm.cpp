// NodeDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Topo.h"
#include "Node.h"
#include "NodeForm.h"

#include "Edge.h"

#include "../Geometry/Poly.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CNodeForm �Ի���

IMPLEMENT_DYNAMIC(CNodeForm, CDialog)

CNodeForm::CNodeForm(CWnd* pParent)
{
	m_pTopo = nullptr;
	m_pNode = nullptr;
}

CNodeForm::CNodeForm(UINT nIDD, CWnd* pParent)
	: CDialog(nIDD, pParent)
{
	m_pTopo = nullptr;
	m_pNode = nullptr;
}

CNodeForm::~CNodeForm()
{
}

void CNodeForm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CNodeForm, CDialog)

	//{{AFX_MSG_MAP(CGeoView)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_NEXT, OnNext)
	//}}AFX_MSG_MAP
	// Standard printing commands
END_MESSAGE_MAP()

// CNodeForm ��Ϣ�������

BOOL CNodeForm::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE; // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CNodeForm::OnOK()
{
}

void CNodeForm::OnCancel()
{
}

void CNodeForm::SetNode(const CTopology* pTopology, CNode* pNode)
{
	if(m_pTopo!=pTopology)
	{
		if(pTopology!=nullptr)
		{
			CString strMaxID;
			strMaxID.Format(_T("%d"), pTopology->m_dwMaxNodeId);
			GetDlgItem(IDC_MAXID)->SetWindowText(strMaxID);
			GetDlgItem(IDC_MINID)->SetWindowText(_T("1"));
		}
		else
		{
			GetDlgItem(IDC_MAXID)->SetWindowText(nullptr);
			GetDlgItem(IDC_MINID)->SetWindowText(nullptr);
		}
	}

	if(m_pNode!=pNode)
	{
		if(pNode!=nullptr)
		{
			CString strID;
			strID.Format(_T("%d"), pNode->m_dwId);
			GetDlgItem(IDC_EDITID)->SetWindowText(strID);
		}
		else
		{
			GetDlgItem(IDC_EDITID)->SetWindowText(nullptr);
		}
	}

	m_pTopo = pTopology;
	m_pNode = pNode;

	DrawView();
}

void CNodeForm::OnPaint()
{
	CDialog::OnPaint();

	DrawView();
}

void CNodeForm::DrawView()
{
	CWnd* pWnd = nullptr;
	CWnd* pViewWnd = GetDlgItem(IDC_VIEW);
	if(pViewWnd!=nullptr)
	{
		CClientDC dc(pViewWnd); // device context for painting

		CRect cliRect;
		pViewWnd->GetClientRect(cliRect);
		dc.IntersectClipRect(cliRect);
		const COLORREF crWindow = IsWindowEnabled() ? GetSysColor(COLOR_WINDOW) : GetSysColor(COLOR_3DFACE);
		dc.FillSolidRect(cliRect, crWindow);

		if(m_pNode!=nullptr&&m_pTopo!=nullptr)
		{
			float fScale = 1.0f;
			const CPoint docCenter = m_pNode->m_Point;
			for(int index = 0; index<m_pNode->m_edgelist.GetCount(); index++)
			{
				const DWORD dwEdge = m_pNode->m_edgelist.GetAt(index);
				CEdge* pEdge = m_pTopo->GetEdge(dwEdge);
				if(pEdge!=nullptr)
				{
					if(pEdge->m_dwFromNode==m_pNode->m_dwId)
					{
						CNode* node = m_pTopo->GetNode(pEdge->m_dwToNode);
						if(node!=nullptr)
						{
							float xScale = (float)std::abs(node->m_Point.x-docCenter.x)/cliRect.Width();
							float yScale = (float)std::abs(node->m_Point.y-docCenter.y)/cliRect.Height();
							float scale = max(xScale, yScale);
							fScale = max(fScale, scale);
						}
					}
					else
					{
						CNode* node = m_pTopo->GetNode(pEdge->m_dwFromNode);
						if(node!=nullptr)
						{
							float xScale = (float)std::abs(node->m_Point.x-docCenter.x)/cliRect.Width();
							float yScale = (float)std::abs(node->m_Point.y-docCenter.y)/cliRect.Height();
							float scale = max(xScale, yScale);
							fScale = max(fScale, scale);
						}
					}
				}
			}
			const CPoint cliCenter = cliRect.CenterPoint();

			Gdiplus::Matrix matrix;
			matrix.Translate(cliCenter.x*fScale-docCenter.x, -cliCenter.y*fScale-docCenter.y);
			matrix.Scale(1.0f/fScale, -1.0f/fScale, Gdiplus::MatrixOrderAppend);

			Gdiplus::Graphics g(dc.m_hDC);
			g.SetPageUnit(Gdiplus::UnitPixel);
			g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
			g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
			g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
			g.SetTransform(&matrix);

			Gdiplus::Pen* pen = ::new Gdiplus::Pen(Gdiplus::Color(0, 255, 255), 3*fScale);
			pen->SetAlignment(Gdiplus::PenAlignmentCenter);
			pen->SetDashStyle(Gdiplus::DashStyleSolid);
			const CString strFont(_T("Arial"));
			const _bstr_t btrFont(strFont);
			Gdiplus::FontFamily fontFamily(btrFont);
			::SysFreeString(btrFont);

			const Gdiplus::Font* font = ::new Gdiplus::Font(&fontFamily, 12*fScale, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);

			font->GetFamily(&fontFamily);
			UINT16 ascent = fontFamily.GetCellAscent(Gdiplus::FontStyleRegular);
			ascent = font->GetSize()*ascent/fontFamily.GetEmHeight(Gdiplus::FontStyleRegular);

			UINT16 descent = fontFamily.GetCellDescent(Gdiplus::FontStyleRegular);
			descent = font->GetSize()*descent/fontFamily.GetEmHeight(Gdiplus::FontStyleRegular);

			Gdiplus::StringFormat stringFormat;
			stringFormat.SetAlignment(Gdiplus::StringAlignmentCenter);
			stringFormat.SetLineAlignment(Gdiplus::StringAlignmentNear);
			stringFormat.SetTrimming(Gdiplus::StringTrimmingNone);

			const Gdiplus::Brush* textbrush = ::new Gdiplus::SolidBrush(Gdiplus::Color(0, 0, 0));
			const long nCount = m_pNode->m_edgelist.GetSize();
			for(int index = 0; index<nCount; index++)
			{
				const DWORD dwEdge = m_pNode->m_edgelist.GetAt(index);
				CEdge* pEdge = m_pTopo->GetEdge(dwEdge);
				CPoly* poly = m_pTopo->GetEdgePoly(pEdge->m_dwId);
				if(poly!=nullptr)
				{
					/*ת��CRoadNodeForm
						if(index == m_matrixCtrl.m_nActiveRow)
						{
							Pen pen1(Color(255,0,255),3*fScale);

							poly->Draw(g,&pen1,nullptr,nullptr,1.0f);
						}
						else
						{
							poly->Draw(g,pen,nullptr,nullptr,1.0f);

							long radius = 25*fScale;
							CPoint point;
							if(pEdge->m_dwFromNode == m_pNode->m_dwId)
							{
								point = poly->GetSample(1, m_pNode->m_Point,radius);
							}
							else
							{
								point = poly->GetSample(-1, m_pNode->m_Point,radius);
							}

							if(m_matrixCtrl.m_nActiveRow != -1)
							{
								g.TransformPoints(CoordinateSpaceDevice,CoordinateSpaceWorld,&point,1);
								point.x -= 8;
								point.y -= 8;
						//		CClientDC clidc(pViewWnd);

						//		m_matrixCtrl.m_imagelist.Draw(&clidc,m_pNode->m_Forbid.GetValue(m_matrixCtrl.m_nActiveRow,index) == false ? 2 : 3, point, ILD_TRANSPARENT);
							}
						}*/

					long radius = 50*fScale;
					CPoint point;
					if(pEdge->m_dwFromNode==m_pNode->m_dwId)
					{
						point = poly->GetSample(1, m_pNode->m_Point, radius);
					}
					else
					{
						point = poly->GetSample(-1, m_pNode->m_Point, radius);
					}

					TEXTMETRIC tm;
					dc.GetTextMetrics(&tm);
					char str[3];
					itoa(index+1, str, 10);
					const Gdiplus::GraphicsState state = g.Save();
					g.TranslateTransform(point.x, point.y);
					g.ScaleTransform(1.0f, -1.0f);
					_bstr_t bstr(str);
					g.DrawString(bstr, -1, font, Gdiplus::PointF(0, -(ascent+descent)/2), &stringFormat, textbrush);
					g.ScaleTransform(1.0f, -1.0f);
					g.TranslateTransform(-point.x, -point.y);
					g.Restore(state);
				}
			}

			pen->SetColor(Gdiplus::Color(255, 0, 0));
			pen->SetWidth(2*fScale);

			g.DrawEllipse(pen, m_pNode->m_Point.x-10*fScale, m_pNode->m_Point.y-10*fScale, 20*fScale, 20*fScale);
			g.DrawEllipse(pen, m_pNode->m_Point.x-1*fScale, m_pNode->m_Point.y-1*fScale, 2*fScale, 2*fScale);

			::delete textbrush;
			::delete font;
			::delete pen;
		}
	}
}

void CNodeForm::OnNext()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}
