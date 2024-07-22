#include "stdafx.h"
#include "Color.h"

#include "Fill.h"
#include "FillRadial.h"
#include "Library.h"
#include "Psboard.h"
#include "../DataView/viewinfo.h"
#include "../Pbf/writer.hpp"
#include <boost/json.hpp>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CFillRadial, CFillGradient, 0)

CFillRadial::CFillRadial()
{
	m_cx = 0.5;
	m_cy = 0.5;
	m_cr = 0.5;

	m_fx = 0;
	m_fy = 0;
	m_fr = 0.5;

	m_centermode = CENTERMODE::Centroid;
	m_radiusmode = RADIUSMODE::OUTERELLIPSE;
	m_focusmode = FOCUSMODE::CENTER;
}
CFillRadial::CFillRadial(float cx, float cy, float cr, float fx, float fy, float fr, std::map<unsigned int, CColor*> stops)
	:CFillGradient(stops)
{
	m_cx = cx;
	m_cy = cy;
	m_cr = cr;
	m_fx = fx;
	m_fy = fy;
	m_fr = fr;

	m_centermode = CENTERMODE::Centroid;
	m_radiusmode = RADIUSMODE::OUTERELLIPSE;
	m_focusmode = FOCUSMODE::CENTER;
}

CFillRadial::~CFillRadial()
{
}
void CFillRadial::Sha1(boost::uuids::detail::sha1& sha1) const
{
	CFill::Sha1(sha1);

	sha1.process_byte(m_centermode);
	sha1.process_byte(m_radiusmode);
	sha1.process_byte(m_focusmode);
	sha1.process_bytes(&m_cx, sizeof(float));
	sha1.process_bytes(&m_cy, sizeof(float));
	sha1.process_bytes(&m_cr, sizeof(float));
	sha1.process_bytes(&m_fx, sizeof(float));
	sha1.process_bytes(&m_fy, sizeof(float));
	sha1.process_bytes(&m_fr, sizeof(float));
}
BOOL CFillRadial::operator==(const CFill& fill) const
{
	if(CFillGradient::operator==(fill)==FALSE)
		return FALSE;
	else if(fill.IsKindOf(RUNTIME_CLASS(CFillRadial))==FALSE)
		return FALSE;
	else if(m_cx!=((CFillRadial&)fill).m_cx)
		return FALSE;
	else if(m_cy!=((CFillRadial&)fill).m_cy)
		return FALSE;
	else if(m_cr!=((CFillRadial&)fill).m_cr)
		return FALSE;
	else if(m_fr!=((CFillRadial&)fill).m_fr)
		return FALSE;
	else if(m_fx!=((CFillRadial&)fill).m_fx)
		return FALSE;
	else if(m_fy!=((CFillRadial&)fill).m_fy)
		return FALSE;
	else if(m_centermode!=((CFillRadial*)&fill)->m_centermode)
		return FALSE;
	else if(m_radiusmode!=((CFillRadial*)&fill)->m_radiusmode)
		return FALSE;
	else if(m_focusmode!=((CFillRadial*)&fill)->m_focusmode)
		return FALSE;
	else
		return TRUE;
}

void CFillRadial::CopyTo(CFill* fill) const
{
	CFillGradient::CopyTo(fill);

	if(fill->IsKindOf(RUNTIME_CLASS(CFillRadial))==TRUE)
	{
		((CFillRadial*)fill)->m_cx = m_cx;
		((CFillRadial*)fill)->m_cy = m_cy;
		((CFillRadial*)fill)->m_cr = m_cr;
		((CFillRadial*)fill)->m_fr = m_fr;
		((CFillRadial*)fill)->m_fx = m_fx;
		((CFillRadial*)fill)->m_fy = m_fy;
	}

	((CFillRadial*)fill)->m_centermode = m_centermode;
	((CFillRadial*)fill)->m_radiusmode = m_radiusmode;
	((CFillRadial*)fill)->m_focusmode = m_focusmode;
}

void CFillRadial::Transform(const double& m11, const double& m21, const double& m31, const double& m12, const double& m22, const double& m32)
{
	if(m_centermode==CENTERMODE::CenterOffset)
	{
		const double x = m_cx*m11+m_cy*m21;
		const double y = m_cx*m12+m_cy*m22;
		m_cx = x;
		m_cy = y;
	}
	else if(m_centermode==CENTERMODE::CenterCoordinate)
	{
		const double x = m11*m_cx+m12*m_cx+m31;
		const double y = m21*m_cy+m22*m_cy+m32;
		m_cx = x;
		m_cy = y;
	}

	if(m_radiusmode==RADIUSMODE::RadiusValue)
	{
		m_cr *= sqrt(m11*m11+m12*m12);
		m_fr *= sqrt(m21*m21+m22*m22);
	}
	if(m_focusmode==FOCUSMODE::FocusOffset)
	{
		m_fx *= sqrt(m11*m11+m12*m12);
		m_fy *= sqrt(m21*m21+m22*m22);
	}
	else if(m_focusmode==FOCUSMODE::FocusCoordinate)
	{
		const double x = m11*m_fx+m12*m_fx+m31;
		const double y = m21*m_fy+m22*m_fy+m32;
		m_fx = x;
		m_fy = y;
	};
}

void CFillRadial::Transform(const CViewinfo& viewinfo, CRect& docrect)
{
	Gdiplus::Point point1(0, 0);
	switch(m_centermode)//valuemode
	{
		case CFillRadial::CENTERMODE::Geomcenter:
		case CFillRadial::CENTERMODE::Centroid:
		{
			point1 = viewinfo.DocToClient <Gdiplus::Point>(docrect.CenterPoint());
		}
		break;
		case CFillRadial::CENTERMODE::CenterPercent:
		{
			const CPoint center = CPoint(docrect.left+docrect.Width()*m_cx, docrect.top+docrect.Height()*m_cy);
			point1 = viewinfo.DocToClient <Gdiplus::Point>(center);
		}
		break;
		case CFillRadial::CENTERMODE::CenterOffset:
		{
			point1 = viewinfo.DocToClient <Gdiplus::Point>(CPoint(docrect.left+m_cx, docrect.top+m_cy));
		}
		break;
		case CFillRadial::CENTERMODE::CenterCoordinate:
		{
			point1 = viewinfo.DocToClient <Gdiplus::Point>(CPoint(m_cx, m_cy));
		}
		break;
		default:
		{
			const CPoint center = docrect.CenterPoint();
			point1 = viewinfo.DocToClient <Gdiplus::Point>(center);
		}
		break;
	}
	m_centermode = CFillRadial::CENTERMODE::CenterCoordinate;
	m_cx = point1.X;
	m_cy = point1.Y;

	CSize size(m_cr, m_fr);
	switch(m_radiusmode)//m_radiusmode
	{
		case CFillRadial::RADIUSMODE::OUTERELLIPSE:
		{
			const Gdiplus::Rect rect = viewinfo.DocToClient <Gdiplus::Rect>(docrect);
			size.cx = ceil(rect.Width/sqrt(2.f));
			size.cy = ceil(rect.Height/sqrt(2.f));
		}
		break;
		case CFillRadial::RADIUSMODE::INNERELLIPSE:
		{
			const Gdiplus::Rect rect = viewinfo.DocToClient <Gdiplus::Rect>(docrect);
			size.cx = ceil(rect.Width/2.f);
			size.cy = ceil(rect.Height/2.f);
		}
		break;
		case CFillRadial::RADIUSMODE::OUTERCIRCLE:
		{
			const Gdiplus::Rect rect = viewinfo.DocToClient <Gdiplus::Rect>(docrect);
			const int A = ceil(rect.Width/sqrt(2.f));
			const int B = ceil(rect.Height/sqrt(2.f));
			size.cx = max(A, B);
			size.cy = max(A, B);
		}
		break;
		case CFillRadial::RADIUSMODE::INNERCIRCLE:
		{
			const Gdiplus::Rect rect = viewinfo.DocToClient <Gdiplus::Rect>(docrect);
			const int R = min(rect.Width/2, rect.Height/2);
			size.cx = R;
			size.cy = R;
		}
		break;
		case CFillRadial::RADIUSMODE::RadiusPercent:
		{
			const Gdiplus::Rect rect = viewinfo.DocToClient <Gdiplus::Rect>(docrect);
			size.cx = rect.Width*m_cr;
			size.cy = rect.Height*m_fr;
		}
		break;
		case CFillRadial::RADIUSMODE::RadiusValue:
		case CFillRadial::RADIUSMODE::OUTPATH:
		default:
			size = viewinfo.DocToClient(CSize(m_cr, m_fr));
			break;
	};
	m_radiusmode = CFillRadial::RADIUSMODE::RadiusValue;
	m_cr = size.cx;
	m_fr = size.cy;

	Gdiplus::Point point2(0, 0);
	switch(m_focusmode)//m_focusmode // in svg focus point means center point of gdiplus
	{
		case CFillRadial::FOCUSMODE::CENTER:
		{
			point2 = point1;
		}
		break;
		case CFillRadial::FOCUSMODE::FocusPercent:
		{
			const Gdiplus::Rect rect = viewinfo.DocToClient <Gdiplus::Rect>(docrect);
			point2.X = rect.X+rect.Width*m_fx;
			point2.Y = rect.Y+rect.Height*m_fy;
		}
		break;
		case CFillRadial::FOCUSMODE::FocusOffset:
		{
			point2 = viewinfo.DocToClient <Gdiplus::Point>(CPoint(docrect.left+m_fx, docrect.top+m_fy));
		}
		break;
		case CFillRadial::FOCUSMODE::FocusCoordinate:
		default:
		{
			point2 = viewinfo.DocToClient <Gdiplus::Point>(CPoint(m_fx, m_fy));
		}
		break;
	}

	m_focusmode = CFillRadial::FOCUSMODE::FocusCoordinate;
	m_fx = point2.X;
	m_fy = point2.Y;
}

Gdiplus::Brush* CFillRadial::GetBrush(const float& fRatio, const CSize& dpi) const
{
	return m_stops.size()<2 ? (Gdiplus::Brush*)nullptr : ::new RadialBrush(m_centermode, m_radiusmode, m_focusmode, m_cx, m_cy, m_fx, m_fy, m_cr, m_fr, m_stops);
}

void CFillRadial::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CFillGradient::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		ar<<m_cx;
		ar<<m_cy;
		ar<<m_cr;
		ar<<m_fr;
		ar<<m_fx;
		ar<<m_fy;
		ar<<m_centermode;
		ar<<m_radiusmode;
		ar<<m_focusmode;
	}
	else
	{
		ar>>m_cx;
		ar>>m_cy;
		ar>>m_cr;
		ar>>m_fr;
		ar>>m_fx;
		ar>>m_fy;
		ar>>m_centermode;
		ar>>m_radiusmode;
		ar>>m_focusmode;
	}
}

void CFillRadial::ReleaseCE(CArchive& ar) const
{
	CFillGradient::ReleaseCE(ar);

	if(ar.IsStoring())
	{
	}
	else
	{
	}
}

void CFillRadial::ReleaseDCOM(CArchive& ar)
{
	CFillGradient::ReleaseDCOM(ar);

	if(ar.IsStoring())
	{
	}
	else
	{
	}
}

void CFillRadial::ReleaseWeb(CFile& file) const
{
	CFillGradient::ReleaseWeb(file);
}

void CFillRadial::ReleaseWeb(BinaryStream& stream) const
{
	CFillGradient::ReleaseWeb(stream);
}
void CFillRadial::ReleaseWeb(boost::json::object& json) const
{
	CFillGradient::ReleaseWeb(json);

	json["CenterMode"] = m_centermode;
	json["RadiusMode"] = m_radiusmode;
	json["FocusMode"] = m_focusmode;

	std::vector<float> child;
	child.push_back(m_cx);
	child.push_back(m_cy);
	child.push_back(m_fx);
	child.push_back(m_fy);
	child.push_back(m_cr);
	child.push_back(m_fr);
	json["Factors"] = boost::json::value_from(child);
}
void CFillRadial::ReleaseWeb(pbf::writer& writer) const
{
	CFillGradient::ReleaseWeb(writer);

	writer.add_byte(m_centermode);
	writer.add_byte(m_radiusmode);
	writer.add_byte(m_focusmode);

	writer.add_float(m_cx);
	writer.add_float(m_cy);
	writer.add_float(m_fx);
	writer.add_float(m_fy);
	writer.add_float(m_cr);
	writer.add_float(m_fr);	
}
void CFillRadial::ExportPs3(FILE* file, const CLibrary& library) const
{
	_ftprintf(file, _T("/clipper load "));
	_ftprintf(file, _T(" radfill\n"));
	_ftprintf(file, _T(" "));
	_ftprintf(file, _T("1112.5957 526.8436 5"));

	CPsboard psboard(library);
	/*if(m_color1 != nullptr)
		m_color1->ExportAIBrush(file,psboard);

	_ftprintf(file, _T(" "));

	if(m_color2 != nullptr)
		m_color2->ExportAIBrush(file,psboard);*/

	_ftprintf(file, _T(" radfill\n"));
}

void AFXAPI operator <<(CArchive& ar, CFillRadial::CENTERMODE& center)
{
	ar<<(BYTE)center;
};

void AFXAPI operator >>(CArchive& ar, CFillRadial::CENTERMODE& center)
{
	BYTE byte;
	ar>>byte;
	center = (CFillRadial::CENTERMODE)byte;
};

void AFXAPI operator <<(CArchive& ar, CFillRadial::RADIUSMODE& radial)
{
	ar<<(BYTE)radial;
};

void AFXAPI operator >>(CArchive& ar, CFillRadial::RADIUSMODE& radial)
{
	BYTE byte;
	ar>>byte;
	radial = (CFillRadial::RADIUSMODE)byte;
};

void AFXAPI operator <<(CArchive& ar, CFillRadial::FOCUSMODE& focus)
{
	ar<<(BYTE)focus;
};

void AFXAPI operator >>(CArchive& ar, CFillRadial::FOCUSMODE& focus)
{
	BYTE byte;
	ar>>byte;
	focus = (CFillRadial::FOCUSMODE)byte;
};
