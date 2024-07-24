#include "stdafx.h"

#include "ExportEPS.h"
#include "ExportAI.h"

#include <cstdio>

#include "../Style/Hint.h"
#include "../Style/Library.h"
#include "../DataView/ViewInfo.h"
#include "../DataView/ViewExporter.h"
#include "../Layer/LayerTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL ExportAI(CLayerTree& layertree, const CDatainfo& datainfo, LPCTSTR pathname, const unsigned int& tolerance)
{
	FILE* file = _tfopen(pathname, _T("wt"));
	if(file == nullptr)
	{
		AfxMessageBox(_T("Cann't create a writeable AI file!"));
		return FALSE;
	}

	std::list<CStringA> fontlist;
	GatherAllFonts(layertree, fontlist);
	CRect docValidBox = GetValidRect(layertree);

	_ftprintf(file, _T("%%!PS-Adobe-3.0\n"));
	_ftprintf(file, _T("%%%%Creator: Adobe Illustrator(R) 8.0\n"));
	_ftprintf(file, _T("%%%%AI8_CreatorVersion: 8\n"));
	_ftprintf(file, _T("%%%%For: (Samson) (Diwatu)\n"));

	_ftprintf(file, _T("%%%%Title: (\\277\\325\\304\\243\\260\\346.ai)\n"));
	_ftprintf(file, _T("%%%%CreationDate: (5/17/2006) (4:19 PM)\n"));
	_ftprintf(file, _T("%%%%Canvassize: 16383\n"));

	int countLayers = 0;
	for(CTree<CLayer>::forwiterator it = layertree.forwbegin(); it != layertree.forwend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_bVisible == false)
			continue;
		if(layer->m_geomlist.GetCount() == 0)
			continue;

		countLayers++;
	}

	CRect rectDoc = datainfo.GetDocRect();
	docValidBox.IntersectRect(docValidBox, rectDoc);
	CViewExporter viewinfo(datainfo);
	viewinfo.m_bViewer = false;
	viewinfo.ScaleTo(nullptr, datainfo.m_scaleSource, CPointF(0, 0));
	Gdiplus::Rect box = viewinfo.DocToClient <Gdiplus::Rect>(docValidBox);
	viewinfo.m_ptViewPos.x = box.GetLeft();
	viewinfo.m_ptViewPos.y = box.GetTop();
	viewinfo.m_sizeView.cy *= -1;
	viewinfo.m_yFactorMapToView *= -1;
	box = viewinfo.DocToClient <Gdiplus::Rect>(docValidBox);

	fprintf(file, "%%%%BoundingBox: %d %d %d %d\n", box.GetLeft(), box.GetTop(), box.GetRight(), box.GetBottom());
	fprintf(file, "%%%%HiResBoundingBox: %d %d %d %d\n", box.GetLeft(), box.GetTop(), box.GetRight(), box.GetBottom());//need check
	fprintf(file, "%%%%DocumentProcessColors: Cyan Magenta Yellow Black\n");

	if(fontlist.size() > 0)
	{
		fprintf(file, "%%%%DocumentFonts:\n");
		for(const CStringA& font : fontlist)
		{
			fprintf(file, "%%%%+%s\n", font);
		}

		fprintf(file, "%%%%DocumentNeededFonts:\n");
		for(const CStringA& font : fontlist)
		{
			fprintf(file, "%%%%+%s\n", font);
		}
	}

	double cx = min(abs(box.Width), 16383);
	double cy = min(abs(box.Height), 16383);
	fprintf(file, "%%%%DocumentSuppliedResources: procset Adobe_level2_AI5 1.2 0\n");
	fprintf(file, "%%%%+ procset Adobe_typography_AI5 1.0 1\n");
	fprintf(file, "%%%%+ procset AGM_Gradient 1.0 0\n");
	fprintf(file, "%%%%+ procset Adobe_ColorImage_AI6 1.3 0\n");
	fprintf(file, "%%%%+ procset Adobe_Illustrator_AI5 1.3 0\n");
	fprintf(file, "%%%%+ procset Adobe_pattern_AI5 1.0 0\n");
	fprintf(file, "%%%%+ procset Adobe_cshow 2.0 8\n");
	fprintf(file, "%%%%+ procset Adobe_shading_AI8 1.0 0\n");
	fprintf(file, "%%AI5_FileFormat 4.0\n");
	fprintf(file, "%%AI3_ColorUsage: Color\n");
	fprintf(file, "%%AI7_ImageSettings: 0\n");
	fprintf(file, "%%%%CMYKProcessColor: 1 1 1 1 ([Registration])\n");
	fprintf(file, "%%%%AI6_ColorSeparationSet: 1 1 (AI6 Default Color Separation Set)\n");
	fprintf(file, "%%%%+ Options: 1 16 0 1 0 1 1 1 0 1 1 1 1 0 0 0 0 0 0 0 0 0 -1 -1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 0 1 2 3 4\n");
	fprintf(file, "%%%%+ PPD: 1 21 0 0 60 45 2 2 1 0 0 1 0 0 0 0 0 0 0 0 0 0 ()\n");
	fprintf(file, "%%AI3_TemplateBox: %g %g %g %g\n", box.X + box.Width / 2.f, box.Y + box.Height / 2.f, box.X + box.Width / 2.f, box.Y + box.Height / 2.f);
	fprintf(file, "%%AI3_TileBox: 12 15 564 743\n");
	fprintf(file, "%%AI3_DocumentPreview: None\n");
	fprintf(file, "%%AI5_ArtSize: %g %g\n", cx, cy);
	fprintf(file, "%%AI5_RulerUnits: 2\n");
	fprintf(file, "%%AI5_ArtFlags: 0 0 0 1 0 0 1 0 0\n");
	fprintf(file, "%%AI5_TargetResolution: 800\n");

	fprintf(file, "%%AI5_NumLayers: %d\n", countLayers);
	fprintf(file, "%%AI8_OpenToView: %g %g 1 1024 768 18 0 0 784 153 0 0\n", box.X + box.Width / 2.f - 1024 / 2.f, box.Y + box.Height / 2.f - 768 / 2.f);
	fprintf(file, "%%AI5_OpenViewLayers: ");
	for(int index = 0; index < countLayers; index++)
		fprintf(file, "7");
	fprintf(file, "\n");
	fprintf(file, "%%%%PageOrigin: 0 0\n");
	fprintf(file, "%%%%AI3_PaperRect:%d %d %d %d\n", box.GetLeft(), box.GetTop(), box.GetRight(), box.GetBottom());
	fprintf(file, "%%%%AI3_Margin:0 0 0 0\n");
	fprintf(file, "%%AI7_GridSettings: 72 8 72 8 1 0 0.8 0.8 0.8 0.9 0.9 0.9\n");
	fprintf(file, "%%AI9_Flatten: 1\n");
	fprintf(file, "%%AI12_CMSettings: 00.MS\n");
	fprintf(file, "%%%%EndComments\n");

	CString strDefault = (CString)GetExeDirectory() + _T("Assistant\\");
	CString strPath = AfxGetProfileString(HKEY_LOCAL_MACHINE, _T("Diwatu"), _T(""), _T("Assistant"), strDefault);
	CString strProlog = strPath + _T("AIProlog.txt");
	CFileException ex;
	CStdioFile prologfile;
	if(prologfile.Open(strProlog, CFile::modeRead | CFile::shareDenyWrite | CFile::typeText, &ex) == TRUE)
	{
		CString string;
		BOOL Notend = prologfile.ReadString(string);
		while(Notend == TRUE)
		{
			_ftprintf(file, _T("%s\n"), string);
			Notend = prologfile.ReadString(string);
		}
		prologfile.Close();
	}
	else
	{
		fclose(file);
		AfxMessageBox(_T("Cann't open AIProlog.txt!"));
		return FALSE;
	}
	CString strSetup = strPath + _T("AISetup.txt");
	CStdioFile setupfile;
	if(setupfile.Open(strSetup, CFile::modeRead | CFile::shareDenyWrite | CFile::typeText) == TRUE)
	{
		CString string;
		while(setupfile.ReadString(string) == TRUE)
		{
			if(string.Right(31) == _T("storerect rectpath clip newpath"))
				_ftprintf(file, _T("%d %d %d %d storerect rectpath clip newpath\n"), 0, 0, cx, cy);
			else
				_ftprintf(file, _T("%s\n"), string);
		}

		setupfile.Close();
	}
	else
	{
		AfxMessageBox(_T("Cann't open AISetup.txt!"));
		fclose(file);
		return FALSE;
	}

	for(CTree<CLayer>::postiterator it = layertree.postbegin(); it != layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_bVisible == false)
			continue;

		layer->ExportGeomPs3(file, viewinfo);
	}

	for(CTree<CLayer>::postiterator it = layertree.postbegin(); it != layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_bVisible == false)
			continue;

		layer->ExportTagPs3(file, viewinfo);
	}

	_ftprintf(file, _T("%%%%PageTrailer\n"));
	_ftprintf(file, _T("gsave annotatepage grestore showpage\n"));
	_ftprintf(file, _T("%%%%Trailer\n"));
	_ftprintf(file, _T("Adobe_Illustrator_AI5 /terminate get exec\n"));
	_ftprintf(file, _T("Adobe_shading_AI8 /terminate get exec\n"));
	_ftprintf(file, _T("Adobe_ColorImage_AI6 /terminate get exec\n"));
	_ftprintf(file, _T("Adobe_pattern_AI5 /terminate get exec\n"));
	_ftprintf(file, _T("AGM_Gradient /terminate get exec\n"));
	_ftprintf(file, _T("Adobe_typography_AI5 /terminate get exec\n"));
	_ftprintf(file, _T("Adobe_level2_AI5 /terminate get exec\n"));
	_ftprintf(file, _T("%%%%EOF\n"));

	fclose(file);

	fontlist.clear();
	return TRUE;
}
