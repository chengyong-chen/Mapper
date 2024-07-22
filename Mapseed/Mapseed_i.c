

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Fri Aug 24 13:14:29 2007
 */
/* Compiler settings for .\Mapseed.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, LIBID_MapseedLib,0x5F5507F6,0x78CE,0x4F17,0xAD,0x6F,0x5A,0x80,0x88,0xEC,0xF6,0xAD);


MIDL_DEFINE_GUID(IID, DIID__DMapseed,0xF119C744,0xE94A,0x4DC7,0x9A,0x81,0xB3,0xF9,0x6A,0x1E,0x72,0x11);


MIDL_DEFINE_GUID(IID, DIID__DMapseedEvents,0xC40570B4,0xC981,0x4492,0x89,0x78,0x76,0xD8,0xFF,0x21,0x25,0xEF);


MIDL_DEFINE_GUID(CLSID, CLSID_Mapseed,0x2A5CCA81,0xDF7F,0x4A80,0xBB,0x90,0xA8,0xDA,0x0A,0x28,0xBF,0x31);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



