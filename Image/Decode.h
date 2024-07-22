#pragma once

#include "Dib.h"

class CDataSource;
class CDecode : public CDib
{
public:
	CDecode();
	~CDecode() override;
public:
	bool Open(LPCTSTR lpszPathName) override;
	virtual void MakeBmp(CDataSource* pDataSrc);
	virtual void DoDecode(RGBAPIXEL** ppPal, int* pDestBPP, CDataSource* pDataSrc);

protected:
	static BYTE ReadByte(CDataSource* pDataSrc);
};

