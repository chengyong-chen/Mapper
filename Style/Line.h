#pragma once

#include "Dash.h"
#include "Midtable.hpp"
#include "SymbolMid.hpp"


#include "../../Thirdparty/libharu/2.4.4/include/hpdf.h"
#include <boost/compute/detail/sha1.hpp>

class CPath;
class CColor;
class CPoly;
class CLibrary;
class CSymbolLine;
class CPsboard;
class CViewinfo;

class __declspec(dllexport) CLine : public CObject
{
protected:
	DECLARE_SERIAL(CLine);

public:
	CLine();
	CLine(CColor* pColor, unsigned short nWidth);

	~CLine() override;

public:
	enum LINETYPE : BYTE
	{
		Inherit = 0XFF,
		Alone = 0,
		Normal,
		Symbol,
		River,
		Blank = 128
	};

public:
	CColor* m_pColor;
	CDash m_dash;
	mutable	unsigned short m_nWidth;
	BYTE m_bCapindex;
	BYTE m_bJoinindex;
	BYTE m_bMiterlimit;
	bool m_bOverprint;

public:
	virtual void Sha1(boost::uuids::detail::sha1::digest_type& hash) const;
	virtual void Sha1(boost::uuids::detail::sha1& sha1) const;
public:
	virtual void Scale(float ratio);
public:
	virtual Gdiplus::Pen* GetPen(const float& fZoom, const CSize& dpi) const;

	virtual void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, const CPath* pPath, float fZoom) const
	{
	};
	virtual void Preoccupy(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, const CPath* pPath, float fZoom) const
	{
	}
public:
	virtual BOOL operator ==(const CLine& line) const;
	virtual BOOL operator !=(const CLine& line) const;
	virtual CLine* Clone() const;
	virtual void CopyTo(CLine* pDen) const;
	virtual unsigned short GetCount() const { return 0; };
	virtual LINETYPE Gettype() const { return LINETYPE::Alone; }
	virtual void Serialize(CArchive& ar, const DWORD& dwVersion);
	virtual void ReleaseCE(CArchive& ar) const;
	virtual void ReleaseDCOM(CArchive& ar);

	virtual DWORD PackPC(CFile* pFile, BYTE*& bytes);
	virtual void ReleaseWeb(CFile& file) const;
	virtual void ReleaseWeb(BinaryStream& stream) const;
	virtual void ReleaseWeb(boost::json::object& json) const;
	virtual void ReleaseWeb(pbf::writer& writer) const;
	virtual void ExportPs3(FILE* file, const CLibrary& library) const
	{
	};
	virtual void ExportPs3(FILE* file, CPsboard& aiKey) const;

	virtual void ExportPs3(FILE* file, const CViewinfo& viewinfo, CPsboard& aiKey, const CPath* pPath) const
	{
	};
	virtual void ExportPdf(HPDF_Page page) const;

	virtual void ExportPdf(HPDF_Doc pdf, HPDF_Page page, const CViewinfo& viewinfo, const CLibrary& library, const CPath* pPath) const
	{
	};

	virtual void SetWidth(const CMidtable<CSymbolMid<CSymbolLine>>& tagtable)
	{
	};
	virtual bool IsSolid() const;

	static CLine* Apply(BYTE type);

	virtual void Decrease(CLibrary& library) const
	{
	};
};

typedef CTypedPtrList<CObList, CLine*> CLineList;