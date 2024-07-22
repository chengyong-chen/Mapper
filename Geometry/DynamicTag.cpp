#include "stdafx.h"
#include <cmath>
#include "DynamicTag.h"
#include "Global.h"
#include "Tag.h"
#include "Geom.h"
#include "Poly.h"
#include "Text.h"

#include "../DataView/ViewInfo.h"
#include "../Style/Hint.h"

#include <gdiplus.h>
#include <corecrt_math_defines.h>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern ANCHOR PrioritizedAnchors[];
extern __declspec(dllimport) CArray<Gdiplus::RectF, Gdiplus::RectF&> d_oRectArray;

CDynamicTag::CDynamicTag()
{
}

CDynamicTag::~CDynamicTag()
{
}

CRectTag::CRectTag()
{
	defAnchor = ANCHOR::ANCHOR_0;
}

void CPointTag::Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CHint* pHint) const
{
	if(m_string.IsEmpty()==TRUE)
		return;
	if(pHint==nullptr)
		return;

	Gdiplus::RectF tagRect = pHint->DrawString(g, viewinfo, m_string, m_point, 0, HALIGN::HA_CENTER, VALIGN::VA_CENTER);
	d_oRectArray.Add(tagRect);
}

void CFixedTag::Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CHint* pHint) const
{
	if(m_string.IsEmpty()==TRUE)
		return;
	if(pHint==nullptr)
		return;

	Gdiplus::RectF tagRect = pHint->DrawString(g, viewinfo, m_string, m_point, 0, HALIGN::HA_LEFT, VALIGN::VA_CENTER);
	d_oRectArray.Add(tagRect);
}

void CRectTag::Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CHint* pHint) const
{
	if(m_string.IsEmpty()==TRUE)
		return;
	if(pHint==nullptr)
		return;

	const Gdiplus::SizeF textSize = pHint->MeasureText<Gdiplus::SizeF>(g, viewinfo, m_string, 0);	
	const float size = pHint->m_fSize*viewinfo.m_sizeDPI.cx/72.f;
	const Gdiplus::RectF& markRect = m_rect;
	Gdiplus::RectF maxRect = markRect;
	const bool bHasEnglish = true;//used to use lucene to detected, removed, need to use a new way
	if(bHasEnglish==true)
	{
		maxRect.Inflate(textSize.Width, textSize.Height);
	}
	else
	{
		const long ex = max(textSize.Width, textSize.Height);
		maxRect.Inflate(ex, ex);
	}

	CArray<Gdiplus::RectF, Gdiplus::RectF&> rectarray;
	const int count = d_oRectArray.GetSize();
	for(int index = 0; index<count; index++)
	{
		Gdiplus::RectF rect2 = d_oRectArray.GetAt(index);
		if(rect2.IntersectsWith(maxRect)==TRUE)
			rectarray.Add(rect2);
	}

	double minArea = 9999999999999999.0f;
	ANCHOR Anchor = ANCHOR::ANCHOR_0;
	Gdiplus::Rect tagRect;

	if(defAnchor!=0)
	{
		HALIGN hAlign;
		VALIGN vAlign;
		CTag::GetDefaultAlign(defAnchor, hAlign, vAlign);
		const Gdiplus::RectF rect = ::GetTagRect(markRect, textSize, defAnchor, hAlign, vAlign);
		const int count = rectarray.GetSize();
		double area = 0.0f;
		for(int j = 0; j<count; j++)
		{
			Gdiplus::RectF rect2 = rectarray.GetAt(j);
			rect2.Intersect(rect);
			if(rect2.IsEmptyArea()==FALSE)
			{
				area += (double)rect2.Width*(double)rect2.Height;
			}
		}
		if(area==0.0f)
		{
			Anchor = defAnchor;
			minArea = 0;
		}
	}

	//��ʾ���Ĺؼ�����
	if(minArea!=0.0f)
	{
		for(int i = 0; i<8; i++)
		{
			ANCHOR anchor = PrioritizedAnchors[i];
			HALIGN hAlign;
			VALIGN vAlign;
			CTag::GetDefaultAlign(anchor, hAlign, vAlign);
			Gdiplus::RectF rect = ::GetTagRect(markRect, textSize, anchor, hAlign, vAlign);
			const int count = rectarray.GetSize();
			double area = 0.0f;
			for(int j = 0; j<count; j++)
			{
				Gdiplus::RectF rect2 = rectarray.GetAt(j);
				rect2.Intersect(rect);
				if(rect2.IsEmptyArea()==FALSE)
				{
					area += (double)rect2.Width*(double)rect2.Height;
				}
			}

			if(area==0.0f)
			{
				Anchor = anchor;
				minArea = 0;
				break;
			}
			else if(area<minArea)
			{
				Anchor = anchor;
				minArea = area;
				continue;
			}
		}
		rectarray.RemoveAll();
	}

	if(minArea<(size*size)/2)
	{
		HALIGN hAlign;
		VALIGN vAlign;
		CTag::GetDefaultAlign(Anchor, hAlign, vAlign);
		Gdiplus::PointF point = CTag::GetAnchorPoint<Gdiplus::RectF, Gdiplus::PointF>(markRect, Anchor);
		Gdiplus::RectF tagRect = pHint->DrawString(g, viewinfo, m_string, point, 0, hAlign, vAlign);
		d_oRectArray.Add(tagRect);
	}
}

CGroupTag::CGroupTag()
{
	m_bDelete = false;
}

CGroupTag::~CGroupTag()
{
	if(m_bDelete==true)
	{
		POSITION pos = m_geomlist.GetHeadPosition();
		while(pos!=nullptr)
		{
			CGeom* pGeom = (CGeom*)m_geomlist.GetNext(pos);
			if(pGeom!=nullptr)
			{
				delete pGeom;
				pGeom = nullptr;
			}
		}
	}

	m_geomlist.RemoveAll();
}

void CGroupTag::Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CHint* pHint, const bool& bPivot)
{
	if(m_string.IsEmpty()==TRUE)
		return;
	if(pHint==nullptr)
		return;

	double tolerence = viewinfo.m_datainfo.m_zoomPointToDoc*2;
	while(m_geomlist.GetCount()>0)
	{
		CPoly* pPoly = (CPoly*)m_geomlist.GetHead();

		CObList sortList;
		sortList.AddTail(pPoly);
		m_geomlist.RemoveHead();

		CPoint head1 = pPoly->GetAnchor(1);
		CPoint tail1 = pPoly->GetAnchor(pPoly->GetAnchors());

		while(m_geomlist.GetCount()>0)
		{
			double miniDistance = 999999999999.0f;
			POSITION miniPosition = nullptr;
			CPoly* miniPoly = nullptr;
			BYTE bCorresponding = 0;

			POSITION Pos11 = m_geomlist.GetHeadPosition();
			POSITION Pos12 = Pos11;
			while((Pos12 = Pos11)!=nullptr)
			{
				CPoly* poly = (CPoly*)m_geomlist.GetNext(Pos11);

				CPoint head2 = poly->GetAnchor(1);
				CPoint tail2 = poly->GetAnchor(poly->GetAnchors());

				double tail1ToHead2 = sqrt(pow((double)(tail1.x-head2.x), 2)+pow((double)(tail1.y-head2.y), 2));
				double tail1Totail2 = sqrt(pow((double)(tail1.x-tail2.x), 2)+pow((double)(tail1.y-tail2.y), 2));
				double head1ToHead2 = sqrt(pow((double)(head1.x-head2.x), 2)+pow((double)(head1.y-head2.y), 2));
				double head1Totail2 = sqrt(pow((double)(head1.x-tail2.x), 2)+pow((double)(head1.y-tail2.y), 2));

				double miniExtremePoint = min(min(tail1ToHead2, tail1Totail2), min(head1ToHead2, head1Totail2));
				if(miniExtremePoint<miniDistance)
				{
					miniDistance = miniExtremePoint;
					miniPosition = Pos12;
					miniPoly = poly;

					if(miniDistance==tail1ToHead2)
					{
						bCorresponding = 1;
					}
					else if(miniDistance==tail1Totail2)
					{
						bCorresponding = 2;
					}
					else if(miniDistance==head1ToHead2)
					{
						bCorresponding = 3;
					}
					else if(miniDistance==head1Totail2)
					{
						bCorresponding = 4;
					}
				}
				if(miniExtremePoint<0.000001)
				{
					break;
				}
			}

			if(miniDistance<tolerence)
			{
				if(bCorresponding==1)
				{
					sortList.AddTail(miniPoly);

					tail1 = miniPoly->GetAnchor(miniPoly->GetAnchors());
				}
				else if(bCorresponding==2)
				{
					miniPoly->Reverse();
					sortList.AddTail(miniPoly);

					tail1 = miniPoly->GetAnchor(miniPoly->GetAnchors());
				}
				else if(bCorresponding==3)
				{
					miniPoly->Reverse();
					sortList.AddHead(miniPoly);

					head1 = miniPoly->GetAnchor(1);
				}
				else if(bCorresponding==4)
				{
					sortList.AddHead(miniPoly);

					head1 = miniPoly->GetAnchor(1);
				}

				m_geomlist.RemoveAt(miniPosition);
			}
			else
			{
				break;
			}
		}

		if(sortList.GetCount()==1)
		{
			pPoly->DrawTag(g, viewinfo, pHint, bPivot);
			if(m_bDelete==true)
			{
				delete pPoly;
				pPoly = nullptr;
			}
		}
		else
		{
			if(bPivot==true)
			{
				double length = 0;
				POSITION pos1 = sortList.GetHeadPosition();
				while(pos1!=nullptr)
				{
					CPoly* poly = (CPoly*)sortList.GetNext(pos1);
					length += poly->GetLength(tolerence);
				}

				double left = length/2;
				POSITION pos2 = sortList.GetHeadPosition();
				while(pos2!=nullptr)
				{
					CPoly* poly = (CPoly*)sortList.GetNext(pos2);
					double len = poly->GetLength(tolerence);
					if(left<len)
					{
						unsigned int nAnchor;
						double t;
						poly->GetAandT(1, 0, left, nAnchor, t);

						CPoint docPoint = poly->GetPoint(nAnchor, t);
						Gdiplus::PointF cliPoint = viewinfo.DocToClient<Gdiplus::PointF>(docPoint);

						Gdiplus::RectF tagRect = pHint->DrawString(g, viewinfo, m_string, cliPoint, 0, HALIGN::HA_CENTER, VALIGN::VA_CENTER);
						d_oRectArray.Add(tagRect);
						break;
					}
					else
						left -= len;
				}
			}
			else
			{
				double length = 0;
				POSITION pos1 = sortList.GetHeadPosition();
				while(pos1!=nullptr)
				{
					CPoly* poly = (CPoly*)sortList.GetNext(pos1);
					length += poly->GetLength(tolerence);
				}

				float cliSize = pHint->m_fSize*viewinfo.m_sizeDPI.cx/72.f;
				CSize docSize = viewinfo.ClientToDoc(Gdiplus::SizeF(cliSize, cliSize));

				bool bHasEnglish = false;
				Gdiplus::REAL labelLength = 0;
				CObArray labelarray;
				CText::AnalyzeString(m_string, labelarray);
				int nWords = labelarray.GetCount();
				for(int index = 0; index<nWords; index++)
				{
					CText::CWord* lable = (CText::CWord*)labelarray.GetAt(index);
					if(lable->bCJK==true)
					{
						labelLength += docSize.cx*lable->str.GetLength();
					}
					else
					{
						labelLength += (docSize.cx/2)*lable->str.GetLength();
						bHasEnglish = true;
					}
				}
				length -= labelLength;

				long step = length/(nWords+1);
				if(step<docSize.cx)
				{
					for(int index = 0; index<labelarray.GetCount(); index++)
					{
						CWord* label = (CWord*)labelarray.GetAt(index);
						delete label;
					}
					labelarray.RemoveAll();
				}
				else
				{
					const Gdiplus::StringFormat* pStringFormat = Gdiplus::StringFormat::GenericTypographic();

					CPoly* headPoly = (CPoly*)sortList.GetHead();
					CPoly* tailPoly = (CPoly*)sortList.GetTail();
					const CPoint& point11 = headPoly->GetAnchor(1);
					const CPoint& point12 = tailPoly->GetAnchor(tailPoly->m_nAnchors);

					long nAngle = (long)(atan2((double)(point12.y-point11.y), (double)(point12.x-point11.x))*180.0f/M_PI);
					bool bForwardCJK = (nAngle>=-135&&nAngle<=45) ? true : false;
					bool bForwardENG = (nAngle>=-90&&nAngle<=90) ? true : false;
					bool bForward = bHasEnglish==true ? bForwardCJK : bForwardENG;

					long curIndex = 0;
					CPoly* curPoly = headPoly;
					long curLeft = headPoly->GetLength(tolerence);
					unsigned int sa = 1;
					unsigned int H = 1;
					double st = 0;
					double T = 0;
					for(int wIndex = 0; wIndex<nWords; wIndex++)
					{
						sa = H;
						st = T;

						CWord* label = (CWord*)labelarray.GetAt(bForward==true ? wIndex : nWords-1-wIndex);
						if(label->bEnglish==false)
						{
							int len = step+(wIndex==0 ? docSize.cx/2 : docSize.cx);
							while(len>curLeft)
							{
								len = len-curLeft;
								curIndex++;
								POSITION pos = sortList.FindIndex(curIndex);
								if(pos!=nullptr)
								{
									curPoly = (CPoly*)sortList.GetAt(pos);
									curLeft = curPoly->GetLength(tolerence);
								}
								sa = 1;
								st = 0.0f;
							}

							curPoly->GetAandT(sa, st, len, H, T);

							CPoint docPoint = curPoly->GetPoint(H, T);
							Gdiplus::PointF cliPoint = viewinfo.DocToClient<Gdiplus::PointF>(docPoint);

							Gdiplus::RectF tagRect = pHint->DrawString(g, viewinfo, m_string, cliPoint, 0, HALIGN::HA_CENTER, VALIGN::VA_CENTER);
							d_oRectArray.Add(tagRect);

							curLeft -= len;
						}
						else
						{
							int len1 = step;
							while(len1>curLeft)
							{
								len1 = len1-curLeft;
								curIndex++;
								POSITION pos = sortList.FindIndex(curIndex);
								if(pos!=nullptr)
								{
									curPoly = (CPoly*)sortList.GetAt(pos);
									curLeft = curPoly->GetLength(tolerence);
									sa = 1;
									st = 0.0f;
								}
								else
								{
									break;
								}
							}
							curPoly->GetAandT(sa, st, len1, H, T);
							CPoint docPoint1 = curPoly->GetPoint(H, T);
							curLeft -= len1;

							sa = H;
							st = T;
							int len2 = label->length/2;
							while(len2>curLeft)
							{
								len2 = len2-curLeft;
								curIndex++;
								POSITION pos = sortList.FindIndex(curIndex);
								if(pos!=nullptr)
								{
									curPoly = (CPoly*)sortList.GetAt(pos);
									curLeft = curPoly->GetLength(tolerence);
									sa = 1;
									st = 0.0f;
								}
								else
								{
									break;
								}
							}
							curPoly->GetAandT(sa, st, len2, H, T);
							CPoint docPoint = curPoly->GetPoint(H, T);
							Gdiplus::Point cliPoint = viewinfo.DocToClient<Gdiplus::Point>(docPoint);
							curLeft -= len2;

							sa = H;
							st = T;
							int len3 = label->length/2;
							while(len3>curLeft)
							{
								len3 = len3-curLeft;
								curIndex++;
								POSITION pos = sortList.FindIndex(curIndex);
								if(pos!=nullptr)
								{
									curPoly = (CPoly*)sortList.GetAt(pos);
									curLeft = curPoly->GetLength(tolerence);
									sa = 1;
									st = 0.0f;
								}
								else
								{
									break;
								}
							}
							curPoly->GetAandT(sa, st, label->length/2, H, T);
							CPoint docPoint2 = curPoly->GetPoint(H, T);
							curLeft -= len3;

							double angle = LineAngle(docPoint1, docPoint2);
							//	CPoint point = bForwardENG == true ? point1 : point2;

							g.TranslateTransform(cliPoint.X, cliPoint.Y);

							angle = bForwardENG==true ? angle : angle-M_PI;
							if(angle!=0.0f)
							{
								int nAngle = (int)(angle*180/M_PI);
								g.RotateTransform(-nAngle);
								Gdiplus::PointF point(0, 0);
								pHint->DrawString(g, viewinfo, m_string, point, 0, HALIGN::HA_CENTER, VALIGN::VA_CENTER);
								g.RotateTransform(+nAngle);
							}
							else
							{
								Gdiplus::PointF point(0, 0);
								pHint->DrawString(g, viewinfo, m_string, point, 0, HALIGN::HA_CENTER, VALIGN::VA_CENTER);
							}

							g.TranslateTransform(-cliPoint.X, -cliPoint.Y);
						}
					}

					for(int index = 0; index<labelarray.GetCount(); index++)
					{
						CWord* label = (CWord*)labelarray.GetAt(index);
						delete label;
					}
					labelarray.RemoveAll();
				}
			}

			if(m_bDelete==true)
			{
				POSITION pos = sortList.GetHeadPosition();
				while(pos!=nullptr)
				{
					CPoly* poly = (CPoly*)sortList.GetNext(pos);
					delete poly;
				}
			}
			sortList.RemoveAll();
		}
	}

	m_geomlist.RemoveAll();
}

CGroupTag* CGroupTag::GetGroup(CObList& list, const CGeom* pGeom)
{
	if(pGeom==nullptr)
		return nullptr;
	if(pGeom->m_bType!=1)
		return nullptr;
	if(pGeom->m_bClosed==true)
		return nullptr;

	POSITION pos = list.GetHeadPosition();
	while(pos!=nullptr)
	{
		CGroupTag* pTag = (CGroupTag*)list.GetNext(pos);
		if(pTag->m_string==pGeom->m_strName)
		{
			CGeom* pGeom = (CGeom*)pTag->m_geomlist.GetHead();
			if(pGeom->m_bClosed==true)
				continue;
			return pTag;
		}
	}

	return nullptr;
}
