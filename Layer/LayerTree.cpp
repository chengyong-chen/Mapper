#include "stdafx.h"
#include <stdio.h>
#include <array>
#include "PickLayDlg.h"

#include "Layer.h"
#include "Laylg.h"
#include "LayerTree.h"
#include "GridMapFile.h"
#include "GeomMapFile.h"

#include "../Style/Line.h"
#include "../Style/Type.h"
#include "../Style/SpotFont.h"

#include "../Geometry/Geom.h"
#include "../Geometry/Mark.h"

#include "../Public/ODBCDatabase.h"
#include "../Database/Datasource.h"

#include <boost/json/array.hpp>
#include <boost/json.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CLayerTree::CLayerTree()
{
	m_root.m_strName = _T("Root");
	if(m_root.m_pSpot==nullptr)
	{
		m_root.m_pSpot = new CSpotFont();
	}
	if(m_root.m_pLine==nullptr)
	{
		m_root.m_pLine = new CLine();
	}
	if(m_root.m_pType==nullptr)
	{
		m_root.m_pType = new CType();
	}
	if(m_root.m_pHint==nullptr)
	{
		m_root.m_pHint = new CHint();
	}
}

CLayerTree::~CLayerTree()
{
	for(std::map<CString, CODBCDatabase*>::iterator it = m_AttDBList.begin(); it!=m_AttDBList.end(); it++)
	{
		CODBCDatabase* pDatabase = it->second;
		if(pDatabase!=nullptr)
		{
			pDatabase->Close();
			delete pDatabase;
			pDatabase = nullptr;
		}
	}
	m_AttDBList.clear();

	for(std::map<CString, CODBCDatabase*>::iterator it = m_GeoDBList.begin(); it!=m_GeoDBList.end(); it++)
	{
		CODBCDatabase* pDatabase = it->second;
		if(pDatabase!=nullptr)
		{
			pDatabase->Close();
			delete pDatabase;
			pDatabase = nullptr;
		}
	}
	m_AttDBList.clear();
}
void CLayerTree::Wipeout()
{
	for(CTree<CLayer>::forwiterator it = this->forwbegin(); it!=this->forwend(); ++it)
	{
		CLayer* layer = *it;
		layer->Wipeout();
	}
	
	for(std::map<CString, CODBCDatabase*>::iterator it = m_AttDBList.begin(); it!=m_AttDBList.end(); it++)
	{
		CODBCDatabase* pDatabase = it->second;
		if(pDatabase!=nullptr)
		{
			pDatabase->Close();
			delete pDatabase;
			pDatabase = nullptr;
		}
	}
	m_AttDBList.clear();

	for(std::map<CString, CODBCDatabase*>::iterator it = m_GeoDBList.begin(); it!=m_GeoDBList.end(); it++)
	{
		CODBCDatabase* pDatabase = it->second;
		if(pDatabase!=nullptr)
		{
			pDatabase->Close();
			delete pDatabase;
			pDatabase = nullptr;
		}
	}
	m_AttDBList.clear();
}
void CLayerTree::Normalize()
{
	for(CTree<CLayer>::forwiterator it = this->forwbegin(); it!=this->forwend(); ++it)
	{
		CLayer* layer = *it;
		layer->Normalize();
	}
}
CODBCDatabase* CLayerTree::GetAttDatabase(CString strName)  const
{
	if(m_AttDBList.find(strName)!=m_AttDBList.end())
		return m_AttDBList.at(strName);
	else
		return nullptr;
}
CODBCDatabase* CLayerTree::GetGeoDatabase(CString strName) const
{
	if(m_GeoDBList.find(strName)!=m_GeoDBList.end())
		return m_GeoDBList.at(strName);
	else
		return nullptr;
}
void CLayerTree::Rematch()
{
	for(CTree<CLayer>::forwiterator it = this->forwbegin(); it!=this->forwend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->Gettype() == 9)
		{
			CLaylg* pLaylg = ((CLaylg*)layer);
			CATTDatasource* pDatasource = pLaylg->GetAttDatasource();
			if(pDatasource == nullptr)
			{
				continue;
			}
			CODBCDatabase* pDatabase = this->GetAttDatabase(pDatasource->m_strDatabase);
			if(pDatabase == nullptr)
				continue;

			CATTHeaderProfile& headerprofile = (CATTHeaderProfile&)(pDatasource->GetHeaderProfile());
			((CLaylg*)layer)->Rematch(pDatabase, headerprofile);
		}
	}
}
void CLayerTree::Initialize()
{
	m_library.Initialize();

	CLayer* layer1 = new CLayer(*this);
	layer1->m_strName.LoadString(IDS_LAYER_NAME);
	layer1->m_strName += _T("1");
	layer1->m_wId = ApplyId();
	m_root.AddChild(nullptr, layer1);

	CLayer* layer2 = new CLayer(*this);
	layer2->m_strName.LoadString(IDS_LAYER_NAME);
	layer2->m_strName += _T("2");
	layer2->m_wId = ApplyId();
	m_root.AddChild(layer1, layer2);
}

WORD CLayerTree::ApplyId() const
{
	WORD wId = 0;
	for(CTree<CLayer>::forwiterator it = this->forwbegin(); it!=this->forwend(); ++it)
	{
		CLayer* layer = *it;
		wId = max(layer->m_wId, wId);
	}
	return wId+1;
}
bool CLayerTree::Replace(WORD wId, CLayer* pLayer)
{
	CLayer* original = CLayerTree::GetLayerById(wId);
	if(original == nullptr)
		return false;
	else if(original == &m_root)
		return false;
	else
	{
		original->ReplaceBy(pLayer);
		delete original;
		return true;
	}
}

CGeom* CLayerTree::GetGeomById(DWORD dwGeomId) const
{
	if(dwGeomId==0)
		return nullptr;

	for(CTree<CLayer>::forwiterator it = this->forwbegin(); it!=this->forwend(); ++it)
	{
		CLayer* layer = *it;
		CTypedPtrList<CObList, CGeom*>& geomlist = layer->GetGeomList();
		POSITION pos2 = geomlist.GetHeadPosition();
		while(pos2!=nullptr)
		{
			CGeom* geom = geomlist.GetNext(pos2);
			if(geom->m_geoId==dwGeomId)
			{
				return geom;
			}
		}
	}

	return nullptr;
}

CGeom* CLayerTree::GetGeomById(WORD wLayerId, DWORD dwGeomId) const
{
	if(dwGeomId==0)
		return nullptr;

	for(CTree<CLayer>::forwiterator it = this->forwbegin(); it!=this->forwend(); ++it)
	{
		CLayer* layer = *it;
		if(wLayerId!=0XFFFF&&wLayerId!=layer->m_wId)
			continue;

		CTypedPtrList<CObList, CGeom*>& geomlist = layer->GetGeomList();
		POSITION pos2 = geomlist.GetHeadPosition();
		while(pos2!=nullptr)
		{
			CGeom* geom = geomlist.GetNext(pos2);
			if(geom->m_geoId==dwGeomId)
			{
				return geom;
			}
		}
	}

	return nullptr;
}

CLayer* CLayerTree::GetLayerById(WORD wId) const
{
	if(wId==0)
		return nullptr;

	for(CTree<CLayer>::forwiterator it = this->forwbegin(); it!=this->forwend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_wId==wId)
			return layer;
	}
	return nullptr;
}

CLayer* CLayerTree::GetLayerByGeom(CGeom* pGeom) const
{
	if(pGeom==nullptr)
		return nullptr;

	for(CTree<CLayer>::forwiterator it = this->forwbegin(); it!=this->forwend(); ++it)
	{
		CLayer* layer = *it;
		CTypedPtrList<CObList, CGeom*>& geomlist = layer->GetGeomList();
		POSITION pos2 = geomlist.GetHeadPosition();
		while(pos2!=nullptr)
		{
			CGeom* geom = geomlist.GetNext(pos2);
			if(geom==pGeom)
			{
				return layer;
			}
		}
	}

	return nullptr;
}

CLayer* CLayerTree::GetByName(CString strName) const
{
	if(strName.IsEmpty()==TRUE)
		return nullptr;

	for(CTree<CLayer>::forwiterator it = this->forwbegin(); it!=this->forwend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_strName==strName)
			return layer;
	}

	return nullptr;
}

void CLayerTree::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	m_library.Serialize(ar, dwVersion);
	m_root.Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		ar<<m_AttDBList.size();
		ar<<m_GeoDBList.size();
		for(std::map<CString, CODBCDatabase*>::iterator it = m_AttDBList.begin(); it!=m_AttDBList.end(); it++)
		{
			ar<<it->first;
			it->second->Serialize(ar, dwVersion);
		}
		for(std::map<CString, CODBCDatabase*>::iterator it = m_GeoDBList.begin(); it!=m_GeoDBList.end(); it++)
		{
			ar<<it->first;
			it->second->Serialize(ar, dwVersion);
		}
	}
	else
	{
		int count1;
		int count2;
		ar>>count1;
		ar>>count2;
		for(int index1 = 0; index1<count1; index1++)
		{
			CString strName;
			ar>>strName;
			CODBCDatabase* pDatabase = new CODBCDatabase();
			pDatabase->Serialize(ar, dwVersion);
			if(m_AttDBList.find(strName)==m_AttDBList.end())
			{
				CString message;
				if(pDatabase->Open(CDatabase::noOdbcDialog) == TRUE)
				{
					m_AttDBList[strName] = pDatabase;
					message.Format(_T("Database opened: %s\n"), strName);
				}
				else
				{	
					message.Format(_T("Failed to open database: %s\n"), strName);					
				}
				OutputDebugString(message);
			}
			else
			{
				delete pDatabase;
				pDatabase = nullptr;
			}
		}
		for(int index2 = 0; index2<count2; index2++)
		{
			CString strName;
			ar>>strName;
			CODBCDatabase* pDatabase = new CODBCDatabase();
			pDatabase->Serialize(ar, dwVersion);
			if(m_GeoDBList.find(strName)==m_GeoDBList.end())
			{
				CString message;
				if(pDatabase->Open(CDatabase::noOdbcDialog) == TRUE)
				{
					m_GeoDBList[strName] = pDatabase;
					message.Format(_T("Database opened: %s\n"), strName);
				}
				else
				{					
					message.Format(_T("Failed to open database: %s\n"), strName);					
				}
				OutputDebugString(message);
			}
			else
			{
				delete pDatabase;
				pDatabase = nullptr;
			}
		}
		for(CTree<CLayer>::forwiterator it = this->forwbegin(); it!=this->forwend(); ++it)
		{
			CLayer* layer = *it;
			if(layer->Gettype() == 9)
			{
				CLaylg* pLaylg = ((CLaylg*)layer);
				CATTDatasource* pDatasource = pLaylg->GetAttDatasource();
				if(pDatasource == nullptr)
				{					
					continue;				
				}
				CODBCDatabase* pDatabase = this->GetAttDatabase(pDatasource->m_strDatabase);
				if(pDatabase == nullptr)
					continue;

				CATTHeaderProfile& headerprofile = (CATTHeaderProfile&)(pDatasource->GetHeaderProfile());
				((CLaylg*)layer)->Rematch(pDatabase, headerprofile);
			}
		}
	}
}

void CLayerTree::ReleaseCE(CArchive& ar) const
{
	m_library.ReleaseCE(ar);
	m_root.ReleaseCE(ar);
}

void CLayerTree::ReleaseDCOM(CArchive& ar)
{
}

void CLayerTree::ExportGrid(CString strGrid, CSize docGrid, CSize docOffset, CSize docCanvas)
{
	CFile file;
	if(file.Open(strGrid, CFile::modeCreate|CFile::modeWrite)==TRUE)
	{
		CArchive ar(&file, CArchive::store);
		CGridMapFile::HEAD head;
		//		head.bFlag = _T("Grd1000");
		head.wLayerCount = CTree<CLayer>::nodesize();
		head.dwHeight = docGrid.cy;
		head.dwWidth = docGrid.cx;
		head.xCount = docCanvas.cx%docGrid.cx==0 ? docCanvas.cx/docGrid.cx : docCanvas.cx/docGrid.cx+1;
		head.yCount = docCanvas.cy%docGrid.cy==0 ? docCanvas.cy/docGrid.cy : docCanvas.cy/docGrid.cy+1;

		ar.Write(&head, sizeof(CGridMapFile::HEAD));

		CGridMapFile::LAYER* pLAYER1 = new CGridMapFile::LAYER[head.wLayerCount];
		CGridMapFile::LAYER* pLAYER2 = pLAYER1;
		const DWORD dwLayerGridBegin = sizeof(CGridMapFile::HEAD)+head.wLayerCount*sizeof(CGridMapFile::LAYER);
		DWORD dwOffset = 0;
		for(CTree<CLayer>::postiterator it = this->postbegin(); it!=this->postend(); ++it)
		{
			CLayer* layer = *it;
			pLAYER2->wId = layer->m_wId;
			pLAYER2->dwGridOffset = dwLayerGridBegin+dwOffset;

			dwOffset += head.xCount*head.yCount*sizeof(CGridMapFile::GRID);

			pLAYER2++;
		}
		ar.Write(pLAYER1, head.wLayerCount*sizeof(CGridMapFile::LAYER));
		CGridMapFile::GRID* pGRID1 = new CGridMapFile::GRID[head.wLayerCount*head.xCount*head.yCount];
		CGridMapFile::GRID* pGRID2 = pGRID1;
		ar.Write(pGRID1, head.wLayerCount*head.xCount*head.yCount*sizeof(CGridMapFile::GRID));
		const DWORD dwGeomBegin = sizeof(CGridMapFile::HEAD)+head.wLayerCount*sizeof(CGridMapFile::LAYER)+head.wLayerCount*head.xCount*head.yCount*sizeof(CGridMapFile::GRID);
		dwOffset = 0;
		for(CTree<CLayer>::postiterator it = this->postbegin(); it!=this->postend(); ++it)
		{
			CLayer* layer = *it;
			for(short y = 0; y<head.yCount; y++)
				for(short x = 0; x<head.xCount; x++)
				{
					CRect rect(x*docGrid.cx, y*docGrid.cy, (x+1)*docGrid.cx, (y+1)*docGrid.cy);
					rect.OffsetRect(docOffset.cx, docOffset.cy);

					pGRID2->dwOffset = dwGeomBegin+dwOffset;
					pGRID2->wGeomCount = 0;
					DWORD dwGeomIndex = 0;
					POSITION Pos21 = layer->m_geomlist.GetTailPosition();
					POSITION Pos22 = Pos21;
					while((Pos22 = Pos21)!=nullptr)
					{
						CGeom* pGeom = layer->m_geomlist.GetPrev(Pos21);
						if(pGeom!=nullptr)
						{
							if(pGeom->m_bType==8)
							{
								const CPoint point = ((CMark*)pGeom)->m_Origin;
								if(rect.PtInRect(point)==TRUE)
								{
									ar<<dwGeomIndex;
									dwOffset += sizeof(DWORD);
									pGRID2->wGeomCount++;

									delete pGeom;
									pGeom = nullptr;
									layer->m_geomlist.SetAt(Pos22, nullptr);
								}
							}
							else if(pGeom->PickIn(rect)==true)
							{
								ar<<dwGeomIndex;
								dwOffset += sizeof(DWORD);
								pGRID2->wGeomCount++;

								CRect intersect = pGeom->m_Rect;
								if(intersect.IntersectRect(intersect, rect)==TRUE)
								{
									if(intersect==pGeom->m_Rect)
									{
										delete pGeom;
										pGeom = nullptr;
										layer->m_geomlist.SetAt(Pos22, nullptr);
									}
								}
							}
						}

						dwGeomIndex++;
					}

					pGRID2++;
				}
		}

		ar.Close();
		file.Close();
		if(file.Open(strGrid, CFile::modeReadWrite|CFile::typeBinary)==TRUE)
		{
			file.SeekToBegin();
			file.Seek(dwLayerGridBegin, CFile::begin);
			file.Write(pGRID1, head.wLayerCount*head.xCount*head.yCount*sizeof(CGridMapFile::GRID));
			file.Close();
		}
		else
		{
		}
		delete[] pLAYER1;
		delete[] pGRID1;
	}
}

void CLayerTree::ExportPCGeom(CString strGeom)
{
	CFile file;
	if(file.Open(strGeom, CFile::modeCreate|CFile::modeWrite)==TRUE)
	{
		const int nCount = CTree<CLayer>::nodesize();
		const DWORD dwHeadBegin = nCount*sizeof(CGeomMapFile::LAYER);

		CGeomMapFile::LAYER* pLAYER1 = new CGeomMapFile::LAYER[nCount];
		CGeomMapFile::LAYER* pLAYER2 = pLAYER1;

		DWORD dwOffset = 0;
		DWORD dwGeomCount = 0;

		for(CTree<CLayer>::postiterator it = this->postbegin(); it!=this->postend(); ++it)
		{
			CLayer* layer = *it;
			const int geomCount = layer->GetGeomCount();
			pLAYER2->wId = layer->m_wId;
			pLAYER2->dwGeomCount = geomCount;
			pLAYER2->dwGeomHeadOffset = dwHeadBegin+dwOffset;

			{
				DWORD dwCode = pLAYER2->wId;
				dwCode = (dwCode<<16)|(dwCode+42342);
				dwCode = dwCode^0X76864594;

				pLAYER2->dwGeomCount = pLAYER2->dwGeomCount^dwCode;
				pLAYER2->dwGeomHeadOffset = pLAYER2->dwGeomHeadOffset^dwCode;
			}

			dwOffset += geomCount*sizeof(CGeomMapFile::GEOMHEAD);
			dwGeomCount += geomCount;
			pLAYER2++;
		}
		file.Write(pLAYER1, nCount*sizeof(CGeomMapFile::LAYER));

		delete[] pLAYER1;
		pLAYER1 = nullptr;

		CGeomMapFile::GEOMHEAD* pGeomhead1 = new CGeomMapFile::GEOMHEAD[dwGeomCount];
		CGeomMapFile::GEOMHEAD* pGeomhead2 = pGeomhead1;

		for(CTree<CLayer>::postiterator it = this->postbegin(); it!=this->postend(); ++it)
		{
			CLayer* layer = *it;
			POSITION Pos21 = layer->m_geomlist.GetTailPosition();
			while(Pos21!=nullptr)
			{
				CGeom* pGeom = layer->m_geomlist.GetPrev(Pos21);
				pGeomhead2->bType = pGeom->Gettype();
				pGeomhead2->dwId = pGeom->m_geoId;
				pGeomhead2->rect = pGeom->m_Rect;

				file.Write(pGeomhead2, sizeof(CGeomMapFile::GEOMHEAD));
				pGeomhead2++;
			}
		}
		const DWORD dwGeomBegin = dwHeadBegin+dwGeomCount*sizeof(CGeomMapFile::GEOMHEAD);
		dwOffset = 0;
		pGeomhead2 = pGeomhead1;
		for(CTree<CLayer>::postiterator it = this->postbegin(); it!=this->postend(); ++it)
		{
			CLayer* layer = *it;
			POSITION Pos31 = layer->m_geomlist.GetTailPosition();
			while(Pos31!=nullptr)
			{
				CGeom* pGeom = layer->m_geomlist.GetPrev(Pos31);
				pGeomhead2->dwGeomOffset = dwGeomBegin+dwOffset;

				BYTE* bytes = nullptr;
				dwOffset += pGeom->PackPC(&file, bytes);
				pGeomhead2->bType = pGeom->Gettype();
				const DWORD dwCode = (pGeomhead2->dwId+1232*pGeomhead2->bType)^0X66546675;
				pGeomhead2->dwGeomOffset = pGeomhead2->dwGeomOffset^dwCode;

				pGeomhead2++;
			}
		}
		file.Close();

		if(file.Open(strGeom, CFile::modeReadWrite|CFile::typeBinary)==TRUE)
		{
			file.SeekToBegin();
			file.Seek(dwHeadBegin, CFile::begin);
			file.Write(pGeomhead1, dwGeomCount*sizeof(CGeomMapFile::GEOMHEAD));
			file.Close();
		}
		else
		{
		}
		delete[] pGeomhead1;
		pGeomhead1 = nullptr;
	}
}

void CLayerTree::ExportCEGeom(CString strGeom)
{
	CFile file;
	if(file.Open(strGeom, CFile::modeCreate|CFile::modeWrite)==TRUE)
	{
		const int nCount = CTree<CLayer>::nodesize();
		const DWORD dwHeadBegin = nCount*sizeof(CGeomMapFile::LAYER);

		CGeomMapFile::LAYER* pLAYER1 = new CGeomMapFile::LAYER[nCount];
		CGeomMapFile::LAYER* pLAYER2 = pLAYER1;

		DWORD dwOffset = 0;
		DWORD dwGeomCount = 0;

		for(CTree<CLayer>::postiterator it = this->postbegin(); it!=this->postend(); ++it)
		{
			CLayer* layer = *it;
			pLAYER2->wId = layer->m_wId;
			pLAYER2->dwGeomCount = layer->m_geomlist.GetCount();
			pLAYER2->dwGeomHeadOffset = dwHeadBegin+dwOffset;

			{
				DWORD dwCode = pLAYER2->wId;
				dwCode = (dwCode<<16)|(dwCode+42342);
				dwCode = dwCode^0X76864594;

				pLAYER2->dwGeomCount = pLAYER2->dwGeomCount^dwCode;
				pLAYER2->dwGeomHeadOffset = pLAYER2->dwGeomHeadOffset^dwCode;
			}

			dwOffset += layer->m_geomlist.GetCount()*sizeof(CGeomMapFile::GEOMHEAD);
			dwGeomCount += layer->m_geomlist.GetCount();

			pLAYER2++;
		}
		file.Write(pLAYER1, nCount*sizeof(CGeomMapFile::LAYER));

		delete[] pLAYER1;
		pLAYER1 = nullptr;

		CGeomMapFile::GEOMHEAD* pGeomhead1 = new CGeomMapFile::GEOMHEAD[dwGeomCount];
		CGeomMapFile::GEOMHEAD* pGeomhead2 = pGeomhead1;

		for(CTree<CLayer>::postiterator it = this->postbegin(); it!=this->postend(); ++it)
		{
			CLayer* layer = *it;
			POSITION pos2 = layer->m_geomlist.GetTailPosition();
			while(pos2!=nullptr)
			{
				CGeom* pGeom = layer->m_geomlist.GetPrev(pos2);
				pGeomhead2->bType = pGeom->Gettype();
				pGeomhead2->dwId = pGeom->m_geoId;

				pGeomhead2->rect.left = pGeom->m_Rect.left/10000;
				pGeomhead2->rect.top = pGeom->m_Rect.top/10000;
				pGeomhead2->rect.right = pGeom->m_Rect.Width()/10000;
				pGeomhead2->rect.bottom = pGeom->m_Rect.Height()/10000;

				file.Write(pGeomhead2, sizeof(CGeomMapFile::GEOMHEAD));
				pGeomhead2++;
			}
		}
		const DWORD dwGeomBegin = dwHeadBegin+dwGeomCount*sizeof(CGeomMapFile::GEOMHEAD);
		dwOffset = 0;
		pGeomhead2 = pGeomhead1;
		for(CTree<CLayer>::forwiterator it = this->forwbegin(); it!=this->forwend(); ++it)
		{
			CLayer* layer = *it;
			POSITION pos2 = layer->m_geomlist.GetTailPosition();
			while(pos2!=nullptr)
			{
				CGeom* pGeom = layer->m_geomlist.GetPrev(pos2);
				pGeomhead2->dwGeomOffset = dwGeomBegin+dwOffset;

				BYTE type;
				dwOffset += pGeom->ReleaseCE(file, type);
				pGeomhead2->bType = type;
				const DWORD dwCode = (pGeomhead2->dwId+1232*pGeomhead2->bType)^0X66546675;
				pGeomhead2->dwGeomOffset = pGeomhead2->dwGeomOffset^dwCode;

				pGeomhead2++;
			}
		}
		file.Close();

		if(file.Open(strGeom, CFile::modeReadWrite|CFile::typeBinary)==TRUE)
		{
			file.SeekToBegin();
			file.Seek(dwHeadBegin, CFile::begin);
			file.Write(pGeomhead1, dwGeomCount*sizeof(CGeomMapFile::GEOMHEAD));
			file.Close();
		}
		else
		{
		}
		delete[] pGeomhead1;
		pGeomhead1 = nullptr;
	}
}

void CLayerTree::ReleaseWvt(CFile& file, const CDatainfo& datainfo)
{
	m_library.ReleaseWeb(file);

	unsigned short nCount = 0;
	for(CTree<CLayer>::forwiterator it = this->forwbegin(); it!=this->forwend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_bVisible==TRUE)
			nCount++;
	}
	ReverseOrder(nCount);
	file.Write(&nCount, sizeof(unsigned short));

	for(CTree<CLayer>::postiterator it = this->postbegin(); it != this->postend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_bVisible == false)
			continue;

		layer->ReleaseCap(file, datainfo);
		if(layer->m_bVisible) 
		{
			int geomCount = 0;
			file.Write(&geomCount, sizeof(int));
		}
	}
}
void CLayerTree::ReleaseWvt(BinaryStream& stream, const CDatainfo& datainfo)
{
	m_library.ReleaseWeb(stream);

	unsigned short nCount = 0;
	for(CTree<CLayer>::forwiterator it = this->forwbegin(); it!=this->forwend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_bVisible==TRUE)
			nCount++;
	}
	stream<<nCount;

	for(CTree<CLayer>::postiterator it = this->postbegin(); it!=this->postend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_bVisible==false)
			continue;

		layer->ReleaseCap(stream, datainfo);
		if(layer->m_bVisible)
		{
			stream << (int)0;
		}
	}
}
void CLayerTree::ReleaseWvt(boost::json::object& json, const CDatainfo& datainfo, const BYTE& minMapLevel, const BYTE& maxMapLevel)
{
	m_library.ReleaseWeb(json);

	unsigned short nCount = 0;
	for(CTree<CLayer>::forwiterator it = this->forwbegin(); it != this->forwend(); ++it)
	{
		CLayer* layer = *it;		
		if(layer->m_bVisible == TRUE)
			nCount++;
	}
	
	boost::json::array layers;
	for(CTree<CLayer>::forwiterator it = this->forwbegin(); it != this->forwend(); ++it)
	{
		CLayer* layer = *it;		
		if(layer->m_bVisible == false)
			continue;

		boost::json::object child;
		layer->ReleaseCap(child, datainfo, minMapLevel, maxMapLevel);
		layers.push_back(child);
	}
	json["Layers"] = layers;
}
void CLayerTree::ReleaseWvt(pbf::writer& writer, pbf::tag tag, const CDatainfo& datainfo, const BYTE& minMapLevel, const BYTE& maxMapLevel)
{
	writer.add_tag(tag, pbf::type::bytes);

	m_library.ReleaseWeb(writer);

	std::string cdata;
	pbf::writer child(cdata);
	for(CTree<CLayer>::forwiterator it = this->forwbegin(); it != this->forwend(); ++it)
	{
		CLayer* layer = *it;
		if(CLayerTree::Releasable(layer) == false)
			continue;

		BYTE type = layer->Gettype();
		child.add_tag((pbf::tag)type, pbf::type::bytes);
		layer->ReleaseCap(child, datainfo, minMapLevel, maxMapLevel, true);		
	}

	writer.add_bytes(cdata.c_str(), cdata.length());
}
void CLayerTree::ReleaseWeb(CFile& file, const CDatainfo& datainfo)
{
	m_library.ReleaseWeb(file);

	unsigned short nCount = 0;
	for(CTree<CLayer>::forwiterator it = this->forwbegin(); it!=this->forwend(); ++it)
	{
		CLayer* layer = *it;
		if(CLayerTree::Releasable(layer)==true)
			nCount++;
	}
	ReverseOrder(nCount);
	file.Write(&nCount, sizeof(unsigned short));

	for(CTree<CLayer>::forwiterator it = this->forwbegin(); it!=this->forwend(); ++it)
	{
		CLayer* layer = *it;
		if(CLayerTree::Releasable(layer)==false)
			continue;
		layer->ReleaseWeb(file, datainfo);
	}
}
void CLayerTree::ReleaseWeb(BinaryStream& stream, const CDatainfo& datainfo)
{
	m_library.ReleaseWeb(stream);

	unsigned short nCount = 0;
	for(CTree<CLayer>::forwiterator it = this->forwbegin(); it!=this->forwend(); ++it)
	{
		CLayer* layer = *it;
		if(CLayerTree::Releasable(layer)==true)
			nCount++;
	}
	stream<<nCount;

	for(CTree<CLayer>::forwiterator it = this->forwbegin(); it!=this->forwend(); ++it)
	{
		CLayer* layer = *it;
		if(CLayerTree::Releasable(layer)==false)
			continue;
		layer->ReleaseWeb(stream, datainfo);
	}
}
void CLayerTree::ReleaseWeb(boost::json::object& json, const CDatainfo& datainfo, const BYTE& minMapLevel, const BYTE& maxMapLevel, bool includeProperties)
{
	m_library.ReleaseWeb(json);

	boost::json::array child1;
	for(CTree<CLayer>::forwiterator it = this->forwbegin(); it != this->forwend(); ++it)
	{
		CLayer* layer = *it;
		if(CLayerTree::Releasable(layer))
		{
			boost::json::object child2;
			if(includeProperties)
			{
				CDatabase* pDatabase = nullptr;
				CString strSQL = _T("");
				CString strIdField = _T("");
				CATTDatasource* pDatasource = layer->GetAttDatasource();
				if(pDatasource != nullptr)
				{
					pDatabase = CLayerTree::GetAttDatabase(pDatasource->m_strDatabase);
					const CHeaderProfile& headerprofile = pDatasource->GetHeaderProfile();
					strSQL = headerprofile.MakeSelectStatment();
					strIdField = headerprofile.m_strIdField;
				}
				layer->ReleaseWeb(child2, datainfo, minMapLevel, maxMapLevel, pDatabase, strSQL, strIdField);
			}
			else
			{
				layer->ReleaseWeb(child2, datainfo, minMapLevel, maxMapLevel, nullptr, _T(""), _T(""));
			}
			child1.push_back(child2);
		}
	}
	json["Layers"]= child1;
}
void CLayerTree::ReleaseWeb(pbf::writer& writer, pbf::tag tag, const CDatainfo& datainfo, const BYTE& minMapLevel, const BYTE& maxMapLevel, bool includeProperties) const
{
	writer.add_tag(tag, pbf::type::bytes);

	m_library.ReleaseWeb(writer);

	std::string cdata;
	pbf::writer child(cdata);
	for(CTree<CLayer>::forwiterator it = this->forwbegin(); it != this->forwend(); ++it)
	{
		CLayer* layer = *it;		
		if(CLayerTree::Releasable(layer) == false)
			continue;

		BYTE type = layer->Gettype();
		if(type == 11)
		{
			child.add_tag((pbf::tag)type, pbf::type::bytes);
			layer->ReleaseCap(child, datainfo, minMapLevel, maxMapLevel, true);			
		}
		else if(includeProperties)
		{
			CDatabase* pDatabase = nullptr;
			CString strSQL = _T("");
			CString strIdField = _T("");
			CATTDatasource* pDatasource = layer->GetAttDatasource();
			if(pDatasource != nullptr)
			{
				pDatabase = CLayerTree::GetAttDatabase(pDatasource->m_strDatabase);
				const CHeaderProfile& headerprofile = pDatasource->GetHeaderProfile();
				strSQL = headerprofile.MakeSelectStatment();
				strIdField = headerprofile.m_strIdField;
			}
			layer->ReleaseWeb(child, (pbf::tag)type, datainfo, minMapLevel, maxMapLevel, pDatabase, strSQL, strIdField);
		}
		else
		{
			layer->ReleaseWeb(child, (pbf::tag)type, datainfo, minMapLevel, maxMapLevel, nullptr, _T(""), _T(""));
		}
	}
	writer.add_bytes(cdata.c_str(),cdata.length());
}
bool CLayerTree::Releasable(CLayer* layer) const
{
	if(layer->m_bVisible)
		return true;

	for(CTree<CLayer>::forwiterator it = this->forwbegin(); it!=this->forwend(); ++it)
	{
		CLayer* layer1 = *it;
		CLayer* parent = layer1->Parent();
		if(parent==nullptr)
			continue;
		if(layer1->m_wId==parent->m_wId)//should never happen, but somehow it does exist
			continue;
		if(parent->m_wId==layer->m_wId)
		{
			if(CLayerTree::Releasable(layer1)==true)
				return true;
		}
	}
	return false;
}
void CLayerTree::MinMaxLevel(BYTE& minLevelObj, BYTE& maxLevelObj, BYTE& minLevelTag, BYTE& maxLevelTag) const
{
	minLevelObj = 25;
	maxLevelObj = 0;
	minLevelTag = 25;
	maxLevelTag = 0;
	for(CTree<CLayer>::forwiterator it = this->forwbegin(); it!=this->forwend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_geomlist.GetCount()==0)
			continue;

		minLevelObj = min(minLevelObj, floor(layer->m_minLevelObj));
		maxLevelObj = max(maxLevelObj, ceil(layer->m_maxLevelObj));
		minLevelTag = min(minLevelTag, floor(layer->m_minLevelTag));
		maxLevelTag = max(maxLevelTag, ceil(layer->m_maxLevelTag));
	}
	minLevelObj = max(minLevelObj, 0);
	maxLevelObj = min(maxLevelObj, 25);
	minLevelTag = max(minLevelTag, 0);
	maxLevelTag = min(maxLevelTag, 25);

	minLevelTag = max(minLevelObj, minLevelTag);
	maxLevelTag = min(maxLevelObj, maxLevelTag);
}