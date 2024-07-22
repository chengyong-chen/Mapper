#pragma once

namespace monochrome
{
	inline BYTE bit1[8] =
	{
		0B10000000,
		0B01000000,
		0B00100000,
		0B00010000,
		0B00001000,
		0B00000100,
		0B00000010,
		0B00000001
	};
	inline BYTE getAt(const BYTE* pByte, const int& pos)
	{
		int offset = pos/8;
		int bitpos = pos%8;
		return (pByte[offset]&bit1[bitpos])>>(7-bitpos);
	}
	inline void setAt(BYTE* pByte, const int& pos)
	{
		int offset = pos/8;
		int bitpos = pos%8;
		pByte[offset] |= bit1[bitpos];
	}

	inline int percent1(const HBITMAP& hBitmap1, int x, int y, const HBITMAP& hBitmap2)
	{
		BITMAP bitmap1;
		::GetObject(hBitmap1, sizeof(BITMAP), &bitmap1);
		BITMAP bitmap2;
		::GetObject(hBitmap2, sizeof(BITMAP), &bitmap2);

		Gdiplus::Rect recta(0, 0, bitmap1.bmWidth-1, bitmap1.bmHeight-1);
		Gdiplus::Rect rectb(0, 0, bitmap2.bmWidth-1, bitmap2.bmHeight-1);
		Gdiplus::Rect rectc(x, y, bitmap2.bmWidth-1, bitmap2.bmHeight-1);
		Gdiplus::Rect rect1 = rectc;
		rect1.Intersect(recta);
		if(rect1.IsEmptyArea())
			return 0;

		Gdiplus::Rect rect2 = rect1;
		rect2.Offset(-x, -y);

		int startRow1 = bitmap1.bmHeight-rect1.GetBottom()-1;
		int endRow1 = bitmap1.bmHeight-rect1.GetTop()-1;
		int startCol1 = rect1.GetLeft();
		int endCol1 = rect1.GetRight();

		int startRow2 = bitmap2.bmHeight-rect2.GetBottom()-1;
		int endRow2 = bitmap2.bmHeight-rect2.GetTop()-1;
		int startCol2 = rect2.GetLeft();
		int endCol2 = rect2.GetRight();
		int count = 0;
		for(int row = 0; row<rect2.Height; row++)
		{
			int row1 = row+startRow1;
			int row2 = row+startRow2;
			BYTE* rowpointer1 = (BYTE*)(bitmap1.bmBits)+row1*bitmap1.bmWidthBytes;
			BYTE* rowpointer2 = (BYTE*)(bitmap2.bmBits)+row2*bitmap2.bmWidthBytes;
			for(int col = 0; col<rect2.Width; col++)
			{
				int col1 = col+startCol1;
				int col2 = col+startCol2;
				if(getAt(rowpointer2, col2)==0X01&&getAt(rowpointer1, col1)==0X01)
				{
					count++;
				}
			}
		}
		return floor((count*100)/(rect2.Width*rect2.Height));
	}
	inline int count1(const BITMAP& bitmap, const int& x, const int& y, const int& cx, const int& cy)
	{
		int ones = 0;
		for(int row = max(0, bitmap.bmHeight-(y+cy)); row<bitmap.bmHeight-y; row++)//from bottom to top
		{
			BYTE* pointer = (BYTE*)(bitmap.bmBits)+row*bitmap.bmWidthBytes;
			for(int col = x; col<x+cx; col++)
			{
				int offset = col/8;
				int bitpos = col%8;
				if(pointer[offset]&bit1[bitpos]==bit1[bitpos])
					ones++;
			}
		}
		return ones;
	}
	inline int both1(BYTE* pData1, const int& width1, const int& height1, BYTE* pData2, const int& width2, const int& height2, const int& x, const int& y)
	{
		return 1;
	}
	inline void SaveHBITMAPToFile(HBITMAP hBitmap, LPCTSTR lpszFileName)
	{
		BITMAP bmp;
		GetObject(hBitmap, sizeof(BITMAP), &bmp);
		WORD clrBits = (WORD)(bmp.bmPlanes*bmp.bmBitsPixel);
		if(clrBits==1)
			clrBits = 1;
		else if(clrBits<=4)
			clrBits = 4;
		else if(clrBits<=8)
			clrBits = 8;
		else if(clrBits<=16)
			clrBits = 16;
		else if(clrBits<=24)
			clrBits = 24;
		else
			clrBits = 32;

		DWORD dwPallete = sizeof(RGBQUAD)*(clrBits<24 ? (1<<clrBits) : 0);
		PBITMAPINFO pbmi = (PBITMAPINFO)LocalAlloc(GMEM_FIXED, sizeof(BITMAPINFOHEADER)+dwPallete);
		pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER)+dwPallete;
		pbmi->bmiHeader.biWidth = bmp.bmWidth;
		pbmi->bmiHeader.biHeight = bmp.bmHeight;
		pbmi->bmiHeader.biPlanes = 1;
		pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;
		pbmi->bmiHeader.biCompression = BI_RGB;
		pbmi->bmiHeader.biSizeImage = 0; // can be 0 for BI_RGB bitmaps
		pbmi->bmiHeader.biXPelsPerMeter = 0; // ignored
		pbmi->bmiHeader.biYPelsPerMeter = 0; // ignored
		pbmi->bmiHeader.biClrUsed = clrBits<24 ? (1<<clrBits) : 0; // ignored
		pbmi->bmiHeader.biClrImportant = 0; // ignored

		DWORD dwBmpSize = ((bmp.bmWidth*bmp.bmBitsPixel+31)/32)*4*bmp.bmHeight;
		HGLOBAL lpbitmap = GlobalAlloc(GMEM_FIXED, dwBmpSize);
		LPBYTE lpbi = (LPBYTE)GlobalLock(lpbitmap);

		HDC hDC = GetDC(nullptr);
		HDC hMemDC = CreateCompatibleDC(hDC);
		ReleaseDC(nullptr, hDC);
		HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

		GetDIBits(hMemDC, hBitmap, 0, bmp.bmHeight, lpbi, (BITMAPINFO*)pbmi, DIB_RGB_COLORS);
		SelectObject(hMemDC, hOldBitmap);
		DeleteDC(hMemDC);

		BITMAPFILEHEADER fileHeader;
		fileHeader.bfReserved1 = 0;
		fileHeader.bfReserved2 = 0;
		fileHeader.bfType = 'MB';
		fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+dwPallete;
		fileHeader.bfSize = fileHeader.bfOffBits+dwBmpSize;

		HANDLE fh = CreateFile(lpszFileName, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		if(fh!=INVALID_HANDLE_VALUE)
		{
			DWORD dwBytesWritten;
			WriteFile(fh, &fileHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, nullptr);
			WriteFile(fh, pbmi, sizeof(BITMAPINFOHEADER)+dwPallete, &dwBytesWritten, nullptr);
			WriteFile(fh, lpbi, dwBmpSize, &dwBytesWritten, nullptr);

			CloseHandle(fh);
		}
		::LocalFree (pbmi);
		GlobalUnlock(lpbitmap);
		GlobalFree(lpbitmap);
	}
	inline HBITMAP Transform(const HBITMAP& hBitmap, const Gdiplus::Matrix& matrix)
	{
		BITMAP bitmap;
		::GetObject(hBitmap, sizeof(bitmap), &bitmap);
		Gdiplus::Point points[]{
			{0, 0},
			{bitmap.bmWidth, 0},
			{bitmap.bmWidth, bitmap.bmHeight},
			{0, bitmap.bmHeight}
		};
		matrix.TransformPoints(points, 4);

		HDC hdc1 = CreateCompatibleDC(nullptr);
		HDC hdc2 = CreateCompatibleDC(nullptr);

		int minx = min(points[0].X, min(points[1].X, min(points[2].X, points[3].X)));
		int miny = min(points[0].Y, min(points[1].Y, min(points[2].Y, points[3].Y)));
		int maxx = max(points[0].X, max(points[1].X, max(points[2].X, points[3].X)));
		int maxy = max(points[0].Y, max(points[1].Y, max(points[2].Y, points[3].Y)));

		// Create a bitmap to hold the result
		HBITMAP hResult = ::CreateCompatibleBitmap(nullptr, maxx-minx, maxy-miny);

		HBITMAP oldBmp1 = (HBITMAP)SelectObject(hdc1, hBitmap);
		HBITMAP oldBmp2 = (HBITMAP)SelectObject(hdc2, hResult);

		//Draw the background color before we change mapping mode
	//      HBRUSH hbrBack = CreateSolidBrush(clrBack);
	//      HBRUSH hbrOld = (HBRUSH)SelectObject(hdc2, hbrBack);
	//      PatBlt(hdc2, 0, 0, w, h, PATCOPY);
	 //     ::DeleteObject(SelectObject(hdc2, hbrOld));

		  // We will use world transform to rotate the bitmap
		SetGraphicsMode(hdc2, GM_ADVANCED);

		Gdiplus::REAL m[6];
		matrix.GetElements(m);
		XFORM xform;
		xform.eM11 = m[0];
		xform.eM12 = m[1];
		xform.eM21 = m[2];
		xform.eM22 = m[3];
		xform.eDx = (float)-minx;
		xform.eDy = (float)-miny;

		SetWorldTransform(hdc2, &xform);

		// Now do the actual rotating - a pixel at a time
		BitBlt(hdc2, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdc1, 0, 0, SRCCOPY);

		// Restore DCs
		::SelectObject(hdc1, oldBmp1);
		::SelectObject(hdc2, oldBmp2);

		return hResult;
	}

	inline Gdiplus::Bitmap* Transform(Gdiplus::Bitmap* gBitmap, const Gdiplus::Matrix& matrix)
	{
		if(gBitmap==nullptr)
			return nullptr;

		Gdiplus::Graphics g1(gBitmap);
		g1.SetTransform(&matrix);

		// Create a new bitmap to hold the rotated image
		Gdiplus::Bitmap* rotated = new Gdiplus::Bitmap(gBitmap->GetWidth(), gBitmap->GetHeight(), &g1);

		// Create a Graphics object from the rotated bitmap
		Gdiplus::Graphics g2(rotated);

		// Draw the rotated image onto the new bitmap
		g2.DrawImage(gBitmap, 0, 0);
		return rotated;
	}
	inline void SaveHBITMAPToFile(Gdiplus::Bitmap* gBitmap, LPCTSTR lpszFileName)
	{
		const CLSID pngEncoderClsId = {0x557cf406, 0x1a04, 0x11d3,{ 0x9a,0x73,0x00,0x00,0xf8,0x1e,0xf3,0x2e }};
		gBitmap->Save(lpszFileName, &pngEncoderClsId, nullptr);

		//	gBitmap->Save(lpszFileName, &Gdiplus::ImageFormatBMP);
	}
	inline int zeroRows(const BYTE* pBytes, const int stride, const int cx, const int cy, const bool direction)
	{
		int rows = 0;
		int bytes = floor(cx/8)+(cx%8==0 ? 0 : 1);
		int start = direction ? 0 : cy-1;
		BYTE* pointer = (BYTE*)pBytes+start*stride;
		do
		{
			bool allzeros = true;
			for(int index = 0; index<bytes; index++)
			{
				if(pointer[index]!=0X00)
				{
					allzeros = false;
					break;
				}
			}

			if(allzeros)
			{
				rows++;
				pointer += (direction ? 1 : -1)*stride;
			}
			else
				break;
		} while(true);
		return rows;
	}
	inline int zeroRows(unsigned int* pPixels, const int cx, const int cy, const bool direction)
	{
		int rows = 0;
		unsigned int* pRowPixels = pPixels+(direction ? 0 : (cy-1)*cx);
		do
		{
			bool allzeros = true;
			for(int col = 0; col<cx; col++)
			{
				if(pRowPixels[col]!=0X00000000)
				{
					allzeros = false;
					break;
				}
			}
			if(allzeros)
			{
				rows++;
				pRowPixels += (direction ? 1 : -1)*cx;
			}
			else
				break;

		} while(rows<cy);

		return rows;
	}
	inline int zeroCols(const BYTE* pBytes, const int stride, const int cx, const int cy, const bool direction)
	{
		int cols = 0;
		int start = direction ? 0 : cx-1;
		do {
			int offset = floor(start/8);
			int bitpos = start%8;
			bool allzeros = true;
			for(int index = 0; index<cy; index++)
			{
				BYTE* pointer2 = (BYTE*)pBytes+index*stride;
				if((pointer2[offset]&bit1[bitpos])!=0X00)
				{
					allzeros = false;
					break;
				}
			}
			if(allzeros)
			{
				cols++;
				start += (direction ? 1 : -1);
			}
			else
				break;
		} while(true);
		return cols;
	}
	inline int zeroCols(unsigned int* pPixels, const int cx, const int cy, const bool direction)
	{
		int cols = 0;
		int offset = direction ? 0 : cx-1;
		do {
			bool allzeros = true;
			for(int row = 0; row<cy; row++)
			{
				if(pPixels[row*cx+offset]!=0X00000000)
				{
					allzeros = false;
					break;
				}
			}
			if(allzeros)
			{
				cols++;
				offset += (direction ? 1 : -1);
			}
			else
				break;
		} while(true);
		return cols;
	}
	inline HBITMAP trim(const HBITMAP& hBitmap, int& left, int& top, int& right, int& bottom)
	{
		BITMAP bitmap;
		::GetObject(hBitmap, sizeof(BITMAP), &bitmap);

		//bitmap.bmBits order is from bottom to top
		left = zeroCols((BYTE*)bitmap.bmBits, bitmap.bmWidthBytes, bitmap.bmWidth, bitmap.bmHeight, true);
		bottom = zeroRows((BYTE*)bitmap.bmBits, bitmap.bmWidthBytes, bitmap.bmWidth, bitmap.bmHeight, true);
		right = zeroCols((BYTE*)bitmap.bmBits, bitmap.bmWidthBytes, bitmap.bmWidth, bitmap.bmHeight, false);
		top = zeroRows((BYTE*)bitmap.bmBits, bitmap.bmWidthBytes, bitmap.bmWidth, bitmap.bmHeight, false);
		int newWidth = max(0, bitmap.bmWidth-left-right);
		int newHeight = max(0, bitmap.bmHeight-top-bottom);
		if(newWidth==0||newHeight==0)
			return nullptr;
		else
		{
			HDC hdc1 = CreateCompatibleDC(nullptr);
			HBITMAP hOld1 = (HBITMAP)::SelectObject(hdc1, hBitmap);

			HDC hdc2 = ::CreateCompatibleDC(nullptr);
			BITMAPINFO* pbmi = (BITMAPINFO*)malloc(sizeof(BITMAPINFOHEADER)+2*sizeof(RGBQUAD));
			pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			pbmi->bmiHeader.biWidth = newWidth;
			pbmi->bmiHeader.biHeight = newHeight;
			pbmi->bmiHeader.biPlanes = 1;
			pbmi->bmiHeader.biBitCount = 1;
			pbmi->bmiHeader.biCompression = BI_RGB;
			pbmi->bmiHeader.biClrUsed = 2;
			pbmi->bmiColors[0].rgbBlue = 0x00;
			pbmi->bmiColors[0].rgbGreen = 0x00;
			pbmi->bmiColors[0].rgbRed = 0x00;
			pbmi->bmiColors[1].rgbBlue = 0xFF;
			pbmi->bmiColors[1].rgbGreen = 0xFF;
			pbmi->bmiColors[1].rgbRed = 0xFF;
			HBITMAP hResult = ::CreateDIBSection(hdc2, pbmi, 0, nullptr, nullptr, 0);
			free(pbmi);

			HBITMAP hOld2 = (HBITMAP)::SelectObject(hdc2, hResult);

			BitBlt(hdc2, 0, 0, newWidth, newHeight, hdc1, left, top, SRCCOPY);

			::SelectObject(hdc2, hOld2);
			::SelectObject(hdc2, hOld1);
			::DeleteDC(hdc1);
			::DeleteDC(hdc2);

			return hResult;
		}
	}
	inline Gdiplus::Bitmap* trim(Gdiplus::Bitmap* gBitmap, int& left, int& top, int& right, int& bottom)
	{
		int cx = gBitmap->GetWidth();
		int cy = gBitmap->GetHeight();
		Gdiplus::Rect rect(0, 0, cx, cy);
		Gdiplus::BitmapData bitmapData;
		if(gBitmap->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bitmapData)==Gdiplus::Ok)
		{
			unsigned int* pPixels = reinterpret_cast<unsigned int*>(bitmapData.Scan0);

			left = zeroCols(pPixels, cx, cy, true);
			top = zeroRows(pPixels, cx, cy, true);
			right = zeroCols(pPixels, cx, cy, false);
			bottom = zeroRows(pPixels, cx, cy, false);
			gBitmap->UnlockBits(&bitmapData);

			int newWidth = max(0, cx-left-right);
			int newHeight = max(0, cy-top-bottom);

			Gdiplus::Bitmap* pResult = new Gdiplus::Bitmap(newWidth, newHeight, PixelFormat32bppARGB);
			Gdiplus::Graphics g(pResult);
			g.DrawImage(gBitmap, 0, 0, left, top, newWidth, newHeight, Gdiplus::UnitPixel);
			return pResult;
		}
		else
			return nullptr;
	}
	inline HBITMAP convert(Gdiplus::Bitmap* gBitmap)
	{
		int width = gBitmap->GetWidth();
		int height = gBitmap->GetHeight();
		Gdiplus::Rect rect(0, 0, width, height);
		Gdiplus::BitmapData bitmapData;
		if(gBitmap->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bitmapData)==Gdiplus::Ok)
		{
			unsigned int* pPixels = reinterpret_cast<unsigned int*>(bitmapData.Scan0);

			HDC hdc = ::CreateCompatibleDC(nullptr);
			BITMAPINFO* pbmi = (BITMAPINFO*)malloc(sizeof(BITMAPINFOHEADER)+2*sizeof(RGBQUAD));
			pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			pbmi->bmiHeader.biWidth = width;
			pbmi->bmiHeader.biHeight = height;
			pbmi->bmiHeader.biPlanes = 1;
			pbmi->bmiHeader.biBitCount = 1;
			pbmi->bmiHeader.biCompression = BI_RGB;
			pbmi->bmiHeader.biClrUsed = 2;
			pbmi->bmiColors[0].rgbBlue = 0x00;
			pbmi->bmiColors[0].rgbGreen = 0x00;
			pbmi->bmiColors[0].rgbRed = 0x00;
			pbmi->bmiColors[1].rgbBlue = 0xFF;
			pbmi->bmiColors[1].rgbGreen = 0xFF;
			pbmi->bmiColors[1].rgbRed = 0xFF;
			HBITMAP hBitmap = ::CreateDIBSection(hdc, pbmi, 0, nullptr, nullptr, 0);
			free(pbmi);
			if(hBitmap==nullptr)
			{
				DWORD code = GetLastError();
				OutputDebugString(_T("Failed to when calling CreateDIBSection"));
			}
			else
			{
				BITMAP bitmap;
				::GetObject(hBitmap, sizeof(BITMAP), &bitmap);
				for(int row = 0; row<height; row++)//in the order of bypes
				{
					BYTE* rowpointer1 = (BYTE*)(bitmap.bmBits)+row*bitmap.bmWidthBytes;
					for(int col = 0; col<width; col++)
					{
						if(pPixels[row*width+col]!=0X00000000)
						{
							setAt(rowpointer1, col);
						}
					}
				}
				::DeleteDC(hdc);
				gBitmap->UnlockBits(&bitmapData);
			}
			return hBitmap;
		}
		else
			return nullptr;
	}
}
