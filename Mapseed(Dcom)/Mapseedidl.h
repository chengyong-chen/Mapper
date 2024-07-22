

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0361 */
/* at Sun Nov 06 10:57:24 2005
 */
/* Compiler settings for .\Mapseed.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING()

#pragma warning(disable: 4049)  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __Mapseedidl_h__
#define __Mapseedidl_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef ___DMapseed_FWD_DEFINED__
#define ___DMapseed_FWD_DEFINED__
typedef interface _DMapseed _DMapseed;
#endif 	/* ___DMapseed_FWD_DEFINED__ */


#ifndef ___DMapseedEvents_FWD_DEFINED__
#define ___DMapseedEvents_FWD_DEFINED__
typedef interface _DMapseedEvents _DMapseedEvents;
#endif 	/* ___DMapseedEvents_FWD_DEFINED__ */


#ifndef __Mapseed_FWD_DEFINED__
#define __Mapseed_FWD_DEFINED__

#ifdef __cplusplus
typedef class Mapseed Mapseed;
#else
typedef struct Mapseed Mapseed;
#endif /* __cplusplus */

#endif 	/* __Mapseed_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 

void*__RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free(void *); 


#ifndef __MapseedLib_LIBRARY_DEFINED__
#define __MapseedLib_LIBRARY_DEFINED__

/* library MapseedLib */
/* [control][helpstring][helpfile][version][uuid] */ 


EXTERN_C const IID LIBID_MapseedLib;

#ifndef ___DMapseed_DISPINTERFACE_DEFINED__
#define ___DMapseed_DISPINTERFACE_DEFINED__

/* dispinterface _DMapseed */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__DMapseed;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("B917C841-4C11-41A6-8C5B-9345D75C8C4F")
    _DMapseed : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DMapseedVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT (STDMETHODCALLTYPE *QueryInterface)(
            _DMapseed*This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG (STDMETHODCALLTYPE *AddRef)(
            _DMapseed*This);
        
        ULONG (STDMETHODCALLTYPE *Release)(
            _DMapseed*This);
        
        HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(
            _DMapseed*This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(
            _DMapseed*This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(
            _DMapseed*This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT (STDMETHODCALLTYPE *Invoke)(
            _DMapseed*This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _DMapseedVtbl;

    interface _DMapseed
    {
        CONST_VTBL struct _DMapseedVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DMapseed_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _DMapseed_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _DMapseed_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _DMapseed_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _DMapseed_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _DMapseed_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _DMapseed_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DMapseed_DISPINTERFACE_DEFINED__ */


#ifndef ___DMapseedEvents_DISPINTERFACE_DEFINED__
#define ___DMapseedEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DMapseedEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__DMapseedEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("EFECF2B1-3F7A-4674-8CB1-79EF14219C2D")
    _DMapseedEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DMapseedEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT (STDMETHODCALLTYPE *QueryInterface)(
            _DMapseedEvents*This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG (STDMETHODCALLTYPE *AddRef)(
            _DMapseedEvents*This);
        
        ULONG (STDMETHODCALLTYPE *Release)(
            _DMapseedEvents*This);
        
        HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(
            _DMapseedEvents*This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(
            _DMapseedEvents*This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(
            _DMapseedEvents*This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT (STDMETHODCALLTYPE *Invoke)(
            _DMapseedEvents*This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _DMapseedEventsVtbl;

    interface _DMapseedEvents
    {
        CONST_VTBL struct _DMapseedEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DMapseedEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _DMapseedEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _DMapseedEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _DMapseedEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _DMapseedEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _DMapseedEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _DMapseedEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DMapseedEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_Mapseed;

#ifdef __cplusplus

class DECLSPEC_UUID("B61D8175-3AC4-4071-A3AF-CD45172A5C1C")
Mapseed;
#endif
#endif /* __MapseedLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


