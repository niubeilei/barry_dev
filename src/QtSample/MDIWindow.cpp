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

#include "MDIWindow.h"
#include <qmessagebox.h>


MDIWindow::MDIWindow( QWidget* parent, const char* name, WFlags wflags )
:QMainWindow(parent, name, wflags),
mIsModified(false)
{
}


MDIWindow::~MDIWindow()
{
}


void 
MDIWindow::closeEvent( QCloseEvent *e )
{
	if (mIsModified) 
	{
		switch( QMessageBox::warning( this, "Save Changes",
					tr("Save changes to %1?").arg( caption() ),
					tr("Yes"), tr("No"), tr("Cancel") ) ) 
		{
			case 0:
				{
					save();
				//	if ( !mFilename.isEmpty() )
						e->accept();
				//	else
				//		e->ignore();
				}
				break;
			case 1:
				e->accept();
				break;
			default:
				e->ignore();
				break;
		}
	} 
	else 
	{
		e->accept();
	}
}

