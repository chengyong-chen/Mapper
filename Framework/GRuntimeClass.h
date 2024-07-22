#pragma once

struct CRuntimeClass;

struct GRuntimeClass : CRuntimeClass
{
	GRuntimeClass(LPCSTR lpszClassName, int nObjectSize, UINT wSchema, CObject* (PASCAL* pfnCreateObject)(CObject&), CRuntimeClass* (PASCAL* pfnGetBaseClass)(), AFX_CLASSINIT* class_init)
	{
		m_lpszClassName = lpszClassName;
		m_nObjectSize = nObjectSize;
		m_wSchema = wSchema;
		m_pGWfnCreateObject = pfnCreateObject;
		m_pfnCreateObject = nullptr;
		m_pfnGetBaseClass = pfnGetBaseClass;
		m_pClassInit = class_init;

		m_pNextClass = nullptr;
	};

	CObject* (PASCAL* m_pGWfnCreateObject)(CObject& parameter); // nullptr => abstract class
	CObject* CreateObject(CObject& parameter)
	{
		ENSURE(this);

		if(m_pGWfnCreateObject==nullptr)
		{
			TRACE(traceAppMsg, 0,
				_T("Error: Trying to create object which is not ")
				_T("DECLARE_DYNCREATE \nor DECLARE_SERIAL: %hs.\n"),
				m_lpszClassName);
			return nullptr;
		}

		CObject* pObject = nullptr;
		TRY
		{
			pObject = (*m_pGWfnCreateObject)(parameter);
		}
			END_TRY

			return pObject;
	};
};
