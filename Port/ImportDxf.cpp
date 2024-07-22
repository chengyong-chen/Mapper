#include "stdafx.h"

#include "ImportDxf.h"

#include <cstdio>
#include "../Geometry/Geom.h"
#include "../Geometry/Poly.h"
#include "../Geometry/Mark.h"
#include "../Geometry/Text.h"
#include "../Layer/Layer.h"
#include "../Layer/LayerTree.h"
#include "../Dataview/DataInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

CImportDxf::CImportDxf(CDatainfo& datainfo, CLayerTree& layertree, DWORD& dwMaxGeomId)
	: CImport(datainfo, layertree, dwMaxGeomId)
{
}

BOOL CImportDxf::Import(LPCTSTR lpszPath)
{
	FILE* file=_tfopen(lpszPath, _T("rt"));
	if(file != nullptr)
	{
		ReadLayerSection(file, m_layertree);
		ReadData(file, m_layertree);

		fclose(file);
	}

	return TRUE;
}

// ʵ�����
bool CImportDxf::ReadLayerSection(FILE* file, CLayerTree& layertree) const
{
	DWORD dwColorIndex; // ��ɫ����
	BYTE wLayerType=0; // ������

	ASSERT(file);
	if(file == nullptr)
		return FALSE;

	BOOL bHasLayerSection=FALSE;

	fseek(file, 0L, SEEK_SET);
	long status=0;
	TCHAR string[1024], string1[1024];
	do
	{
		_ftscanf(file, _T("%s\n"), string);
		if(!_tcscmp(string, _T("LAYER")))
		{
			bHasLayerSection=TRUE;
			break;
		}
		else if(!_tcscmp(string, _T("ENTITIES")) || !_tcscmp(string, _T("BLOCKS")))
		{
			break;
		}
	} while(1);

	if(bHasLayerSection == FALSE)
	{
		CLayer* layer=layertree.GetByName(_T("Layer1"));
		if(layer == nullptr)
		{
			layer=new CLayer(layertree);
			layer->m_wId=layertree.ApplyId();
			layer->m_strName=_T("Layer1");
			layertree.m_root.AddChild(nullptr, layer);
		}
		return TRUE;
	}

	CLayer* layer=nullptr;
	do
	{
		_ftscanf(file, _T("%s\n"), string); // ����
		_ftscanf(file, _T("%s\n"), string1); // ��ֵ
		if(_tcscmp(string1, _T("LAYER")) == 0)
		{
			//		layer = new CLayer(m_datainfo.m_levelMinimum,m_datainfo.m_levelMaximum);	
			//		WORD Id = layertree.ApplyId();
			//		layer->m_wId = Id;
			//		layertree.AddTail(layer);			
		}
		else if(_tcscmp(string, _T("2")) == 0) // Layer Name
		{
			CLayer* layer=layertree.GetByName(string1);
			if(layer == nullptr)
			{
				layer=new CLayer(layertree);
				layer->m_wId=layertree.ApplyId();
				layer->m_strName=string1;
				layertree.m_root.AddChild(nullptr, layer);
			}
		}
		else if(_tcscmp(string, _T("62")) == 0) // Layer Color
		{
			dwColorIndex=(DWORD)_ttoi(string1);
		}
		else if(_tcscmp(string1, _T("ENDTAB")) == 0)
		{
			break;
		}
	} while(!feof(file));

	return TRUE;
}

bool CImportDxf::ReadData(FILE* file, CLayerTree& layertree)
{
	ASSERT(file);
	if(file == nullptr)
		return false;

	fseek(file, 0L, SEEK_SET);

	TCHAR string[1024];
	{
		_ftscanf(file, _T("%s\n"), string);
	}
	while(!feof(file) && _tcscmp(string, _T("$ENTITIES")) != 0);

	do
	{
		_ftscanf(file, _T("%s\n"), string);

		if(_tcscmp(string, _T("POINT")) == 0)
		{
			if(ReadMark(file, layertree) == false)
				return false;
		}
		else if(_tcscmp(string, _T("POLYLINE")) == 0)
		{
			if(ReadPoly(file, layertree) == false)
				return false;
		}
		else if(_tcscmp(string, _T("TEXT")) == 0)
		{
			if(ReadText(file, layertree, m_datainfo.m_zoomPointToDoc) == false)
				return false;
		}
		else if(_tcscmp(string, _T("LINE")) == 0)
		{
			if(ReadLine(file, layertree) == false)
				return false;
		}
		else if(_tcscmp(string, _T("EOF")) == 0)
			break;
	} while(!feof(file));

	return true;
}

BOOL CImportDxf::ReadMark(FILE* file, CLayerTree& layertree)
{
	ASSERT(file);
	if(file == nullptr)
		return FALSE;

	double x, y, z;
	CPoint point;
	TCHAR string[255];
	TCHAR szLayerName[255];

	CMark* mark=new CMark;
	mark->m_geoId=++m_dwMaxGeomId;

	CLayer* layer=nullptr;
	do
	{
		_ftscanf(file, _T("%s\n"), string);
		if(!_tcscmp(string, _T("8")))
		{
			_ftscanf(file, _T("%s\n"), szLayerName);
			layer=layertree.GetByName(szLayerName);
			if(layer == nullptr)
			{
				delete mark;
				break;
			}

			mark->CalCorner(layer->GetSpot(), m_datainfo, layertree.m_library);
			layer->m_geomlist.AddTail(mark);
		}
		else if(!_tcscmp(string, _T("10")))
		{
			_ftscanf(file, _T("%lf\n"), &x);
		}
		else if(!_tcscmp(string, _T("20")))
		{
			_ftscanf(file, _T("%lf\n"), &y);

			point=Change(x, y);
			mark->m_Origin=point;

			mark->CalCorner(layer->GetSpot(), m_datainfo, layertree.m_library);
		}
		else if(!_tcscmp(string, _T("30")))
		{
			_ftscanf(file, _T("%lf\n"), &z);
		}
		else if(!_tcscmp(string, _T("0")))
			break;
		else
			_ftscanf(file, _T("%s\n"), string);
	} while(!feof(file));

	return TRUE;
}

BOOL CImportDxf::ReadLine(FILE* file, CLayerTree& layertree)
{
	ASSERT(file);
	if(file == nullptr)
		return FALSE;

	double x, y, z;
	CPoint point;

	TCHAR string[255];
	TCHAR szLayerName[255];

	CPoly* poly=new CPoly;
	poly->m_geoId=++m_dwMaxGeomId;

	CLayer* layer=nullptr;
	do
	{
		_ftscanf(file, _T("%s\n"), string);
		if(!_tcscmp(string, _T("8")))
		{
			_ftscanf(file, _T("%s\n"), szLayerName);
			layer=layertree.GetByName(szLayerName);
			if(layer == nullptr)
			{
				delete poly;
				break;
			}
			layer->m_geomlist.AddTail(poly);
		}
		else if(!_tcscmp(string, _T("10")))
		{
			_ftscanf(file, _T("%lf\n"), &x);
		}
		else if(!_tcscmp(string, _T("20")))
		{
			_ftscanf(file, _T("%lf\n"), &y);

			point=Change(x, y);
			poly->AddAnchor(point, true, true);
		}
		else if(!_tcscmp(string, _T("30")))
		{
			_ftscanf(file, _T("%lf\n"), &z);
		}
		else if(!_tcscmp(string, _T("11")))
		{
			_ftscanf(file, _T("%lf\n"), &x);
		}
		else if(!_tcscmp(string, _T("21")))
		{
			_ftscanf(file, _T("%lf\n"), &y);

			point=Change(x, y);
			poly->AddAnchor(point, true, true);
		}
		else if(!_tcscmp(string, _T("31")))
		{
			_ftscanf(file, _T("%lf\n"), &z);
		}
		else if(!_tcscmp(string, _T("0")))
			break;
		else
			_ftscanf(file, _T("%s\n"), string);
	} while(!feof(file));

	return TRUE;
}

BOOL CImportDxf::ReadVertex(FILE* file, CPoint& point) const
{
	ASSERT(file);
	if(file == nullptr)
		return FALSE;

	double x, y, z;
	TCHAR string[255];
	TCHAR szLayerName[255];
	do
	{
		_ftscanf(file, _T("%s\n"), string);
		if(_tcscmp(string, _T("8")) == 0)
			_ftscanf(file, _T("%s\n"), szLayerName);
		else if(_tcscmp(string, _T("10")) == 0)
		{
			_ftscanf(file, _T("%lf\n"), &x);
		}
		else if(_tcscmp(string, _T("20")) == 0)
		{
			_ftscanf(file, _T("%lf\n"), &y);

			point=Change(x, y);
		}
		else if(_tcscmp(string, _T("30")) == 0)
		{
			_ftscanf(file, _T("%lf\n"), &z);
		}
		else if(_tcscmp(string, _T("0")) == 0)
			break;
		else
			_ftscanf(file, _T("%s\n"), string);
	} while(!feof(file));

	return TRUE;
}

BOOL CImportDxf::ReadPoly(FILE* file, CLayerTree& layertree)
{
	ASSERT(file);
	if(file == nullptr)
		return FALSE;

	CPoint* pPoints=nullptr;
	WORD wPoint=0;
	BYTE bLineType=0;

	TCHAR string[255];
	TCHAR szLayerName[255];

	CPoly* poly=new CPoly;
	poly->m_geoId=++m_dwMaxGeomId;

	CLayer* layer=nullptr;
	do
	{
		_ftscanf(file, _T("%s\n"), string);
		if(_tcscmp(string, _T("8")) == 0)
		{
			_ftscanf(file, _T("%s\n"), szLayerName);
			layer=layertree.GetByName(szLayerName);
			if(layer == nullptr)
			{
				delete poly;
				break;
			}
			layer->m_geomlist.AddTail(poly);
		}
		else if(_tcscmp(string, _T("6")) == 0)
		{
			_ftscanf(file, _T("%s\n"), string);
			if(_tcscmp(string, _T("CONTINUOUS")) == 0)
			{
				bLineType=0;
			}
			else if(_tcscmp(string, _T("BYBLOCK")) == 0)
			{
				bLineType=1;
			}
			else if(_tcscmp(string, _T("BYLAYER")) == 0)
			{
				bLineType=2;
			}
		}
		else if(_tcscmp(string, _T("70")) == 0)
		{
			_ftscanf(file, _T("%s\n"), string);
			if(_tcscmp(string, _T("1")) == 0)
			{
				poly->m_bClosed=true;
			}
		}
		else if(_tcscmp(string, _T("0")) == 0)
		{
			do
			{
				_ftscanf(file, _T("%s\n"), string);
				if(_tcscmp(string, _T("VERTEX")) == 0)
				{
					CPoint point;
					if(ReadVertex(file, point))
						poly->AddAnchor(point, true, true);
					else
						return FALSE;
				}
				else if(_tcscmp(string, _T("SEQEND")) == 0)
				{
					if(poly != nullptr && poly->GetAnchors() > 2 && poly->m_bClosed == true)
					{
						const CPoint point=poly->m_pPoints[0];
						poly->AddAnchor(point, true, true);
					}

					return TRUE;
				}
			} while(!feof(file));
		}
		else if(_tcscmp(string, _T("SEQEND")) == 0)
			break;
		else
			_ftscanf(file, _T("%s\n"), string);
	} while(!feof(file));

	return TRUE;
}

BOOL CImportDxf::ReadText(FILE* file, CLayerTree& layertree, float fPointToDoc)
{
	ASSERT(file);
	if(file == nullptr)
		return FALSE;

	double x, y, z;
	CPoint point[2];

	TCHAR string[255];
	TCHAR szLayerName[255];
	BOOL bIsTwoPos=FALSE;

	CText* text=new CText;
	text->m_geoId=++m_dwMaxGeomId;

	CLayer* layer=nullptr;
	do
	{
		_ftscanf(file, _T("%s\n"), string);
		if(!_tcscmp(string, _T("8")))
		{
			_ftscanf(file, _T("%s\n"), szLayerName);
			layer=layertree.GetByName(szLayerName);
			if(layer == nullptr)
			{
				delete text;
				break;
			}
			text->CalCorner(layer->GetType(), fPointToDoc, 1.f);
			layer->m_geomlist.AddTail(text);
		}
		else if(!_tcscmp(string, _T("1")))
		{
			char CC[1024];
			fgets(CC, 1024, file);
			fgets(CC, 1024, file);
			text->m_strName=CC;
			text->m_strName.Replace(_T("\r\n"), nullptr);
		}
		else if(!_tcscmp(string, _T("10")))
		{
			_ftscanf(file, _T("%lf\n"), &x);
		}
		else if(!_tcscmp(string, _T("20")))
		{
			_ftscanf(file, _T("%lf\n"), &y);

			point[0]=Change(x, y);
		}
		else if(!_tcscmp(string, _T("30")))
		{
			_ftscanf(file, _T("%lf\n"), &z);
		}
		else if(!_tcscmp(string, _T("11")))
		{
			bIsTwoPos=TRUE;
			_ftscanf(file, _T("%lf\n"), &x);
		}
		else if(!_tcscmp(string, _T("21")))
		{
			_ftscanf(file, _T("%lf\n"), &y);

			point[1]=Change(x, y);
		}
		else if(!_tcscmp(string, _T("31")))
		{
			_ftscanf(file, _T("%lf\n"), &z);
		}
		else if(!_tcscmp(string, _T("0")))
			break;
		else
			_ftscanf(file, _T("%s\n"), string);
	} while(!feof(file));

	if(layer != nullptr && text != nullptr)
	{
		text->m_Origin=point[0];

		text->CalCorner(layer->GetType(), fPointToDoc, 1.f);
	}

	return TRUE;
}

//	TABLES section
//	���
//		0	
//	TABLE
//		2
//	LAYER
//		70
//			9
//		0			||Start Identifier
//	LAYER			||Table Name
//		2			||LayNameStart
//	0				||LayName number or ASCII
//		70			||BitCodeStart
//			0		||BitCode
//		62			||ColorStart
//			7		||Color Identifier
//		6			||LineTypeStart
//	CONTINUOUS	||LineType Identifier
//		0
//	LAYER
//		2
//	1
//		70
//			0
//		62
//			1
//		6
//	CONTINUOUS
//		0
//	ENDTAB

// �����
//		0
//	TABLE
//		2
//	STYLE
//		70
//			1
//		0
//	STYLE
//		2
//	STANDARD
//		70
//			0
//		40
//	0.0
//		41
//	1.0
//		50
//	0.0
//		71
//			0
//		42
//	0.2
//		3
//	TXT
//		4
//
//		0
//	ENDTAB

//ENTITIES section
//	��ʵ��	||3D
//		0		||Start Identifier
//	POINT		||Entity Name
//		8		||LayStart Identifier
//	0			||LayName or Number in ASCII,e.g., (0) or (LAY1)
//		10		||XStart Identifier
//	6.405815	||X coordinate
//		20		||YStart Identifier
//	3.884078	||Y coordinate
//		30		||ZStart Identifier
//	0.0		||Z coordinate

//	��ʵ��	3D
//		0
//	LINE
//		8
//	0
//		10
//	4.064986
//		20
//	4.085196
//		30
//	0.0
//		11
//	5.927581
//		21
//	6.624302
//		31
//	0.0

//		0
//	POLYLINE
//		8
//	0
//		6			||Line Type name(e.g., CONTINUOUS),or BYLAYER or BYBLOCK (0)
//	CONTINUOUS
//		66			||Always followed by 1, indicating that a VERTEX list fllows the POLYLINE entity
//	1
//		0
//	VERTEX
//		8
//	0
//		10
//	79.279
//		20
//	0.152
//		30
//	0.0
//		0
//	VERTEX
//		8
//	0
//		10
//	79.279
//		20
//	0.533
//		30
//	0.0
//		0
//	SEQEND
