void CFontComboBox::LoadFontList(CString strFolder)
{
	WIN32_FIND_DATA finddata;
	HANDLE hFile2 = FindFirstFile(strFolder + _T("\\*.*"), &finddata);
	do
	{	
		if (_tcscmp(finddata.cFileName, _T(".")) == 0 || _tcscmp(finddata.cFileName, _T("..")) == 0)
			continue;
		else if ((finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
			LoadFontList(strFolder + _T('\\') + finddata.cFileName);
		else {
			std::filesystem::path filePath = finddata.cFileName;
			std::wstring ext = filePath.extension();
			std::wstring filename = wstring(strFolder) + L"\\" + finddata.cFileName;
			if (ext == L".ttf" || ext == L".otf")
			{	TTFParser parser;
				if (parser.Parse(filename) == false)
				{
					MessageBox(L"Font file cannot be parsed!", L"Error", MB_ICONERROR);
				}
				else
				{
					std::wostringstream stream;
					stream << L"FontName : " << parser.GetFontName() << L"\r\n\r\n";
					stream << L"Copyright : " << parser.GetCopyright() << L"\r\n\r\n";
					stream << L"FontFamilyName : " << parser.GetFontFamilyName() << L"\r\n\r\n";
					stream << L"FontSubFamilyName : " << parser.GetFontSubFamilyName() << L"\r\n\r\n";
					stream << L"FontID : " << parser.GetFontID() << L"\r\n\r\n";
					stream << L"Version : " << parser.GetVersion() << L"\r\n\r\n";
					stream << L"PostScriptName : " << parser.GetPostScriptName() << L"\r\n\r\n";
					stream << L"Trademark : " << parser.GetTrademark() << L"\r\n\r\n";
					CSpotFont::m_fontcollection.AddFontFile(filename.c_str());

					CFontDesc* pDesc = new CFontDesc(parser.GetFontName().c_str(), parser.GetFontFamilyName().c_str(), TTF_FONT);
					m_mapFonts.SetAt(CString(parser.GetFontName().c_str()), pDesc);
				}
			}
			else if (ext == L".ttc")
			{
				TTCParser parser;
				if (parser.Parse(filename) == false)
				{
					MessageBox(L"Font file cannot be parsed!", L"Error", MB_ICONERROR);
					return;
				}
				else
				{
					CString szStr = L"";
					for (size_t i = 0; i < parser.Size(); ++i)
					{
						std::wostringstream stream;
						stream << L"FontName : " << parser.GetFontName(i) << L"\r\n\r\n";
						stream << L"Copyright : " << parser.GetCopyright(i) << L"\r\n\r\n";
						stream << L"FontFamilyName : " << parser.GetFontFamilyName(i) << L"\r\n\r\n";
						stream << L"FontSubFamilyName : " << parser.GetFontSubFamilyName(i) << L"\r\n\r\n";
						stream << L"FontID : " << parser.GetFontID(i) << L"\r\n\r\n";
						stream << L"Version : " << parser.GetVersion(i) << L"\r\n\r\n";
						stream << L"PostScriptName : " << parser.GetPostScriptName(i) << L"\r\n\r\n";
						stream << L"Trademark : " << parser.GetTrademark(i) << L"\r\n\r\n";

						if (i != parser.Size() - 1)
							stream << L"==========================================" << L"\r\n\r\n";

						szStr += stream.str().c_str();
						CSpotFont::m_fontcollection.AddFontFile(filename.c_str());
						CFontDesc* pDesc = new CFontDesc(parser.GetFontName(i).c_str(), parser.GetFontFamilyName(i).c_str(), TTF_FONT);
						m_mapFonts.SetAt(CString(parser.GetFontName(i).c_str()), pDesc);
					}
				}
			}
		}
	} while (FindNextFile(hFile2, &finddata) == TRUE);
}
