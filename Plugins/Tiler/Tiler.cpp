#include "stdafx.h"
#include "Tiler.h"
#include "TilerDlg.h"

#include <math.h>
#include <io.h>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>
#include <boost/math/special_functions/round.hpp>
#include <MagickWand/MagickWand.h>
#include <shlwapi.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void Replace(CStringA& string, CStringA original, int data)
{
	char chars[256];
	sprintf_s(chars, 256, "%d", data);
	string.Replace(original, chars);
}

std::string ReadTextFile(std::string file)
{
	return std::string("");
}

CStringA GetCurrentModuleDirectory()
{
	HMODULE hModule = nullptr;

	if(!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCSTR)"DllMain", &hModule))
	{
		const int ret = GetLastError();
		fprintf(stderr, "GetModuleHandle returned %d\n", ret);
		return _T("");
	}
	else
	{
		char path[MAX_PATH];
		GetModuleFileNameA(hModule, path, sizeof(path));
		PathRemoveFileSpecA(path);
		return CStringA(path);
	}
}

void CTiler::Invoke(int count,...)
{
	CTilerDlg dlg;
	if(dlg.DoModal() == IDOK)
	{
		int minLevel = dlg.m_minLevel;
		int maxLevel = dlg.m_maxLevel;

		CString strSource = dlg.m_imagefile;
		CString strOutput = dlg.m_outputpath;
		if(_taccess(strSource, 00) == -1)
			return;
		if(strOutput.IsEmpty())
			return;
		if(PathIsDirectory(strOutput) == FALSE)
		CreateDirectory(strOutput,nullptr);

		MagickWandGenesis();
		MagickWand* pSourceWand = NewMagickWand();
		if(MagickReadImage(pSourceWand, CStringA(strSource)) == MagickFalse)
			return;

		AfxGetApp()->BeginWaitCursor();
		int cxSource = MagickGetImageWidth(pSourceWand);
		int cySource = MagickGetImageHeight(pSourceWand);
		int squareSource = max(cxSource,cySource);
		int levelSource = boost::math::iround(log(squareSource/256.f)/log(2.f));
		squareSource = 256*pow(2, levelSource);

		for(int level = minLevel; level <= maxLevel; level++)
		{
			int squareLevel = pow(2, level)*256;
			int cxLevel = boost::math::iround(cxSource*pow(2, level - levelSource));
			int cyLevel = boost::math::iround(cySource*pow(2, level - levelSource));
			int xgapLevel = boost::math::iround(max(0,(squareLevel-cxLevel)/2));
			int ygapLevel = boost::math::iround(max(0,(squareLevel-cyLevel)/2));
			MagickWand* pLevelWant = CloneMagickWand(pSourceWand);
			if(MagickResizeImage(pLevelWant, cxLevel, cyLevel, FilterType::UndefinedFilter) == MagickFalse)
				continue;

			CString levelfolder;
			levelfolder.Format(_T("%s\\%d"), strOutput, level);
			if(PathIsDirectory(levelfolder) == FALSE)
			CreateDirectory(levelfolder,nullptr);

			int count = pow(2.0, level);
			for(int row = 0; row < count; row++)
			{
				if(256*(row + 1) < ygapLevel)
					continue;
				if(256*(row + 0) > ygapLevel + cyLevel)
					continue;

				CString rowfolder;
				rowfolder.Format(_T("%s\\%d"), levelfolder, row);
				if(PathIsDirectory(rowfolder) == FALSE)
				CreateDirectory(rowfolder,nullptr);

				for(int col = 0; col < count; col++)
				{
					if(256*(col + 1) < xgapLevel)
						continue;
					if(256*(col + 0) > xgapLevel + cxLevel)
						continue;

					MagickWand* pTileWant = MagickGetImageRegion(pLevelWant, 256, 256, col*256 - xgapLevel, row*256 - ygapLevel);
					if(col*256 - xgapLevel < 0 || row*256 - ygapLevel < 0 || col*256 - xgapLevel + 256 > cxLevel || row*256 - ygapLevel + 256 > cyLevel) //sitting on edge
					{
						PixelWand* transparentColor = NewPixelWand();
						PixelSetColor(transparentColor, "transparent");
						MagickSetImageBackgroundColor(pTileWant, transparentColor);
						MagickExtentImage(pTileWant, 256, 256,min(0,col*256-xgapLevel),min(0,row*256-ygapLevel));
					}

					if(pTileWant != nullptr)
					{
						CString strTile;
						strTile.Format(_T("%s\\%d.png"), rowfolder, col);
						MagickWriteImage(pTileWant, CStringA(strTile));
						DestroyMagickWand(pTileWant);
					}
				}
			}
			DestroyMagickWand(pLevelWant);
		}
		TCHAR fname[_MAX_FNAME];
		_tsplitpath_s(strSource, nullptr, 0,nullptr, 0, fname,_MAX_FNAME, nullptr, 0);
		CFileException fileexc;
		CStringA pluginpath = GetCurrentModuleDirectory();
		CFile sourcehtmlfile;
		CFile targethtmlfile;
		CStringA htmltxt;
		if(sourcehtmlfile.Open(CString(pluginpath) + _T("\\Tiler.html"), CFile::modeRead, &fileexc) && targethtmlfile.Open(strOutput + _T("\\") + fname + _T(".html"), CFile::modeCreate | CFile::modeReadWrite | CFile::typeText))
		{
			DWORD len1 = __min(sourcehtmlfile.GetLength(), INT_MAX);
			len1 = sourcehtmlfile.Read(htmltxt.GetBuffer(len1), len1);
			htmltxt.ReleaseBuffer(len1);
			Replace(htmltxt, "[minLevel]", minLevel);
			Replace(htmltxt, "[maxLevel]", maxLevel);
			Replace(htmltxt, "[cx]", cxSource);
			Replace(htmltxt, "[cy]", cySource);
			Replace(htmltxt, "[square]", squareSource);
			Replace(htmltxt, "[centerx]", cxSource/2);
			Replace(htmltxt, "[centery]", cySource/2);

			std::string string(htmltxt);
			std::string javascript;

			std::smatch matched;
			std::regex rgx("<script type=\"text/javascript\" src=\"(.*?)\"></script>");
			while(std::regex_search(string, matched, rgx))
			{
				std::ifstream stream(matched[1]);
				std::stringstream buffer;
				buffer << stream.rdbuf();
				javascript += buffer.str();

				string = matched.suffix().str();
			}

			DWORD len2 = __min(htmltxt.GetLength(), INT_MAX);
			targethtmlfile.Write(htmltxt.GetBuffer(len2), len2);
			htmltxt.ReleaseBuffer(len2);
			sourcehtmlfile.Close();
			targethtmlfile.Close();
		}

		DestroyMagickWand(pSourceWand);
		MagickWandTerminus();
		AfxGetApp()->EndWaitCursor();
	}
}
