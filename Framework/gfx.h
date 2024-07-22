#pragma once

#include "Afx.h"
#include "GRuntimeClass.h"

#define _RUNTIME_CLASS_T(class_name,T)		((CRuntimeClass*)(&class_name<T>::class##class_name))
#define _GWRUNTIME_CLASS(class_name)		((GRuntimeClass*)(&class_name::class##class_name))
#define _GWRUNTIME_CLASS_T(class_name,T)	((GRuntimeClass*)(&class_name<T>::class##class_name))

#ifdef _AFXDLL
#define RUNTIME_CLASS_T(class_name,T)		(class_name<T>::GetThisClass())
#define GWRUNTIME_CLASS(class_name)			(class_name::GetThisClass())
#define GWRUNTIME_CLASS_T(class_name,T)		(class_name<T>::GetThisClass())
#else
#define RUNTIME_CLASS_T(class_name,T)		_RUNTIME_CLASS_T(class_name,T)
#define GWRUNTIME_CLASS(class_name)			_GWRUNTIME_CLASS(class_name)
#define GWRUNTIME_CLASS_T(class_name,T)		_GWRUNTIME_CLASS_T(class_name,T)
#endif

#define ASSERT_KINDOF_T(class_name, T, object) \
	ASSERT((object)->IsKindOf(RUNTIME_CLASS_T(class_name, T1)))

#ifdef _DEBUG
#define STATIC_DOWNCAST_T(class_name, T, object)											\
	(static_cast<class_name<T>*>(AfxStaticDownCast(RUNTIME_CLASS_T(class_name,T), object)))
#else
#define STATIC_DOWNCAST_T(class_name, T, object) (static_cast<class_name<T>*>(object))
#endif

//////////////////////////////////////////////////////////////////////////////
// Helper macros for declaring CRuntimeClass compatible classes

#ifdef _AFXDLL
#define DECLARE_DYNAMIC_T(class_name,T)						\
	protected:												\
	static CRuntimeClass* PASCAL _GetBaseClass();			\
	public:													\
	static const CRuntimeClass class##class_name;			\
	static CRuntimeClass* PASCAL GetThisClass();			\
	virtual CRuntimeClass* GetRuntimeClass() const;
#define DECLARE_GWDYNAMIC(class_name)						\
	protected:												\
	static CRuntimeClass* PASCAL _GetBaseClass();			\
	public:													\
	static const GRuntimeClass class##class_name;			\
	static GRuntimeClass* PASCAL GetThisClass();			\
	virtual CRuntimeClass* GetRuntimeClass() const;
#define DECLARE_GWDYNAMIC_T(class_name,T)					\
	protected:												\
	static CRuntimeClass* PASCAL _GetBaseClass();			\
	public:													\
	static const GRuntimeClass class##class_name;			\
	static GRuntimeClass* PASCAL GetThisClass();			\
	virtual CRuntimeClass* GetRuntimeClass() const;
#else
#define DECLARE_DYNAMIC_T(class_name,T)						\
	public:													\
	static const CRuntimeClass class##class_name;			\
	virtual CRuntimeClass* GetRuntimeClass() const;			\

#define DECLARE_GWDYNAMIC(class_name)						\
	public:													\
	static const GRuntimeClass class##class_name;			\
	virtual CRuntimeClass* GetRuntimeClass() const;			\

#define DECLARE_GWDYNAMIC_T(class_name,T)					\
	public:													\
	static const GRuntimeClass class##class_name;			\
	virtual CRuntimeClass* GetRuntimeClass() const;			\

#endif

#define DECLARE_DYNCREATE_T(class_name,T)					\
	DECLARE_DYNAMIC_T(class_name,T)							\
	static CObject* PASCAL CreateObject();

#define DECLARE_GWDYNCREATE(class_name)						\
	DECLARE_GWDYNAMIC(class_name)							\
	static CObject* PASCAL CreateObject(CObject& parameter);

#define DECLARE_GWDYNCREATE_T(class_name,T)					\
	DECLARE_GWDYNAMIC_T(class_name,T)						\
	static CObject* PASCAL CreateObject(CObject& parameter);

#ifdef _AFXDLL
#define IMPLEMENT_RUNTIMECLASS_T(class_name, T, base_class_name, wSchema, pfnNew, class_init)	\
	template<typename T>		\
	AFX_COMDAT const CRuntimeClass class_name<T>::class##class_name = {#class_name, sizeof(class class_name<T>), wSchema, pfnNew, &base_class_name::GetThisClass, nullptr, class_init }; \
	template<typename T>		\
	CRuntimeClass* PASCAL class_name<T>::GetThisClass()											\
{ return _RUNTIME_CLASS_T(class_name,T); }														\
	template<typename T>		\
	CRuntimeClass* class_name<T>::GetRuntimeClass() const										\
	{ return _RUNTIME_CLASS_T(class_name,T); }
#define _IMPLEMENT_RUNTIMECLASS_T(class_name, T, base_class_name, wSchema, pfnNew, class_init)	\
	CRuntimeClass* PASCAL class_name<T>::GetThisClass()											\
		{ return RUNTIME_CLASS(base_class_name); }												\
		AFX_COMDAT CRuntimeClass class_name<T>::class##class_name = {#class_name, sizeof(class class_name<T>), wSchema, pfnNew,&base_class_name<T>::GetThisClass, nullptr, class_init }; \
		CRuntimeClass* PASCAL class_name::GetThisClass()											\
		{ return _RUNTIME_CLASS_T(class_name, T); }												\
		CRuntimeClass* class_name<T1>::GetRuntimeClass() const									\
		{ return _RUNTIME_CLASS_T(class_name, T); }
#define IMPLEMENT_GWRUNTIMECLASS(class_name, base_class_name, wSchema, pfnNew, class_init)		\
	CRuntimeClass* PASCAL class_name::_GetBaseClass()											\
	{ return RUNTIME_CLASS(base_class_name); }													\
	AFX_COMDAT const GRuntimeClass class_name::class##class_name(#class_name, sizeof(class class_name), wSchema, pfnNew,&class_name::_GetBaseClass, class_init ); \
	GRuntimeClass* PASCAL class_name::GetThisClass()											\
	{ return _GWRUNTIME_CLASS(class_name); }													\
	CRuntimeClass* class_name::GetRuntimeClass() const											\
	{ return _GWRUNTIME_CLASS(class_name); }

#define IMPLEMENT_GWRUNTIMECLASS_T(class_name,T, base_class_name, wSchema, pfnNew, class_init)	\
	template<typename T>																		\
	CRuntimeClass* PASCAL class_name<T>::_GetBaseClass()										\
	{ return RUNTIME_CLASS(base_class_name); }													\
	template<typename T>																		\
	AFX_COMDAT const GRuntimeClass class_name<T>::class##class_name(#class_name, sizeof(class class_name<T>), wSchema, pfnNew,&class_name<T>::_GetBaseClass, class_init ); \
	template<typename T>																		\
	GRuntimeClass* PASCAL class_name<T>::GetThisClass()										\
	{ return _GWRUNTIME_CLASS_T(class_name,T); }												\
	template<typename T>																		\
	CRuntimeClass* class_name<T>::GetRuntimeClass() const										\
	{ return _GWRUNTIME_CLASS_T(class_name,T); }

#else
#define IMPLEMENT_RUNTIMECLASS_T(class_name, T, base_class_name, wSchema, pfnNew, class_init)	\	
AFX_COMDAT const CRuntimeClass class_name<T>::class##class_name = {#class_name, sizeof(class class_name<T1), wSchema, pfnNew, RUNTIME_CLASS(base_class_name), nullptr, class_init}; \
	CRuntimeClass* class_name<T>::GetRuntimeClass() const										\
{ return RUNTIME_CLASS_T(class_name, T); }												\

#define _IMPLEMENT_RUNTIMECLASS_T(class_name, T, base_class_name, wSchema, pfnNew, class_init) 
	AFX_COMDAT CRuntimeClass class_name<T1>::class##class_name = {#class_name, sizeof(class class_name<T>), wSchema, pfnNew, RUNTIME_CLASS(base_class_name), nullptr, class_init}; \
	CRuntimeClass* class_name<T1>::GetRuntimeClass() const										\
{ return RUNTIME_CLASS_T(class_name, T); }												\

#define IMPLEMENT_GWRUNTIMECLASS(class_name, base_class_name, wSchema, pfnNew, class_init)			\
	AFX_COMDAT const GRuntimeClass class_name::class##class_name = {								\
#class_name, sizeof(class class_name), wSchema, nullptr,												\
	RUNTIME_CLASS(base_class_name), nullptr, class_init, pfnNew };										\
	CRuntimeClass* class_name::GetRuntimeClass() const												\
{ return GWRUNTIME_CLASS(class_name); }

#define IMPLEMENT_GWRUNTIMECLASS_T(class_name,T, base_class_name, wSchema, pfnNew, class_init)		\
	AFX_COMDAT const GRuntimeClass class_name:<T>:class##class_name = {							\
#class_name, sizeof(class class_name<T>), wSchema, nullptr,											\
	RUNTIME_CLASS(base_class_name), nullptr, class_init, pfnNew };										\
	template<typename T>																			\		
	CRuntimeClass* class_name<T>::GetRuntimeClass() const											\
{ return GWRUNTIME_CLASS_T(class_name, T); }

#endif

#define IMPLEMENT_DYNAMIC_T(class_name, T, base_class_name)						\
	IMPLEMENT_RUNTIMECLASS_T(class_name, T, base_class_name, 0xFFFF, nullptr, nullptr)

#define IMPLEMENT_DYNCREATE_T(class_name, T, base_class_name)					\
	template<class T>												\
	CObject* PASCAL class_name<T>::CreateObject()								\
{ return new class_name<T>; }													\
	IMPLEMENT_RUNTIMECLASS_T(class_name, T, base_class_name, 0xFFFF, class_name<T>::CreateObject, nullptr)

#define IMPLEMENT_GWDYNCREATE(class_name, base_class_name)						\
	CObject* PASCAL class_name::CreateObject(CObject& parameter)				\
{ return new class_name(parameter); }											\
	IMPLEMENT_GWRUNTIMECLASS(class_name, base_class_name, 0xFFFF, class_name::CreateObject, nullptr)

#define IMPLEMENT_GWDYNCREATE_T(class_name,T,base_class_name)					\
	template<typename T>														\
	CObject* PASCAL class_name<T>::CreateObject(CObject& parameter)				\
{ return new class_name<T>(parameter); }										\
	IMPLEMENT_GWRUNTIMECLASS_T(class_name, T,base_class_name, 0xFFFF, class_name<T>::CreateObject, nullptr)
