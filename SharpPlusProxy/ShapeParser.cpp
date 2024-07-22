#include "ShapeParser.h"

#include <list>
#include <any>
#include <string>
#include <msclr\marshal_cppstd.h>
#include <filesystem>
#include <tchar.h>

using namespace System;
using namespace System::IO;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Windows;
using namespace System::Runtime::InteropServices;
using namespace PdfSharp::Pdf;
using namespace PdfSharp::Pdf::IO;
using namespace PdfSharp::Pdf::Content;
using namespace PdfSharp::Pdf::Content::Objects;
using namespace PdfSharp::Pdf::Advanced;
using namespace ShapeConverter::BusinessLogic::Helper;
using namespace ShapeConverter::BusinessLogic::Parser::Pdf::Pattern;
using namespace ShapeConverter::BusinessLogic::Parser::Pdf::Shading;
using namespace ShapeConverter::BusinessLogic::Parser::Pdf::XObject;
using namespace ShapeConverter::Parser::Pdf;
using namespace ShapeConverter::Parser::Pdf::ExtendedStates;

std::list<std::pair<std::string, FontDescriptor*>> ExtractFonts(PdfResources^ resources)
{
	std::list<std::pair<std::string, FontDescriptor*>> ret;
	std::string tempPath = std::filesystem::temp_directory_path().string();
	std::string tempDir = tempPath+"Import";
	std::filesystem::create_directory(tempDir);

	int filenameCounter = 0;
	PdfDictionary^ fonts = resources->Fonts;
	for each(String^font in fonts->Elements->Keys)
	{
		auto fontdict = fonts->Elements->GetDictionary(font);
		auto subtype = fontdict->Elements->GetName("/Subtype");
		auto descriptor = fontdict->Elements->GetDictionary("/FontDescriptor");
		if(descriptor==nullptr)
			continue;
		std::string ext;
		if(subtype=="/TrueType")
			ext = "ttf";
		else if(subtype=="/Type1")
			ext = "font";
		else
			continue;
		
		auto fontDescriptor = new FontDescriptor();
		for each(auto key2 in descriptor->Elements->Keys)
		{
			if(key2=="/FontFile"||key2=="/FontFile2"||key2=="/FontFile3")
			{
				auto file = descriptor->Elements->GetDictionary(key2);
				auto stream = System::IO::File::Open(String::Format("{0}\\Font{1}.{2}", gcnew String(tempDir.c_str()), filenameCounter++, gcnew String(ext.c_str())), FileMode::Create);
				BinaryWriter^ writer = gcnew BinaryWriter(stream);
				writer->Write(file->Stream->UnfilteredValue);
				fontDescriptor->file = msclr::interop::marshal_as<std::string>(stream->Name);
			}
			else if(key2=="/FontFamily")
			{
				fontDescriptor->family = msclr::interop::marshal_as<std::string>(descriptor->Elements->GetString(key2));
			}
			else if(key2=="/FontName")
			{
				auto cname = descriptor->Elements->GetName(key2);
				fontDescriptor->name = msclr::interop::marshal_as<std::string>(cname->Substring(1, cname->Length-1));
			}
		}
		ret.push_back(std::make_pair(msclr::interop::marshal_as<std::string>(font), fontDescriptor));
	}
	return ret;
}

std::string SystemToStd(String^ string){
	IntPtr ip = Marshal::StringToHGlobalAnsi(string);
	std::string converted = static_cast<const char*>(ip.ToPointer());
	Marshal::FreeHGlobal(ip);
	return converted;
}

std::list<std::pair<const char*, std::stack<std::any>>>* Interpreter(PdfSharp::Pdf::PdfPage^ page, Objects::CSequence^ sequence)
{
	if(page==nullptr)
		return nullptr;

	auto dictionary = page->Elements->GetDictionary("/Resources");
	PdfResources^ resources = gcnew PdfResources(dictionary);

	//	auto extendedStatesManager = gcnew ExtendedStates::ExtendedStatesManager();
	//	extendedStatesManager->Init(resources);
	//	auto fontManager = gcnew FontManager();
	//	fontManager->Init(resources);
	//
	//	auto patternManager = gcnew PatternManager();
	//	patternManager->Init(resources);
	//
	//	auto colorSpaceManager = gcnew ColorSpaceManager(patternManager);
	//	colorSpaceManager->Init(resources);
	//
	//	auto shadingManager = gcnew ShadingManager();
	//	shadingManager->Init(resources);
	//
	//	auto xObjectManager = gcnew XObjectManager();
	//	xObjectManager->Init(resources);

	std::list<std::pair<const char*, std::stack<std::any>>>* cmds = new std::list<std::pair<const char*, std::stack<std::any>>>();
	{
		std::list<std::pair<std::string, FontDescriptor*>> fonts = ExtractFonts(resources);
		std::stack<std::any> stack;
		stack.push(fonts);
		cmds->push_back(std::make_pair("fonts", stack));
	}
	
	for(int index = 0; index<sequence->Count; index++)
	{
		Objects::COperator^ cmd = (Objects::COperator^)sequence[index];
		std::stack<std::any> stack;
		switch(cmd->OpCode->OpCodeName)
		{
			case OpCodeName::re:
			{
				auto x = PdfUtilities::GetDouble(cmd->Operands[0]);
				auto y = PdfUtilities::GetDouble(cmd->Operands[1]);
				auto width = PdfUtilities::GetDouble(cmd->Operands[2]);
				auto height = PdfUtilities::GetDouble(cmd->Operands[3]);
				stack.push(x);
				stack.push(y);
				stack.push(width);
				stack.push(height);
				cmds->push_back(std::make_pair("re", stack));
				break;
			}
			case OpCodeName::m:// move to
			{
				auto x = PdfUtilities::GetDouble(cmd->Operands[0]);
				auto y = PdfUtilities::GetDouble(cmd->Operands[1]);
				stack.push(x);
				stack.push(y);
				cmds->push_back(std::make_pair("m", stack));
				break;
			}
			case OpCodeName::l:// line to
			{
				auto x = PdfUtilities::GetDouble(cmd->Operands[0]);
				auto y = PdfUtilities::GetDouble(cmd->Operands[1]);
				stack.push(x);
				stack.push(y);
				cmds->push_back(std::make_pair("l", stack));
				break;
			}
			case OpCodeName::c:// cubic bezier
			{
				auto x1 = PdfUtilities::GetDouble(cmd->Operands[0]);
				auto y1 = PdfUtilities::GetDouble(cmd->Operands[1]);
				auto x2 = PdfUtilities::GetDouble(cmd->Operands[2]);
				auto y2 = PdfUtilities::GetDouble(cmd->Operands[3]);
				auto x3 = PdfUtilities::GetDouble(cmd->Operands[4]);
				auto y3 = PdfUtilities::GetDouble(cmd->Operands[5]);
				stack.push(x1);
				stack.push(y1);
				stack.push(x2);
				stack.push(y2);
				stack.push(x3);
				stack.push(y3);
				cmds->push_back(std::make_pair("c", stack));
				break;
			}
			case OpCodeName::v:// quadratic bezier
			{
				auto x2 = PdfUtilities::GetDouble(cmd->Operands[0]);
				auto y2 = PdfUtilities::GetDouble(cmd->Operands[1]);
				auto x3 = PdfUtilities::GetDouble(cmd->Operands[2]);
				auto y3 = PdfUtilities::GetDouble(cmd->Operands[3]);
				stack.push(x2);
				stack.push(y2);
				stack.push(x3);
				stack.push(y3);
				cmds->push_back(std::make_pair("v", stack));
				break;
			}
			case OpCodeName::y:// quadratic bezier
			{
				auto x1 = PdfUtilities::GetDouble(cmd->Operands[0]);
				auto y1 = PdfUtilities::GetDouble(cmd->Operands[1]);
				auto x2 = PdfUtilities::GetDouble(cmd->Operands[2]);
				auto y2 = PdfUtilities::GetDouble(cmd->Operands[3]);
				stack.push(x1);
				stack.push(y1);
				stack.push(x2);
				stack.push(y2);
				cmds->push_back(std::make_pair("y", stack));
				break;
			}
			case OpCodeName::n:// path painting operators end the path without filling and stroking
			{
				cmds->push_back(std::make_pair("n", stack));
				break;
			}
			case OpCodeName::W:// set clipping path
			{
				cmds->push_back(std::make_pair("W", stack));
				break;
			}
			case OpCodeName::Wx:
			{
				//ccy	graphicsState->ClippingPath = currentGeometry;
				//ccy	graphicGroup = new GraphicGroup();
				//ccy	graphicGroup.Clip = currentGeometry;
				//ccy	returnGraphicGroup.Children.Add(graphicGroup);
				cmds->push_back(std::make_pair("Wx", stack));
				break;
			}
			case OpCodeName::h:// close path
			{
				cmds->push_back(std::make_pair("h", stack));
			}
			break;
			case OpCodeName::s:// close and fill the path
			{
				cmds->push_back(std::make_pair("s", stack));
				break;
			}
			//ccy   
			case OpCodeName::S:// stroke the path
			{
				cmds->push_back(std::make_pair("S", stack));
				break;
			}
			//ccy   
			case OpCodeName::b:// close, fill and stroke the path
			case OpCodeName::bx:
			{
				cmds->push_back(std::make_pair("bx", stack));
				break;
			}
			//ccy  
			case OpCodeName::B: // fill and stroke the path
			{
				cmds->push_back(std::make_pair("B", stack));
				break;
			}
			case OpCodeName::Bx:// fill and stroke the path
			{
				//ccy       auto path = GetCurrentPathFilledAndStroked();
				//ccy       currentGeometry.FillRule = GraphicFillRule.NoneZero;
				//ccy       currentGeometry.FillRule = GraphicFillRule.EvenOdd;
				//ccy       graphicGroup.Children.Add(path);
				//ccy       ResetCurrentGeometry();
				cmds->push_back(std::make_pair("Bx", stack));
				break;
			}
			case OpCodeName::F:// fill the path
			case OpCodeName::f:
			{
				//ccy       auto path = GetCurrentPathFilled();
				//ccy       currentGeometry.FillRule = GraphicFillRule.NoneZero;
				//ccy       graphicGroup.Children.Add(path);
				//ccy       ResetCurrentGeometry();
				cmds->push_back(std::make_pair("f", stack));
				break;
			}
			case OpCodeName::fx:// fill the path
			{
				cmds->push_back(std::make_pair("fx", stack));
				break;
			}
			case OpCodeName::CS:// set color space for stroking operations
			{
				auto colorspace = ((CName^)cmd->Operands[0])->Name;
				stack.push(SystemToStd(colorspace));
				cmds->push_back(std::make_pair("CS", stack));
				break;
			}
			case OpCodeName::cs:// set color space for nonstroking operations
			{
				auto colorspace = ((CName^)cmd->Operands[0])->Name;
				stack.push(SystemToStd(colorspace));
				cmds->push_back(std::make_pair("cs", stack));
				break;
			}
			case OpCodeName::rg:// set /DeviceRGB and non-stroked color
			{
				auto r = PdfUtilities::GetDouble(cmd->Operands[0]);
				auto g = PdfUtilities::GetDouble(cmd->Operands[1]);
				auto b = PdfUtilities::GetDouble(cmd->Operands[2]);
				stack.push(r);
				stack.push(g);
				stack.push(b);
				cmds->push_back(std::make_pair("rg", stack));
				break;
			}
			case OpCodeName::k:// set /DeviceCMYK and non-stroked color
			{
				auto c = PdfUtilities::GetDouble(cmd->Operands[0]);
				auto m = PdfUtilities::GetDouble(cmd->Operands[1]);
				auto y = PdfUtilities::GetDouble(cmd->Operands[2]);
				auto k = PdfUtilities::GetDouble(cmd->Operands[3]);
				stack.push(c);
				stack.push(m);
				stack.push(y);
				stack.push(k);
				cmds->push_back(std::make_pair("k", stack));
				break;
			}
			case OpCodeName::g:// set /DeviceGray and non-stroked color
			{
				auto gray = PdfUtilities::GetDouble(cmd->Operands[0]);
				stack.push(gray);
				cmds->push_back(std::make_pair("g", stack));
				break;
			}
			case OpCodeName::sc:// non-stroked color
			{
				auto c1 = PdfUtilities::GetDouble(cmd->Operands[0]);
				auto c2 = PdfUtilities::GetDouble(cmd->Operands[1]);
				auto c3 = PdfUtilities::GetDouble(cmd->Operands[2]);
				stack.push(c1);
				stack.push(c2);
				stack.push(c3);
				cmds->push_back(std::make_pair("sc", stack));
				break;
			}
			case OpCodeName::scn:// ICC based non-stroked color
			{
				auto v1 = PdfUtilities::GetDouble(cmd->Operands[0]);
				auto v2 = PdfUtilities::GetDouble(cmd->Operands[1]);
				auto v3 = PdfUtilities::GetDouble(cmd->Operands[2]);
				auto v4 = PdfUtilities::GetDouble(cmd->Operands[3]);
				stack.push(v1);
				stack.push(v2);
				stack.push(v3);
				stack.push(v4);
				cmds->push_back(std::make_pair("scn", stack));
				break;
			}
			case OpCodeName::SCN:// ICC based stroked color
			{
				auto v1 = PdfUtilities::GetDouble(cmd->Operands[0]);
				auto v2 = PdfUtilities::GetDouble(cmd->Operands[1]);
				auto v3 = PdfUtilities::GetDouble(cmd->Operands[2]);
				auto v4 = PdfUtilities::GetDouble(cmd->Operands[3]);
				stack.push(v1);
				stack.push(v2);
				stack.push(v3);
				stack.push(v4);
				cmds->push_back(std::make_pair("SCN", stack));
				break;
			}
			case OpCodeName::RG:// set /DeviceRGB and stroked color
			{
				//ccy       graphicsState->StrokeColorSpace = colorSpaceManager.GetColorSpace(PdfKeys.DeviceRGB);
				//ccy       graphicsState->StrokeBrush =
				//ccy           graphicsState->StrokeColorSpace.GetBrushDescriptor(cmd->Operands,
				//ccy               graphicsState->CurrentTransformationMatrix,
				//ccy               graphicsState->StrokeAlpha.Current);
				cmds->push_back(std::make_pair("RG", stack));
				break;
			}
			case OpCodeName::G:// set /DeviceGray and stroked color
			{
				//ccy       graphicsState->StrokeColorSpace = colorSpaceManager.GetColorSpace(PdfKeys.DeviceGray);
				//ccy       graphicsState->StrokeBrush =
				//ccy           graphicsState->StrokeColorSpace.GetBrushDescriptor(cmd->Operands,
				//ccy               graphicsState->CurrentTransformationMatrix,
				//ccy               graphicsState->StrokeAlpha.Current);
				cmds->push_back(std::make_pair("G", stack));
				break;
			}
			case OpCodeName::K:// set /DeviceCMYK and stroked color
			{
				auto c = PdfUtilities::GetDouble(cmd->Operands[0]);
				auto m = PdfUtilities::GetDouble(cmd->Operands[1]);
				auto y = PdfUtilities::GetDouble(cmd->Operands[2]);
				auto k = PdfUtilities::GetDouble(cmd->Operands[3]);
				stack.push(c);
				stack.push(m);
				stack.push(y);
				stack.push(k);
				cmds->push_back(std::make_pair("K", stack));
				break;
			}
			case OpCodeName::SC:// set stroked color
			{
				//ccy       graphicsState->StrokeBrush =
				//ccy           graphicsState->StrokeColorSpace.GetBrushDescriptor(cmd->Operands,
				//ccy               graphicsState->CurrentTransformationMatrix,
				//ccy               graphicsState->StrokeAlpha.Current);
				cmds->push_back(std::make_pair("SC", stack));
				break;
			}
			case OpCodeName::sh:// shading
			{
				//ccy       auto graphicPath = new GraphicPath();
				//ccy       auto shadingDescriptor = shadingManager.GetShading(cmd->Operands);
				//ccy       graphicPath.Geometry = graphicsState->ClippingPath;
				//ccy       graphicPath.FillBrush = shadingDescriptor.GetBrush(graphicsState->CurrentTransformationMatrix,
				//ccy           graphicsState->ClippingPath.Bounds,
				//ccy           graphicsState->FillAlpha.Current,
				//ccy           graphicsState->SoftMask);
				//ccy       graphicPath.ColorPrecision = shadingDescriptor.ColorPrecision;
				//ccy       graphicGroup.Children.Add(graphicPath);
				cmds->push_back(std::make_pair("sh", stack));
				break;
			}
			case OpCodeName::BT:// begin text
			{
				cmds->push_back(std::make_pair("BT", stack));
				break;
			}
			case OpCodeName::Tf:// set current font
			{
				auto font = ((CName^)cmd->Operands[0])->ToString();
				stack.push(SystemToStd(font));
				auto size = PdfUtilities::GetDouble(cmd->Operands[1]);
				stack.push(size);
				cmds->push_back(std::make_pair("Tf", stack));
				break;
			}
			case OpCodeName::Tr:// set rendering mode
			{
				auto mode = (int)PdfUtilities::GetDouble(cmd->Operands[0]);
				stack.push(mode);
				cmds->push_back(std::make_pair("Tr", stack));
				break;
			}
			case OpCodeName::Tm:// set font transformation matrix
			{
				auto m11 = PdfUtilities::GetDouble(cmd->Operands[0]);
				auto m12 = PdfUtilities::GetDouble(cmd->Operands[1]);
				auto m21 = PdfUtilities::GetDouble(cmd->Operands[2]);
				auto m22 = PdfUtilities::GetDouble(cmd->Operands[3]);
				auto cx = PdfUtilities::GetDouble(cmd->Operands[4]);
				auto cy = PdfUtilities::GetDouble(cmd->Operands[5]);
				stack.push(m11);
				stack.push(m12);
				stack.push(m21);
				stack.push(m22);
				stack.push(cx);
				stack.push(cy);
				cmds->push_back(std::make_pair("Tm", stack));
				break;
			}
			case OpCodeName::Td:// translate line matrix
			{
				auto x = PdfUtilities::GetDouble(cmd->Operands[0]);
				auto y = PdfUtilities::GetDouble(cmd->Operands[1]);
				stack.push(x);
				stack.push(y);
				cmds->push_back(std::make_pair("Td", stack));
				break;
			}
			case OpCodeName::TD:
			{
				auto x = PdfUtilities::GetDouble(cmd->Operands[0]);
				auto y = PdfUtilities::GetDouble(cmd->Operands[1]);
				stack.push(x);
				stack.push(y);
				cmds->push_back(std::make_pair("TD", stack));
				break;
			}
			case OpCodeName::TL:
			{
				auto leading = PdfUtilities::GetDouble(cmd->Operands[0]);
				stack.push(leading);
				cmds->push_back(std::make_pair("TL", stack));
				break;
			}
			case OpCodeName::Tx:
			{
				cmds->push_back(std::make_pair("Tx", stack));
				break;
			}
			case OpCodeName::QuoteSingle:
			{
				break;
			}
			case OpCodeName::QuoteDbl:
			{
				break;
			}
			case OpCodeName::Tj:// a single string
			{
				auto text = ((CString^)cmd->Operands[0])->Value;
				stack.push(SystemToStd(text));
				cmds->push_back(std::make_pair("Tj", stack));
				break;
			}
			case OpCodeName::TJ:// multiple strings plus formatting
			{
				auto array = (CArray^)cmd->Operands[0];
				for(int index = array->Count-1; index>=0; index--)
				{
					//	switch(array[index])
					//	auto value = PdfUtilities::GetDouble(array[index]);
					//	stack.push(value);
				}
				stack.push(array->Count);
				cmds->push_back(std::make_pair("TJ", stack));
				break;
			}
			case OpCodeName::q:// graphics state operators push state onto the stack
			{
				cmds->push_back(std::make_pair("q", stack));
				break;
			}
			case OpCodeName::Q:// pop state from the stack
			{
				cmds->push_back(std::make_pair("Q", stack));
				break;
			}
			case OpCodeName::cm:// current transform matrix
			{
				auto m11 = PdfUtilities::GetDouble(cmd->Operands[0]);
				auto m12 = PdfUtilities::GetDouble(cmd->Operands[1]);
				auto m21 = PdfUtilities::GetDouble(cmd->Operands[2]);
				auto m22 = PdfUtilities::GetDouble(cmd->Operands[3]);
				auto dx = PdfUtilities::GetDouble(cmd->Operands[4]);
				auto dy = PdfUtilities::GetDouble(cmd->Operands[5]);
				stack.push(m11);
				stack.push(m12);
				stack.push(m21);
				stack.push(m22);
				stack.push(dx);
				stack.push(dy);
				cmds->push_back(std::make_pair("cm", stack));
				break;
			}
			case OpCodeName::J:
			{
				auto cap = ((CInteger^)cmd->Operands[0])->Value;
				stack.push(cap);
				cmds->push_back(std::make_pair("J", stack));
				break;
			}
			case OpCodeName::j:
			{
				auto join = ((CInteger^)cmd->Operands[0])->Value;
				stack.push(join);
				cmds->push_back(std::make_pair("j", stack));
				break;
			}
			case OpCodeName::M:
			{
				auto miter = PdfUtilities::GetDouble(cmd->Operands[0]);
				stack.push(miter);
				cmds->push_back(std::make_pair("M", stack));
				break;
			}
			case OpCodeName::d:
			{
				auto dashArray = (CArray^)cmd->Operands[0];
				for(int index = 0; index<dashArray->Count; index++)
				{
					auto value = PdfUtilities::GetDouble(dashArray[index]);
					stack.push(value);
				}
				stack.push(dashArray->Count);
				auto phase = PdfUtilities::GetDouble(cmd->Operands[1]);
				stack.push(phase);
				cmds->push_back(std::make_pair("d", stack));
				break;
			}
			case OpCodeName::w:// line width
			{
				auto width = PdfUtilities::GetDouble(cmd->Operands[0]);
				stack.push(width);
				cmds->push_back(std::make_pair("w", stack));
				break;
			}
			case OpCodeName::gs:// set parameters in the current graphic state of the given state name
			{
				auto name = ((CName^)cmd->Operands[0])->Name;
				auto exts = resources->ExtGStates;
				if(exts!=nullptr)
				{
					PdfDictionary^ dictionay = exts->Elements->GetDictionary(name);
					if(dictionay!=nullptr)
					{
						for each(String^key in  dictionay->Elements->Keys)
						{
							if(key=="/LW")
							{
								auto width = dictionay->Elements->GetReal(key);
								std::stack<std::any> stack1;
								stack1.push(width);
								stack1.push("LW");
								cmds->push_back(std::make_pair("gs", stack1));
							}
							else if(key=="/ca")
							{
								auto alpha = dictionay->Elements->GetReal(key);
								std::stack<std::any> stack2;
								stack2.push(alpha);
								stack2.push("ca");
								cmds->push_back(std::make_pair("gs", stack));

							}
							else if(key=="/CA")
							{
								auto alpha = dictionay->Elements->GetReal(key);
								std::stack<std::any> stack3;
								stack3.push(alpha);
								stack3.push("CA");
								cmds->push_back(std::make_pair("gs", stack3));
							}
							else if(key=="/Font")
							{
								auto array = dictionay->Elements->GetArray(key);
								auto fontdict = array->Elements->GetDictionary(0);

								//	font.FontDescriptor = FontManager.ReadFontDescriptor(fontdict);
								//	font.FontSize = fontArray.Elements.GetReal(1);
							}
							else if(key=="/SMask")
							{
								auto smaskDict = dictionay->Elements->GetDictionary(key);
								if(smaskDict!=nullptr)
								{
									//	softMaskState.SoftMask = GetSoftMask(smaskDict);
								}
							}
						}
					}
				}
				break;
			}
			case OpCodeName::Do:
			{
				//ccy       auto xObjectName = ((CName)cmd->Operands[0]).Name;
				//ccy       RunXObject(xObjectName);
				cmds->push_back(std::make_pair("Do", stack));
				break;
			}
			default:
				break;
		}
	}
	return cmds;
};
std::list<std::pair<const char*, std::stack<std::any>>>* ParseFile(std::string ext, std::wstring file)
{
	if(ext=="pdf"||ext=="ai")
	{
		String^ result = msclr::interop::marshal_as<String^>(file);
		PdfSharp::Pdf::PdfDocument^ document = PdfReader::Open(result);

		for(int i = 0; i<document->Pages->Count; i++)
		{
			PdfSharp::Pdf::PdfPage^ page = document->Pages[i];
			Objects::CSequence^ sequence = ContentReader::ReadContent(page);
			return Interpreter(page, sequence);
		}
	}
	return nullptr;
};