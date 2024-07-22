#pragma once

#include "Collection.h"

using namespace std;

class CGeom;
class CDib;

class AFX_EXT_CLASS CMask sealed : public CCollection<CGeom>
{
protected:
	DECLARE_SERIAL(CMask);
	CMask();
	CMask(CDib* pMask);
	
public:
	BOOL operator ==(const CGeom& geom) const override;
public:
	~CMask() override;
public:
	bool IsArea() const  override { return true; };
public:
	void Sha1(boost::uuids::detail::sha1& sha1) const override;
	void Sha1(boost::uuids::detail::sha1& sha1, const CSize offset) const override;

public:
	void Move(const int& dx, const int& dy) override;
	void Move(const CSize& delta) override;
	void RecalRect() override;
	bool Letitgo(CTypedPtrList<CObList, CGeom*>& geomlist) override;
	
public:
	void Draw(Gdiplus::Graphics& g, const Gdiplus::Matrix& matrix, Context& context, const CSize& dpi) const override;
	void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const float& ratio) const override;
	void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const override;
	
public:
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
	void ReleaseWeb(boost::json::object& json) const override;
	void ReleaseWeb(pbf::writer& writer, const CSize offset = CSize(0, 0)) const override;

public:
	CDib* m_pMask;
};
