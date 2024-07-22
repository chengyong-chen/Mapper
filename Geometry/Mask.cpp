#include "stdafx.h"
#include "Mask.h"

#include "../Image/Dib.h"
#include "../Style/lineSymbol.h"
#include "../Pbf/writer.hpp"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CMask, CCollection<CGeom>, 0)

CMask::CMask()
	: CCollection<CGeom>()
{
	m_bType = 17;
	m_pMask = nullptr;
}
CMask::CMask(CDib* pMask)
	: CMask()
{
	if(pMask!=nullptr)
	{
		m_pMask = pMask;
	}
}
CMask::~CMask()
{
}
void CMask::Sha1(boost::uuids::detail::sha1& sha1) const
{
	CCollection<CGeom>::Sha1(sha1);

	if(m_pMask!=nullptr)
	{
		m_pMask->Sha1(sha1);
	}
}
void CMask::Sha1(boost::uuids::detail::sha1& sha1, const CSize offset) const
{
	CCollection<CGeom>::Sha1(sha1, offset);

	if(m_pMask!=nullptr)
	{
		m_pMask->Sha1(sha1);
	}
}
BOOL CMask::operator==(const CGeom& geom) const
{
	if(CGeom::operator==(geom)==FALSE)
		return FALSE;
	else if(geom.IsKindOf(RUNTIME_CLASS(CMask))==FALSE)
		return FALSE;
	else
		return TRUE;
}
void CMask::RecalRect()
{
	CCollection::RecalRect();

}
void CMask::Move(const int& dx, const int& dy)
{

	CCollection::Move(dx, dy);
}
void CMask::Move(const CSize& Δ)
{


	CCollection::Move(Δ);
}
void CMask::Draw(Gdiplus::Graphics& g, const Gdiplus::Matrix& matrix, Context& context, const CSize& dpi) const
{
	CCollection::Draw(g, matrix, context, dpi);

}

void CMask::Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const float& ratio) const
{
	CCollection::Draw(g, viewinfo, ratio);

}

void CMask::Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const
{
	if(m_pMask!=nullptr)
	{
		Gdiplus::Rect rect = viewinfo.DocToClient<Gdiplus::Rect>(m_Rect);
		Gdiplus::Bitmap bitmap(rect.Width, rect.Height, PixelFormat32bppARGB);
		Gdiplus::Graphics graphic(&bitmap);
		graphic.Clear(Gdiplus::Color::Transparent);
		graphic.SetInterpolationMode(Gdiplus::InterpolationMode::InterpolationModeHighQualityBicubic);
		graphic.SetCompositingMode(Gdiplus::CompositingMode::CompositingModeSourceCopy);
		graphic.SetCompositingQuality(Gdiplus::CompositingQuality::CompositingQualityHighQuality);

		const int x = rect.X;
		const int y = rect.Y;
		rect.Offset(-x, -y);
		const CPoint oldOrigin = viewinfo.m_ptViewPos;
		viewinfo.m_ptViewPos.Offset(x, y);
		CCollection::Draw(graphic, viewinfo, library, context);

		graphic.SetCompositingMode(Gdiplus::CompositingMode::CompositingModeSourceCopy);
		m_pMask->Draw(graphic, rect, rect, true);
		
		viewinfo.m_ptViewPos = oldOrigin;
		const Gdiplus::CompositingMode oldMode = g.GetCompositingMode();
		g.SetCompositingMode(Gdiplus::CompositingMode::CompositingModeSourceOver);
		g.DrawImage(&bitmap, x, y);
		g.SetCompositingMode(oldMode);
	}
	else
	{
		CCollection::Draw(g, viewinfo, library, context);
	}
}
void CMask::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CCollection::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
	}
	else
	{
	}
}
void CMask::ReleaseWeb(boost::json::object& json) const
{
	CCollection::ReleaseWeb(json);
	if(m_pMask != nullptr)
	{
		m_pMask->ReleaseWeb(json, "mask");
	}
}
void CMask::ReleaseWeb(pbf::writer& writer, const CSize offset) const
{
	CCollection::ReleaseWeb(writer, offset);

	if(m_pMask != nullptr)
	{
		writer.add_bool(true);
		m_pMask->ReleaseWeb(writer, offset);
	}
	else
		writer.add_bool(false);
}
bool CMask::Letitgo(CTypedPtrList<CObList, CGeom*>& geomlist)
{
	CCollection::UnGroup(geomlist);

	return true;
}