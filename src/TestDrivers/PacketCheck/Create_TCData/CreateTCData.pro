TEMPLATE	= app
LANGUAGE	= C++

CONFIG	+= qt warn_on release

HEADERS	+= PCTestcaseConfig.h

SOURCES	+= main.cpp \
	PCTestcaseConfig.cpp

FORMS	= mydialog.ui \
	tceditdlg.ui

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}


