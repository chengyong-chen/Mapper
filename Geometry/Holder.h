#pragma once
#include "Geom.h"

namespace Holder
{
	//class CHolder abstract : public CGeom
	//{
	//public:
	//	void Draw(Gdiplus::Graphics& g, CLine* pLine, const CViewinfo& viewinfo, const float& ratio) const{};
	//	void Draw(Gdiplus::Graphics& g, const Gdiplus::Matrix& matrix, Context& context, const CSize& dpi) const override {};
	//	void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const float& ratio) const override {};
	//	void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const override {};

	//public:
	//	void ExportPs3(FILE* file, const Gdiplus::Matrix& matrix, CPsboard& aiKey) const override {};
	//	void ExportPs3(FILE* file, const CViewinfo& viewinfo, CPsboard& aiKey) const override {};
	//	void ExportPs3(FILE* file, const CLine* pLine, const CViewinfo& viewinfo, CPsboard& aiKey) const {};
	//	void ExportPdf(HPDF_Doc pdf, HPDF_Page page, const Gdiplus::Matrix& matrix) const override {};
	//	void ExportPdf(HPDF_Doc pdf, HPDF_Page page, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const override {};
	//};

	class AFX_EXT_CLASS Submap : public CPRect
	{
	public:
		CString m_strFile;
	public:
		Submap()
		{
			m_bType = 41;
		}
	public:
		void Serialize(CArchive& ar, const DWORD& dwVersion) override
		{
			CPRect::Serialize(ar, dwVersion);

			if(ar.IsStoring())
			{
			}
			else
			{	
				ar>>m_strFile;
				AfxToFullPath(ar.m_strFileName, m_strFile);
			}
		}
	};
};