#include "stdafx.h"
#include "Decode.h"

#include "Filesrc.h"
#include "except.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CDecode::CDecode()
	: CDib()
{
}

CDecode::~CDecode()
{
}

bool CDecode::Open(LPCTSTR lpszPathName)
{
	CFileSource* pFileSrc = nullptr;
	int err;

	char sz[256];
	/* sprintf(sz, "--- Decoding file %s. ---\n", lpszPathName);
	 Trace (1, sz);
 */
	try
	{
		//   pFileSrc = new CFileSource (pProgNot);
		pFileSrc = new CFileSource(nullptr);

		err = pFileSrc->Open(lpszPathName);
		if(err)
		{
			sprintf(sz, "Opening %s failed", lpszPathName);
			//      raiseError (err, sz);
		}

		MakeBmp(pFileSrc);
		pFileSrc->Close();
	}
	catch(CTextException*ex)
	{
		delete pFileSrc;
		throw;
	}
	catch(CException*ex)
	{
		ex->Delete();
	}

	// Clean up
	delete pFileSrc;
	return TRUE;
}

void CDecode::MakeBmp(CDataSource* pDataSrc)
{
	RGBAPIXEL* pPal = nullptr;

	int DestBPP = 0;

	try
	{
		DoDecode(&pPal, &DestBPP, pDataSrc);
		delete pPal;
	}
	catch(CTextException* ex)
	{
		delete pPal;
		throw; // Pass the error on.
	}
}

void CDecode::DoDecode(RGBAPIXEL** ppPal, int* pDestBPP, CDataSource* pDataSrc)
{
	ASSERT(FALSE);
}

BYTE CDecode::ReadByte(CDataSource* pDataSrc)
{
	return *(pDataSrc->ReadNBytes(1));
}
