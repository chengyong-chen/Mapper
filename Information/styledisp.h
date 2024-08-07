#pragma once

// Machine generated IDispatch wrapper class(es) created by Microsoft Visual C++

// NOTE: Do not modify the contents of this file.  If this class is regenerated by
//  Microsoft Visual C++, your modifications will be overwritten.

// Dispatch interfaces referenced by this interface
class COleFont;

/////////////////////////////////////////////////////////////////////////////
// CStyleDisp wrapper class

class CStyleDisp : public COleDispatchDriver
{
	public:
	CStyleDisp()
	{
	} // Calls COleDispatchDriver default constructor
	CStyleDisp(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch)
	{
	}

	CStyleDisp(const CStyleDisp& dispatchSrc) : COleDispatchDriver(dispatchSrc)
	{
	}

	// Attributes
	public:
	unsigned long GetForeColor() const;
	void SetForeColor(unsigned long);
	unsigned long GetBackColor() const;
	void SetBackColor(unsigned long);
	COleFont GetFont() const;
	void SetFont(LPDISPATCH);
	long GetAlignment() const;
	void SetAlignment(long);
	long GetVerticalAlignment() const;
	void SetVerticalAlignment(long);
	BOOL GetLocked() const;
	void SetLocked(BOOL);
	CString GetName() const;
	VARIANT GetParent() const;
	void SetParent(const VARIANT&);
	VARIANT GetValue() const;
	void SetValue(const VARIANT&);
	BOOL GetWrapText() const;
	void SetWrapText(BOOL);
	BOOL GetTransparentForegroundPicture() const;
	void SetTransparentForegroundPicture(BOOL);
	VARIANT GetForegroundPicture() const;
	void SetForegroundPicture(const VARIANT&);
	VARIANT GetBackgroundPicture() const;
	void SetBackgroundPicture(const VARIANT&);
	long GetForegroundPicturePosition() const;
	void SetForegroundPicturePosition(long);
	long GetBackgroundPictureDrawMode() const;
	void SetBackgroundPictureDrawMode(long);
	long GetBorderAppearance() const;
	void SetBorderAppearance(long);
	short GetBorderSize() const;
	void SetBorderSize(short);
	BOOL GetEllipsisText() const;
	void SetEllipsisText(BOOL);
	unsigned long GetBorderColor() const;
	void SetBorderColor(unsigned long);
	short GetBorderType() const;
	void SetBorderType(short);

	// Operations
	public:
	void Reset();
};
