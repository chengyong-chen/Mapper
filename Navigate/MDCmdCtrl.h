/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Wed Aug 06 19:54:28 2003
 */
/* Compiler settings for D:\Zhaowc\mapday\MDCmdCtrl\MDCmdCtrl.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING()


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
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

#ifndef __MDCmdCtrl_h__
#define __MDCmdCtrl_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IMDCmd_FWD_DEFINED__
#define __IMDCmd_FWD_DEFINED__
typedef interface IMDCmd IMDCmd;
#endif 	/* __IMDCmd_FWD_DEFINED__ */


#ifndef __IGISvrCtrl_FWD_DEFINED__
#define __IGISvrCtrl_FWD_DEFINED__
typedef interface IGISvrCtrl IGISvrCtrl;
#endif 	/* __IGISvrCtrl_FWD_DEFINED__ */


#ifndef __MDCmd_FWD_DEFINED__
#define __MDCmd_FWD_DEFINED__

#ifdef __cplusplus
typedef class MDCmd MDCmd;
#else
typedef struct MDCmd MDCmd;
#endif /* __cplusplus */

#endif 	/* __MDCmd_FWD_DEFINED__ */


#ifndef ___IGISvrCtrlEvents_FWD_DEFINED__
#define ___IGISvrCtrlEvents_FWD_DEFINED__
typedef interface _IGISvrCtrlEvents _IGISvrCtrlEvents;
#endif 	/* ___IGISvrCtrlEvents_FWD_DEFINED__ */


#ifndef __GISvrCtrl_FWD_DEFINED__
#define __GISvrCtrl_FWD_DEFINED__

#ifdef __cplusplus
typedef class GISvrCtrl GISvrCtrl;
#else
typedef struct GISvrCtrl GISvrCtrl;
#endif /* __cplusplus */

#endif 	/* __GISvrCtrl_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR*__RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free(void __RPC_FAR *); 

#ifndef __IMDCmd_INTERFACE_DEFINED__
#define __IMDCmd_INTERFACE_DEFINED__

/* interface IMDCmd */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IMDCmd;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0F451748-2076-4DBF-90FF-842C29BD4F44")
    IMDCmd : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ShowPanel(void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetSocket(
            DWORD hSocket) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SendCmd(
            DWORD dwId,
            BSTR bstrCmd) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMDCmdVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT (STDMETHODCALLTYPE __RPC_FAR *QueryInterface)(
            IMDCmd __RPC_FAR*This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG (STDMETHODCALLTYPE __RPC_FAR *AddRef)(
            IMDCmd __RPC_FAR*This);
        
        ULONG (STDMETHODCALLTYPE __RPC_FAR *Release)(
            IMDCmd __RPC_FAR*This);
        
        HRESULT (STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount)(
            IMDCmd __RPC_FAR*This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT (STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo)(
            IMDCmd __RPC_FAR*This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT (STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames)(
            IMDCmd __RPC_FAR*This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT (STDMETHODCALLTYPE __RPC_FAR *Invoke)(
            IMDCmd __RPC_FAR*This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT (STDMETHODCALLTYPE __RPC_FAR *ShowPanel)(
            IMDCmd __RPC_FAR*This);
        
        /* [helpstring][id] */ HRESULT (STDMETHODCALLTYPE __RPC_FAR *SetSocket)(
            IMDCmd __RPC_FAR*This,
            DWORD hSocket);
        
        /* [helpstring][id] */ HRESULT (STDMETHODCALLTYPE __RPC_FAR *SendCmd)(
            IMDCmd __RPC_FAR*This,
            DWORD dwId,
            BSTR bstrCmd);
        
        END_INTERFACE
    } IMDCmdVtbl;

    interface IMDCmd
    {
        CONST_VTBL struct IMDCmdVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMDCmd_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMDCmd_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMDCmd_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMDCmd_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMDCmd_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMDCmd_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMDCmd_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMDCmd_ShowPanel(This)	\
    (This)->lpVtbl -> ShowPanel(This)

#define IMDCmd_SetSocket(This,hSocket)	\
    (This)->lpVtbl -> SetSocket(This,hSocket)

#define IMDCmd_SendCmd(This,dwId,bstrCmd)	\
    (This)->lpVtbl -> SendCmd(This,dwId,bstrCmd)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IMDCmd_ShowPanel_Proxy(
    IMDCmd __RPC_FAR*This);


void __RPC_STUB IMDCmd_ShowPanel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IMDCmd_SetSocket_Proxy(
    IMDCmd __RPC_FAR*This,
    DWORD hSocket);


void __RPC_STUB IMDCmd_SetSocket_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IMDCmd_SendCmd_Proxy(
    IMDCmd __RPC_FAR*This,
    DWORD dwId,
    BSTR bstrCmd);


void __RPC_STUB IMDCmd_SendCmd_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMDCmd_INTERFACE_DEFINED__ */


#ifndef __IGISvrCtrl_INTERFACE_DEFINED__
#define __IGISvrCtrl_INTERFACE_DEFINED__

/* interface IGISvrCtrl */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IGISvrCtrl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("58ECEF16-157A-48D0-8025-65C9C408F303")
    IGISvrCtrl : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ShowPanel(void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetSocket(
            DWORD hSocket) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IGISvrCtrlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT (STDMETHODCALLTYPE __RPC_FAR *QueryInterface)(
            IGISvrCtrl __RPC_FAR*This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG (STDMETHODCALLTYPE __RPC_FAR *AddRef)(
            IGISvrCtrl __RPC_FAR*This);
        
        ULONG (STDMETHODCALLTYPE __RPC_FAR *Release)(
            IGISvrCtrl __RPC_FAR*This);
        
        HRESULT (STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount)(
            IGISvrCtrl __RPC_FAR*This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT (STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo)(
            IGISvrCtrl __RPC_FAR*This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT (STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames)(
            IGISvrCtrl __RPC_FAR*This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT (STDMETHODCALLTYPE __RPC_FAR *Invoke)(
            IGISvrCtrl __RPC_FAR*This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT (STDMETHODCALLTYPE __RPC_FAR *ShowPanel)(
            IGISvrCtrl __RPC_FAR*This);
        
        /* [helpstring][id] */ HRESULT (STDMETHODCALLTYPE __RPC_FAR *SetSocket)(
            IGISvrCtrl __RPC_FAR*This,
            DWORD hSocket);
        
        END_INTERFACE
    } IGISvrCtrlVtbl;

    interface IGISvrCtrl
    {
        CONST_VTBL struct IGISvrCtrlVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IGISvrCtrl_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IGISvrCtrl_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IGISvrCtrl_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IGISvrCtrl_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IGISvrCtrl_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IGISvrCtrl_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IGISvrCtrl_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IGISvrCtrl_ShowPanel(This)	\
    (This)->lpVtbl -> ShowPanel(This)

#define IGISvrCtrl_SetSocket(This,hSocket)	\
    (This)->lpVtbl -> SetSocket(This,hSocket)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGISvrCtrl_ShowPanel_Proxy(
    IGISvrCtrl __RPC_FAR*This);


void __RPC_STUB IGISvrCtrl_ShowPanel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGISvrCtrl_SetSocket_Proxy(
    IGISvrCtrl __RPC_FAR*This,
    DWORD hSocket);


void __RPC_STUB IGISvrCtrl_SetSocket_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IGISvrCtrl_INTERFACE_DEFINED__ */



#ifndef __MDCMDCTRLLib_LIBRARY_DEFINED__
#define __MDCMDCTRLLib_LIBRARY_DEFINED__

/* library MDCMDCTRLLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_MDCMDCTRLLib;

EXTERN_C const CLSID CLSID_MDCmd;

#ifdef __cplusplus

class DECLSPEC_UUID("8D4665FC-CD3C-4549-A3A7-C5D7A30D259E")
MDCmd;
#endif

#ifndef ___IGISvrCtrlEvents_DISPINTERFACE_DEFINED__
#define ___IGISvrCtrlEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IGISvrCtrlEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IGISvrCtrlEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("2D43C86C-CEEF-46D1-BE1A-823349B04D0A")
    _IGISvrCtrlEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IGISvrCtrlEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT (STDMETHODCALLTYPE __RPC_FAR *QueryInterface)(
            _IGISvrCtrlEvents __RPC_FAR*This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG (STDMETHODCALLTYPE __RPC_FAR *AddRef)(
            _IGISvrCtrlEvents __RPC_FAR*This);
        
        ULONG (STDMETHODCALLTYPE __RPC_FAR *Release)(
            _IGISvrCtrlEvents __RPC_FAR*This);
        
        HRESULT (STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount)(
            _IGISvrCtrlEvents __RPC_FAR*This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT (STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo)(
            _IGISvrCtrlEvents __RPC_FAR*This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT (STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames)(
            _IGISvrCtrlEvents __RPC_FAR*This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT (STDMETHODCALLTYPE __RPC_FAR *Invoke)(
            _IGISvrCtrlEvents __RPC_FAR*This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        END_INTERFACE
    } _IGISvrCtrlEventsVtbl;

    interface _IGISvrCtrlEvents
    {
        CONST_VTBL struct _IGISvrCtrlEventsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IGISvrCtrlEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _IGISvrCtrlEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _IGISvrCtrlEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _IGISvrCtrlEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _IGISvrCtrlEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _IGISvrCtrlEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _IGISvrCtrlEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IGISvrCtrlEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_GISvrCtrl;

#ifdef __cplusplus

class DECLSPEC_UUID("F901EEDD-02CE-4A56-81EB-77F59ADB6965")
GISvrCtrl;
#endif
#endif /* __MDCMDCTRLLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(    unsigned long __RPC_FAR *, unsigned long            , BSTR __RPC_FAR *); 
unsigned char __RPC_FAR*__RPC_USER  BSTR_UserMarshal( unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR *); 
unsigned char __RPC_FAR*__RPC_USER  BSTR_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR *); 
void                      __RPC_USER  BSTR_UserFree(    unsigned long __RPC_FAR *, BSTR __RPC_FAR *); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
