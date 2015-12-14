////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// Wednesday, January 30, 2008: Created by Allen Xu 
//
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_TorturerGUI_MDIWindow_h
#define Aos_TorturerGUI_MDIWindow_h

#include <qmainwindow.h>

enum MDIWindowType
{
	eMDIWnd_RVG = 0,
	eMDIWnd_Table
};


class MDIWindow : public QMainWindow
{
	Q_OBJECT

protected:
	QString mFilename;
	bool mIsModified;

public:
	MDIWindow( QWidget* parent = 0, const char* name = 0, WFlags wflags = 0 );
	~MDIWindow();

	virtual void save() = 0;
	virtual void saveAs() = 0;

protected:
    void closeEvent( QCloseEvent * );

signals:
	void message(const QString&, int );

};

#endif

