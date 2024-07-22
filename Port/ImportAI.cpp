#include "stdafx.h"

#include "ImportAI.h"
#include "TokenString.h"
#include <cstdlib>
#include "../Geometry/Text.h"
#include "../Dataview/DataInfo.h"
#include "../Style/Line.h"
#include "../Layer/Layer.h"
#include "../Layer/LayerTree.h"
#include "../Public/Function.h"

using namespace std;
using namespace StyleSheet;

#include <vector>

#include <boost/any.hpp>
#include <boost/Tokenizer.hpp>

extern unsigned long uDecimal;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CImportAI::CImportAI(CDatainfo& datainfo, CLayerTree& layertree, DWORD& dwMaxGeomId)
	: CImport(datainfo, layertree, dwMaxGeomId), m_datahandler(dwMaxGeomId), m_pCurrentLayer(nullptr)
{
	m_fVersion = 4.0;

	m_datahandler.SetCallBack(this/*,&CImport::OnGeomParsed,&CImport::Change*/);
	gradientfill = false;
	in_gradient_instance = false;
	in_palette = false;
}

CImportAI::~CImportAI()
{
}

BOOL CImportAI::Import(LPCTSTR lpszFile)
{
	CPSTokenFile file;
	if(!file.Open(lpszFile, CFile::modeRead|CFile::typeBinary))
	{
		AfxMessageBox(_T("Cann't open this file "));
		return FALSE;
	}

	EPSHEADER header;
	file.Read(&header, 32);
	if(header.filetype==0XC6D3D0C5)//EPS
		file.Seek(header.startPS, 0);
	else //AI
		file.SeekToBegin();

	int token = -1;
	file.Next(token);
	if(ReadProlog(file)==false)
	{
		file.Close();
		AfxMessageBox(_T("Unsupported Illustrator file!"));
		return FALSE;
	}
	else
	{
		ReadSetup(file);

		while(token!=END)
		{
			std::any anyValue = file.Next(token);
			switch(token)
			{
				case DSC_COMMENT:
					if(std::any_cast<std::string>(anyValue)=="EOF")
					{
						token = END;
					}
					break;
				case END:
					break;
				case OPERATOR:
					this->ExcuteCommand(std::any_cast<std::string>(anyValue));
					break;
				default:
					if(token<=MAX_DATA_TOKEN)
					{
						m_epsStack.Push(anyValue);
					}
					break;
			}
		}
		file.Close();

		return CImport::Import(lpszFile);
	}
}

bool CImportAI::ReadProlog(CPSTokenFile& file)
{
	typedef boost::tokenizer<boost::char_separator<char>> Tokenizer;

	int token;
	while(true)
	{
		std::any anyValue = file.Next(token);
		if(token==DSC_COMMENT)
		{
			std::string strKey;
			std::string strValue = std::any_cast<std::string>(anyValue);
			if(strValue.find(':')!=-1)
			{
				Tokenizer tok(strValue, boost::char_separator<char>(":"));
				Tokenizer::iterator it = tok.begin();
				strKey = *it++;
				if(it!=tok.end())
					strValue = *it;
				else
					strValue = "";
			}
			else
				strKey = strValue;

			if(strKey=="EndProlog")
				break;
			if(strKey=="BeginSetup")
				break;
			if(strKey=="BeginData")
			{
				char* str = (char*)strValue.c_str();
				std::string strbytes = CTokenString::read_word(str, CHAR_DIGIT);
				if(CTokenString::IsInt(strbytes))
				{
					int bytes = atoi(strbytes.c_str());
					file.Seek(bytes, CFile::current);
				}
			}
			else if(strKey.substr(0, 14)==("AI5_FileFormat"))
				m_fVersion = strtod(strKey.substr(14).c_str(), nullptr);
			else if(strKey=="BeginProcSet")
				file.skip_to_dsc("EndProcSet", "EndProlog");//some ai files exported by corel draw don"t have an EndProcSet comment after a BeginProcSet...
			else if(strKey=="BeginResource")
				file.skip_to_dsc("EndResource", "EndProlog");
			else if(strKey=="BoundingBox")
			{
				stringstream ss(strValue);
				double left;
				double top;
				double right;
				double bottom;
				ss>>left;
				ss>>top;
				ss>>right;
				ss>>bottom;
				m_datainfo.m_mapOrigin.x = left;
				m_datainfo.m_mapOrigin.y = top;
				m_datainfo.m_mapCanvas.cx = right-left;
				m_datainfo.m_mapCanvas.cy = bottom-top;
			}
			//else if strKey == "Creator":
			//// try to determine whether the file really is an
			//// illustrator file as opposed to some other EPS
			//// file. It seems that Illustrator itself only
			//// accepts EPS files as illustrator files if they
			//// contain "Adobe Illustrator" in their Create
			//// DSC-comment
			//if string.find(value, "Adobe Illustrator") == -1:
			//add_message("This is probably not an"
			//" Illustrator file."
			//" Try embedding it as EPS")*/
		}
		else if(token==END)
			break;
	}

	return token!=END;
}

bool CImportAI::ReadSetup(CPSTokenFile& file) const
{
	int token;
	std::any anyValue = file.Next(token);
	std::string strValue = std::any_cast<std::string>(anyValue);
	if(strValue!="BeginSetup")
		return false;

	typedef boost::tokenizer<boost::char_separator<char>> Tokenizer;
	do
	{
		anyValue = file.Next(token);
		if(token==DSC_COMMENT)
		{
			std::string strKey;
			std::string strValue = std::any_cast<std::string>(anyValue);
			if(strValue.find(':')!=-1)
			{
				Tokenizer tok(strValue, boost::char_separator<char>(":"));
				Tokenizer::iterator it = tok.begin();
				strKey = *it++;
				strValue = *it;
			}
			else
			{
				strKey = strValue;
			}

			if(strKey=="PageTrailer")
				break;
			else if(strKey=="Trailer")
				break;
			else if(strKey=="EndSetup")
				break;
			if(strKey=="BeginData")
			{
				char* str = (char*)strValue.c_str();
				std::string strbytes = CTokenString::read_word(str, CHAR_DIGIT);
				if(CTokenString::IsInt(strbytes))
				{
					int bytes = atoi(strbytes.c_str());
					file.Seek(bytes, CFile::current);
				}
			}
			else if(strKey=="AI5_BeginPalette")
				file.skip_to_dsc("AI5_EndPalette", "EndSetup");
			else if(strKey=="AI8_BeginBrushPattern")
				file.skip_to_dsc("AI8_EndBrushPattern", "EndSetup");
			else if(strKey=="AI8_BeginPluginObject")
				file.skip_to_dsc("AI8_EndPluginObject", "EndSetup");
			else if(strKey=="AI5_Begin_NonPrinting")
				file.skip_to_dsc("AI5_End_NonPrinting--", "EndSetup");
		}
	} while(token!=END);

	return token!=END;
}

void CImportAI::ExcuteCommand(std::string strCmd)
{
	try
	{
		if(strCmd=="Xa") //set_fill_rgb 
		{
			float b = m_epsStack.PopFloat();
			float g = m_epsStack.PopFloat();
			float r = m_epsStack.PopFloat();
			m_epsStack.PopFloat();
			m_epsStack.PopFloat();
			m_epsStack.PopFloat();
			m_epsStack.PopFloat();
			m_datahandler.onFillColor(r, g, b);
		}
		else if(strCmd=="x") //set_fill_cmyk_custom 
		{
			float tint = m_epsStack.PopFloat();
			std::string name = m_epsStack.PopString();
			float k = m_epsStack.PopFloat();
			float y = m_epsStack.PopFloat();
			float m = m_epsStack.PopFloat();
			float c = m_epsStack.PopFloat();
			m_datahandler.onFillColor(c, m, y, k);
		}
		else if(strCmd=="XA") //set_line_rgb 
		{
			float b = m_epsStack.PopFloat();
			float g = m_epsStack.PopFloat();
			float r = m_epsStack.PopFloat();
			m_epsStack.PopFloat();
			m_epsStack.PopFloat();
			m_epsStack.PopFloat();
			m_epsStack.PopFloat();
			m_datahandler.onStrokeColor(r, g, b);
		}
		else if(strCmd=="X") //set_line_cmyk_custom 
		{
			float tint = m_epsStack.PopFloat();
		std:string name = m_epsStack.PopString();
			float k = m_epsStack.PopFloat();
			float y = m_epsStack.PopFloat();
			float m = m_epsStack.PopFloat();
			float c = m_epsStack.PopFloat();
			m_datahandler.onStrokeColor(c, m, y, k);
		}
		else if(strCmd=="XX") //set_line_generic_custom 
		{
			std::string name;
			int type = 0;
			if(type==0)
			{
				float k = m_epsStack.PopFloat();
				float y = m_epsStack.PopFloat();
				float m = m_epsStack.PopFloat();
				float c = m_epsStack.PopFloat();
				m_datahandler.onStrokeColor(c, m, y, k);
			}
			else
			{
				float b = m_epsStack.PopFloat();
				float g = m_epsStack.PopFloat();
				float r = m_epsStack.PopFloat();
				m_datahandler.onStrokeColor(r, g, b);
			}
		}
		else if(strCmd=="P") //set_line_pattern 
		{
			//name, px, py, sx, sy, angle, rf, r, k, ka,matrix
			//	if not in_palette:
			//	add_message(_("Vector patterns not supported. Using black"))
			//if(line_color != nullptr) delete line_color;
			//line_color = new CColorSpot(0,0,0);
		}
		else if(strCmd=="Xx") //set_fill_generic_custom 
		{
			CString name;
			float type = 0.0;
			if(type==0.0f)
			{
				float tint = m_epsStack.PopFloat();
				float k = m_epsStack.PopFloat();
				float y = m_epsStack.PopFloat();
				float m = m_epsStack.PopFloat();
				float c = m_epsStack.PopFloat();
				m_datahandler.onFillColor(c*tint, m*tint, y*tint, k*tint);
			}
			else
			{
				float b = m_epsStack.PopFloat();
				float g = m_epsStack.PopFloat();
				float r = m_epsStack.PopFloat();
				m_datahandler.onFillColor(r, g, b);
			}
		}
		else if(strCmd=="N") //invisible # an invisible open path
		{
			m_datahandler.onPathDiscard();
		}
		else if(strCmd=="u") //begin_group 
		{
			m_datahandler.onGroupBegin();
		}
		else if(strCmd=="U") //end_group 
		{
			m_datahandler.onCollectionEnd();
		}
		else if(strCmd=="*u"||strCmd=="newpath") //begin_compound_path 			
		{
			m_datahandler.onDonutBegin();
		}
		else if(strCmd=="*U") //strCmd == "gsave"  //end_compound_path 
		{
			m_datahandler.onCollectionEnd();
		}
		else if(strCmd=="*") //guide
		{
			std::string strOpe = m_epsStack.PopString();
			this->ExcuteCommand(strOpe);
		}
		else if(strCmd=="[") //mark 
		{
			m_epsStack.Push(std::string(""));
		}
		else if(strCmd=="]") //make_array
		{
			std::vector<std::any> a = m_epsStack.PopTo("");
			m_epsStack.Push(a);
		}
		else if(strCmd=="@") //ignore_operator 
		{
			int i = 0;
		}
		else if(strCmd=="&") //ignore_operator 
		{
			int i = 0;
		}
		
		else if(strCmd=="Lb") //begin_ai_layer 
		{
			CLayer* pLayer = new CLayer(m_layertree);
			pLayer->m_wId = m_layertree.ApplyId();
			delete pLayer->m_pLine;
			pLayer->m_pLine = nullptr;
			m_layertree.m_root.AddChild(nullptr, pLayer);
			m_pCurrentLayer = pLayer;
			if(m_fVersion>=4.0)
			{
				m_epsStack.Pop();//unused
				m_epsStack.Pop();//unused
			}
			m_epsStack.Pop();// blue,
			m_epsStack.Pop();//green,
			m_epsStack.Pop();//red, 
			m_epsStack.Pop();//color, 
			m_epsStack.Pop();//has_mlm
			if(m_fVersion>=4.0)
			{
				m_epsStack.Pop();//unused
			}
			m_epsStack.Pop();//dimmed,
			m_epsStack.Pop();//printing, 
			m_epsStack.Pop();//enabled,
			m_epsStack.Pop();//preview 
			m_pCurrentLayer->m_bVisible = m_epsStack.PopBool();
		}
		else if(strCmd=="Ln") //name_layer
		{
			if(m_pCurrentLayer!=nullptr)
			{
				std::string stdName = m_epsStack.PopString();
				OLECHAR* pOlechars = MultiByteToUnicode(CP_ACP, stdName.c_str());
				m_pCurrentLayer->m_strName = pOlechars;
				delete[] pOlechars;
			}
		}
		else if(strCmd=="LB") //end_ai_layer 
		{
			if(m_pCurrentLayer!=nullptr)
			{
				//POSITION pos = m_pLayer->m_geomlist.GetHeadPosition();
				//while(pos != nullptr)
				//{
				//	CGeom* pGeom = m_pLayer->m_geomlist.GetNext(pos);
				//	if(pGeom == nullptr)
				//		continue;

				//	pGeom->Polylize(m_pLayer->m_geomlist,10000);
				//}
				m_pCurrentLayer = nullptr;
			}
		}
		else if(strCmd=="Pb") //begin_palette 
		{
			in_palette = 1;
		}
		else if(strCmd=="PB") //end_palette 
		{
			in_palette = 0;
		}
		
		else if(strCmd=="To") //begin_text 
		{
			int type = m_epsStack.PopInt();
		}
		else if(strCmd=="TO") //end_text
		{
			//// first, turn the text accumulated in the list text_string into
			//// a single string and unify line endings to newline characters.
			//text = string.join(text_string, '');
			//text = string.replace(text, '\r\n', '\n');
			//text = string.replace(text, '\r', '\n');

			//// remove a trailing newline. Many Illustrator files contain a
			//// trailing newline as 'overflow' text, there's probably a better
			//// way to deal with this...
			//if(text[-1:] == "\n")
			//	text = text[:-1];

			//// Re-encode to Latin1
			//text = text_font.Reencode(text);

			//if(not string.strip(text))
			//	if(text_type == 2)
			//		end_composite();
			//		del composite_items[-1];
			//		if(len(composite_items) > 0)
			//			object = composite_items[-1];
			//	return;

			//// first create a simple text object
			//fs();
			//style.font = GetFont(text_font.psname);
			//style.font_size = text_size;
			//simple_text(text, text_trafo,
			//					halign = 6_ai_text_align[text_align])

			//// if we're actually supposed to create a path-text object, turn
			//// the text object just created into a path-text object
			//if(text_type == 2)
			//	GenericLoader.end_group(self);
			//	group = pop_last();
			//	objects = group.GetObjects();
			//	if(len(objects) == 2)
			//		path, text = objects;
			//		append_object(PathText(text, path,
			//									start_pos = text_start_pos));
			//		//composite_items[-1] = object

			//// we've finished the text object
			//in_text = 0;
		}

		

		else if(strCmd=="Tp") //begin_text_path 
		{
			int start = m_epsStack.PopInt();
			m_datahandler.FontState.textmatrix.dy = m_epsStack.PopFloat();
			m_datahandler.FontState.textmatrix.dx = m_epsStack.PopFloat();
			m_datahandler.FontState.textmatrix.m22 = m_epsStack.PopFloat();
			m_datahandler.FontState.textmatrix.m21 = m_epsStack.PopFloat();
			m_datahandler.FontState.textmatrix.m12 = m_epsStack.PopFloat();
			m_datahandler.FontState.textmatrix.m11 = m_epsStack.PopFloat();
		}
		else if(strCmd=="TP") //end_text_path
		{
			int i = m_epsStack.PopInt();
		}
		else if(strCmd=="XI") //raster_image 
		{
			//m_datahandler.onImageBegin();

			//if(bits != 8 or mode not in (1, 3))
			//add_message(_("Only images with 1 or 3 components "
			//					"and 8 bits/component supported"));
			//skip_to_dsc("AI5_EndRaster");
			//return;
			//decode = streamfilter.SubFileDecode(Tokenizer.source,
			//									'%AI5_EndRaster');
			//if(encoding == 0)
			//	decode = streamfilter.HexDecode(decode);
			//data_length = mode*width*height;
			//data = decode.read(data_length);
			////f = open("/tmp/dump.ppm", "w")
			////if mode == 1:
			////    f.write("P5\n%d %d\n255\n" % (width, height))
			////else
			////    f.write("P6\n%d %d\n255\n" % (width, height))
			////f.write(data)
			////f.close()
			//if(mode == 1)
			//	mode = 'L';
			//else if(mode == 3)
			//	mode = 'RGB';
			//else if(mode == 4)
			//	mode == 'CMYK';
			//image = Image.fromstring(mode, (width, height), data, 'raw', mode);
			//image(image, apply(Trafo, tuple(trafo)));
		}
		else if(strCmd=="O")
		{
		}
		else if(strCmd=="R")
		{
		}
		else if(strCmd=="XR")
		{
		}
		else if(strCmd=="Xy")
		{
		}

	}
	catch(CException* ex)
	{
		OutputDebugStringA("Error happened when export this map to AI!");
		OutputDebugString(_T("Error when export data to AI "));
		ex->Delete();
	}
}
