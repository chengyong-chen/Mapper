#pragma once
#include "Mapinfo.h"
#include "BackInfo.h"

class CViewMonitor;
class CProjection;
class BinaryStream;
struct Vertex;
enum Interpolation : BYTE;

namespace boost {
	namespace json {
		class object;
	}
};
namespace pbf {
	class writer;
	using tag = uint8_t;
};
class __declspec(dllexport) CDatainfo : public CMapinfo
{
public:
	CDatainfo();
	CDatainfo(const CDatainfo& datainfo);

	~CDatainfo() override;

public:
	CString m_strType;
	CString m_strName;
	CTime m_TimeLine;
	DWORD m_dwMapKey;
	CBackInfo m_BackInfo;

public:
	float m_scaleMinimum;
	float m_scaleSource;
	float m_scaleMaximum;

public:
	CRect GetDocRect() const;
	CSize GetDocCanvas() const;
	CPoint GetDocOrigin() const;

public:
	double m_zoomPointToDoc;
	CRect WanToDoc(CRect rect) const;

public:
	void ResetProjection(CProjection* pProjection, bool action) override;

public:
	CDatainfo& operator=(const CDatainfo& datainfo);
	CString GetName() const
	{
		return m_strName;
	};
	double CalPointToDocZoom(const CProjection* pProjection) const;
	double GetLevelFromScale(const float& scale) const;

public:
	virtual void Serialize(CArchive& ar, const DWORD& dwVersion);
	virtual void ReleaseCE(CArchive& ar) const;
	virtual void ReleaseDCOM(CArchive& ar);
	virtual void ReleaseWeb(CFile& file) const;
	virtual void ReleaseWeb(BinaryStream& stream) const;
	virtual void ReleaseWeb(boost::json::object& json, BYTE& minMapLevel, BYTE& maxMapLevel, float& sourceLevel) const;
	virtual void ReleaseWeb(pbf::writer& writer, pbf::tag tag, BYTE& minMapLevel, BYTE& maxMapLevel, float& sourceLevel) const;
public:
	static double GetMapToViewFactorFromScale(const CProjection* pProjection, const float& fScale, const CPointF& mapPoint, const CSize& dpi);
	static float GetScaleFromMapToViewRatio(const CProjection* pProjection, const float& factorMapToView, const CPointF& mapPoint, const CSize& dpi);
	static CPoint Project(const CDatainfo& fMapinfo, const CDatainfo& tMapinfo, const CPoint& point);
};
