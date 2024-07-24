#include "stdafx.h"

#include "ExportEps.h"

#include "../Style/Hint.h"
#include "../Style/GetFontFile.h"

#include "../DataView/ViewExporter.h"
#include "../Projection/Projection1.h"
#include "../Layer/LayerTree.h"

#include "../../Thirdparty/libharu/2.4.4/include/hpdf.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

jmp_buf env;

void Errorhandler(HPDF_STATUS error_no, HPDF_STATUS detail_no, void* user_data)
{
	CString string;
	string.Format(_T("ERROR: error_no=%04X, detail_no=%u\n", (HPDF_UINT)error_no, (HPDF_UINT)detail_no));
	OutputDebugString(string);
	//	longjmp(env,1);
}

BOOL ExportPdf(CLayerTree& layertree, CViewExporter& viewinfo, LPCTSTR pathname, const unsigned int& tolerance)
{
	HPDF_Doc pdf = HPDF_New(Errorhandler, nullptr);
	if(pdf == nullptr)
	{
		AfxMessageBox(_T("error: cannot create Pdf file\n"));
		return FALSE;
	}
	HPDF_SetCompressionMode(pdf, HPDF_COMP_ALL);
	//HPDF_UseJPFonts(pdf);
	//HPDF_UseKRFonts(pdf);
	//HPDF_UseCNSFonts(pdf);
	//HPDF_UseCNTFonts(pdf);

	//HPDF_UseKREncodings(pdf);
	//HPDF_UseCNSEncodings(pdf);
	//HPDF_UseCNTEncodings(pdf);
	HPDF_UseUTFEncodings(pdf);
	//HPDF_SetCurrentEncoder(pdf, "UTF-8");

	std::list<CStringA> fontlist;
	GatherAllFonts(layertree, fontlist);
	for(CStringA& strFont : fontlist)
	{
		if(CFontDesc::FontRealNameExists(strFont) == false)
		{
			strFont = "";
			continue;
		}
		const HPDF_FontDef def = HPDF_Doc_FindFontDef(pdf, strFont);
		if(def == nullptr) //have to load from the font file
		{
			CString strFile = CFontDesc::GetFileByReal(strFont);
			if(strFile.IsEmpty() == FALSE)
			{
				if(GetFontType(strFile) == "TTF")
				{
					const char* pFontName = HPDF_LoadTTFontFromFile(pdf, CStringA(strFile), HPDF_TRUE);
				}
				else if(GetFontType(strFile) == "TTC")
				{
					const int count = GetTTCFontCount(strFile);
					for(int index = 0; index < count; index++)
					{
						CStringA strSubFont = GetTTCFontName(strFile, index);
						if(strSubFont == strFont)
						{
							const char* pFontName = HPDF_LoadTTFontFromFile2(pdf, CStringA(strFile), index, HPDF_TRUE);
						}
					}
				}
			}
		}
	}

	CRect docRect = viewinfo.m_datainfo.GetDocRect();
	CRect docValidBox = GetValidRect(layertree);
	docValidBox.IntersectRect(docValidBox, docRect);
	const CRectF mapRect = viewinfo.m_datainfo.DocToMap(docValidBox);
	const double minX = (mapRect.left - (viewinfo.m_datainfo.m_pProjection == nullptr ? viewinfo.m_datainfo.m_mapOrigin.x : viewinfo.m_mapOrigin.x)) * viewinfo.m_xFactorMapToView;
	const double maxX = (mapRect.right - (viewinfo.m_datainfo.m_pProjection == nullptr ? viewinfo.m_datainfo.m_mapOrigin.x : viewinfo.m_mapOrigin.x)) * viewinfo.m_xFactorMapToView;
	const double maxY = viewinfo.m_sizeView.cy - (mapRect.top - (viewinfo.m_datainfo.m_pProjection == nullptr ? viewinfo.m_datainfo.m_mapOrigin.y : viewinfo.m_mapOrigin.y)) * viewinfo.m_yFactorMapToView;
	const double minY = viewinfo.m_sizeView.cy - (mapRect.bottom - (viewinfo.m_datainfo.m_pProjection == nullptr ? viewinfo.m_datainfo.m_mapOrigin.y : viewinfo.m_mapOrigin.y)) * viewinfo.m_yFactorMapToView;

	viewinfo.m_ptViewPos.x = minX;
	viewinfo.m_ptViewPos.y = viewinfo.m_sizeView.cy - maxY;
	viewinfo.m_sizeView.cy = 0;
	viewinfo.m_yFactorMapToView *= -1;
	const HPDF_REAL cx = min(maxX - minX, 1500000 * 72.f);
	const HPDF_REAL cy = min(maxY - minY, 1500000 * 72.f);
	try
	{
		const HPDF_Page page = HPDF_AddPage(pdf);
		if(page == nullptr)
			return false;

		HPDF_REAL width = HPDF_Page_SetWidth(page, cx);
		HPDF_REAL height = HPDF_Page_SetHeight(page, cy);
		const CStringA defaultFont = fontlist.front();
		const HPDF_Font font = HPDF_GetFont(pdf, defaultFont, "UTF-8");
		HPDF_Page_SetFontAndSize(page, font, 12);

		for(CTree<CLayer>::postiterator it = layertree.postbegin(); it != layertree.postend(); ++it)
		{
			CLayer* layer = *it;
			if(layer->m_bVisible == false)
				continue;

			layer->ExportGeomPdf(pdf, page, viewinfo);
		}

		for(CTree<CLayer>::postiterator it = layertree.postbegin(); it != layertree.postend(); ++it)
		{
			CLayer* layer = *it;
			if(layer->m_bVisible == false)
				continue;

			layer->ExportTagPdf(pdf, page, viewinfo);
		}
		const HPDF_STATUS status = HPDF_SaveToFile(pdf, CStringA(pathname));
		if(status == 0)
		{
		}
	}
	catch(CException* ex)
	{
		OutputDebugStringA("Error happened when export this map to pdf!");
		HPDF_Free(pdf);
		pdf = nullptr;
		ex->Delete();
	}

	if(pdf != nullptr)
	{
		HPDF_Free(pdf);
		return TRUE;
	}
	else
		return FALSE;
}
