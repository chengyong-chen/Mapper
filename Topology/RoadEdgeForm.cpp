// EdgeDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "EdgeForm.h"
#include "RoadEdgeForm.h"
#include "Topodb.h"
#include "Edge.h"
#include "RoadEdge.h"
#include "Node.h"
#include "RoadNode.h"

#include "../Geometry/Geom.h"
#include "../Geometry/Poly.h"

#include "../Style/Line.h"
#include "../Style/ColorSpot.h"
#include "../Dataview/ViewMonitor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CRoadEdgeForm �Ի���

IMPLEMENT_DYNAMIC(CRoadEdgeForm, CEdgeForm)

CRoadEdgeForm::CRoadEdgeForm(CWnd* pParent /*=nullptr*/)
	: CEdgeForm(CRoadEdgeForm::IDD, pParent)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_bmpPermit.LoadBitmap(IDB_PERMIT);
	m_bmpRefuse.LoadBitmap(IDB_REFUSE);
}

CRoadEdgeForm::~CRoadEdgeForm()
{
}

void CRoadEdgeForm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CRoadEdgeForm, CDialog)
	ON_BN_CLICKED(IDC_BUTTONAB, OnClickedButtonAB)
	ON_BN_CLICKED(IDC_BUTTONBA, OnClickedButtonBA)

	ON_BN_CLICKED(IDC_CHARGE_NO, &CRoadEdgeForm::OnClickedChargeNo)
	ON_BN_CLICKED(IDC_CHARGE_MILE, &CRoadEdgeForm::OnClickedChargeMile)
	ON_BN_CLICKED(IDC_CHARGE_VEHICLE, &CRoadEdgeForm::OnClickedChargeVehicle)

	ON_BN_CLICKED(IDC_RESTICTSPEED_NO, &CRoadEdgeForm::OnClickedRestictSpeedNo)
	ON_BN_CLICKED(IDC_RESTICTSPEED_UPPER, &CRoadEdgeForm::OnClickedRestictSpeedUpper)
	ON_BN_CLICKED(IDC_RESTICTSPEED_LOWER, &CRoadEdgeForm::OnClickedRestictSpeedLower)
END_MESSAGE_MAP()

BOOL CRoadEdgeForm::OnInitDialog()
{
	CEdgeForm::OnInitDialog();

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CRoadEdgeForm::SetEdge(const CTopology* pTopology, CEdge* pEdge)
{
	if(m_pTopo!=pTopology)
	{
		if(pTopology!=nullptr)
		{
			CString strMaxID;
			strMaxID.Format(_T("%d"), pTopology->m_dwMaxEdgeId);
			GetDlgItem(IDC_MAXID)->SetWindowText(strMaxID);
			GetDlgItem(IDC_MINID)->SetWindowText(_T("1"));
		}
		else
		{
			GetDlgItem(IDC_MAXID)->SetWindowText(nullptr);
			GetDlgItem(IDC_MINID)->SetWindowText(nullptr);
		}
	}

	if(m_pEdge!=pEdge)
	{
		if(m_pEdge!=nullptr)
		{
			//���������
			{
				//		int index = ((CComboBox*)GetDlgItem(IDC_COMBO_SECLUTION))->GetCurSel();
				//		m_pEdge->m_typeSeclusion = index;	
			}
			//��·��״
			{
				//		int index = ((CComboBox*)GetDlgItem(IDC_COMBO_SHAPE))->GetCurSel();
				//		m_pEdge->m_typeShape = index;
			}
			//��·�߶�
			{
				//		int index = ((CComboBox*)GetDlgItem(IDC_COMBO_HEIGHT))->GetCurSel();
				//		m_pEdge->m_typeHeight = index;
			}
			//��·�¶�
			{
				//		int index = ((CComboBox*)GetDlgItem(IDC_COMBO_GRADIENT))->GetCurSel();
				//		m_pEdge->m_typeSlope = index;
			}
			//���򳵵�����
			{
				//		int index = ((CComboBox*)GetDlgItem(IDC_COMBO_WAYS))->GetCurSel();
				//		m_pEdge->m_laneCount = index;
			}
			//��·����
			{
				//		m_pEdge->m_typePortion = ((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->GetCurSel();
			}

			if(((CButton*)GetDlgItem(IDC_RESTICTSPEED_NO))->GetCheck()==BST_CHECKED)
				((CRoadEdge*)m_pEdge)->m_modeRestrictSpeed = 0;
			else if(((CButton*)GetDlgItem(IDC_RESTICTSPEED_UPPER))->GetCheck()==BST_CHECKED)
				((CRoadEdge*)m_pEdge)->m_modeRestrictSpeed = 1;
			else if(((CButton*)GetDlgItem(IDC_RESTICTSPEED_LOWER))->GetCheck()==BST_CHECKED)
				((CRoadEdge*)m_pEdge)->m_modeRestrictSpeed = 2;

			if(((CButton*)GetDlgItem(IDC_CHARGE_NO))->GetCheck()==BST_CHECKED)
				((CRoadEdge*)m_pEdge)->m_modeToll = 0;
			else if(((CButton*)GetDlgItem(IDC_CHARGE_MILE))->GetCheck()==BST_CHECKED)
				((CRoadEdge*)m_pEdge)->m_modeToll = 1;
			else if(((CButton*)GetDlgItem(IDC_CHARGE_VEHICLE))->GetCheck()==BST_CHECKED)
				((CRoadEdge*)m_pEdge)->m_modeToll = 2;

			//�շѽ��
			{
				CString str;
				GetDlgItem(IDC_CHARGE)->GetWindowText(str);
				((CRoadEdge*)m_pEdge)->m_cashToll = _ttoi(str);
			}
			//�����ٶ�
			{
				CString str;
				GetDlgItem(IDC_RESTRICTSPEED)->GetWindowText(str);
				((CRoadEdge*)m_pEdge)->m_restrictSpeed = _ttoi(str);
			}
			//����ٶ�
			{
				CString str;
				GetDlgItem(IDC_AVERAGESPEED)->GetWindowText(str);
				((CRoadEdge*)m_pEdge)->m_designedSpeed = _ttoi(str);
			}
			//��·����
			{
				CString str;
				GetDlgItem(IDC_LENGTH)->GetWindowText(str);
				((CRoadEdge*)m_pEdge)->m_fLength = _tcstof(str, nullptr);
			}
		}

		if(pEdge!=nullptr)
		{
			//ID��Ч
			{
				CString strID;
				strID.Format(_T("%d"), pEdge->m_dwId);
				GetDlgItem(IDC_EDITID)->SetWindowText(strID);
			}
			//��·��ͨ��
			CRoadNode* nodeA = (CRoadNode*)pTopology->GetNode(pEdge->m_dwFromNode);
			CRoadNode* nodeB = (CRoadNode*)pTopology->GetNode(pEdge->m_dwToNode);
			if(nodeA!=nullptr&&nodeB!=nullptr)
			{
				if(nodeA->CanThruEdge(pEdge->m_dwId)==true||nodeB->CanEdgeThru(pEdge->m_dwId)==true)
				{
					GetDlgItem(IDC_BUTTONAB)->EnableWindow(TRUE);
					((CStatic*)GetDlgItem(IDC_IMAGEAB))->SetBitmap(m_bmpPermit);
				}
				else
				{
					GetDlgItem(IDC_BUTTONAB)->EnableWindow(FALSE);
					((CStatic*)GetDlgItem(IDC_IMAGEAB))->SetBitmap(HBITMAP(m_bmpRefuse));
				}

				if(nodeB->CanThruEdge(pEdge->m_dwId)==true||nodeA->CanEdgeThru(pEdge->m_dwId)==true)
				{
					GetDlgItem(IDC_BUTTONBA)->EnableWindow(TRUE);
					((CStatic*)GetDlgItem(IDC_IMAGEBA))->SetBitmap(HBITMAP(m_bmpPermit));
				}
				else
				{
					GetDlgItem(IDC_BUTTONBA)->EnableWindow(FALSE);
					((CStatic*)GetDlgItem(IDC_IMAGEBA))->SetBitmap(HBITMAP(m_bmpRefuse));
				}
			}
			else
			{
				GetDlgItem(IDC_BUTTONAB)->EnableWindow(FALSE);
				GetDlgItem(IDC_BUTTONBA)->EnableWindow(FALSE);
			}

			//�ٶ���������
			((CButton*)GetDlgItem(IDC_RESTICTSPEED_NO))->SetCheck(((CRoadEdge*)pEdge)->m_modeRestrictSpeed==0 ? TRUE : FALSE);
			((CButton*)GetDlgItem(IDC_RESTICTSPEED_UPPER))->SetCheck(((CRoadEdge*)pEdge)->m_modeRestrictSpeed==1 ? TRUE : FALSE);
			((CButton*)GetDlgItem(IDC_RESTICTSPEED_LOWER))->SetCheck(((CRoadEdge*)pEdge)->m_modeRestrictSpeed==2 ? TRUE : FALSE);
			GetDlgItem(IDC_RESTRICTSPEED)->EnableWindow(((CRoadEdge*)pEdge)->m_modeRestrictSpeed==0 ? FALSE : TRUE);

			//�Ƿ��շ�
			((CButton*)GetDlgItem(IDC_CHARGE_NO))->SetCheck(((CRoadEdge*)pEdge)->m_modeToll==0 ? TRUE : FALSE);
			((CButton*)GetDlgItem(IDC_CHARGE_MILE))->SetCheck(((CRoadEdge*)pEdge)->m_modeToll==1 ? TRUE : FALSE);
			((CButton*)GetDlgItem(IDC_CHARGE_VEHICLE))->SetCheck(((CRoadEdge*)pEdge)->m_modeToll==2 ? TRUE : FALSE);
			GetDlgItem(IDC_CHARGE)->EnableWindow(((CRoadEdge*)pEdge)->m_modeToll==0 ? FALSE : TRUE);

			//�Ƿ��и��������
			{
				//	((CComboBox*)GetDlgItem(IDC_COMBO_SECLUTION))->SetCurSel(byte);
			}
			//��·��״
			{
				//	((CComboBox*)GetDlgItem(IDC_COMBO_SHAPE))->SetCurSel(byte);
			}
			//��·�߶�
			{
				//	((CComboBox*)GetDlgItem(IDC_COMBO_HEIGHT))->SetCurSel(byte);
			}
			//��·�¶�
			{
				//	((CComboBox*)GetDlgItem(IDC_COMBO_GRADIENT))->SetCurSel(byte);
			}
			//���򳵵�����
			{
				//				((CComboBox*)GetDlgItem(IDC_COMBO_WAYS))->SetCurSel(byte);
			}
			//��·����
			{
				//	 ((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->SetCurSel(pEdge->m_bRoadType);
			}
			//�շѽ��
			{
				CString str;
				str.Format(_T("%d"), ((CRoadEdge*)pEdge)->m_cashToll);
				GetDlgItem(IDC_CHARGE)->SetWindowText(str);
			}
			//���Ƶ��ٶ�
			{
				CString str;
				str.Format(_T("%d"), ((CRoadEdge*)pEdge)->m_restrictSpeed);
				GetDlgItem(IDC_RESTRICTSPEED)->SetWindowText(str);
			}
			//��·����
			{
				CString str;
				str.Format(_T("%.3f"), ((CRoadEdge*)pEdge)->m_fLength);
				GetDlgItem(IDC_LENGTH)->SetWindowText(str);
			}
			//����ٶ�
			{
				CString str;
				str.Format(_T("%d"), ((CRoadEdge*)pEdge)->m_designedSpeed);
				GetDlgItem(IDC_AVERAGESPEED)->SetWindowText(str);
			}
		}
		else
		{
			GetDlgItem(IDC_EDITID)->SetWindowText(nullptr);
		}
	}
	else
	{
		GetDlgItem(IDC_EDITID)->SetWindowText(nullptr);
	}

	m_pTopo = pTopology;
	m_pEdge = pEdge;

	DrawView();
}

void CRoadEdgeForm::DrawView()
{
	CWnd* pViewWnd = GetDlgItem(IDC_VIEW);
	if(pViewWnd==nullptr)
		return;

	CDatainfo datainfo;
	CViewMonitor viewinfo(datainfo);

	CClientDC dc(pViewWnd); // device context for painting

	CRect cliRect;
	pViewWnd->GetClientRect(cliRect);
	dc.IntersectClipRect(cliRect);

	COLORREF crWindow = IsWindowEnabled() ? GetSysColor(COLOR_WINDOW) : GetSysColor(COLOR_3DFACE);
	dc.FillSolidRect(cliRect, crWindow);

	if(m_pEdge!=nullptr&&m_pTopo!=nullptr)
	{
		CPoly* poly = m_pTopo->GetEdgePoly(m_pEdge->m_dwId);
		if(poly!=nullptr)
		{
			float fScale = 1.0f;
			CRect docRect = poly->m_Rect;
			if(docRect.Width()>cliRect.Width()||docRect.Height()>cliRect.Height())
			{
				float xScale = (float)docRect.Width()/(cliRect.Width()-40);
				float yScale = (float)docRect.Height()/(cliRect.Height()-40);
				fScale = max(xScale, yScale);
			}

			CPoint cliCenter = cliRect.CenterPoint();
			CPoint docCenter = docRect.CenterPoint();

			docCenter.x = docRect.left+docRect.Width()/2;//��zoomRect��ֵ����ʱ��CenterPoint��������ִ������
			docCenter.y = docRect.top+docRect.Height()/2;

			Gdiplus::Graphics g(dc.m_hDC);
			g.SetPageUnit(Gdiplus::UnitPixel);
			g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
			g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
			g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);

			Gdiplus::Matrix matrix;
			matrix.Translate(cliCenter.x*fScale-docCenter.x, -cliCenter.y*fScale-docCenter.y);
			matrix.Scale(1.0f/fScale, -1.0f/fScale, Gdiplus::MatrixOrderAppend);
			g.SetTransform(&matrix);

			CLine line;
			line.m_nWidth = 3*fScale;
			line.m_pColor = new CColorSpot(255, 0, 0, 255);
			CLine* pOld = poly->m_pLine;
			poly->m_pLine = &line;

			poly->Draw(g, viewinfo, 1.f);

			poly->m_pLine = pOld;
			delete line.m_pColor;
			line.m_pColor = new CColorSpot(0, 0, 0, 255);

			Gdiplus::Brush* textbrush = ::new Gdiplus::SolidBrush(Gdiplus::Color(255, 255, 255));

			CString strFont(_T("Arial"));
			_bstr_t btrFont(strFont);
			Gdiplus::FontFamily fontFamily(btrFont);
			::SysFreeString(btrFont);

			Gdiplus::Font* font = ::new Gdiplus::Font(&fontFamily, 12*fScale, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);

			Gdiplus::RectF charRect;
			const Gdiplus::StringFormat* pStringFormat = Gdiplus::StringFormat::GenericTypographic();
			_bstr_t bstr(_T("A"));
			g.MeasureString(bstr, -1, font, Gdiplus::PointF(0, 0), pStringFormat, &charRect);
			Gdiplus::SizeF charSize;
			charRect.GetSize(&charSize);
			int charHeight = charSize.Height;

			Gdiplus::StringFormat stringFormat;
			stringFormat.SetAlignment(Gdiplus::StringAlignmentCenter);
			stringFormat.SetLineAlignment(Gdiplus::StringAlignmentNear);
			stringFormat.SetTrimming(Gdiplus::StringTrimmingNone);

			CNode* pNode1 = m_pTopo->GetNode(m_pEdge->m_dwFromNode);
			if(pNode1!=nullptr)
			{
				long nCount = pNode1->m_edgelist.GetSize();
				for(int index = 0; index<nCount; index++)
				{
					DWORD dwEdge = pNode1->m_edgelist.GetAt(index);
					CRoadEdge* pEdge = (CRoadEdge*)m_pTopo->GetEdge(dwEdge);
					if(pEdge==m_pEdge)
						continue;

					CPoly* poly = m_pTopo->GetEdgePoly(pEdge->m_dwId);

					CLine* pOld = poly->m_pLine;
					poly->m_pLine = &line;
					poly->Draw(g, viewinfo, 1.f);
					poly->m_pLine = pOld;
				}

				Gdiplus::RectF nodeRect(pNode1->m_Point.x, pNode1->m_Point.y, 0, 0);
				nodeRect.Inflate(8*fScale, 8*fScale);

				Gdiplus::Brush* brush = ::new Gdiplus::SolidBrush(Gdiplus::Color(0, 0, 0));
				g.FillEllipse(brush, nodeRect);
				::delete brush;

				Gdiplus::GraphicsState state = g.Save();
				g.TranslateTransform(pNode1->m_Point.x, pNode1->m_Point.y);
				g.ScaleTransform(1.0f, -1.0f);
				_bstr_t bstr(_T("A"));
				g.DrawString(bstr, -1, font, Gdiplus::PointF(0, -charHeight/2.0), &stringFormat, textbrush);
				g.ScaleTransform(1.0f, -1.0f);
				g.TranslateTransform(-pNode1->m_Point.x, -pNode1->m_Point.y);
				g.Restore(state);
			}

			CNode* pNode2 = m_pTopo->GetNode(m_pEdge->m_dwToNode);
			if(pNode2!=nullptr)
			{
				long nCount = pNode2->m_edgelist.GetSize();
				for(int index = 0; index<nCount; index++)
				{
					DWORD dwEdge = pNode2->m_edgelist.GetAt(index);
					CRoadEdge* pEdge = (CRoadEdge*)m_pTopo->GetEdge(dwEdge);
					if(pEdge==m_pEdge)
						continue;

					CPoly* poly = m_pTopo->GetEdgePoly(pEdge->m_dwId);
					CLine* pOld = poly->m_pLine;
					poly->m_pLine = &line;
					poly->Draw(g, viewinfo, 1.f);
					poly->m_pLine = pOld;
				}

				Gdiplus::Rect nodeRect(pNode2->m_Point.x, pNode2->m_Point.y, 0, 0);
				nodeRect.Inflate(8*fScale, 8*fScale);

				Gdiplus::Brush* brush = ::new Gdiplus::SolidBrush(Gdiplus::Color(0, 0, 0));
				g.FillEllipse(brush, nodeRect);
				::delete brush;
				brush = nullptr;

				Gdiplus::GraphicsState state = g.Save();
				g.TranslateTransform(pNode2->m_Point.x, pNode2->m_Point.y);
				g.ScaleTransform(1.0f, -1.0f);
				_bstr_t bstr(_T("B"));
				g.DrawString(bstr, -1, font, Gdiplus::PointF(0, -charHeight/2), &stringFormat, textbrush);
				g.ScaleTransform(1.0f, -1.0f);
				g.TranslateTransform(-pNode2->m_Point.x, -pNode2->m_Point.y);
				g.Restore(state);
			}

			::delete textbrush;
			textbrush = nullptr;
			::delete font;
			font = nullptr;

			g.ReleaseHDC(dc.m_hDC);
		}
	}
}

void CRoadEdgeForm::OnClickedButtonAB()
{
	if(m_pEdge!=nullptr)
	{
		const CRoadNode* nodeA = (CRoadNode*)m_pTopo->GetNode(m_pEdge->m_dwFromNode);
		const BYTE indexB = nodeA->GetEdgeIndex(m_pEdge);
		for(BYTE i = 0; i<nodeA->m_Forbid.m_nDimension; i++)
		{
			nodeA->m_Forbid.SetValue(i, indexB, false);
		}

		const CRoadNode* nodeB = (CRoadNode*)m_pTopo->GetNode(m_pEdge->m_dwToNode);
		const BYTE indexA = nodeB->GetEdgeIndex(m_pEdge);
		for(BYTE i = 0; i<nodeB->m_Forbid.m_nDimension; i++)
		{
			if(i==indexA)
				continue;

			nodeB->m_Forbid.SetValue(indexA, i, false);
		}

		GetDlgItem(IDC_BUTTONAB)->EnableWindow(FALSE);
		((CStatic*)GetDlgItem(IDC_IMAGEAB))->SetBitmap(HBITMAP(m_bmpRefuse));

		m_pEdge->m_bModified = true;
	}
}

void CRoadEdgeForm::OnClickedButtonBA()
{
	if(m_pEdge!=nullptr)
	{
		const CRoadNode* nodeB = (CRoadNode*)m_pTopo->GetNode(m_pEdge->m_dwToNode);
		const BYTE indexA = nodeB->GetEdgeIndex(m_pEdge);
		for(BYTE i = 0; i<nodeB->m_Forbid.m_nDimension; i++)
		{
			nodeB->m_Forbid.SetValue(i, indexA, false);
		}

		const CRoadNode* nodeA = (CRoadNode*)m_pTopo->GetNode(m_pEdge->m_dwFromNode);
		const BYTE indexB = nodeA->GetEdgeIndex(m_pEdge);
		for(BYTE i = 0; i<nodeA->m_Forbid.m_nDimension; i++)
		{
			if(i==indexB)
				continue;

			nodeA->m_Forbid.SetValue(indexB, i, false);
		}

		GetDlgItem(IDC_BUTTONBA)->EnableWindow(FALSE);
		((CStatic*)GetDlgItem(IDC_IMAGEBA))->SetBitmap(HBITMAP(m_bmpRefuse));

		m_pEdge->m_bModified = true;
	}
}

void CRoadEdgeForm::OnClickedChargeNo()
{
	GetDlgItem(IDC_CHARGE)->EnableWindow(FALSE);
}

void CRoadEdgeForm::OnClickedChargeMile()
{
	GetDlgItem(IDC_CHARGE)->EnableWindow(TRUE);
}

void CRoadEdgeForm::OnClickedChargeVehicle()
{
	GetDlgItem(IDC_CHARGE)->EnableWindow(TRUE);
}

void CRoadEdgeForm::OnClickedRestictSpeedNo()
{
	GetDlgItem(IDC_RESTRICTSPEED)->EnableWindow(TRUE);
}

void CRoadEdgeForm::OnClickedRestictSpeedUpper()
{
	GetDlgItem(IDC_RESTRICTSPEED)->EnableWindow(TRUE);
}

void CRoadEdgeForm::OnClickedRestictSpeedLower()
{
	GetDlgItem(IDC_RESTRICTSPEED)->EnableWindow(TRUE);
}
