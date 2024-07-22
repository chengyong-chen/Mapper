#pragma once

#include "Fill.h"

#include "../../Thirdparty/libharu/2.4.4/include/hpdf.h"

class __declspec(dllexport) CFillGeneral abstract : public CFill
{
protected:
	CFillGeneral();

public:
	bool m_bOverprint;
public:
	BOOL operator ==(const CFill& fill) const override;

	void CopyTo(CFill* pDen) const override;

public:
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
public:
	virtual void ExportPs3(FILE* file, const CLibrary& library) const
	{
	};

	virtual void ExportPs3(FILE* file, CPsboard& aiKey) const
	{
	};

	virtual void ExportPs3(FILE* file, const CViewinfo& viewinfo, CPsboard& aiKey, const CGeom* pGeom) const
	{
	};

	virtual void ExportPdf(HPDF_Page page) const
	{
	};

	virtual void ExportPdf(HPDF_Doc pdf, HPDF_Page page, const CViewinfo& viewinfo, const CLibrary& library, const CGeom* pGeom) const
	{
	};
public:
	static CFillGeneral* Apply(BYTE type);
};
