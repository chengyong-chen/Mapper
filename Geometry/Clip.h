#pragma once

#include "Collection.h"
namespace boost {
	namespace json {
		class object;
	}
};
using namespace std;
class CGeom;

class AFX_EXT_CLASS CClip sealed : public CCollection<CGeom>
{
protected:
	DECLARE_SERIAL(CClip);
	CClip();
	CClip(Gdiplus::FillMode mode);
	
public:
	BOOL operator ==(const CGeom& geom) const override;
public:
	~CClip() override;

public:
	void Sha1(boost::uuids::detail::sha1& sha1) const override;
	void Sha1(boost::uuids::detail::sha1& sha1, const CSize offset) const override;

public:
	void Move(const int& dx, const int& dy) override;
	void Move(const CSize& delta) override;
	void RecalRect() override;
	bool Letitgo(CTypedPtrList<CObList, CGeom*>& geomlist) override;
	bool PickOn(const CPoint& point, const unsigned long& gap) const override;
	bool PickIn(const CPoint& point) const override;
	bool PickIn(const CRect& rect) const override;
	bool PickIn(const CPoint& center, const unsigned long& radius) const override;
	bool PickIn(const CPoly& polygon) const override;

public:
	void Draw(Gdiplus::Graphics& g, const Gdiplus::Matrix& matrix, Context& context, const CSize& dpi) const override;
	void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const float& ratio) const override;
	void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const override;

public:
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
	void ReleaseWeb(boost::json::object& json) const override;
	void ReleaseWeb(pbf::writer& writer, const CSize offset = CSize(0, 0)) const override;
public:
	CTypedPtrList<CObList, CPath*> m_paths;
	Gdiplus::FillMode m_mode;

	friend class CPath;
};
