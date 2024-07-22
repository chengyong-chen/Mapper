#include "stdafx.h"
#include <cmath>
#include "Color.h"

#include "Fill.h"
#include "FillLinear.h"
#include "Library.h"
#include "Psboard.h"
#include "../Dataview/viewinfo.h"
#include <boost/json.hpp>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CFillLinear, CFillGradient, 0)

CFillLinear::CFillLinear()
	: CFillGradient()
{
	m_valuemode = CFillLinear::VALUEMODE::Angle;
	m_nAngle = 0;
	m_spread = CFillLinear::SPREADMODE::pad;

	m_x1 = 0;
	m_y1 = 0;
	m_x2 = 1;
	m_y2 = 1;
}
CFillLinear::CFillLinear(double x1, double y1, double x2, double y2, short angle, std::map<unsigned int, CColor*> stops)
	: CFillGradient(stops)
{
	m_spread = CFillLinear::SPREADMODE::pad;

	m_nAngle = angle;
	m_x1 = x1;
	m_y1 = y1;
	m_x2 = x2;
	m_y2 = y2;
}

CFillLinear::~CFillLinear()
{
}
void CFillLinear::Sha1(boost::uuids::detail::sha1& sha1) const
{
	CFill::Sha1(sha1);

	sha1.process_byte(m_spread);
	sha1.process_bytes(&m_nAngle, sizeof(short));
	sha1.process_bytes(&m_x1, sizeof(double));
	sha1.process_bytes(&m_y1, sizeof(double));
	sha1.process_bytes(&m_x2, sizeof(double));
	sha1.process_bytes(&m_y2, sizeof(double));
}
BOOL CFillLinear::operator==(const CFill& fill) const
{
	if(CFillGradient::operator==(fill) == FALSE)
		return FALSE;
	else if(fill.IsKindOf(RUNTIME_CLASS(CFillLinear)) == FALSE)
		return FALSE;
	else if(m_valuemode != ((CFillLinear&)fill).m_valuemode)
		return FALSE;
	else if(m_nAngle != ((CFillLinear&)fill).m_nAngle)
		return FALSE;
	else if(m_x1 != ((CFillLinear&)fill).m_x1)
		return FALSE;
	else if(m_y1 != ((CFillLinear&)fill).m_y1)
		return FALSE;
	else if(m_x2 != ((CFillLinear&)fill).m_x2)
		return FALSE;
	else if(m_y2 != ((CFillLinear&)fill).m_y2)
		return FALSE;
	else
		return TRUE;
}

void CFillLinear::CopyTo(CFill* fill) const
{
	CFillGradient::CopyTo(fill);

	if(fill->IsKindOf(RUNTIME_CLASS(CFillLinear)) == TRUE)
	{
		((CFillLinear*)fill)->m_valuemode = m_valuemode;
		((CFillLinear*)fill)->m_nAngle = m_nAngle;
		((CFillLinear*)fill)->m_x1 = m_x1;
		((CFillLinear*)fill)->m_y1 = m_y1;
		((CFillLinear*)fill)->m_x2 = m_x2;
		((CFillLinear*)fill)->m_y2 = m_y2;
	}
}

void CFillLinear::Transform(const double& m11, const double& m21, const double& m31, const double& m12, const double& m22, const double& m32)
{
	if(m_valuemode == VALUEMODE::VALUEOffset)
	{
		const double x1 = m_x1*m11 + m_y1*m21;
		const double y1 = m_x1*m12 + m_y1*m22;
		const double x2 = m_x2*m11 + m_y2*m21;
		const double y2 = m_x2*m12 + m_y2*m22;
		m_x1 = x1;
		m_y1 = y1;
		m_x2 = x2;
		m_y2 = y2;
	}
	else if(m_valuemode == VALUEMODE::VALUEActual)
	{
		const double x1 = m_x1*m11 + m_y1*m21 + m31;
		const double y1 = m_x1*m12 + m_y1*m22 + m32;
		const double x2 = m_x2*m11 + m_y2*m21 + m31;
		const double y2 = m_x2*m12 + m_y2*m22 + m32;
		m_x1 = x1;
		m_y1 = y1;
		m_x2 = x2;
		m_y2 = y2;
	}
}

void CFillLinear::Transform(const CViewinfo& viewinfo, CRect& docrect)
{
	Gdiplus::Point point1(0, 0);
	Gdiplus::Point point2(0, 0);
	switch(m_valuemode)//valuemode
	{
		case CFillLinear::VALUEMODE::Angle:
			{
				Gdiplus::Rect rect = viewinfo.DocToClient <Gdiplus::Rect>(docrect);
				const Gdiplus::Point origin(rect.GetLeft(), rect.GetTop());
				rect.Offset(-origin.X, -origin.Y);
				if(m_nAngle >= 0 && m_nAngle < 90)
				{
					const double tg = tan(m_nAngle*M_PI/180);
					point1 = Gdiplus::Point(0, 0);
					point2 = Gdiplus::Point(rect.GetRight(), rect.Width*tg);
				}
				else if(m_nAngle >= 90 && m_nAngle < 180)
				{
					const double tg = tan((180 - m_nAngle)*M_PI/180);
					point1 = Gdiplus::Point(rect.GetRight(), 0);
					point2 = Gdiplus::Point(0, rect.Width*tg);
				}
				else if(m_nAngle >= 180 && m_nAngle < 270)
				{
					const double tg = tan((m_nAngle - 180)*M_PI/180);
					point1 = Gdiplus::Point(rect.GetRight(), rect.GetBottom());
					point2 = Gdiplus::Point(0, rect.Width*tg);
				}
				else if(m_nAngle >= 270 && m_nAngle < 360)
				{
					const double tg = tan((m_nAngle - 180)*M_PI/180);
					point1 = Gdiplus::Point(0, rect.GetBottom());
					point2 = Gdiplus::Point(rect.GetRight(), rect.Width*tg);
				}
			}
			break;
		case CFillLinear::VALUEMODE::VALUEPercent:
			{
				point1 = viewinfo.DocToClient <Gdiplus::Point>(CPoint(docrect.left + docrect.Width()*m_x1, docrect.top + docrect.Height()*m_y1));
				point2 = viewinfo.DocToClient <Gdiplus::Point>(CPoint(docrect.left + docrect.Width()*m_x2, docrect.top + docrect.Height()*m_y2));
			}
			break;
		case CFillLinear::VALUEMODE::VALUEOffset:
			{
				point1 = viewinfo.DocToClient <Gdiplus::Point>(CPoint(docrect.left + m_x1, docrect.top + m_y1));
				point2 = viewinfo.DocToClient <Gdiplus::Point>(CPoint(docrect.left + m_x2, docrect.top + m_y2));
			}
			break;
		case CFillLinear::VALUEMODE::VALUEActual:
		default:
			{
				point1 = viewinfo.DocToClient <Gdiplus::Point>(CPoint(m_x1, m_y1));
				point2 = viewinfo.DocToClient <Gdiplus::Point>(CPoint(m_x2, m_y2));
			}
			break;
	}
	m_valuemode = CFillLinear::VALUEMODE::VALUEActual;
	m_x1 = point1.X;
	m_y1 = point1.Y;
	m_x2 = point2.X;
	m_y2 = point2.Y;
}

Gdiplus::Brush* CFillLinear::GetBrush(const float& fRatio, const CSize& dpi) const
{
	return m_stops.size() < 2 ? (Gdiplus::Brush*)nullptr : ::new LinearBrush(m_valuemode, m_nAngle, m_x1, m_y1, m_x2, m_y2, m_stops);



	//const int stops = m_stops.size();
	//if(stops<2)
	//	return nullptr;
	//const Gdiplus::Color color1 = m_stops.begin()->second->GetColor();
	//const Gdiplus::Color color2 = m_stops.rbegin()->second->GetColor();
	//Gdiplus::LinearGradientBrush* brush = ::new Gdiplus::LinearGradientBrush(Gdiplus::RectF(m_x1, m_y1, m_x2, m_y2), color1, color2, 0);
	//brush->SetGammaCorrection(TRUE);
	//float* positions = new float[stops];
	//Gdiplus::Color* colors = new Gdiplus::Color[stops];
	//std::map<unsigned int, CColor*>::iterator it = m_stops.begin();
	//for(int index = 0; index<stops; index++)
	//{
	//	positions[index] = it->first/100.f;
	//	colors[index] = it->second->GetColor();
	//	it++;
	//}
	//brush->SetInterpolationColors(colors, positions, stops);
	//return brush;
}

void CFillLinear::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CFillGradient::Serialize(ar, dwVersion);

	BYTE reserved2 = 0X00;
	if(ar.IsStoring())
	{
		ar << m_valuemode;
		ar << m_nAngle;
		ar << m_x1;
		ar << m_y1;
		ar << m_x2;
		ar << m_y2;
	}
	else
	{
		ar >> m_valuemode;
		ar >> m_nAngle;
		ar >> m_x1;
		ar >> m_y1;
		ar >> m_x2;
		ar >> m_y2;
	}
}

void CFillLinear::ReleaseCE(CArchive& ar) const
{
	CFillGradient::ReleaseCE(ar);

	if(ar.IsStoring())
	{
		ar << m_nAngle;
	}
}

void CFillLinear::ReleaseDCOM(CArchive& ar)
{
	CFillGradient::ReleaseDCOM(ar);

	if(ar.IsStoring())
	{
		ar << m_nAngle;
	}
	else
	{
		ar >> m_nAngle;
	}
}

DWORD CFillLinear::PackPC(CFile* pFile, BYTE*& bytes)
{
	return CFillGradient::PackPC(pFile, bytes);
}

void CFillLinear::ReleaseWeb(CFile& file) const
{
	CFillGradient::ReleaseWeb(file);
}

void CFillLinear::ReleaseWeb(BinaryStream& stream) const
{
	CFillGradient::ReleaseWeb(stream);
}
void CFillLinear::ReleaseWeb(boost::json::object& json) const
{
	CFillGradient::ReleaseWeb(json);

	json["ValueMode"] = m_valuemode;

	std::vector<int> child;
	child.push_back(m_nAngle);
	child.push_back(m_x1);
	child.push_back(m_y1);
	child.push_back(m_x2);
	child.push_back(m_y2);
	json["Factors"] = boost::json::value_from(child);
}
void CFillLinear::ReleaseWeb(pbf::writer& writer) const
{
	CFillGradient::ReleaseWeb(writer);

	writer.add_byte(m_valuemode);

	writer.add_variant_int16(m_nAngle);
	writer.add_double(m_x1);
	writer.add_double(m_y1);
	writer.add_double(m_x2);
	writer.add_double(m_y2);

}
void CFillLinear::ExportPs3(FILE* file, const CLibrary& library) const
{
	CPsboard psboard(library);
	/*if(m_color1 != nullptr)
		m_color1->ExportAIBrush(file,psboard);
	if(m_color2 != nullptr)
		m_color2->ExportAIBrush(file,psboard);*/

	_ftprintf(file, _T("%d O\n"), m_bOverprint);
	_ftprintf(file, _T("F\n"));
}

void AFXAPI operator <<(CArchive& ar, CFillLinear::VALUEMODE& mode)
{
	ar << (BYTE)mode;
};

void AFXAPI operator >>(CArchive& ar, CFillLinear::VALUEMODE& mode)
{
	BYTE byte;
	ar >> byte;
	mode = (CFillLinear::VALUEMODE)byte;
};
