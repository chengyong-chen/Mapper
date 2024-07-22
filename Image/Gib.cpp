#include "stdafx.h"
#include <limits.h>

#include "Dib.h"
#include "Bmb.h"
#include "Gib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CGib::CGib()
{
}

CGib::~CGib()
{
}

void CGib::ReadBlock(long BlockNum)
{
	m_pBmbInfo[BlockNum].DatePoint = ::GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT, m_pBmbInfo[BlockNum].BlockLength);
	if(m_pBmbInfo[BlockNum].DatePoint==nullptr)
	{
		AfxGetApp()->m_pMainWnd->MessageBox(_T("Memory is not enogh"), nullptr, MB_OK);
		AfxGetApp()->m_pMainWnd->PostMessage(WM_COMMAND, ID_APP_EXIT, 0);
	}
	GifImageWidth = m_pBmbInfo[BlockNum].BlockWidth;

	pBuffer = (LPSTR)::GlobalLock(m_pBmbInfo[BlockNum].DatePoint);
	const LPBITMAPINFO DibInfobuf = (LPBITMAPINFO)::GlobalLock(m_hInfo);

	m_file.Seek(m_pBmbInfo[BlockNum].BeginPosition, CFile::begin);
	LzwExpand(DibInfobuf->bmiHeader.biBitCount);

	::GlobalUnlock(m_pBmbInfo[BlockNum].DatePoint);
	::GlobalUnlock(m_hInfo);
}

void CGib::Clear_bufgif(void)
{
	cgy_bits = 0x00;
	Bytecnt = 0;
	curByte = 0;
	curBit = 1;
}

void CGib::Load_buf()
{
	curByte = 0;
	curBit = 1;
	m_file.Read(&Bytecnt, 1);
	m_file.Read(Buffer, Bytecnt);
}

int CGib::Get_code()
{
	if(Bytecnt==0)
	{
		Load_buf();
	}
	int Code = 0;
	BYTE req;
	req = Bits_of_Dic;
	while(req>0)
	{
		if(req>cgy_bits)
		{
			tem = tem|(((UINT)Buffer[curByte])<<cgy_bits);
			curByte++;
			Bytecnt--;
			if(Bytecnt==0)
			{
				Load_buf();
			}
			cgy_bits += 8;
		}
		else
		{
			Code = (WORD)tem&((1<<req)-1);
			tem = tem>>req;
			cgy_bits = cgy_bits-req;
			req = 0;
		}
	}

	return Code;
}

void CGib::Init_dic(void)
{
	Bits_of_Dic = bitSperPixel;
	dic_Size = 1<<Bits_of_Dic;
	for(int i = 0; i<=dic_Size+1; i++)
	{
		Dic[i].Prefix = EMPTY;
		Dic[i].Character = i;
	}
	CLEAR_CODE = dic_Size;
	END_CODE = dic_Size+1;
	dic_Last_Position = dic_Size+1;
	Bits_of_Dic += 1;
	dic_Size = 1<<Bits_of_Dic;
}

void CGib::Put_char(int Code)
{
	int Count = 0;
	char Buf[4096];
	int line4;

	if((line4 = GifImageWidth%4)!=0)
	{
		line4 = 4-line4;
	}

	Buf[Count++] = Dic[Code].Character;
	while(Dic[Code].Prefix!=EMPTY)
	{
		Code = Dic[Code].Prefix;
		Buf[Count++] = Dic[Code].Character;
	}

	for(int i = Count-1; i>=0; i--)
	{
		if((FackImageSize%GifImageWidth==0)&&(FackImageSize!=0))
		{
			for(int a = 0; a<line4; a++)
			{
				*pBuffer++ = 0x00;
				FackImageSize++;
			}
		}
		*pBuffer++ = Buf[i];
		FackImageSize++;
		imageSize++;
	}
}

BYTE CGib::First_char(int Code) const
{
	while(Dic[Code].Prefix!=EMPTY)
	{
		Code = Dic[Code].Prefix;
	}
	return Dic[Code].Character;
}

int CGib::Dic_is_full(void) const
{
	if((dic_Last_Position==dic_Size-1)&&(Bits_of_Dic!=12))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void CGib::Insert_to_dic(NEW_STRING Ns)
{
	dic_Last_Position++;
	Dic[dic_Last_Position].Prefix = Ns.Prefix;
	Dic[dic_Last_Position].Character = Ns.Character;
}

bool CGib::LzwExpand(int minbit)
{
	if(minbit==1)
	{
		bitSperPixel = 2;
	}
	else
	{
		bitSperPixel = minbit;
	}
	imageSize = 0;
	tem = 0;

	if(pBuffer==nullptr)
	{
		return FALSE;
	}
	Clear_bufgif();
	Bits_of_Dic = bitSperPixel+1;
	int Nc = Get_code();
	Init_dic();
	Nc = Get_code();
	Put_char(Nc);
	int Rm = Nc;
	Nc = Get_code();
	NEW_STRING Ns;
	while(Nc!=END_CODE)
	{
		if(Nc==CLEAR_CODE)
		{
			Init_dic();
			Nc = Get_code();
			Put_char(Nc);
			Rm = Nc;
		}
		else
		{
			Ns.Prefix = Rm;
			if(Nc<=dic_Last_Position)
			{
				Ns.Character = First_char(Nc);
			}
			else
			{
				Ns.Character = First_char(Rm);
			}
			Insert_to_dic(Ns);
			Put_char(Nc);
			Rm = Nc;
			if(Dic_is_full())
			{
				Bits_of_Dic = Bits_of_Dic+1;
				dic_Size <<= 1;
			}
		}
		Nc = Get_code();
	}
	return TRUE;
}

void CGib::ExportToBmp(CFile& file)
{
	BITMAPFILEHEADER bmfHeader;

	bmfHeader.bfType = 0X00004d42;
	bmfHeader.bfSize = m_Size.cx*m_Size.cy+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD);
	bmfHeader.bfReserved1 = 0;
	bmfHeader.bfReserved2 = 0;
	bmfHeader.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD);

	file.Write(&bmfHeader, sizeof(BITMAPFILEHEADER));

	LPBITMAPINFO DibInfobuf;
	DibInfobuf = (LPBITMAPINFO) ::GlobalLock(m_hInfo);
	DibInfobuf->bmiHeader.biWidth = m_Size.cx;
	DibInfobuf->bmiHeader.biHeight = m_Size.cy;
	file.Write(DibInfobuf, sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD));
	::GlobalUnlock(m_hInfo);

	HGLOBAL buf;
	LPSTR line;
	buf = ::GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT, m_Size.cx);
	line = (LPSTR)::GlobalLock(buf);

	int i = 0;
	int j = 0;
	for(i = 0; i<m_nVertBlockNum; i++)
	{
		for(j = 0; j<m_nHorzBlockNum; j++)
		{
			const int n = i*m_nHorzBlockNum+j;
			if(m_pBmbInfo[n].DatePoint==nullptr)
				ReadBlock(n);
		}

		for(j = 0; j<m_pBmbInfo[i*m_nHorzBlockNum].BlockHeight; j++)
		{
			int k;
			for(k = 0; k<m_nHorzBlockNum; k++)
			{
				const DWORD length = m_pBmbInfo[i*m_nHorzBlockNum+k].BlockWidth;
				const DWORD position = m_pBmbInfo[i*m_nHorzBlockNum+k].BlockWidth*j;
				const LPSTR Dibdatabuf = (LPSTR)::GlobalLock(m_pBmbInfo[i*m_nHorzBlockNum+k].DatePoint);

				memcpy((LPSTR)line, (Dibdatabuf+position), length);
				file.Write(line, length);
				::GlobalUnlock(m_pBmbInfo[i*m_nHorzBlockNum+k].DatePoint);
			}
		}
	}

	::GlobalUnlock(buf);
	::GlobalFree(buf);
}

CDib* CGib::Clone() const
{
	CGib* pGib = new CGib;

	Copy(pGib);

	return pGib;
}

void CGib::Copy(CGib* denGib) const
{
	CBmb::Copy(denGib);
}
