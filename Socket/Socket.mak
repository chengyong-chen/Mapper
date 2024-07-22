# Microsoft Developer Studio Generated NMAKE File, Based on Socket.dsp
!IF "$(CFG)" == ""
CFG=Socket - Win32 Debug
!MESSAGE No configuration specified. Defaulting to Socket - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Socket - Win32 Release" && "$(CFG)" != "Socket - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Socket.mak" CFG="Socket - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Socket - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Socket - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "Socket - Win32 Release"

OUTDIR=.\../Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\../Release
# End Custom Macros

ALL : "$(OUTDIR)\Socket.dll" "$(OUTDIR)\Socket.bsc"


CLEAN :
	-@erase "$(INTDIR)\ApacheXMLParser.obj"
	-@erase "$(INTDIR)\ApacheXMLParser.sbr"
	-@erase "$(INTDIR)\BtnST.obj"
	-@erase "$(INTDIR)\BtnST.sbr"
	-@erase "$(INTDIR)\CWzdVehicleAdd.obj"
	-@erase "$(INTDIR)\CWzdVehicleAdd.sbr"
	-@erase "$(INTDIR)\debug_functions.obj"
	-@erase "$(INTDIR)\debug_functions.sbr"
	-@erase "$(INTDIR)\DevicesCodingManager.obj"
	-@erase "$(INTDIR)\DevicesCodingManager.sbr"
	-@erase "$(INTDIR)\diskid32.obj"
	-@erase "$(INTDIR)\diskid32.sbr"
	-@erase "$(INTDIR)\DlgAddAClientUser.obj"
	-@erase "$(INTDIR)\DlgAddAClientUser.sbr"
	-@erase "$(INTDIR)\DlgAddaDriver.obj"
	-@erase "$(INTDIR)\DlgAddaDriver.sbr"
	-@erase "$(INTDIR)\DlgChoiceAppVer.obj"
	-@erase "$(INTDIR)\DlgChoiceAppVer.sbr"
	-@erase "$(INTDIR)\DlgCSWQuery.obj"
	-@erase "$(INTDIR)\DlgCSWQuery.sbr"
	-@erase "$(INTDIR)\DlgLoginSystem.obj"
	-@erase "$(INTDIR)\DlgLoginSystem.sbr"
	-@erase "$(INTDIR)\DlgModifyClientPassword.obj"
	-@erase "$(INTDIR)\DlgModifyClientPassword.sbr"
	-@erase "$(INTDIR)\DlgMsgQuery.obj"
	-@erase "$(INTDIR)\DlgMsgQuery.sbr"
	-@erase "$(INTDIR)\DlgPosQuery.obj"
	-@erase "$(INTDIR)\DlgPosQuery.sbr"
	-@erase "$(INTDIR)\DlgQueryVhcDispatch.obj"
	-@erase "$(INTDIR)\DlgQueryVhcDispatch.sbr"
	-@erase "$(INTDIR)\DlgUniformStyle.obj"
	-@erase "$(INTDIR)\DlgUniformStyle.sbr"
	-@erase "$(INTDIR)\DlgVehicleQuery.obj"
	-@erase "$(INTDIR)\DlgVehicleQuery.sbr"
	-@erase "$(INTDIR)\DlgVhcPswModify.obj"
	-@erase "$(INTDIR)\DlgVhcPswModify.sbr"
	-@erase "$(INTDIR)\ManipulateRegistry.obj"
	-@erase "$(INTDIR)\ManipulateRegistry.sbr"
	-@erase "$(INTDIR)\ManipulateServerApp.obj"
	-@erase "$(INTDIR)\ManipulateServerApp.sbr"
	-@erase "$(INTDIR)\MonitorSetting.obj"
	-@erase "$(INTDIR)\MonitorSetting.sbr"
	-@erase "$(INTDIR)\MyPropertySheet.obj"
	-@erase "$(INTDIR)\MyPropertySheet.sbr"
	-@erase "$(INTDIR)\Socket.obj"
	-@erase "$(INTDIR)\Socket.pch"
	-@erase "$(INTDIR)\Socket.res"
	-@erase "$(INTDIR)\Socket.sbr"
	-@erase "$(INTDIR)\SQL_Excute.obj"
	-@erase "$(INTDIR)\SQL_Excute.sbr"
	-@erase "$(INTDIR)\SQLConvert.obj"
	-@erase "$(INTDIR)\SQLConvert.sbr"
	-@erase "$(INTDIR)\SQLExecutor.obj"
	-@erase "$(INTDIR)\SQLExecutor.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\WzdVehicleAdd_4.obj"
	-@erase "$(INTDIR)\WzdVehicleAdd_4.sbr"
	-@erase "$(INTDIR)\XPButton.obj"
	-@erase "$(INTDIR)\XPButton.sbr"
	-@erase "$(OUTDIR)\Socket.bsc"
	-@erase "$(OUTDIR)\Socket.dll"
	-@erase "$(OUTDIR)\Socket.exp"
	-@erase "$(OUTDIR)\Socket.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_WINDLL" /D "_MBCS" /D "_AFXEXT" /D "RUI_OP" /D "_NEEDLOG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Socket.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\Socket.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Socket.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\ApacheXMLParser.sbr" \
	"$(INTDIR)\BtnST.sbr" \
	"$(INTDIR)\CWzdVehicleAdd.sbr" \
	"$(INTDIR)\debug_functions.sbr" \
	"$(INTDIR)\DevicesCodingManager.sbr" \
	"$(INTDIR)\diskid32.sbr" \
	"$(INTDIR)\DlgAddAClientUser.sbr" \
	"$(INTDIR)\DlgAddaDriver.sbr" \
	"$(INTDIR)\DlgChoiceAppVer.sbr" \
	"$(INTDIR)\DlgCSWQuery.sbr" \
	"$(INTDIR)\DlgLoginSystem.sbr" \
	"$(INTDIR)\DlgModifyClientPassword.sbr" \
	"$(INTDIR)\DlgMsgQuery.sbr" \
	"$(INTDIR)\DlgPosQuery.sbr" \
	"$(INTDIR)\DlgQueryVhcDispatch.sbr" \
	"$(INTDIR)\DlgUniformStyle.sbr" \
	"$(INTDIR)\DlgVehicleQuery.sbr" \
	"$(INTDIR)\DlgVhcPswModify.sbr" \
	"$(INTDIR)\ManipulateRegistry.sbr" \
	"$(INTDIR)\ManipulateServerApp.sbr" \
	"$(INTDIR)\MonitorSetting.sbr" \
	"$(INTDIR)\MyPropertySheet.sbr" \
	"$(INTDIR)\Socket.sbr" \
	"$(INTDIR)\SQL_Excute.sbr" \
	"$(INTDIR)\SQLConvert.sbr" \
	"$(INTDIR)\SQLExecutor.sbr" \
	"$(INTDIR)\StdAfx.sbr" \
	"$(INTDIR)\WzdVehicleAdd_4.sbr" \
	"$(INTDIR)\XPButton.sbr"

"$(OUTDIR)\Socket.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=xerces-c_2.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\Socket.pdb" /machine:I386 /def:".\Socket.def" /out:"$(OUTDIR)\Socket.dll" /implib:"$(OUTDIR)\Socket.lib" 
DEF_FILE= \
	".\Socket.def"
LINK32_OBJS= \
	"$(INTDIR)\ApacheXMLParser.obj" \
	"$(INTDIR)\BtnST.obj" \
	"$(INTDIR)\CWzdVehicleAdd.obj" \
	"$(INTDIR)\debug_functions.obj" \
	"$(INTDIR)\DevicesCodingManager.obj" \
	"$(INTDIR)\diskid32.obj" \
	"$(INTDIR)\DlgAddAClientUser.obj" \
	"$(INTDIR)\DlgAddaDriver.obj" \
	"$(INTDIR)\DlgChoiceAppVer.obj" \
	"$(INTDIR)\DlgCSWQuery.obj" \
	"$(INTDIR)\DlgLoginSystem.obj" \
	"$(INTDIR)\DlgModifyClientPassword.obj" \
	"$(INTDIR)\DlgMsgQuery.obj" \
	"$(INTDIR)\DlgPosQuery.obj" \
	"$(INTDIR)\DlgQueryVhcDispatch.obj" \
	"$(INTDIR)\DlgUniformStyle.obj" \
	"$(INTDIR)\DlgVehicleQuery.obj" \
	"$(INTDIR)\DlgVhcPswModify.obj" \
	"$(INTDIR)\ManipulateRegistry.obj" \
	"$(INTDIR)\ManipulateServerApp.obj" \
	"$(INTDIR)\MonitorSetting.obj" \
	"$(INTDIR)\MyPropertySheet.obj" \
	"$(INTDIR)\Socket.obj" \
	"$(INTDIR)\SQL_Excute.obj" \
	"$(INTDIR)\SQLConvert.obj" \
	"$(INTDIR)\SQLExecutor.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\WzdVehicleAdd_4.obj" \
	"$(INTDIR)\XPButton.obj" \
	"$(INTDIR)\Socket.res"

"$(OUTDIR)\Socket.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Socket - Win32 Debug"

OUTDIR=.\../bin
INTDIR=.\debug
# Begin Custom Macros
OutDir=.\../bin
# End Custom Macros

ALL : "$(OUTDIR)\Socket.dll" "$(OUTDIR)\Socket.bsc"


CLEAN :
	-@erase "$(INTDIR)\ApacheXMLParser.obj"
	-@erase "$(INTDIR)\ApacheXMLParser.sbr"
	-@erase "$(INTDIR)\BtnST.obj"
	-@erase "$(INTDIR)\BtnST.sbr"
	-@erase "$(INTDIR)\CWzdVehicleAdd.obj"
	-@erase "$(INTDIR)\CWzdVehicleAdd.sbr"
	-@erase "$(INTDIR)\debug_functions.obj"
	-@erase "$(INTDIR)\debug_functions.sbr"
	-@erase "$(INTDIR)\DevicesCodingManager.obj"
	-@erase "$(INTDIR)\DevicesCodingManager.sbr"
	-@erase "$(INTDIR)\diskid32.obj"
	-@erase "$(INTDIR)\diskid32.sbr"
	-@erase "$(INTDIR)\DlgAddAClientUser.obj"
	-@erase "$(INTDIR)\DlgAddAClientUser.sbr"
	-@erase "$(INTDIR)\DlgAddaDriver.obj"
	-@erase "$(INTDIR)\DlgAddaDriver.sbr"
	-@erase "$(INTDIR)\DlgChoiceAppVer.obj"
	-@erase "$(INTDIR)\DlgChoiceAppVer.sbr"
	-@erase "$(INTDIR)\DlgCSWQuery.obj"
	-@erase "$(INTDIR)\DlgCSWQuery.sbr"
	-@erase "$(INTDIR)\DlgLoginSystem.obj"
	-@erase "$(INTDIR)\DlgLoginSystem.sbr"
	-@erase "$(INTDIR)\DlgModifyClientPassword.obj"
	-@erase "$(INTDIR)\DlgModifyClientPassword.sbr"
	-@erase "$(INTDIR)\DlgMsgQuery.obj"
	-@erase "$(INTDIR)\DlgMsgQuery.sbr"
	-@erase "$(INTDIR)\DlgPosQuery.obj"
	-@erase "$(INTDIR)\DlgPosQuery.sbr"
	-@erase "$(INTDIR)\DlgQueryVhcDispatch.obj"
	-@erase "$(INTDIR)\DlgQueryVhcDispatch.sbr"
	-@erase "$(INTDIR)\DlgUniformStyle.obj"
	-@erase "$(INTDIR)\DlgUniformStyle.sbr"
	-@erase "$(INTDIR)\DlgVehicleQuery.obj"
	-@erase "$(INTDIR)\DlgVehicleQuery.sbr"
	-@erase "$(INTDIR)\DlgVhcPswModify.obj"
	-@erase "$(INTDIR)\DlgVhcPswModify.sbr"
	-@erase "$(INTDIR)\ManipulateRegistry.obj"
	-@erase "$(INTDIR)\ManipulateRegistry.sbr"
	-@erase "$(INTDIR)\ManipulateServerApp.obj"
	-@erase "$(INTDIR)\ManipulateServerApp.sbr"
	-@erase "$(INTDIR)\MonitorSetting.obj"
	-@erase "$(INTDIR)\MonitorSetting.sbr"
	-@erase "$(INTDIR)\MyPropertySheet.obj"
	-@erase "$(INTDIR)\MyPropertySheet.sbr"
	-@erase "$(INTDIR)\Socket.obj"
	-@erase "$(INTDIR)\Socket.pch"
	-@erase "$(INTDIR)\Socket.res"
	-@erase "$(INTDIR)\Socket.sbr"
	-@erase "$(INTDIR)\SQL_Excute.obj"
	-@erase "$(INTDIR)\SQL_Excute.sbr"
	-@erase "$(INTDIR)\SQLConvert.obj"
	-@erase "$(INTDIR)\SQLConvert.sbr"
	-@erase "$(INTDIR)\SQLExecutor.obj"
	-@erase "$(INTDIR)\SQLExecutor.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\WzdVehicleAdd_4.obj"
	-@erase "$(INTDIR)\WzdVehicleAdd_4.sbr"
	-@erase "$(INTDIR)\XPButton.obj"
	-@erase "$(INTDIR)\XPButton.sbr"
	-@erase "$(OUTDIR)\Socket.bsc"
	-@erase "$(OUTDIR)\Socket.dll"
	-@erase "$(OUTDIR)\Socket.exp"
	-@erase "$(OUTDIR)\Socket.ilk"
	-@erase "$(OUTDIR)\Socket.lib"
	-@erase "$(OUTDIR)\Socket.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUGfs" /D "_DEBUG" /D "_WINDLL" /D "_MBCS" /D "_AFXEXT" /D "RUI_OP" /D "_NEEDLOG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Socket.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\Socket.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Socket.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\ApacheXMLParser.sbr" \
	"$(INTDIR)\BtnST.sbr" \
	"$(INTDIR)\CWzdVehicleAdd.sbr" \
	"$(INTDIR)\debug_functions.sbr" \
	"$(INTDIR)\DevicesCodingManager.sbr" \
	"$(INTDIR)\diskid32.sbr" \
	"$(INTDIR)\DlgAddAClientUser.sbr" \
	"$(INTDIR)\DlgAddaDriver.sbr" \
	"$(INTDIR)\DlgChoiceAppVer.sbr" \
	"$(INTDIR)\DlgCSWQuery.sbr" \
	"$(INTDIR)\DlgLoginSystem.sbr" \
	"$(INTDIR)\DlgModifyClientPassword.sbr" \
	"$(INTDIR)\DlgMsgQuery.sbr" \
	"$(INTDIR)\DlgPosQuery.sbr" \
	"$(INTDIR)\DlgQueryVhcDispatch.sbr" \
	"$(INTDIR)\DlgUniformStyle.sbr" \
	"$(INTDIR)\DlgVehicleQuery.sbr" \
	"$(INTDIR)\DlgVhcPswModify.sbr" \
	"$(INTDIR)\ManipulateRegistry.sbr" \
	"$(INTDIR)\ManipulateServerApp.sbr" \
	"$(INTDIR)\MonitorSetting.sbr" \
	"$(INTDIR)\MyPropertySheet.sbr" \
	"$(INTDIR)\Socket.sbr" \
	"$(INTDIR)\SQL_Excute.sbr" \
	"$(INTDIR)\SQLConvert.sbr" \
	"$(INTDIR)\SQLExecutor.sbr" \
	"$(INTDIR)\StdAfx.sbr" \
	"$(INTDIR)\WzdVehicleAdd_4.sbr" \
	"$(INTDIR)\XPButton.sbr"

"$(OUTDIR)\Socket.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=xerces-c_2D.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\Socket.pdb" /debug /machine:I386 /def:".\Socket.def" /out:"$(OUTDIR)\Socket.dll" /implib:"$(OUTDIR)\Socket.lib" /pdbtype:sept 
DEF_FILE= \
	".\Socket.def"
LINK32_OBJS= \
	"$(INTDIR)\ApacheXMLParser.obj" \
	"$(INTDIR)\BtnST.obj" \
	"$(INTDIR)\CWzdVehicleAdd.obj" \
	"$(INTDIR)\debug_functions.obj" \
	"$(INTDIR)\DevicesCodingManager.obj" \
	"$(INTDIR)\diskid32.obj" \
	"$(INTDIR)\DlgAddAClientUser.obj" \
	"$(INTDIR)\DlgAddaDriver.obj" \
	"$(INTDIR)\DlgChoiceAppVer.obj" \
	"$(INTDIR)\DlgCSWQuery.obj" \
	"$(INTDIR)\DlgLoginSystem.obj" \
	"$(INTDIR)\DlgModifyClientPassword.obj" \
	"$(INTDIR)\DlgMsgQuery.obj" \
	"$(INTDIR)\DlgPosQuery.obj" \
	"$(INTDIR)\DlgQueryVhcDispatch.obj" \
	"$(INTDIR)\DlgUniformStyle.obj" \
	"$(INTDIR)\DlgVehicleQuery.obj" \
	"$(INTDIR)\DlgVhcPswModify.obj" \
	"$(INTDIR)\ManipulateRegistry.obj" \
	"$(INTDIR)\ManipulateServerApp.obj" \
	"$(INTDIR)\MonitorSetting.obj" \
	"$(INTDIR)\MyPropertySheet.obj" \
	"$(INTDIR)\Socket.obj" \
	"$(INTDIR)\SQL_Excute.obj" \
	"$(INTDIR)\SQLConvert.obj" \
	"$(INTDIR)\SQLExecutor.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\WzdVehicleAdd_4.obj" \
	"$(INTDIR)\XPButton.obj" \
	"$(INTDIR)\Socket.res"

"$(OUTDIR)\Socket.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("Socket.dep")
!INCLUDE "Socket.dep"
!ELSE 
!MESSAGE Warning: cannot find "Socket.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Socket - Win32 Release" || "$(CFG)" == "Socket - Win32 Debug"
SOURCE=.\ApacheXMLParser.cpp

"$(INTDIR)\ApacheXMLParser.obj"	"$(INTDIR)\ApacheXMLParser.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Socket.pch"


SOURCE=.\BtnST.cpp

"$(INTDIR)\BtnST.obj"	"$(INTDIR)\BtnST.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Socket.pch"


SOURCE=.\CWzdVehicleAdd.cpp

"$(INTDIR)\CWzdVehicleAdd.obj"	"$(INTDIR)\CWzdVehicleAdd.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Socket.pch"


SOURCE=.\debug_functions.cpp

"$(INTDIR)\debug_functions.obj"	"$(INTDIR)\debug_functions.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Socket.pch"


SOURCE=.\DevicesCodingManager.cpp

"$(INTDIR)\DevicesCodingManager.obj"	"$(INTDIR)\DevicesCodingManager.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Socket.pch"


SOURCE=.\diskid32.cpp

"$(INTDIR)\diskid32.obj"	"$(INTDIR)\diskid32.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Socket.pch"


SOURCE=.\DlgAddAClientUser.cpp

"$(INTDIR)\DlgAddAClientUser.obj"	"$(INTDIR)\DlgAddAClientUser.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Socket.pch"


SOURCE=.\DlgAddaDriver.cpp

"$(INTDIR)\DlgAddaDriver.obj"	"$(INTDIR)\DlgAddaDriver.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Socket.pch"


SOURCE=.\DlgChoiceAppVer.cpp

"$(INTDIR)\DlgChoiceAppVer.obj"	"$(INTDIR)\DlgChoiceAppVer.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Socket.pch"


SOURCE=.\DlgCSWQuery.cpp

"$(INTDIR)\DlgCSWQuery.obj"	"$(INTDIR)\DlgCSWQuery.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Socket.pch"


SOURCE=.\DlgLoginSystem.cpp

"$(INTDIR)\DlgLoginSystem.obj"	"$(INTDIR)\DlgLoginSystem.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Socket.pch"


SOURCE=.\DlgModifyClientPassword.cpp

"$(INTDIR)\DlgModifyClientPassword.obj"	"$(INTDIR)\DlgModifyClientPassword.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Socket.pch"


SOURCE=.\DlgMsgQuery.cpp

"$(INTDIR)\DlgMsgQuery.obj"	"$(INTDIR)\DlgMsgQuery.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Socket.pch"


SOURCE=.\DlgPosQuery.cpp

"$(INTDIR)\DlgPosQuery.obj"	"$(INTDIR)\DlgPosQuery.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Socket.pch"


SOURCE=.\DlgQueryVhcDispatch.cpp

"$(INTDIR)\DlgQueryVhcDispatch.obj"	"$(INTDIR)\DlgQueryVhcDispatch.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Socket.pch"


SOURCE=.\DlgUniformStyle.cpp

"$(INTDIR)\DlgUniformStyle.obj"	"$(INTDIR)\DlgUniformStyle.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Socket.pch"


SOURCE=.\DlgVehicleQuery.cpp

"$(INTDIR)\DlgVehicleQuery.obj"	"$(INTDIR)\DlgVehicleQuery.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Socket.pch"


SOURCE=.\DlgVhcPswModify.cpp

"$(INTDIR)\DlgVhcPswModify.obj"	"$(INTDIR)\DlgVhcPswModify.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Socket.pch"


SOURCE=.\ManipulateRegistry.cpp

"$(INTDIR)\ManipulateRegistry.obj"	"$(INTDIR)\ManipulateRegistry.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Socket.pch"


SOURCE=.\ManipulateServerApp.cpp

"$(INTDIR)\ManipulateServerApp.obj"	"$(INTDIR)\ManipulateServerApp.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Socket.pch"


SOURCE=.\MonitorSetting.cpp

"$(INTDIR)\MonitorSetting.obj"	"$(INTDIR)\MonitorSetting.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Socket.pch"


SOURCE=.\MyPropertySheet.cpp

"$(INTDIR)\MyPropertySheet.obj"	"$(INTDIR)\MyPropertySheet.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Socket.pch"


SOURCE=.\Socket.cpp

"$(INTDIR)\Socket.obj"	"$(INTDIR)\Socket.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Socket.pch"


SOURCE=.\Socket.rc

"$(INTDIR)\Socket.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\SQL_Excute.cpp

"$(INTDIR)\SQL_Excute.obj"	"$(INTDIR)\SQL_Excute.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Socket.pch"


SOURCE=.\SQLConvert.cpp

"$(INTDIR)\SQLConvert.obj"	"$(INTDIR)\SQLConvert.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Socket.pch"


SOURCE=.\SQLExecutor.cpp

"$(INTDIR)\SQLExecutor.obj"	"$(INTDIR)\SQLExecutor.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Socket.pch"


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "Socket - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_WINDLL" /D "_MBCS" /D "_AFXEXT" /D "RUI_OP" /D "_NEEDLOG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Socket.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\Socket.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Socket - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUGfs" /D "_DEBUG" /D "_WINDLL" /D "_MBCS" /D "_AFXEXT" /D "RUI_OP" /D "_NEEDLOG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Socket.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\Socket.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\WzdVehicleAdd_4.cpp

"$(INTDIR)\WzdVehicleAdd_4.obj"	"$(INTDIR)\WzdVehicleAdd_4.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Socket.pch"


SOURCE=.\XPButton.cpp

"$(INTDIR)\XPButton.obj"	"$(INTDIR)\XPButton.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Socket.pch"



!ENDIF 

