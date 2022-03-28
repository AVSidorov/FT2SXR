# Microsoft Developer Studio Project File - Name="BRD" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=BRD - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "brd.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "brd.mak" CFG="BRD - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "BRD - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "BRD - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "BRD - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "BRD_EXPORTS" /YX /FD /c
# ADD CPP /nologo /Zp1 /MD /W3 /GX /Od /I "..\BRDINC" /I "..\..\..\brdinc" /I "..\..\..\brdshell" /I "..\..\..\BrdLibs" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "BRD_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"..\BIN\BRD.dll"

!ELSEIF  "$(CFG)" == "BRD - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "BRD_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /Zp1 /MDd /W3 /Gm /GX /ZI /Od /I "..\BRDINC" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "BRD_EXPORTS" /D "NOT_DEASY_SUPPORT" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"..\BIN\BRD.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "BRD - Win32 Release"
# Name "BRD - Win32 Debug"
# Begin Group "Others"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\V01\Abrd\Abrd.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\BRD.def
# End Source File
# Begin Source File

SOURCE=.\brd.rc
# End Source File
# Begin Source File

SOURCE=.\brd01.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\brdctrl.h
# End Source File
# Begin Source File

SOURCE=.\Getregry.cpp
# End Source File
# Begin Source File

SOURCE=.\idxbrd.cpp
# End Source File
# Begin Source File

SOURCE=.\idxbrd.h
# End Source File
# Begin Source File

SOURCE=.\utypes.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\brd.cpp
# End Source File
# Begin Source File

SOURCE=..\Brdinc\brd.h
# End Source File
# Begin Source File

SOURCE=..\Brdinc\brdapi.h
# End Source File
# Begin Source File

SOURCE=..\Brdinc\brderr.h
# End Source File
# Begin Source File

SOURCE=.\brdi.h
# End Source File
# Begin Source File

SOURCE=.\brdinit.cpp
# End Source File
# Begin Source File

SOURCE=.\brdisup.cpp
# End Source File
# Begin Source File

SOURCE=.\brdpu.cpp
# End Source File
# Begin Source File

SOURCE=.\brdreinit.cpp
# End Source File
# Begin Source File

SOURCE=.\brdserv.cpp
# End Source File
# Begin Source File

SOURCE=.\captor.cpp
# End Source File
# Begin Source File

SOURCE=.\captor.h
# End Source File
# Begin Source File

SOURCE=.\Dbprintf.cpp
# End Source File
# End Target
# End Project
