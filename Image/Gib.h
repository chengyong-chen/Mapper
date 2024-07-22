#pragma once

#include "Bmb.h"

class CGib : public CBmb
{
public:
	int GifImageWidth;
	int FackImageSize;
	CGib();

	~CGib() override;
private:
	void ReadBlock(long BlockNum) override;
	void ExportToBmp(CFile& file) override;

	int bitSperPixel;
	int dic_Last_Position;
	int dic_Size;
	int curByte;
	int curBit;
	int CLEAR_CODE;
	int END_CODE;
	int imageSize;
	LPSTR pBuffer;
	BYTE Bytecnt;
	BYTE Bits_of_Dic;
	BYTE Buffer[256];
	DICTIONARY Dic[4096];
	UINT tem;
	BYTE cgy_bits;
private:
	void Clear_bufgif();
	void Load_buf();
	int Get_code();
	void Init_dic();
	void Put_char(int Code);
	BYTE First_char(int Code) const;
	int Dic_is_full() const;
	void Insert_to_dic(NEW_STRING Ns);
	bool LzwExpand(int minbit);
public:
	CDib* Clone() const override;
	virtual void Copy(CGib* GibDen) const;
};
