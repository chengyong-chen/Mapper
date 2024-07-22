#pragma once

namespace boost {
	namespace 	json {
		class object;
	}
};
namespace pbf {
	class writer;
	using tag = uint8_t;
};
class BinaryStream;

class __declspec(dllexport) CMid abstract : public CObject
{
	DECLARE_DYNAMIC(CMid);
protected:
	CMid();

public:
	~CMid() override;

public:
	CString m_strFile;
	WORD m_wId;

	unsigned short m_nUsers;

public:
	virtual CMid* Clone() const;
	virtual void CopyTo(CMid* pMid) const;

public:
	virtual bool Equals(CMid* pMid) const;
public:
	virtual BOOL Load(CString strFile);
	virtual void Serialize(CArchive& ar, const DWORD& dwVersion);
	virtual void ReleaseCE(CArchive& ar) const;
	virtual void ReleaseDCOM(CArchive& ar);
	virtual void ReleaseWeb(CFile& file) const;
	virtual void ReleaseWeb(BinaryStream& stream) const;
	virtual void ReleaseWeb(boost::json::object& json) const;
	virtual void ReleaseWeb(pbf::writer& writer) const;
};
