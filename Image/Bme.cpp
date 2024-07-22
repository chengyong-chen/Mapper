#include "stdafx.h"
#include "Dib.h"
#include "Bme.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CBme::CBme()
	: CDib()
{
}

CBme::~CBme()
{
}

bool CBme::Open(LPCTSTR lpszPathName)
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

	m_bmfHeader.bfSize = (m_bmfHeader.bfSize^0X4352);
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

	BYTE prev = 0X82;
	BYTE* pByte = (BYTE*)Dibbuf;
	for(int index = 0; index<dwBitsSize; index++)
	{
		const BYTE prev1 = *pByte;
		*pByte = (*pByte)^prev;
		prev = prev1;
		pByte++;
	}

	::GlobalUnlock(m_hData);
	file.Close();
	return true;
}

void CBme::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	if(ar.IsStoring())
	{
		ar.Write(&m_bmfHeader, sizeof(BITMAPFILEHEADER));
		const LPSTR iBuf = (LPSTR)::GlobalLock(m_hInfo);
		const size_t iSize = GlobalSize(iBuf);
		//		ar << iSize;
		ar.Write(iBuf, iSize);
		::GlobalUnlock(m_hInfo);
		const LPSTR dBuf = (LPSTR)::GlobalLock(m_hData);
		const size_t dSize = GlobalSize(dBuf);
		//		ar << dSize;
		ar.Write(dBuf, dSize);

		::GlobalUnlock(m_hData);
	}
	else
	{
	}
}

CDib* CBme::Clone() const
{
	CBme* pBme = new CBme;

	Copy(pBme);

	return pBme;
}

void CBme::Copy(CBme* denBme) const
{
	CDib::Copy(denBme);

	denBme->m_bmfHeader = m_bmfHeader;
}
