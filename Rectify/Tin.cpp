#include "stdafx.h"

#include "Tin.h"
#include "Triangle.h"
#include "Datum.h"

#include <stdio.h>

IMPLEMENT_SERIAL(CTin, CObject, 0)

CTin::CTin()
{
}

CTin::~CTin()
{
	Clear();
}

void CTin::Clear()
{
	ClearTins();

	POSITION pos = m_datumlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CDatum* pDatum = m_datumlist.GetNext(pos);
		if(pDatum!=nullptr)
		{
			delete pDatum;
			pDatum = nullptr;
		}
	}
	m_datumlist.RemoveAll();
}

void CTin::ClearTins()
{
	POSITION pos = m_trianglelist.GetHeadPosition();
	while(pos!=nullptr)
	{
		const CTriangle* pTriangle = m_trianglelist.GetNext(pos);
		delete pTriangle;
	}
	m_trianglelist.RemoveAll();
}

void CTin::Invalidate(CWnd* pWnd, const CViewinfo& viewinfo) const
{
	POSITION pos1 = m_trianglelist.GetHeadPosition();
	while(pos1!=nullptr)
	{
		CTriangle* pTriangle = m_trianglelist.GetNext(pos1);
		if(pTriangle!=nullptr)
		{
			pTriangle->Invalidate(pWnd, viewinfo);
		}
	}

	POSITION pos2 = m_datumlist.GetHeadPosition();
	while(pos2!=nullptr)
	{
		CDatum* pDatum = m_datumlist.GetNext(pos2);
		if(pDatum!=nullptr)
		{
			pDatum->Invalidate(pWnd, viewinfo);
		}
	}
}

CPoint CTin::Rectify(const CPoint& point)
{
	POSITION pos1 = m_trianglelist.GetHeadPosition();
	while(pos1!=nullptr)
	{
		CTriangle* Tri = m_trianglelist.GetNext(pos1);
		if(Tri->PointIn(point))
		{
			return Tri->Rectify(point);
		}
	}

	UINT miniDis = 0XFFFFFFFF;
	CDatum* Node = nullptr;
	POSITION pos2 = m_datumlist.GetHeadPosition();
	while(pos2!=nullptr)
	{
		CDatum* pDatum = m_datumlist.GetNext(pos2);
		const CPoint point1 = pDatum->m_rPoint;
		const double dis = sqrt(pow((double)(point1.x-point.x), 2))+sqrt(pow((double)(point1.y-point.y), 2));
		if(dis<miniDis)
		{
			miniDis = dis;
			Node = pDatum;
		}
	}

	UINT miniPerimeter = 0XFFFFFFFF;
	CTriangle* pTriangle = nullptr;

	POSITION Pos3 = m_trianglelist.GetHeadPosition();
	while(Pos3!=nullptr)
	{
		CTriangle* tri = m_trianglelist.GetNext(Pos3);
		if(tri->m_pDatum1==Node||tri->m_pDatum2==Node||tri->m_pDatum3==Node)
		{
			const long perimeter = tri->PointTo(point);
			if(perimeter<miniPerimeter)
			{
				miniPerimeter = perimeter;
				pTriangle = tri;
			}
		}
	}

	return pTriangle!=nullptr ? pTriangle->Rectify(point) : point;
}

void CTin::AddDatum(CDatum* pDatum)
{
	m_datumlist.AddTail(pDatum);
}

void CTin::RemoveDatum(CDatum* pDatum)
{
	POSITION pos1 = m_trianglelist.GetHeadPosition();
	while(pos1!=nullptr)
	{
		const CTriangle* pTriangle = m_trianglelist.GetNext(pos1);
		delete pTriangle;
	}
	m_trianglelist.RemoveAll();

	POSITION pos2 = m_datumlist.GetHeadPosition();
	POSITION Pos3 = pos2;
	while((Pos3 = pos2)!=nullptr)
	{
		CDatum* datum = m_datumlist.GetNext(pos2);
		if(datum==pDatum)
		{
			m_datumlist.RemoveAt(Pos3);
			delete pDatum;
			break;
		}
	}

	CTin::Create();
}

CDatum* CTin::PickDatum(const CViewinfo& viewinfo, const CPoint& docPoint)
{
	POSITION pos = m_datumlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CDatum* pDatum = m_datumlist.GetNext(pos);
		if(pDatum->Pick(viewinfo, docPoint)==TRUE)
		{
			return pDatum;
		}
	}

	return nullptr;
}

void CTin::Create()
{
	CTin::ClearTins();
	CTin::RemoveIdentical();

	//search the minpoint
	UINT miniDis = 0XFFFFFFFF;
	CDatum* Node1 = nullptr;
	CDatum* Node2 = nullptr;

	POSITION pos = m_datumlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CDatum* pDatum = m_datumlist.GetNext(pos);
		CPoint point = pDatum->m_rPoint;
		double dis = sqrt(pow((double)point.x, 2))+sqrt(pow((double)point.y, 2));
		if(dis<miniDis)
		{
			miniDis = dis;
			Node1 = pDatum;
		}
	}
	if(Node1==nullptr)
		return;

	pos = m_datumlist.Find(Node1);
	m_datumlist.RemoveAt(pos);
	m_datumlist.AddHead(Node1);
	//��С��Ϊdatumlist[0]

	//search the nearest point to Point0
	miniDis = 0XFFFFFFFF;
	pos = m_datumlist.GetHeadPosition();
	CPoint point1 = Node1->m_rPoint;
	while(pos!=nullptr)
	{
		CDatum* pDatum = m_datumlist.GetNext(pos);
		if(pDatum==Node1)
			continue;

		CPoint point = pDatum->m_rPoint;
		double dis = sqrt(pow((double)(point.x-point1.x), 2)+pow((double)(point.y-point1.y), 2));
		if(dis<miniDis)
		{
			miniDis = dis;
			Node2 = pDatum;
		}
	}
	if(Node2==nullptr)
		return;

	pos = m_datumlist.Find(Node2);
	m_datumlist.RemoveAt(pos);
	pos = m_datumlist.GetHeadPosition();
	m_datumlist.InsertAfter(pos, Node2);

	//����datumlist[1]��datumlist[0]���е�����ĵ�,�����һ��������
	//
	miniDis = 0XFFFFFFFF;
	CDatum* Node3 = nullptr;
	CPoint point2 = Node2->m_rPoint;
	pos = m_datumlist.FindIndex(2);
	while(pos!=nullptr)
	{
		CDatum* pDatum = m_datumlist.GetNext(pos);
		CPoint point = pDatum->m_rPoint;
		if((point.x-point1.x)*(point2.y-point1.y)==(point.y-point1.y)*(point2.x-point1.x))
			continue;//�Ҳ������ϵĵ�

		double dis = sqrt(pow((double)(point.x-point1.x), 2)+pow((double)(point.y-point1.y), 2))+sqrt(pow((double)(point.x-point2.x), 2)+pow((double)(point.y-point2.y), 2));
		if(dis<miniDis)
		{
			miniDis = dis;
			Node3 = pDatum;
		}
	}
	if(Node3==nullptr)
		return;

	CTriangle* pTriangle = new CTriangle;
	pTriangle->m_pDatum1 = Node1;
	pTriangle->m_pDatum2 = Node2;
	pTriangle->m_pDatum3 = Node3;
	m_trianglelist.AddTail(pTriangle);

	pos = m_trianglelist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CTriangle* pTriangle = m_trianglelist.GetAt(pos);
		for(long i = 0; i<3; i++)
		{
			CDatum* Node1 = nullptr;
			CDatum* Node2 = nullptr;
			CDatum* Node3 = nullptr;
			if(i==0)
			{
				Node1 = pTriangle->m_pDatum1;
				Node2 = pTriangle->m_pDatum2;
				Node3 = pTriangle->m_pDatum3;
			}
			if(i==1)
			{
				Node1 = pTriangle->m_pDatum2;
				Node2 = pTriangle->m_pDatum3;
				Node3 = pTriangle->m_pDatum1;
			}
			if(i==2)
			{
				Node1 = pTriangle->m_pDatum1;
				Node2 = pTriangle->m_pDatum3;
				Node3 = pTriangle->m_pDatum2;
			}

			CPoint point1 = Node1->m_rPoint;
			CPoint point2 = Node2->m_rPoint;
			CPoint point3 = Node3->m_rPoint;

			//if this edge is used twice
			bool full = 0;
			POSITION prePos = m_trianglelist.GetHeadPosition();
			while(prePos!=nullptr)
			{
				CTriangle* Tri = m_trianglelist.GetNext(prePos);
				if(Tri==pTriangle)
					continue;

				if((Tri->m_pDatum1==Node1||Tri->m_pDatum2==Node1||Tri->m_pDatum3==Node1)&&(Tri->m_pDatum1==Node2||Tri->m_pDatum2==Node2||Tri->m_pDatum3==Node2))
				{
					full = TRUE;
					break;
				}
			}
			if(full==TRUE)
			{
				continue;
			}

			CDatum* Node = nullptr;
			miniDis = 0XFFFFFFFF;
			POSITION dPos = m_datumlist.GetHeadPosition();
			while(dPos!=nullptr)
			{
				CDatum* pDatum = m_datumlist.GetNext(dPos);
				if(pDatum==Node1||pDatum==Node2||pDatum==Node3)
					continue;

				CPoint point = pDatum->m_rPoint;
				if((double)(point.x-point1.x)*(double)(point2.y-point1.y)==(double)(point.y-point1.y)*(double)(point2.x-point1.x))
					continue;

				double dis1 = (double)(point3.y-point1.y)*(double)(point2.x-point1.x)-(double)(point3.x-point1.x)*(double)(point2.y-point1.y);
				double dis2 = (double)(point.y-point1.y)*(double)(point2.x-point1.x)-(double)(point.x-point1.x)*(double)(point2.y-point1.y);
				if(dis1*dis2>0)
					continue;

				//decide if this m_Node can be used.
				long usedto1 = 0;
				long usedto2 = 0;
				POSITION prePos = m_trianglelist.GetHeadPosition();
				while(prePos!=nullptr)
				{
					CTriangle* Tri = m_trianglelist.GetNext(prePos);
					if(Tri==pTriangle)
						continue;

					if((Tri->m_pDatum1==Node1||Tri->m_pDatum2==Node1||Tri->m_pDatum3==Node1)&&(Tri->m_pDatum1==pDatum||Tri->m_pDatum2==pDatum||Tri->m_pDatum3==pDatum))
					{
						usedto1++;
					}
					else if((Tri->m_pDatum1==Node2||Tri->m_pDatum2==Node2||Tri->m_pDatum3==Node2)&&(Tri->m_pDatum1==pDatum||Tri->m_pDatum2==pDatum||Tri->m_pDatum3==pDatum))
					{
						usedto2++;
					}

					if(usedto1>=2||usedto2>=2)
					{
						break;
					}
				}
				if(usedto1>=2||usedto2>=2)
				{
					continue;
				}

				//decide if intersect with other triangle
				bool brea = false;
				prePos = m_trianglelist.GetHeadPosition();
				while(prePos!=nullptr)
				{
					CTriangle* Tri = m_trianglelist.GetNext(prePos);
					if(Tri==pTriangle)
						continue;

					//donn't cal with the triangles which has edge of m_pDatum1 and m_pDatum2
					//					if(Tri->m_pDatum1 == m_pDatum1 || Tri->m_pDatum2 == m_pDatum1 || Tri->m_pDatum3 == m_pDatum1)
					//						continue;
					//					if(Tri->m_pDatum1 == m_pDatum2 || Tri->m_pDatum2 == m_pDatum2 || Tri->m_pDatum3 == m_pDatum2)
					//						continue;
					//					if(Tri->m_pDatum1 == pDatum || Tri->m_pDatum2 == pDatum || Tri->m_pDatum3 == pDatum)
					//						continue;

					//cal with 3 edge
					for(long i = 0; i<3; i++)
					{
						CPoint point11;
						CPoint point22;

						if(i==0)
						{
							point11 = Tri->m_pDatum1->m_rPoint;
							point22 = Tri->m_pDatum2->m_rPoint;
						}
						if(i==1)
						{
							point11 = Tri->m_pDatum2->m_rPoint;
							point22 = Tri->m_pDatum3->m_rPoint;
						}
						if(i==2)
						{
							point11 = Tri->m_pDatum1->m_rPoint;
							point22 = Tri->m_pDatum3->m_rPoint;
						}

						double dis1 = (double)(point22.y-point11.y)*(point.x-point11.x)-(double)(point22.x-point11.x)*(point.y-point11.y);
						double dis2 = (double)(point22.y-point11.y)*(point1.x-point11.x)-(double)(point22.x-point11.x)*(point1.y-point11.y);
						double dis3 = (double)(point.y-point1.y)*(point11.x-point1.x)-(double)(point.x-point1.x)*(point11.y-point1.y);
						double dis4 = (double)(point.y-point1.y)*(point22.x-point1.x)-(double)(point.x-point1.x)*(point22.y-point1.y);
						if(dis1*dis2<0&&dis3*dis4<0)
						{
							brea = TRUE;
							break;
						}

						dis1 = (double)(point22.y-point11.y)*(point.x-point11.x)-(double)(point22.x-point11.x)*(point.y-point11.y);
						dis2 = (double)(point22.y-point11.y)*(point2.x-point11.x)-(double)(point22.x-point11.x)*(point2.y-point11.y);
						dis3 = (double)(point.y-point2.y)*(point11.x-point2.x)-(double)(point.x-point2.x)*(point11.y-point2.y);
						dis4 = (double)(point.y-point2.y)*(point22.x-point2.x)-(double)(point.x-point2.x)*(point22.y-point2.y);
						if(dis1*dis2<0&&dis3*dis4<0)
						{
							brea = TRUE;
							break;
						}
					}

					if(brea==TRUE)
					{
						break;
					}
				}

				if(brea==TRUE)
				{
					continue;
				}

				double dis = sqrt(pow((double)(point.x-point1.x), 2)+pow((double)(point.y-point1.y), 2))+sqrt(pow((double)(point.x-point2.x), 2)+pow((double)(point.y-point2.y), 2));
				if(dis<miniDis)
				{
					miniDis = dis;
					Node = pDatum;
				}
			}

			if(Node!=nullptr)
			{
				CTriangle* pTriangle = new CTriangle;
				pTriangle->m_pDatum1 = Node1;
				pTriangle->m_pDatum2 = Node2;
				pTriangle->m_pDatum3 = Node;
				m_trianglelist.AddTail(pTriangle);
			}
		}

		m_trianglelist.GetNext(pos);
	}

	pos = m_trianglelist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CTriangle* pTriangle = m_trianglelist.GetNext(pos);
		pTriangle->CalParameter();
	}
}

void CTin::Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo)
{
	POSITION pos1 = m_trianglelist.GetHeadPosition();
	while(pos1!=nullptr)
	{
		CTriangle* pTriangle = m_trianglelist.GetNext(pos1);
		pTriangle->Draw(g, viewinfo);
	}

	POSITION pos2 = m_datumlist.GetHeadPosition();
	while(pos2!=nullptr)
	{
		CDatum* pDatum = m_datumlist.GetNext(pos2);
		pDatum->Draw(g, viewinfo);
	}
}

void CTin::Load()
{
	Clear();

	TCHAR CurrentDir[256];
	GetCurrentDirectory(255, CurrentDir);

	CFileDialog dlg(TRUE,
		_T("*.Ret"),
		nullptr,
		OFN_HIDEREADONLY,
		_T("Tie Files (*.Ret)|*.Ret||"),
		AfxGetMainWnd());

	if(dlg.DoModal()!=IDOK)
	{
		SetCurrentDirectory(CurrentDir);
		return;
	}
	SetCurrentDirectory(CurrentDir);
	const CString strPath = dlg.GetPathName();

	CFile pfile;
	if(pfile.Open(strPath, CFile::modeRead|CFile::shareDenyWrite)==TRUE)
	{
		CArchive ar(&pfile, CArchive::load);
		m_datumlist.Serialize(ar);
		ar.Close();
	}

	pfile.Close();
}

void CTin::Store()
{
	TCHAR CurrentDir[256];
	GetCurrentDirectory(255, CurrentDir);

	CFileDialog dlg(TRUE,
		_T("*.Ret"),
		nullptr,
		OFN_HIDEREADONLY,
		_T("Tie Files (*.Ret)|*.Ret||"),
		AfxGetMainWnd());

	if(dlg.DoModal()!=IDOK)
	{
		SetCurrentDirectory(CurrentDir);
		return;
	}
	SetCurrentDirectory(CurrentDir);
	const CString strPath = dlg.GetPathName();

	CFile file;
	file.Open(strPath, CFile::modeCreate|CFile::modeWrite);

	CArchive ar(&file, CArchive::store);

	m_datumlist.Serialize(ar);

	ar.Close();
	file.Close();
}

void CTin::RemoveIdentical()
{
	POSITION pos1 = m_datumlist.GetHeadPosition();
	while(pos1!=nullptr)
	{
		CDatum* pDatum1 = m_datumlist.GetAt(pos1);
		if(pDatum1!=nullptr)
		{
			POSITION pos2 = pos1;
			m_datumlist.GetNext(pos2);
			POSITION Pos3;
			while((Pos3 = pos2)!=nullptr)
			{
				CDatum* pDatum2 = m_datumlist.GetNext(pos2);
				if(pDatum1->m_rPoint==pDatum2->m_rPoint||pDatum1->m_bPoint==pDatum2->m_bPoint)
				{
					delete pDatum2;
					m_datumlist.RemoveAt(Pos3);
				}
			}
		}

		m_datumlist.GetNext(pos1);
	}
}
