# Microsoft Developer Studio Project File - Name="sshx" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=sshx - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SSHX.MAK".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SSHX.MAK" CFG="sshx - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "sshx - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "sshx - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "sshx - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /I "source\./" /I "source\charset/" /I "source\windows/" /I "source\unix/" /I "source\mac/" /I "source\macosx/" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "source\./" /I "source\charset/" /I "source\windows/" /I "source\unix/" /I "source\mac/" /I "source\macosx/" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "NO_IPV6" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 advapi32.lib comctl32.lib comdlg32.lib gdi32.lib imm32.lib shell32.lib user32.lib winmm.lib winspool.lib ws2_32.lib /nologo /subsystem:windows /machine:I386 /out:"sshX.exe"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "sshx - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "source\./" /I "source\charset/" /I "source\windows/" /I "source\unix/" /I "source\mac/" /I "source\macosx/" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "source\./" /I "source\charset/" /I "source\windows/" /I "source\unix/" /I "source\mac/" /I "source\macosx/" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "NO_IPV6" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 comctl32.lib imm32.lib winmm.lib ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"SSHX.exe" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "sshx - Win32 Release"
# Name "sshx - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=source\be_all.c
# End Source File
# Begin Source File

SOURCE=source\cmdline.c
# End Source File
# Begin Source File

SOURCE=source\config.c
# End Source File
# Begin Source File

SOURCE=source\cproxy.c
# End Source File
# Begin Source File

SOURCE=source\dialog.c
# End Source File
# Begin Source File

SOURCE=source\ldisc.c
# End Source File
# Begin Source File

SOURCE=source\ldiscucs.c
# End Source File
# Begin Source File

SOURCE=source\minibidi.c
# End Source File
# Begin Source File

SOURCE=source\misc.c
# End Source File
# Begin Source File

SOURCE=source\pinger.c
# End Source File
# Begin Source File

SOURCE=source\portfwd.c
# End Source File
# Begin Source File

SOURCE=source\pproxy.c
# End Source File
# Begin Source File

SOURCE=source\proxy.c
# End Source File
# Begin Source File

SOURCE=source\raw.c
# End Source File
# Begin Source File

SOURCE=source\rlogin.c
# End Source File
# Begin Source File

SOURCE=source\settings.c
# End Source File
# Begin Source File

SOURCE=source\windows\sizetip.c
# End Source File
# Begin Source File

SOURCE=source\ssh.c

!IF  "$(CFG)" == "sshx - Win32 Release"

!ELSEIF  "$(CFG)" == "sshx - Win32 Debug"

# ADD CPP /Zi

!ENDIF 

# End Source File
# Begin Source File

SOURCE=source\sshaes.c
# End Source File
# Begin Source File

SOURCE=source\sshblowf.c
# End Source File
# Begin Source File

SOURCE=source\sshbn.c
# End Source File
# Begin Source File

SOURCE=source\sshcrc.c
# End Source File
# Begin Source File

SOURCE=source\sshcrcda.c
# End Source File
# Begin Source File

SOURCE=source\sshdes.c
# End Source File
# Begin Source File

SOURCE=source\sshdh.c
# End Source File
# Begin Source File

SOURCE=source\sshdss.c
# End Source File
# Begin Source File

SOURCE=source\sshmd5.c
# End Source File
# Begin Source File

SOURCE=source\sshpubk.c
# End Source File
# Begin Source File

SOURCE=source\sshrand.c
# End Source File
# Begin Source File

SOURCE=source\sshrsa.c
# End Source File
# Begin Source File

SOURCE=source\sshsh512.c
# End Source File
# Begin Source File

SOURCE=source\sshsha.c
# End Source File
# Begin Source File

SOURCE=source\sshzlib.c
# End Source File
# Begin Source File

SOURCE=source\telnet.c
# End Source File
# Begin Source File

SOURCE=source\terminal.c
# End Source File
# Begin Source File

SOURCE=source\timing.c
# End Source File
# Begin Source File

SOURCE=source\tree234.c
# End Source File
# Begin Source File

SOURCE=source\version.c
# End Source File
# Begin Source File

SOURCE=source\wcwidth.c
# End Source File
# Begin Source File

SOURCE=source\wildcard.c
# End Source File
# Begin Source File

SOURCE=source\windows\wincfg.c
# End Source File
# Begin Source File

SOURCE=source\windows\winctrls.c
# End Source File
# Begin Source File

SOURCE=source\windows\windefs.c
# End Source File
# Begin Source File

SOURCE=source\windows\windlg.c
# End Source File
# Begin Source File

SOURCE=source\windows\window.c
# End Source File
# Begin Source File

SOURCE=source\windows\winmisc.c
# End Source File
# Begin Source File

SOURCE=source\windows\winnet.c
# End Source File
# Begin Source File

SOURCE=source\windows\winnoise.c
# End Source File
# Begin Source File

SOURCE=source\windows\winpgntc.c
# End Source File
# Begin Source File

SOURCE=source\windows\winprint.c
# End Source File
# Begin Source File

SOURCE=source\windows\winstore.c
# End Source File
# Begin Source File

SOURCE=source\windows\wintime.c
# End Source File
# Begin Source File

SOURCE=source\windows\winucs.c
# End Source File
# Begin Source File

SOURCE=source\windows\winutils.c
# End Source File
# Begin Source File

SOURCE=source\x11fwd.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=source\charset\charset.h
# End Source File
# Begin Source File

SOURCE=source\dialog.h
# End Source File
# Begin Source File

SOURCE=source\int64.h
# End Source File
# Begin Source File

SOURCE=source\ldisc.h
# End Source File
# Begin Source File

SOURCE=source\macstuff.h
# End Source File
# Begin Source File

SOURCE=source\misc.h
# End Source File
# Begin Source File

SOURCE=source\network.h
# End Source File
# Begin Source File

SOURCE=source\osx.h
# End Source File
# Begin Source File

SOURCE=source\proxy.h
# End Source File
# Begin Source File

SOURCE=source\windows\rcstuff.h
# End Source File
# Begin Source File

SOURCE=.\source\windows\resource.h
# End Source File
# Begin Source File

SOURCE=source\ssh.h
# End Source File
# Begin Source File

SOURCE=source\sshx.h
# End Source File
# Begin Source File

SOURCE=source\sshxmem.h
# End Source File
# Begin Source File

SOURCE=source\sshxps.h
# End Source File
# Begin Source File

SOURCE=source\storage.h
# End Source File
# Begin Source File

SOURCE=source\terminal.h
# End Source File
# Begin Source File

SOURCE=source\tree234.h
# End Source File
# Begin Source File

SOURCE=source\unix\unix.h
# End Source File
# Begin Source File

SOURCE=source\windows\win_res.h
# End Source File
# Begin Source File

SOURCE=source\windows\winhelp.h
# End Source File
# Begin Source File

SOURCE=source\windows\winstuff.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\source\windows\PUTTY.ICO
# End Source File
# Begin Source File

SOURCE=.\source\windows\PUTTYCFG.ICO
# End Source File
# Begin Source File

SOURCE=source\windows\sshx.ico
# End Source File
# Begin Source File

SOURCE=source\windows\sshxcfg.ico
# End Source File
# Begin Source File

SOURCE=source\windows\win_res.rc
# End Source File
# End Group
# Begin Group "My Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\source\aos_auth.c
# End Source File
# Begin Source File

SOURCE=.\aosWinLogFunc.c
# End Source File
# End Group
# Begin Group "My Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\source\aos_auth.h
# End Source File
# Begin Source File

SOURCE=.\aosWinLogFunc.h
# End Source File
# Begin Source File

SOURCE=source\logging.c
# End Source File
# End Group
# Begin Source File

SOURCE=.\source\windows\sshX.mft
# End Source File
# End Target
# End Project
