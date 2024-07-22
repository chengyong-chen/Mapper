#include "stdafx.h"
#include "Global.h"
#include "GradientDlg.h"
#include "afxdialogex.h"
#include "Color.h"
#include "ColorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include<map>
using namespace std;

CGradientDlg::CGradientDlg(CWnd* pParent, std::map<unsigned int, CColor*>& stops)
	: CDialogEx(CGradientDlg::IDD, pParent), m_stops(stops), m_stopposition(0)
{
	m_stopcolor = m_stops.begin()->second->Clone();
}

void CGradientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STOPPOSITION, m_stopposition);
	DDV_MinMaxInt(pDX, m_stopposition, 0, 100);
}

BEGIN_MESSAGE_MAP(CGradientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_QUERYDRAGICON()
	ON_STN_CLICKED(IDC_STOPPERHOLDER, OnStnClickedStopperholder)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_EN_CHANGE(IDC_STOPPOSITION, OnEnChangeStopposition)
	ON_STN_DBLCLK(IDC_COLOR, OnDblclickColor)
	ON_STN_CLICKED(IDB_RULER, OnStnClickedRuler)
END_MESSAGE_MAP()

// CGradientDlg message handlers

BOOL CGradientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	ActiveStop(0);
	return TRUE; // return TRUE  unless you set the focus to a control
}

void CGradientDlg::OnPaint()
{
	CDialogEx::OnPaint();

	Redrawpreview();
	Redrawactive();
}

void CGradientDlg::OnStnClickedStopperholder()
{
	POINT point;
	GetCursorPos(&point);
	GetDlgItem(IDC_STOPPERHOLDER)->ScreenToClient(&point);
	const CWnd* pColorBar = GetDlgItem(IDC_GRADIENTBAR);
	const CWnd* pHolder = GetDlgItem(IDC_STOPPERHOLDER);

	CRect rect1;
	CRect rect2;
	pColorBar->GetClientRect(rect1);
	pHolder->GetClientRect(rect2);
	const float offset = (rect2.Width()-rect1.Width())/2.f;
	for(std::map<unsigned int, CColor*>::iterator it = m_stops.begin(); it!=m_stops.end(); it++)
	{
		const int position = it->first;
		const int x = offset+(rect1.Width()-1)*position/100.f;
		CRect rect(x-4, 0, x-4+9, 13);
		if(rect.PtInRect(point)==TRUE)
		{
			char str[4];
			ltoa(position, str, 10);
			GetDlgItem(IDC_STOPPOSITION)->SetWindowText(CString(str)); // this will trigger the activestop

			break;
		}
	}
}

void CGradientDlg::ActiveStop(int position)
{
	const std::map<unsigned int, CColor*>::iterator it = m_stops.find(position);
	if(it!=m_stops.end())
	{
		delete m_stopcolor;

		m_stopposition = position;
		m_stopcolor = it->second->Clone();

		GetDlgItem(IDC_REMOVE)->EnableWindow(TRUE);
		GetDlgItem(IDC_ADD)->EnableWindow(FALSE);
		if(m_stopposition==0||m_stopposition==100)
			GetDlgItem(IDC_REMOVE)->EnableWindow(FALSE);
		else
			GetDlgItem(IDC_REMOVE)->EnableWindow(TRUE);

		GetDlgItem(IDC_ADD)->EnableWindow(FALSE);

		Redrawactive();
	}
	else
	{
		GetDlgItem(IDC_REMOVE)->EnableWindow(FALSE);
		GetDlgItem(IDC_ADD)->EnableWindow(TRUE);
	}
}

void CGradientDlg::OnRemove()
{
	if(m_stops.size()<2)
		return;
	if(m_stopposition==0)
		return;
	if(m_stopposition==100)
		return;
	const std::map<unsigned int, CColor*>::iterator it = m_stops.find(m_stopposition);
	if(it!=m_stops.end())
	{
		delete it->second;

		m_stops.erase(it);
		ActiveStop(0);

		Redrawpreview();
	}
}

void CGradientDlg::OnAdd()
{
	if(m_stopposition==0)
		return;
	if(m_stopposition==100)
		return;
	const std::map<unsigned int, CColor*>::iterator it = m_stops.find(m_stopposition);
	if(it==m_stops.end())
	{
		m_stops.insert(std::make_pair(m_stopposition, m_stopcolor->Clone()));

		ActiveStop(m_stopposition);

		Redrawpreview();
	}
}

void CGradientDlg::OnEnChangeStopposition()
{
	UpdateData(TRUE);
	ActiveStop(m_stopposition);
}

void CGradientDlg::OnDblclickColor()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CColorDlg dlg(this, m_stopcolor, RGB(255, 255, 255));
	if(dlg.DoModal()==IDOK)
	{
		delete m_stopcolor;
		m_stopcolor = dlg.d_pColor;
		dlg.d_pColor = nullptr;

		const std::map<unsigned int, CColor*>::iterator it = m_stops.find(m_stopposition);
		if(it!=m_stops.end())
		{
			delete it->second;
			m_stops[m_stopposition] = m_stopcolor->Clone();
		}

		this->Invalidate();
	}
}

void CGradientDlg::Redrawpreview() const
{
	const int size = m_stops.size();
	if(size<2)
		return;

	CWnd* pColorBar = GetDlgItem(IDC_GRADIENTBAR);
	if(pColorBar!=nullptr)
	{
		const CClientDC dc(pColorBar);
		Gdiplus::Graphics g(dc.m_hDC);

		CRect rect;
		pColorBar->GetClientRect(rect);
		const Gdiplus::Color color1 = m_stops.begin()->second->GetColor();
		const Gdiplus::Color color2 = m_stops.rbegin()->second->GetColor();
		Gdiplus::LinearGradientBrush brush(Gdiplus::Rect(0, 0, rect.Width(), rect.Height()), color1, color2, Gdiplus::LinearGradientMode::LinearGradientModeHorizontal);
		brush.SetGammaCorrection(TRUE);
		if(size>2)
		{
			Gdiplus::Color* colors = new Gdiplus::Color[size];
			Gdiplus::REAL* positions = new Gdiplus::REAL[size];
			std::map<unsigned int, CColor*>::iterator it = m_stops.begin();
			for(int index = 0; index<size; index++)
			{
				colors[index] = it->second->GetColor();
				positions[index] = it->first/100.f;
				it++;
			}

			brush.SetInterpolationColors(colors, positions, size);
		}
		g.FillRectangle((const Gdiplus::Brush*)&brush, (int)rect.left, rect.top, rect.right, rect.bottom);
	}
	CWnd* pHolder = GetDlgItem(IDC_STOPPERHOLDER);
	if(pHolder!=nullptr)
	{
		CBitmap pencil;
		pencil.LoadBitmap(IDB_PENCIL);

		CRect rect1;
		pColorBar->GetClientRect(rect1);

		CRect rect2;
		pHolder->GetClientRect(rect2);

		CClientDC dc(pHolder);
		dc.FillSolidRect(rect2, dc.GetBkColor());

		CDC memdc;
		memdc.CreateCompatibleDC(&dc);
		memdc.SelectObject(&pencil);
		const float offset = (rect2.Width()-rect1.Width())/2.f;
		for(std::map<unsigned int, CColor*>::iterator it = m_stops.begin(); it!=m_stops.end(); it++)
		{
			const int position = it->first;
			const int x = offset+(rect1.Width()-1)*position/100.f;
			dc.BitBlt(x-4, 0, 9, 13, &memdc, 0, 0, SRCCOPY);
			dc.FillSolidRect(x-4+2, 6, 5, 5, it->second->GetMonitorRGB());
		}
	}
	GetParent()->SendMessage(WM_REDRAWPREVIEW);
}

void CGradientDlg::Redrawactive() const
{
	CWnd* pStopColor = GetDlgItem(IDC_COLOR);
	if(pStopColor!=nullptr)
	{
		CRect rect;
		pStopColor->GetClientRect(rect);

		CClientDC dc(pStopColor);
		dc.FillSolidRect(rect, m_stopcolor->GetMonitorRGB());
	}
}

void CGradientDlg::OnStnClickedRuler()
{
	// TODO: Add your control notification handler code here
}

void CGradientDlg::OnDestroy()
{
	CDialog::OnDestroy();

	delete m_stopcolor;
	m_stopcolor = nullptr;
}
