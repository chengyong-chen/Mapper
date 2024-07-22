

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0361 */
/* at Sun Nov 06 15:11:43 2005
 */
/* Compiler settings for .\Mapsvr.idl:
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

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __Mapsvr_h__
#define __Mapsvr_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IMapdcom_FWD_DEFINED__
#define __IMapdcom_FWD_DEFINED__
typedef interface IMapdcom IMapdcom;
#endif 	/* __IMapdcom_FWD_DEFINED__ */


#ifndef __CMapdcom_FWD_DEFINED__
#define __CMapdcom_FWD_DEFINED__

#ifdef __cplusplus
typedef class CMapdcom CMapdcom;
#else
typedef struct CMapdcom CMapdcom;
#endif /* __cplusplus */

#endif 	/* __CMapdcom_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void*__RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free(void *); 

#ifndef __IMapdcom_INTERFACE_DEFINED__
#define __IMapdcom_INTERFACE_DEFINED__

/* interface IMapdcom */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IMapdcom;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("EF327845-40A8-4AE0-9050-EA7FAD0275EA")
    IMapdcom : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OpenAtlas(
            BSTR bstrAtlas) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OpenMap(
            USHORT wMapId,
            /* [out] */ VARIANT *varHead) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetMapData(
            USHORT wMapId,
            USHORT nZoom,
            LONG left,
            LONG top,
            LONG right,
            LONG bottom,
            /* [out] */ VARIANT *varData) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetMapId(
            BSTR bstrMap,
            /* [out] */ USHORT *wMapId) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetParentMap(
            USHORT wMapId,
            /* [out] */ USHORT *wParentMapId) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetSubMap(
            USHORT wMapId,
            DOUBLE X,
            DOUBLE Y,
            /* [out] */ USHORT *wSubMapId) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE MatchMap(
            DOUBLE fLng,
            DOUBLE fLat,
            /* [out] */ USHORT *wMapId) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMapdcomVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT (STDMETHODCALLTYPE *QueryInterface)(
            IMapdcom*This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG (STDMETHODCALLTYPE *AddRef)(
            IMapdcom*This);
        
        ULONG (STDMETHODCALLTYPE *Release)(
            IMapdcom*This);
        
        HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(
            IMapdcom*This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(
            IMapdcom*This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(
            IMapdcom*This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT (STDMETHODCALLTYPE *Invoke)(
            IMapdcom*This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT (STDMETHODCALLTYPE *OpenAtlas)(
            IMapdcom*This,
            BSTR bstrAtlas);
        
        /* [helpstring][id] */ HRESULT (STDMETHODCALLTYPE *OpenMap)(
            IMapdcom*This,
            USHORT wMapId,
            /* [out] */ VARIANT *varHead);
        
        /* [helpstring][id] */ HRESULT (STDMETHODCALLTYPE *GetMapData)(
            IMapdcom*This,
            USHORT wMapId,
            USHORT nZoom,
            LONG left,
            LONG top,
            LONG right,
            LONG bottom,
            /* [out] */ VARIANT *varData);
        
        /* [helpstring][id] */ HRESULT (STDMETHODCALLTYPE *GetMapId)(
            IMapdcom*This,
            BSTR bstrMap,
            /* [out] */ USHORT *wMapId);
        
        /* [helpstring][id] */ HRESULT (STDMETHODCALLTYPE *GetParentMap)(
            IMapdcom*This,
            USHORT wMapId,
            /* [out] */ USHORT *wParentMapId);
        
        /* [helpstring][id] */ HRESULT (STDMETHODCALLTYPE *GetSubMap)(
            IMapdcom*This,
            USHORT wMapId,
            DOUBLE X,
            DOUBLE Y,
            /* [out] */ USHORT *wSubMapId);
        
        /* [helpstring][id] */ HRESULT (STDMETHODCALLTYPE *MatchMap)(
            IMapdcom*This,
            DOUBLE fLng,
            DOUBLE fLat,
            /* [out] */ USHORT *wMapId);
        
        END_INTERFACE
    } IMapdcomVtbl;

    interface IMapdcom
    {
        CONST_VTBL struct IMapdcomVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMapdcom_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMapdcom_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMapdcom_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMapdcom_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMapdcom_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMapdcom_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMapdcom_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMapdcom_OpenAtlas(This,bstrAtlas)	\
    (This)->lpVtbl -> OpenAtlas(This,bstrAtlas)

#define IMapdcom_OpenMap(This,wMapId,varHead)	\
    (This)->lpVtbl -> OpenMap(This,wMapId,varHead)

#define IMapdcom_GetMapData(This,wMapId,nZoom,left,top,right,bottom,varData)	\
    (This)->lpVtbl -> GetMapData(This,wMapId,nZoom,left,top,right,bottom,varData)

#define IMapdcom_GetMapId(This,bstrMap,wMapId)	\
    (This)->lpVtbl -> GetMapId(This,bstrMap,wMapId)

#define IMapdcom_GetParentMap(This,wMapId,wParentMapId)	\
    (This)->lpVtbl -> GetParentMap(This,wMapId,wParentMapId)

#define IMapdcom_GetSubMap(This,wMapId,X,Y,wSubMapId)	\
    (This)->lpVtbl -> GetSubMap(This,wMapId,X,Y,wSubMapId)

#define IMapdcom_MatchMap(This,fLng,fLat,wMapId)	\
    (This)->lpVtbl -> MatchMap(This,fLng,fLat,wMapId)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IMapdcom_OpenAtlas_Proxy(
    IMapdcom*This,
    BSTR bstrAtlas);


void __RPC_STUB IMapdcom_OpenAtlas_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IMapdcom_OpenMap_Proxy(
    IMapdcom*This,
    USHORT wMapId,
    /* [out] */ VARIANT *varHead);


void __RPC_STUB IMapdcom_OpenMap_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IMapdcom_GetMapData_Proxy(
    IMapdcom*This,
    USHORT wMapId,
    USHORT nZoom,
    LONG left,
    LONG top,
    LONG right,
    LONG bottom,
    /* [out] */ VARIANT *varData);


void __RPC_STUB IMapdcom_GetMapData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IMapdcom_GetMapId_Proxy(
    IMapdcom*This,
    BSTR bstrMap,
    /* [out] */ USHORT *wMapId);


void __RPC_STUB IMapdcom_GetMapId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IMapdcom_GetParentMap_Proxy(
    IMapdcom*This,
    USHORT wMapId,
    /* [out] */ USHORT *wParentMapId);


void __RPC_STUB IMapdcom_GetParentMap_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IMapdcom_GetSubMap_Proxy(
    IMapdcom*This,
    USHORT wMapId,
    DOUBLE X,
    DOUBLE Y,
    /* [out] */ USHORT *wSubMapId);


void __RPC_STUB IMapdcom_GetSubMap_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IMapdcom_MatchMap_Proxy(
    IMapdcom*This,
    DOUBLE fLng,
    DOUBLE fLat,
    /* [out] */ USHORT *wMapId);


void __RPC_STUB IMapdcom_MatchMap_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMapdcom_INTERFACE_DEFINED__ */



#ifndef __MAPSVRLib_LIBRARY_DEFINED__
#define __MAPSVRLib_LIBRARY_DEFINED__

/* library MAPSVRLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_MAPSVRLib;

EXTERN_C const CLSID CLSID_CMapdcom;

#ifdef __cplusplus

class DECLSPEC_UUID("EB914BA1-3C9C-4321-BB87-D2F8E3EFEB09")
CMapdcom;
#endif
#endif /* __MAPSVRLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(    unsigned long *, unsigned long            , BSTR *); 
unsigned char*__RPC_USER  BSTR_UserMarshal( unsigned long *, unsigned char *, BSTR *); 
unsigned char*__RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR *); 
void                      __RPC_USER  BSTR_UserFree(    unsigned long *, BSTR *); 

unsigned long             __RPC_USER  VARIANT_UserSize(    unsigned long *, unsigned long            , VARIANT *); 
unsigned char*__RPC_USER  VARIANT_UserMarshal( unsigned long *, unsigned char *, VARIANT *); 
unsigned char*__RPC_USER  VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT *); 
void                      __RPC_USER  VARIANT_UserFree(    unsigned long *, VARIANT *); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


