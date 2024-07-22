/* this file contains the actual definitions of */
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 5.01.0164 */
/* at Wed Aug 06 19:54:28 2003
 */
/* Compiler settings for D:\Zhaowc\mapday\MDCmdCtrl\MDCmdCtrl.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )
#ifdef __cplusplus
extern "C"{
#endif 


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

const IID IID_IMDCmd = {0x0F451748,0x2076,0x4DBF,{0x90,0xFF,0x84,0x2C,0x29,0xBD,0x4F,0x44}};


const IID IID_IGISvrCtrl = {0x58ECEF16,0x157A,0x48D0,{0x80,0x25,0x65,0xC9,0xC4,0x08,0xF3,0x03}};


const IID LIBID_MDCMDCTRLLib = {0x79EA554F,0x3A4D,0x4524,{0x8E,0x40,0xBD,0xC5,0x0A,0x8C,0x10,0x85}};


const CLSID CLSID_MDCmd = {0x8D4665FC,0xCD3C,0x4549,{0xA3,0xA7,0xC5,0xD7,0xA3,0x0D,0x25,0x9E}};


const IID DIID__IGISvrCtrlEvents = {0x2D43C86C,0xCEEF,0x46D1,{0xBE,0x1A,0x82,0x33,0x49,0xB0,0x4D,0x0A}};


const CLSID CLSID_GISvrCtrl = {0xF901EEDD,0x02CE,0x4A56,{0x81,0xEB,0x77,0xF5,0x9A,0xDB,0x69,0x65}};


#ifdef __cplusplus
}
#endif

