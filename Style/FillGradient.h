#pragma once

#include "FillCompact.h"

using namespace std;
#include <map>

class CColor;
class CLibrary;

class __declspec(dllexport) CFillGradient abstract : public CFillCompact
{
public:
	enum SPREADMODE : BYTE
	{
		pad = 0,
		reflect,
		repeat
	};

public:
	CFillGradient();
	CFillGradient(std::map<unsigned int, CColor*> stops);

	~CFillGradient() override;

public:
	SPREADMODE m_spread;
	std::map<unsigned int, CColor*> m_stops;
public:
	void Sha1(boost::uuids::detail::sha1& sha1) const override;
public:
	void ClearStops();
	CColor* GetColor() const override
	{
		return nullptr;
	};
	void SetColor(CColor* pColor) override {};

public:
	BOOL operator ==(const CFill& fill) const override;
	void CopyTo(CFill* pDen) const override;
	FILLTYPE Gettype() const override { return FILLTYPE::Linear; };
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
	void ReleaseCE(CArchive& ar) const override;
	void ReleaseDCOM(CArchive& ar) override;
	DWORD PackPC(CFile* pFile, BYTE*& bytes) override;
	void ReleaseWeb(CFile& file) const override;
	void ReleaseWeb(BinaryStream& stream) const override;
	void ReleaseWeb(boost::json::object& json) const override;
	void ReleaseWeb(pbf::writer& writer) const override;
	void ExportPs3(FILE* file, const CLibrary& library) const override;
	void ExportPdf(HPDF_Page page) const override	{};
};

void AFXAPI operator <<(CArchive& ar, CFillGradient::SPREADMODE& spread);
void AFXAPI operator >>(CArchive& ar, CFillGradient::SPREADMODE& spread);
