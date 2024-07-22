#pragma once

#include "Decode.h"
struct tiff;
typedef tiff TIFF;
class CDib;
class CDataSource;
class CTiff : public CDecode
{
public:
	CTiff();
	~CTiff() override;
public:
	void DoDecode(RGBAPIXEL** ppPal, int* pDestBPP, CDataSource* pDataSrc) override;

private:
	void doHiColor(TIFF* tif, int* pDestBPP);
	void doLoColor(TIFF* tif, int* pDestBPP);
	
public:
	CDib* Clone() const override;
	virtual void Copy(CTiff* TiffDen) const;

private:
	static int checkcmap(int n, unsigned short* r, unsigned short* g, unsigned short* b);
};
