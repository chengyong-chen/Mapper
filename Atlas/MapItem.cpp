#include "stdafx.h"
#include "MapItem.h"
//#include "Lzari.h"

#include  <io.h>
#include "../Style/Line.h"
#include "../Style/Type.h"

#include "../HyperLink/Link.h"
#include "../Layer/GridMemFile.h"
#include "../Layer/GeomMemFile.h"
#include "../Projection/Projection1.h"
namespace boost {
	namespace 	json {
		class object;
	}
};
namespace pbf {
	class writer;
};

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMapItem::CMapItem(void)
{
	m_dwMaxGeomId = 0;
	m_wMapId = 0;
	m_pProjection = nullptr;
	m_pGridMemFile = nullptr;
	m_pGeomMemFile = nullptr;
}

CMapItem::~CMapItem(void)
{
	delete m_pProjection;

	for(std::list<CLink*>::iterator it = m_linklist.begin(); it!=m_linklist.end(); ++it)
	{
		const CLink* link = *it;
		delete link;
	}
	if(m_pGridMemFile!=nullptr)
	{
		m_pGridMemFile->Close();
		delete m_pGridMemFile;
		m_pGridMemFile = nullptr;
	}
	if(m_pGeomMemFile!=nullptr)
	{
		m_pGeomMemFile->Close();
		delete m_pGeomMemFile;
		m_pGeomMemFile = nullptr;
	}

	if(m_varHead.parray!=nullptr)
	{
		SafeArrayDestroy(m_varHead.parray);
		m_varHead.parray = nullptr;
	}

	m_database.Close();
}

BOOL CMapItem::Open(CString strFile)
{
	strFile.MakeUpper();
	if(strFile.Right(4)==_T(".Grf")||strFile.Right(4)==_T(".GIS"))
	{
		CFile file;
		if(file.Open(strFile, CFile::modeRead|CFile::shareDenyWrite)==TRUE)
		{
			CArchive ar(&file, CArchive::load);

			DWORD dwVersion;
			ar >> dwVersion;
			if(dwVersion > AfxGetCurrentArVersion())
			{
				AfxMessageBox(_T("你的软件版本太老了!\n请安装新版本的软件."));				
				ar.Close();
				file.Close();
				return FALSE;
			}

			m_Datainfo.Serialize(ar, dwVersion);

			CStringA proj4;
			CStringA ellps;
			ar>>proj4;
			ar>>ellps;
			m_pProjection = CProjection::Apply(proj4.GetString(), ellps.GetString());
			if(m_pProjection!=nullptr)
			{
				m_pProjection->Serialize(ar, dwVersion);
			}

			unsigned char byte;
			ar>>byte;
			ar>>m_dwMaxGeomId;

			m_library.Serialize(ar, dwVersion);
			m_layertree.Serialize(ar, dwVersion);

			CPoint point;
			ar>>byte;;
			ar>>point;

			CString strGrid = ar.m_strFileName;
			CString strGeom = ar.m_strFileName;

			strGrid = strGrid.Left(strGrid.ReverseFind(_T('.')));
			strGeom = strGeom.Left(strGeom.ReverseFind(_T('.')));
			strGrid = strGrid+_T(".Grd");
			strGeom = strGeom+_T(".Geo");

			if(_taccess(strGrid, 00)!=-1||_taccess(strGeom, 00)!=-1)
			{
				m_pGridMemFile = new CGridMemFile();
				m_pGeomMemFile = new CGeomMemFile();
				if(m_pGridMemFile->Open(strGrid)==false||m_pGeomMemFile->Open(strGeom)==false)
				{
					delete m_pGridMemFile;
					m_pGridMemFile = nullptr;
					delete m_pGeomMemFile;
					m_pGeomMemFile = nullptr;
				}
			}

			if(strFile.Right(4)==_T(".GIS"))
			{
				m_database.Serialize(ar, dwVersion);
				//	m_linklist.Serialize(ar);
			}

			ar.Close();
			file.Close();

			CMemFile mFile;
			CArchive mAr((CFile*)&mFile, CArchive::store);

			mAr<<m_wMapId;
			m_Datainfo.ReleaseDCOM(mAr);
			const BYTE type2 = m_pProjection==nullptr ? 0XFF : m_pProjection->Gettype();
			mAr<<type2;
			if(m_pProjection!=nullptr)
			{
				m_pProjection->ReleaseDCOM(mAr);
			}

			m_library.ReleaseDCOM(mAr);
			m_layertree.ReleaseDCOM(mAr);

			m_database.ReleaseDCOM(mAr);
			mAr<<m_linklist.size();
			for(std::list<CLink*>::iterator it = m_linklist.begin(); it!=m_linklist.end(); ++it)
			{
				CLink* link = *it;
				link->ReleaseDCOM(mAr);
			}

			mAr.Flush();
			mAr.Close();
			mFile.Flush();
			const int length = mFile.GetLength();
			unsigned char* pData = mFile.Detach();

			if(pData!=nullptr&&length>0)
			{
				//	CLzari lzari;
				//	unsigned char* pNewData = lzari.Compress(pData,length,length);
				//	
				//	delete pData;
				//	pData = path;

				//	pData = pNewData;
				//	
				//	SAFEARRAYBOUND bound;
				//	bound.cElements = length; 
				//	bound.lLbound = 0;

				//	m_varHead.vt = VT_UI1 | VT_ARRAY;
				//	m_varHead.parray = SafeArrayCreate(VT_UI1, 1, &bound); // Create it

				//	void* pDest;
				//	SafeArrayAccessData(m_varHead.parray, &pDest);
				//	memcpy(pDest, pData,length); 
				//	SafeArrayUnaccessData(m_varHead.parray);

				////	delete pData;
				////	pData = path;
				//	lzari.Release();
			}

			//	mFile.Close();
			return TRUE;
		}
		else
		{
			//	mFile.Close();
			return FALSE;
		}
	}
	else
		return FALSE;
}

STDMETHODIMP CMapItem::GetData(unsigned int uZoom, CSize docOffset, CRect clipper, VARIANT* varData, CObList& vestigelist) const
{
	int xMin = 0;
	int yMin = 0;
	int xMax = 0;
	int yMax = 0;
	if(m_pGridMemFile!=nullptr&&m_pGeomMemFile!=nullptr)
	{
		xMin = (clipper.left-docOffset.cy)/(int)(m_pGridMemFile->m_Head.dwWidth);
		yMin = (clipper.top-docOffset.cy)/(int)(m_pGridMemFile->m_Head.dwHeight);
		xMax = (clipper.right-docOffset.cy)/(int)(m_pGridMemFile->m_Head.dwWidth);
		yMax = (clipper.bottom-docOffset.cy)/(int)(m_pGridMemFile->m_Head.dwHeight);

		if(xMin<0)
			xMin = 0;
		if(yMin<0)
			yMin = 0;
		if(xMax>=m_pGridMemFile->m_Head.xCount)
			xMax = m_pGridMemFile->m_Head.xCount-1;
		if(yMax>=m_pGridMemFile->m_Head.yCount)
			yMax = m_pGridMemFile->m_Head.yCount-1;
	}

	CMemFile file;
	CArchive ar((CFile*)&file, CArchive::store);
	POSITION pos = vestigelist.GetHeadPosition();
	for(CTree<CLayer>::forwiterator it = m_layertree.forwbegin(); it!=m_layertree.forwend(); ++it)
	{
		CLayer* layer = *it;
		CLayerVestige* vestige = (CLayerVestige*)vestigelist.GetNext(pos);
		if(layer->m_bVisible==false)
		{
			ar<<(WORD)0X0000;//for vestige
			ar<<(WORD)0X0000;
			continue;
		}
		if(uZoom>layer->m_minLevelObj&&(uZoom>layer->m_minLevelTag||layer->m_bShowTag==false))
		{
			ar<<(WORD)0X0000;//for vestige
			ar<<(WORD)0X0000;//for objcount
			continue;
		}
		if(uZoom<layer->m_maxLevelObj&&(uZoom<layer->m_maxLevelTag||layer->m_bShowTag==false))
		{
			ar<<(WORD)0X0000;//for vestige
			ar<<(WORD)0X0000;//for objcount
			continue;
		}

		CObList geomlist;
		POSITION pos = layer->m_geomlist.GetHeadPosition();
		while(pos!=nullptr)
		{
			CGeom* pGeom = layer->m_geomlist.GetNext(pos);			
			if((pGeom->m_Rect&clipper).IsRectEmpty()==TRUE)
				continue;

			DWORD id;
			if(vestige->lasttimeId->Lookup(pGeom->m_geoId, id)==TRUE)
			{
				vestige->existentId.Add(pGeom->m_geoId);
				vestige->currentId->SetAt(pGeom->m_geoId, pGeom->m_geoId);
				continue;
			}

			if((pGeom->m_bType==1||pGeom->m_bType==2)&&pGeom->m_bClosed==true)
			{
				if(layer->GetFill()==nullptr&&pGeom->m_pFill==nullptr)
				{
					pGeom->m_bClosed = false;
				}
			}

			CTypedPtrList<CObList, CGeom*> list;
			//	if(pGeom->ClipIn(clipper,list) == true) temperally commented out
			{
				if(list.GetCount()>0)
				{
					POSITION pos = list.GetHeadPosition();
					while(pos!=nullptr)
					{
						CGeom* geom = list.GetNext(pos);
						geomlist.AddTail(geom);
					}
					list.RemoveAll();
				}
				else//对于没有经过裁减的元素,把Id保存起来,以便下次不再传送
				{
					vestige->currentId->SetAt(pGeom->m_geoId, pGeom->m_geoId);
					vestige->existentId.Add(pGeom->m_geoId);

					pGeom = pGeom->Clone();
					geomlist.AddTail(pGeom);
				}
			}
		}

		if(m_pGridMemFile!=nullptr&&m_pGeomMemFile!=nullptr)
		{
			int wLayerIndex = m_pGridMemFile->GetLayerIndex(layer->m_wId);;
			CMap<DWORD, DWORD&, DWORD, DWORD&> usedGeom;
			for(int y = yMin; y<=yMax; y++)
				for(int x = xMin; x<=xMax; x++)
				{
					CGridMemFile::GRID* pLayerGrid = m_pGridMemFile->GetGrid(wLayerIndex, y, x);
					if(pLayerGrid->wGeomCount==0)
						continue;

					DWORD* dwGeomlist = m_pGridMemFile->GetGridGeomList(pLayerGrid);
					for(WORD index = 0; index<pLayerGrid->wGeomCount; index++)
					{
						DWORD dwGeomIndex = *dwGeomlist;
						dwGeomlist++;

						CGeomMemFile::GEOMHEAD* pGeomHead = m_pGeomMemFile->GetGeomHeadByIndex(wLayerIndex, dwGeomIndex);
						CRect rect = pGeomHead->rect;
						if((rect&clipper).IsRectEmpty()==TRUE)
							continue;

						DWORD id;
						if(vestige->lasttimeId->Lookup(pGeomHead->dwId, id)==TRUE)
						{
							vestige->existentId.Add(pGeomHead->dwId);
							vestige->currentId->SetAt(pGeomHead->dwId, pGeomHead->dwId);
							continue;
						}

						if(pGeomHead->bType!=8)
						{
							DWORD id;
							if(usedGeom.Lookup(pGeomHead->dwId, id))
								continue;
							else
								usedGeom.SetAt(pGeomHead->dwId, pGeomHead->dwId);
						}

						CGeom* pGeom = m_pGeomMemFile->GetGeom(pGeomHead);
						if((pGeomHead->bType==1||pGeomHead->bType==2)&&pGeom->m_bClosed==true)
						{
							if(layer->GetFill()==nullptr&&pGeom->m_pFill==nullptr)
							{
								pGeom->m_bClosed = false;
							}
						}

						CTypedPtrList<CObList, CGeom*> list;
						/* temperally commented out	if(pGeom->ClipIn(clipper,list) == true)
							{
								if(list.GetCount()>0)
								{
									POSITION pos = list.GetHeadPosition();
									while(pos != path)
									{
										CGeom* geom = list.GetNext(pos);
										if(geom == path)
											continue;

										geomlist.AddTail(geom);
									}
									list.RemoveAll();

									delete pGeom;
									pGeom = path;
								}
								else//对于没有经过裁减的元素,把Id保存起来,以便下次不再传送
								{
									vestige->currentId->SetAt(pGeom->m_geoId,pGeom->m_geoId);
									vestige->existentId.Add(pGeom->m_geoId);

									geomlist.AddTail(pGeom);
								}
							}
							else
							{
								delete pGeom;
								pGeom = path;
							}*/
					}
				}
			usedGeom.RemoveAll();
		}

		unsigned short vestigecount = vestige->existentId.GetCount();
		ar<<vestigecount;
		for(unsigned short i = 0; i<vestigecount; i++)
		{
			DWORD id = vestige->existentId[i];
			ar<<id;
		}

		unsigned short nCount = geomlist.GetCount();
		ar<<nCount;
		POSITION pos2 = geomlist.GetHeadPosition();
		while(pos2!=nullptr)
		{
			CGeom* pGeom = (CGeom*)geomlist.GetNext(pos2);

			unsigned char type = pGeom->Gettype();
			ar<<type;
			pGeom->ReleaseDCOM(ar);

			delete pGeom;
			pGeom = nullptr;
		}
		geomlist.RemoveAll();
	}

	ar.Flush();
	ar.Close();

	file.Flush();

	//	
	int length = file.GetLength();
	unsigned char* pData = file.Detach();
	if(pData!=nullptr&&length>0)
	{
		//CLzari lzari;
		//unsigned char* pNewData = lzari.Compress(pData,length,length);
		//
		//delete pData;
		//pData = nullptr;
		//pData = pNewData;

		//SAFEARRAYBOUND bound;
		//bound.cElements = length; 
		//bound.lLbound = 0;

		//varData->vt = VT_UI1 | VT_ARRAY;
		//varData->parray = SafeArrayCreate(VT_UI1, 1, &bound); // Create it

		//void* pDest;
		//SafeArrayAccessData(varData->parray, &pDest);
		//memcpy(pDest, pData, bound.cElements); // Copy into array
		//SafeArrayUnaccessData(varData->parray);

		//lzari.Release();

		//	file.Close();
		return S_OK;
	}
	else
	{
		//	file.Close();
		return S_FALSE;
	}
}
