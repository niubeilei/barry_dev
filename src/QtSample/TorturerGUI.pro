TEMPLATE	= app
LANGUAGE	= C++

CONFIG	+= qt warn_on release

LIBS	+= -L../../lib -lProgAid -lAction -lRules -lTorturer -lConditions -lSemanticData -lAction -lSemantics -lRules -lProgAid -lRVG -lRandom -lCliClient -lTcpAppServer -lKernelAPI -laos -lKernelUtil -lTester -lAppMgr -lThread -laosUtil -laos_alarms -lLogicExpr -lUtilComm -lConfig -lPorting -lSingleton -lAlarm -lXmlParser -lPorting -lTracer -lDebug -laos_util -lUtil1 -laosUtil -lUtilComm -lOmnUtil -lUtilComm -lUtil1 -lDebug -lUtilComm -lXmlLib -lz -lnsl -lpthread -lstdc++

DEFINES	+= OMN_PLATFORM_UNIX

INCLUDEPATH	+= ../

HEADERS	+= TorTree.h \
	MDIWndRVG.h \
	MDIWindow.h

SOURCES	+= main.cpp \
	TorTree.cpp \
	MDIWndRVG.cpp \
	MDIWindow.cpp

FORMS	= MainWindow.ui \
	UI/rvgeditorui.ui \
	UI/NewRVGDialog.ui \
	UI/RVGEditorDlg.ui \
	UI/RVGTestDlg.ui

IMAGES	= images/filenew \
	images/fileopen \
	images/filesave \
	images/print \
	images/undo \
	images/redo \
	images/editcut \
	images/editcopy \
	images/editpaste \
	images/searchfind \
	images/filenew_1 \
	images/fileopen_1 \
	images/filesave_1 \
	images/print_1 \
	images/undo_1 \
	images/redo_1 \
	images/editcut_1 \
	images/editcopy_1 \
	images/editpaste_1 \
	images/searchfind_1 \
	images/filenew_2 \
	images/fileopen_2 \
	images/filesave_2 \
	images/print_2 \
	images/undo_2 \
	images/redo_2 \
	images/editcut_2 \
	images/editcopy_2 \
	images/editpaste_2 \
	images/searchfind_2

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}

