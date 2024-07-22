#pragma once

#include "Decode.h"

class CGif : public CDecode
{
public:
	int FackImageSize;
	int GifImageWidth;
	CGif();
public:
	void DoDecode(RGBAPIXEL** ppPal, int* pDestBPP, CDataSource* pDataSrc) override;

private:
	bool m_bInterlaced;

	SHORT init_exp(SHORT size);
	SHORT get_next_code(CDataSource* pDataSrc);

	/* Static variables */
	SHORT curr_size; /* The current code size */
	SHORT clear; /* Value for a clear code */
	SHORT ending; /* Value for a ending code */
	SHORT newcodes; /* First available code */
	SHORT top_slot; /* Highest code for current size */
	SHORT slot; /* Last read code */

	/* The following static variables are used
	*for seperating out codes
	 */
	SHORT navail_bytes; /* # bytes left in block */
	SHORT nbits_left; /* # bits left in current byte */
	BYTE b1; /* Current byte */
	BYTE byte_buff[257]; /* Current block */
	LPBYTE pbytes; /* Pointer to next byte in block */

	int rownum;
	BYTE bypass;
	/* bad_code_count is incremented each time an out of range code is read.
	*When this value is non-zero after a decode, your GIF file is probably
	*corrupt in some way...
	 */

	short decode(CDataSource* pDataSrc);

	int out_line(LPBYTE pixels, int linelen);

public:
	CDib* Clone() const override;
	virtual void Copy(CGif* GifDen) const;
};

#pragma pack(push, my_gif, 1)

typedef struct tagGIFHEADER
{
	BYTE bySignature[3];
	BYTE byVersion[3];
} GIFHEADER, FAR* LPGIFHEADER;

typedef struct tagGIFSCRDESC
{
	WORD wWidth;
	WORD wDepth;

	struct globalflag
	{
		BYTE PalBits : 3;
		BYTE SortFlag : 1;
		BYTE ColorRes : 3;
		BYTE GlobalPal : 1;
	} GlobalFlag;

	BYTE byBackground;
	BYTE byAspect;
} GIFSCRDESC, FAR* LPGIFSCRDESC;

typedef struct tagGIFIMAGE
{
	WORD wLeft;
	WORD wTop;
	WORD wWidth;
	WORD wDepth;

	struct localflag
	{
		BYTE PalBits : 3;
		BYTE Reserved : 2;
		BYTE SortFlag : 1;
		BYTE Interlace : 1;
		BYTE LocalPal : 1;
	} LocalFlag;
} GIFIMAGE, FAR* LPGIFIMAGE;

typedef struct tagGIFCONTROL
{
	BYTE byBlockSize;

	struct flag
	{
		BYTE Transparency : 1;
		BYTE UserInput : 1;
		BYTE DisposalMethod : 3;
		BYTE Reserved : 3;
	} Flag;

	WORD wDelayTime;
	BYTE byTransparencyIndex;
	BYTE byTerminator;
} GIFCONTROL, FAR* LPGIFCONTROL;

typedef struct tagGIFPLAINTEXT
{
	BYTE byBlockSize;
	WORD wTextGridLeft;
	WORD wTextGridTop;
	WORD wTextGridWidth;
	WORD wTextGridDepth;
	BYTE byCharCellWidth;
	BYTE byCharCellDepth;
	BYTE byForeColorIndex;
	BYTE byBackColorIndex;
} GIFPLAINTEXT, FAR* LPGIFPLAINTEXT;

typedef struct tagGIFAPPLICATION
{
	BYTE byBlockSize;
	BYTE byIdentifier[8];
	BYTE byAuthentication[3];
} GIFAPPLICATION, FAR* LPGIFAPPLICATION;

typedef struct tagGIFD_VAR
{
	LPSTR lpDataBuff;
	LPSTR lpBgnBuff;
	LPSTR lpEndBuff;
	DWORD dwDataLen;
	WORD wMemLen;
	WORD wWidth;
	WORD wDepth;
	WORD wLineBytes;
	WORD wBits;
	BOOL bEOF;
	BOOL bInterlace;
} GIFD_VAR, FAR* LPGIFD_VAR;

typedef struct tagGIFC_VAR
{
	LPSTR lpDataBuff;
	LPSTR lpEndBuff;
	DWORD dwTempCode;
	WORD wWidth;
	WORD wDepth;
	WORD wLineBytes;
	WORD wBits;
	WORD wByteCnt;
	WORD wBlockNdx;
	BYTE byLeftBits;
} GIFC_VAR, FAR* LPGIFC_VAR;

#define MAX_TABLE_SIZE  4096

#pragma pack(pop, my_gif)
