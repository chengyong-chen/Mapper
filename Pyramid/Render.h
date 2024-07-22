#pragma once

#include <afxtempl.h>

class CRender abstract : public CObject
{
public:
	CRender();

public:
		bool m_isInitialized;
		bool m_isSelectable;
//		RenderList m_ParentList;

		CString m_dbfPath;
		bool m_dbfIsInZip;
		
		CString Description;

protected:
		CString name;
		CString m_description;
//		Hashtable _metaData = new Hashtable();
		Vector3 position;
		Quaternion orientation;
		bool isOn;		
		BYTE m_opacity;
	//	RenderPriority m_renderPriority = RenderPriority.SurfaceImages;
	//	Form m_propertyBrowser;

//		Image m_thumbnailImage;
		CString m_iconImagePath;
//		Image m_iconImage;
//		World m_world;
		CString m_thumbnail;


	virtual ~CRender();
};


