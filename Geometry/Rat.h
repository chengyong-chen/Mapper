#pragma once

class CGeom;

class AFX_EXT_CLASS CRat : public CGeom
{ 
protected:
	DECLARE_SERIAL(CRat);
	CRat();
	
public:
	CRat(const CRect& rect);

public:
	CTypedPtrList<CObList, CPoly*> m_polylist;

public:	
	virtual void  Draw(Gdiplus::Graphics& g,CViewinfo& viewinfo,CLibrary& library,Context& context) ;

public:
	virtual      ~CRat();
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
	DWORD PackPC(CFile* pFile,BYTE*& bytes) override;
	DWORD ReleaseCE(CFile& file,BYTE& type) const override;
	void  ReleaseWeb(CFile& file);
	void  ReleaseWeb(boost::json::object json);
#ifdef _DEBUG
	void   AssertValid() const override;
#endif
};