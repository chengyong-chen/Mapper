#include "stdafx.h"
#include "Dib.h"
#include "Bmp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CBmp::CBmp() : CDib()
{
}

CBmp::~CBmp()
{
}

bool CBmp::Open(LPCTSTR lpszPathName)
{
	CFile file;
	CFileException fe;
	DWORD dwBitsSize;

	if(CDib::Open(lpszPathName)==false)
	{
		CString messge;
		messge.Format(_T("Unable to open %s!"), lpszPathName);
		AfxMessageBox(messge);
		return false;
	}

	if(!file.Open(lpszPathName, CFile::modeRead|CFile::shareDenyWrite, &fe))
	{
		CString messge;
		messge.Format(_T("Unable to open %s!"), lpszPathName);
		AfxMessageBox(messge);
		return false;
	}

	file.Read(&m_bmfHeader, sizeof(BITMAPFILEHEADER));
	BITMAPINFOHEADER infoHeader;
	file.Read(&infoHeader, sizeof(BITMAPINFOHEADER));

	file.Seek(sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+infoHeader.biClrUsed*sizeof(RGBQUAD), CFile::begin);
	dwBitsSize = file.GetLength()-sizeof(BITMAPFILEHEADER)-sizeof(BITMAPINFOHEADER)-infoHeader.biClrUsed*sizeof(RGBQUAD);

	m_hData = ::GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT, dwBitsSize);
	if(m_hData==nullptr)
	{
		file.Close();
		AfxMessageBox(_T("Error with allocating memory!"));
		return false;
	}
	const LPSTR Dibbuf = (LPSTR)::GlobalLock(m_hData);

	if(file.Read(Dibbuf, dwBitsSize)!=dwBitsSize)
	{
		::GlobalUnlock(m_hData);
		::GlobalFree(m_hData);
		m_hData = nullptr;
		file.Close();

		AfxMessageBox(_T("Unable to read data from a bmp file!"));
		return false;
	}

	::GlobalUnlock(m_hData);
	file.Close();
	return true;
}

void CBmp::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	if(ar.IsStoring())
	{
		ar.Write(&m_bmfHeader, sizeof(BITMAPFILEHEADER));
		const LPSTR iBuf = (LPSTR)::GlobalLock(m_hInfo);
		const size_t iSize = GlobalSize(iBuf);
		ar<<iSize;
		ar.Write(iBuf, iSize);
		::GlobalUnlock(m_hInfo);
		const LPSTR dBuf = (LPSTR)::GlobalLock(m_hData);
		const size_t dSize = GlobalSize(dBuf);
		ar<<dSize;
		ar.Write(dBuf, dSize);

		::GlobalUnlock(m_hData);
	}
	else
	{
	}
}

CDib* CBmp::Clone() const
{
	CBmp* pBmp = new CBmp;

	Copy(pBmp);

	return pBmp;
}

void CBmp::Copy(CBmp* denBmp) const
{
	CDib::Copy(denBmp);

	denBmp->m_bmfHeader = m_bmfHeader;
}
