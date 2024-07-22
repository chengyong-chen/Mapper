#pragma once
#include <boost/compute/detail/sha1.hpp>

namespace boost {
	namespace json {
		class object;
	}
};
namespace pbf {
	class writer;
	using tag = uint8_t;
};
class CLibrary;
class CGeom;
class CViewinfo;
class CPsboard;
class BinaryStream;

class __declspec(dllexport) CFill abstract : public CObject
{
public:
	enum FILLTYPE : BYTE
	{
		Inherit = 0XFF,
		Alone = 0,
		Normal,
		Symbol,
		Linear,
		Radial,
		Pattern,
		Flash,
		Blank = 128
	};

protected:
	CFill();
public:
	virtual void Sha1(boost::uuids::detail::sha1::digest_type& hash) const;
	virtual void Sha1(boost::uuids::detail::sha1& sha1) const;
public:
	virtual FILLTYPE Gettype() const = 0;
	virtual Gdiplus::Brush* GetBrush(const float& fRatio, const CSize& dpi) const { return nullptr; };
	virtual Gdiplus::Brush* GetBrush(CLibrary& library, const float& fRatio, const CSize& dpi) const { return GetBrush(fRatio, dpi); };

public:
	virtual BOOL operator ==(const CFill& fill) const { return TRUE; };
	virtual BOOL operator !=(const CFill& fill) const;
	virtual CFill* Clone() const;

	virtual void CopyTo(CFill* pDen) const
	{
	};

	virtual void Transform(const double& m11, const double& m21, const double& m31, const double& m12, const double& m22, const double& m32)
	{
	};

	virtual void Transform(const CViewinfo& viewinfo, CRect& docrect)
	{
	};

public:
	virtual void Serialize(CArchive& ar, const DWORD& dwVersion)
	{
	};

	virtual void Decrease(CLibrary& library) const
	{
	};

	virtual void ReleaseCE(CArchive& ar) const
	{
	};

	virtual void ReleaseDCOM(CArchive& ar)
	{
	};
	virtual DWORD PackPC(CFile* pFile, BYTE*& bytes) { return 0; };

	virtual void ReleaseWeb(CFile& file) const
	{
	};
	virtual void ReleaseWeb(BinaryStream& stream) const
	{

	};
	virtual void ReleaseWeb(boost::json::object& json) const
	{
	};
	virtual void ReleaseWeb(pbf::writer& writer) const
	{
	};
	static CFill* Apply(BYTE type);
};
