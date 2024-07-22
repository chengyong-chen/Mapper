

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Thu Jun 28 14:33:56 2007
 */
/* Compiler settings for .\Mapeasy.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __Mapeasyidl_h__
#define __Mapeasyidl_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef ___DMapeasy_FWD_DEFINED__
#define ___DMapeasy_FWD_DEFINED__
typedef interface _DMapeasy _DMapeasy;
#endif 	/* ___DMapeasy_FWD_DEFINED__ */


#ifndef ___DMapeasyEvents_FWD_DEFINED__
#define ___DMapeasyEvents_FWD_DEFINED__
typedef interface _DMapeasyEvents _DMapeasyEvents;
#endif 	/* ___DMapeasyEvents_FWD_DEFINED__ */


#ifndef __Mapeasy_FWD_DEFINED__
#define __Mapeasy_FWD_DEFINED__

#ifdef __cplusplus
typedef class Mapeasy Mapeasy;
#else
typedef struct Mapeasy Mapeasy;
#endif /* __cplusplus */

#endif 	/* __Mapeasy_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 

void*__RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void*); 


#ifndef __MapeasyLib_LIBRARY_DEFINED__
#define __MapeasyLib_LIBRARY_DEFINED__

/* library MapeasyLib */
/* [control][helpstring][helpfile][version][uuid] */ 


EXTERN_C const IID LIBID_MapeasyLib;

#ifndef ___DMapeasy_DISPINTERFACE_DEFINED__
#define ___DMapeasy_DISPINTERFACE_DEFINED__

/* dispinterface _DMapeasy */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__DMapeasy;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("B917C841-4C11-41A6-8C5B-9345D75C8C4F")
    _DMapeasy : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DMapeasyVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DMapeasy*This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DMapeasy*This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DMapeasy*This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DMapeasy*This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DMapeasy*This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DMapeasy*This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DMapeasy*This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _DMapeasyVtbl;

    interface _DMapeasy
    {
        CONST_VTBL struct _DMapeasyVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DMapeasy_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _DMapeasy_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _DMapeasy_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _DMapeasy_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _DMapeasy_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _DMapeasy_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _DMapeasy_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DMapeasy_DISPINTERFACE_DEFINED__ */


#ifndef ___DMapeasyEvents_DISPINTERFACE_DEFINED__
#define ___DMapeasyEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DMapeasyEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__DMapeasyEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("EFECF2B1-3F7A-4674-8CB1-79EF14219C2D")
    _DMapeasyEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DMapeasyEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DMapeasyEvents*This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DMapeasyEvents*This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DMapeasyEvents*This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DMapeasyEvents*This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DMapeasyEvents*This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DMapeasyEvents*This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DMapeasyEvents*This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _DMapeasyEventsVtbl;

    interface _DMapeasyEvents
    {
        CONST_VTBL struct _DMapeasyEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DMapeasyEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _DMapeasyEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _DMapeasyEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _DMapeasyEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _DMapeasyEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _DMapeasyEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _DMapeasyEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DMapeasyEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_Mapeasy;

#ifdef __cplusplus

class DECLSPEC_UUID("B61D8175-3AC4-4071-A3AF-CD45172A5C1C")
Mapeasy;
#endif
#endif /* __MapeasyLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


