#include "stdafx.h"
#include "Dib.h"
#include "Bmb.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CBmb::CBmb()
	: CDib()
{
	m_nHorzBlockNum = 0;
	m_nVertBlockNum = 0;

	m_pBmbInfo = nullptr;
}

CBmb::~CBmb()
{
	if(m_pBmbInfo!=nullptr)
	{
		for(long i = 0; i<(m_nHorzBlockNum*m_nVertBlockNum); i++)
		{
			if(m_pBmbInfo[i].DatePoint!=nullptr)
			{
				::GlobalFree(m_pBmbInfo[i].DatePoint);
				m_pBmbInfo[i].DatePoint = nullptr;
			}
		}
		delete[] m_pBmbInfo;
		m_pBmbInfo = nullptr;
	}

	if(m_file.m_hFile!=nullptr)
	{
		m_file.Close();
	}
}

bool CBmb::Open(LPCTSTR lpszPathName)
{
	if(CDib::Open(lpszPathName)==false)
		return false;

	CFileException fe;
	if(!m_file.Open(lpszPathName, CFile::modeRead|CFile::shareDenyWrite, &fe))
	{
		CString messge;
		messge.Format(_T("Unable to open %s!"), lpszPathName);
		AfxMessageBox(messge);
		return false;
	}

	m_file.Seek(20, CFile::begin);
	m_file.Read(&m_nHorzBlockNum, 4);
	m_file.Read(&m_nVertBlockNum, 4);
	m_pBmbInfo = new BMBINFO[m_nVertBlockNum*m_nHorzBlockNum];

	for(long i = 0; i<m_nVertBlockNum; i++)
		for(long j = 0; j<m_nHorzBlockNum; j++)
		{
			m_file.Read(&m_pBmbInfo[i*m_nHorzBlockNum+j].BeginPosition, 4);
			m_file.Read(&m_pBmbInfo[i*m_nHorzBlockNum+j].BlockLength, 4);
			m_file.Read(&m_pBmbInfo[i*m_nHorzBlockNum+j].BlockWidth, 4);
			m_file.Read(&m_pBmbInfo[i*m_nHorzBlockNum+j].BlockHeight, 4);
			m_pBmbInfo[i*m_nHorzBlockNum+j].Originx = j*320;
			m_pBmbInfo[i*m_nHorzBlockNum+j].Originy = (m_nVertBlockNum-1-i)*240;
			m_pBmbInfo[i*m_nHorzBlockNum+j].DatePoint = nullptr;
		}

	return true;
}

void CBmb::Draw(Gdiplus::Graphics& g, const CRect& rect, CRect drawRect, int nAlpha)
{
	drawRect.OffsetRect(-rect.left, -rect.bottom); //�ػ��������ͼ�����Ͻǵ��ĵ�����
	drawRect.top *= -1;
	drawRect.bottom *= -1;
	//	drawRect.NormalizeRect();//������ϵY�᷽���Ϊ����
	const float xZoom = (float)rect.Width()/m_Size.cx;
	const float yZoom = (float)rect.Height()/m_Size.cy;

	long topVertNum = drawRect.top/(long)(m_pBmbInfo[(m_nVertBlockNum-1)*m_nHorzBlockNum].BlockHeight*xZoom);
	long bottomVertNum = drawRect.bottom/(long)(m_pBmbInfo[(m_nVertBlockNum-1)*m_nHorzBlockNum].BlockHeight*yZoom);
	long leftHorzNum = drawRect.left/(long)(m_pBmbInfo[0].BlockWidth*xZoom);
	long rightHorzNum = drawRect.right/(long)(m_pBmbInfo[0].BlockWidth*yZoom);

	if(topVertNum<0)
		topVertNum = 0;
	if(bottomVertNum>m_nVertBlockNum-1)
		bottomVertNum = m_nVertBlockNum-1;
	if(leftHorzNum<0)
		leftHorzNum = 0;
	if(rightHorzNum>m_nHorzBlockNum-1)
		rightHorzNum = m_nHorzBlockNum-1;

	for(long i = topVertNum; i<=bottomVertNum; i++)
		for(long j = leftHorzNum; j<=rightHorzNum; j++)
		{
			const long n = (m_nVertBlockNum-i-1)*m_nHorzBlockNum+j;//
			if(m_pBmbInfo[n].DatePoint==nullptr)
			{
				ReadBlock(n);
			}

			if(m_pBmbInfo[n].DatePoint!=nullptr)
			{
				BITMAPINFO* DibInfo = (BITMAPINFO*)::GlobalLock((HGLOBAL)m_hInfo);
				BYTE* pBits = (BYTE*)::GlobalLock(m_pBmbInfo[n].DatePoint);

				DibInfo->bmiHeader.biWidth = m_pBmbInfo[n].BlockWidth;
				DibInfo->bmiHeader.biHeight = m_pBmbInfo[n].BlockHeight;

				Gdiplus::Bitmap* bitmap = ::new Gdiplus::Bitmap(DibInfo, pBits);
				g.ScaleTransform(1.0f, -1.0f);

				Gdiplus::ImageAttributes ImgAttr;
				ImgAttr.SetWrapMode(Gdiplus::WrapMode::WrapModeTileFlipXY);
				if(nAlpha>0)
				{
					const float fBlend = (100-nAlpha)/100.0;
					Gdiplus::ColorMatrix BitmapMatrix = {
						1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
						0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
						0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
						0.0f, 0.0f, 0.0f, fBlend, 0.0f,
						0.0f, 0.0f, 0.0f, 0.0f, 1.0f
					};

					ImgAttr.SetColorMatrix(&BitmapMatrix, Gdiplus::ColorMatrixFlagsDefault, Gdiplus::ColorAdjustTypeBitmap);
				}

				Gdiplus::Status status = g.DrawImage(bitmap, Gdiplus::RectF(m_pBmbInfo[n].Originx*xZoom, m_pBmbInfo[n].Originy*yZoom, m_pBmbInfo[n].BlockWidth*xZoom, m_pBmbInfo[n].BlockHeight*yZoom), 0, 0, m_pBmbInfo[n].BlockWidth, m_pBmbInfo[n].BlockHeight, Gdiplus::UnitPixel, &ImgAttr);

				::GlobalUnlock(m_pBmbInfo[n].DatePoint);
				::GlobalUnlock(m_hInfo);

				::delete bitmap;
				bitmap = nullptr;
				g.ScaleTransform(1.0f, -1.0f);
			}
		}
}

//void CBmb::ReleaseBlock(CWnd* pWnd,CRect rect)
//{
//	static long LtopVertNum,LleftHorzNum,LbottomVertNum,LrightHorzNum;
//
//	CRect Client;
//	pWnd->GetClientRect(Client);	
//	CRect docRect = viewinfo.ClientToDoc(Client);
//	float xZoom,yZoom;
//	xZoom=(float)(rect.Width() /m_Size.cx);	
//	yZoom=(float)(rect.Height()/m_Size.cy);
//
////	Client.IntersectRect(Client,CRect(0,0,10000000,10000000));
//	docRect.IntersectRect(docRect,rect);
//	docRect.OffsetRect(-rect.left,-rect.bottom);
//
//	long  topVertNum,   leftHorzNum,
//		  bottomVertNum,rightHorzNum;
//	
//	topVertNum    =-docRect.top   /(int)(m_pBmbInfo[m_nHorzBlockNum+1].BlockHeight*xZoom);
//	bottomVertNum =-docRect.bottom/(int)(m_pBmbInfo[m_nHorzBlockNum+1].BlockHeight*yZoom);
//	leftHorzNum   = docRect.left  /(int)(m_pBmbInfo[1].BlockWidth*xZoom);
//	rightHorzNum  = docRect.right /(int)(m_pBmbInfo[1].BlockWidth*yZoom);
//
//	if(topVertNum   >m_nVertBlockNum-1) topVertNum=m_nVertBlockNum-1;
//	if(bottomVertNum<0) bottomVertNum=0;
//	if(leftHorzNum  <0) leftHorzNum = 0;
//	if(rightHorzNum >m_nHorzBlockNum-1) rightHorzNum=m_nHorzBlockNum-1;
//	
//	topVertNum = m_nVertBlockNum-1 - topVertNum;
//	bottomVertNum = m_nVertBlockNum-1 - bottomVertNum;
//
//	for(long i=LtopVertNum;  i<=LbottomVertNum;i++)
//	for(long j=LleftHorzNum; j<=LrightHorzNum;j++)
//	{
//		if(i<0 || i>=m_nVertBlockNum || j<0 || j>=m_nHorzBlockNum)
//			continue;
//		if(i>=(topVertNum-1) && i<=(bottomVertNum+1) && j>=(leftHorzNum-1) && j<=(rightHorzNum+1))
//			continue;
//		int n=i*m_nHorzBlockNum+j; 
//		if(m_pBmbInfo[n].DatePoint != nullptr)
//		{
//			::GlobalFree(m_pBmbInfo[n].DatePoint);
//			m_pBmbInfo[n].DatePoint = nullptr;
//		}
//	}
//
//	LtopVertNum    = topVertNum-1;
//	LbottomVertNum = bottomVertNum+1;
//	LleftHorzNum   = leftHorzNum-1;
//	LrightHorzNum  = rightHorzNum+1;
//}

void CBmb::ReadBlock(long BlockNum)
{
	if(BlockNum<0||BlockNum>=m_nVertBlockNum*m_nHorzBlockNum)
		return;
	if(m_pBmbInfo[BlockNum].DatePoint!=nullptr)
		return;

	m_pBmbInfo[BlockNum].DatePoint = ::GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT, m_pBmbInfo[BlockNum].BlockLength);
	if(m_pBmbInfo[BlockNum].DatePoint!=nullptr)
	{
		const LPSTR Dibbuf = (LPSTR)::GlobalLock(m_pBmbInfo[BlockNum].DatePoint);
		if(m_file.m_hFile!=nullptr)
		{
			m_file.Seek(m_pBmbInfo[BlockNum].BeginPosition, CFile::begin);
			m_file.Read(Dibbuf, m_pBmbInfo[BlockNum].BlockLength);
		}
		::GlobalUnlock(m_pBmbInfo[BlockNum].DatePoint);
	}
	else
	{
		AfxGetApp()->m_pMainWnd->MessageBox(_T("Memory is not enogh"), nullptr, MB_OK);
	}
}

//bool CBmb::ReadOutBlockDate(CWnd* pWnd,CViewinfo& viewinfo,CDC* pDC,int IdleCount)
//{	
//	CRect Client;
//	long  topVertNum,   leftHorzNum,
//		  bottomVertNum,rightHorzNum;
//
//	if(pDC->IsPrinting() == FALSE)
//		return false;
//
//	pWnd->GetClientRect(Client);	
//	CRect docRect = viewinfo.ClientToDoc(Client);
//
//	topVertNum    = m_nVertBlockNum-1-docRect.top   /m_pBmbInfo[m_nHorzBlockNum+1].BlockHeight;
//	bottomVertNum = m_nVertBlockNum-1-docRect.bottom/m_pBmbInfo[m_nHorzBlockNum+1].BlockHeight;
//	leftHorzNum   = docRect.left  /m_pBmbInfo[1].BlockWidth;
//	rightHorzNum  = docRect.right /m_pBmbInfo[1].BlockWidth;
//
//	long i,j;
//	if(IdleCount>0 && IdleCount<=topVertNum-bottomVertNum+2)
//	{
//		j=leftHorzNum-1;
//		i=bottomVertNum+IdleCount-1;
//	}
//	else if(IdleCount>topVertNum-bottomVertNum+2 && IdleCount<=topVertNum-bottomVertNum+2+rightHorzNum-leftHorzNum+2)
//	{
//		j=leftHorzNum+IdleCount-(topVertNum-bottomVertNum+2)-1;
//		i=topVertNum+1;
//	}
//	else if(IdleCount>topVertNum-bottomVertNum+2+rightHorzNum-leftHorzNum+2  && IdleCount<=2*topVertNum-2*bottomVertNum+4+rightHorzNum-leftHorzNum+2)
//	{
//		j=rightHorzNum+1;
//		i=topVertNum-(IdleCount-(topVertNum-bottomVertNum+2+rightHorzNum-leftHorzNum+2)-1);
//	}
//	else if(IdleCount>2*topVertNum-2*bottomVertNum+4+rightHorzNum-leftHorzNum+2  && IdleCount<=2*topVertNum-2*bottomVertNum+4+2*rightHorzNum-2*leftHorzNum+4)
//	{
//		j=rightHorzNum-(IdleCount-(2*topVertNum-2*bottomVertNum+4+rightHorzNum-leftHorzNum+2)-1);
//		i=bottomVertNum-1;
//	}
//	else
//		return false;
//		
//	if(i<0  || i>=m_nVertBlockNum || j<0 || j>=m_nHorzBlockNum)
//		return TRUE;
//
//	int n=i*m_nHorzBlockNum+j;
//	if(m_pBmbInfo[n].DatePoint != nullptr)
//		return true;
//
//	ReadBlock(n);
//
//	if(IdleCount==2*topVertNum-2*bottomVertNum+4+2*rightHorzNum-2*leftHorzNum+4)
//		return false;
//
//	return true;
//}

CDib* CBmb::GetSubDib(const CRect& rect) const
{
	CDib* pDib = new CDib();
	pDib->Create(rect.Width(), rect.Height(), m_nBpp, m_bAlphaChannel);

	long topVertNum = rect.bottom/(long)(m_pBmbInfo[m_nHorzBlockNum+1].BlockHeight);
	long bottomVertNum = rect.top/(long)(m_pBmbInfo[m_nHorzBlockNum+1].BlockHeight);
	long leftHorzNum = rect.left/(long)(m_pBmbInfo[1].BlockWidth);
	long rightHorzNum = rect.right/(long)(m_pBmbInfo[1].BlockWidth);

	if(topVertNum>m_nVertBlockNum-1) topVertNum = m_nVertBlockNum-1;
	if(bottomVertNum<0) bottomVertNum = 0;
	if(leftHorzNum<0) leftHorzNum = 0;
	if(rightHorzNum>m_nHorzBlockNum-1) rightHorzNum = m_nHorzBlockNum-1;

	topVertNum = m_nVertBlockNum-1-topVertNum;
	bottomVertNum = m_nVertBlockNum-1-bottomVertNum;

	for(long i = topVertNum; i<=bottomVertNum; i++)
		for(long j = leftHorzNum; j<=rightHorzNum; j++)
		{
			const long n = i*m_nHorzBlockNum+j;
			if(m_pBmbInfo[n].DatePoint==nullptr)
			{
				//	ReadBlock(n);
			}

			if(m_pBmbInfo[n].DatePoint!=nullptr)
			{
				LPSTR Dibbuf = (LPSTR) ::GlobalLock(m_pBmbInfo[n].DatePoint);
				LPBITMAPINFO DibInfobuf = (LPBITMAPINFO)::GlobalLock(m_hInfo);

				::GlobalUnlock(m_pBmbInfo[n].DatePoint);
				::GlobalUnlock(m_hInfo);
			}
		}

	return pDib;
}

CDib* CBmb::Clone() const
{
	CBmb* pBmb = new CBmb;

	Copy(pBmb);

	return pBmb;
}

void CBmb::Copy(CBmb* denBmb) const
{
	CDib::Copy(denBmb);

	if(m_file.m_hFile!=nullptr)
	{
		const CString strFile = m_file.GetFilePath();
		CFileException fe;
		denBmb->m_file.Open(strFile, CFile::modeRead|CFile::shareDenyWrite, &fe);
	}

	denBmb->m_nHorzBlockNum = m_nHorzBlockNum;
	denBmb->m_nVertBlockNum = m_nVertBlockNum;
	if(m_pBmbInfo!=nullptr)
	{
		denBmb->m_pBmbInfo = new BMBINFO[m_nVertBlockNum*m_nHorzBlockNum];
		memcpy(denBmb->m_pBmbInfo, m_pBmbInfo, m_nVertBlockNum*m_nHorzBlockNum*sizeof(BMBINFO));
		/*	for(long i=0;i<m_nVertBlockNum;i++)
			for(long j=0;j<m_nHorzBlockNum;j++)
			{
				denBmb->m_pBmbInfo[i*m_nHorzBlockNum+j] = m_pBmbInfo[i*m_nHorzBlockNum+j];
			}*/
	}
}
