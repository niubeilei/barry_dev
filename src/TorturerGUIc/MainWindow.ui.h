/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/








void MainWindow::helpIndex()
{

}


void MainWindow::helpContents()
{

}


void MainWindow::helpAbout()
{

}




void MainWindow::initTorTree()
{
    QDockWindow *dw = new QDockWindow( QDockWindow::InDock, this );
    dw->setResizeEnabled( TRUE );
    dw->setVerticalStretchable( TRUE );
    addDockWindow( dw, DockLeft );
    setDockEnabled( dw, DockTop, FALSE );
    setDockEnabled( dw, DockBottom, FALSE );
    dw->setCloseMode( QDockWindow::Always );

    mTorTree = AosTorTree::instance( dw );//new AosTorTree(dw); 
	mTorTree->init();
    dw->setWidget( mTorTree );
    dw->setCaption( tr( "Objects Browser" ) );
    
    connect(mTorTree, SIGNAL(doubleClicked(QListViewItem*)), this, SLOT(newMDIWnd(QListViewItem*)));
    
}


void MainWindow::initWorkspace()
{
    QVBox* vb = new QVBox( this );
    vb->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
    mWorkSpace = new QWorkspace( vb );
    mWorkSpace->setScrollBarsEnabled( TRUE );
    setCentralWidget( vb );

    statusBar()->message( "Ready", 2000 );
}


void MainWindow::newMDIWnd( QListViewItem * item )
{
    AosBaseTreeItem* base = dynamic_cast<AosBaseTreeItem*>(item);
    if (base)
    {
		MDIWindow* wnd = NULL;
		switch (base->getType())
		{
			case eTreeItem_Table:
				//	    wnd = new MDIWndTable(mWorkSpace, 0, WDestructiveClose);
				break;
			case eTreeItem_RVG:
				{
					AosRVGTreeItem* item = dynamic_cast<AosRVGTreeItem*>(base);
					if (item)
					{
						AosRVGPtr ptr = item->getRVG();
						newRVGWindow(ptr, item);
					}
					break;
				}
			case eTreeItem_Command:
				//	    wnd = new MDIWndCommand(mWorkSpace, 0, WDestructiveClose);
				break;
			case eTreeItem_Module:
				//	    wnd = new MDIWndModule(mWorkSpace, 0, WDestructiveClose);
				break;
			case eTreeItem_Torturer:
				//	    wnd = new MDIWndTorturer(mWorkSpace, 0, WDestructiveClose);
				break;
			default:
				break;
		}
    }
}


void MainWindow::windowCloseAction_activated()
{
    mWorkSpace->closeActiveWindow();
/*    MDIWindow* wnd = (MDIWindow*)mWorkSpace->activeWindow();
    if (wnd)
    {
	wnd->close();
    }*/
}


void MainWindow::windowClose_AllAction_activated()
{
    mWorkSpace->closeAllWindows();
/*    QWidgetList windows = mWorkSpace->windowList();
    int wndCount = windows.count();

    for (int i=0; i<wndCount; i++)
    {
	QWidget *wnd = windows.at(i);
	if (wnd)
	{
	    wnd->close();
	}
    }*/
}


void MainWindow::windowCascadeAction_activated()
{
    mWorkSpace->cascade();
}


void MainWindow::windowTitleAction_activated()
{
    mWorkSpace->tile();
}


void MainWindow::windowTitle_HorizontallyAction_activated()
{
     // primitive horizontal tiling
    QWidgetList windows = mWorkSpace->windowList();
     if ( !windows.count() )
     return;
 
     int heightForEach = mWorkSpace->height() / windows.count();
     int y = 0;
     for ( int i = 0; i < int(windows.count()); ++i ) 
     {
	 QWidget *window = windows.at(i);
	 if ( window->testWState( WState_Maximized ) ) 
	 {
	     // prevent flicker
	     window->hide();
	     window->showNormal();
	 }
	 int preferredHeight = window->minimumHeight()+window->parentWidget()->baseSize().height();
	 int actHeight = QMAX(heightForEach, preferredHeight);
	 
	 window->parentWidget()->setGeometry( 0, y, mWorkSpace->width(), actHeight );
	 y += actHeight;
     }

}


void MainWindow::newRVGWindow( AosRVGPtr & rvg, AosRVGTreeItem *item)
{
    QWidgetList windows = mWorkSpace->windowList();
    for ( int i = 0; i < int(windows.count()); ++i ) 
     {
	 MDIWndRVG* rvgWnd = dynamic_cast<MDIWndRVG*>( windows.at(i));
	 if (rvgWnd && rvgWnd->getRVG() == rvg)
	 {
	     rvgWnd->showNormal();
	     rvgWnd->setFocus();
	     return;
	 }
     }
	 
	MDIWndRVG* wnd = NULL;
    wnd = new MDIWndRVG(rvg, mWorkSpace, 0, WDestructiveClose);
	wnd->setTreeItem(item);
    wnd->setCaption("RVG Editor");
	if (mWorkSpace->windowList().isEmpty())
	{
		wnd->showMaximized();
	}
	else
	{
		wnd->show();
	}
}


void MainWindow::newRVGAction()
{
    NewRVGDialog dlg(this, "NewRVGDialog", TRUE);
    if (dlg.exec() == QDialog::Accepted)
    {
		AosRVGPtr rvg = AosRVG::RVGFactory(dlg.getRVGType());
		rvg->setProductName(dlg.getProductName());
		newRVGWindow(rvg, NULL);
	}
}


void MainWindow::SaveObjectAction()
{
    MDIWindow* wnd = (MDIWindow*)mWorkSpace->activeWindow();
    if (wnd)
    {
		wnd->save();
    }
}



void MainWindow::RVGDeleteAction_activated()
{
	AosBaseTreeItem* item = (AosBaseTreeItem*)mTorTree->selectedItem();
	if (item == NULL)
	{
		return;
	}
	if (item->getType() == eTreeItem_RVG)
	{
		AosRVGTreeItem* rvgItem = (AosRVGTreeItem*)item;
		if (rvgItem &&  rvgItem->getRVG())
		{
			switch( QMessageBox::warning( this, "Delete RVG",
						tr("Delete the RVG or not?"),
						tr("Yes"), tr("No"), 0, 0, 1)) 
			{
				case 0:
					{
						rvgItem->getRVG()->deleteConfigFile();
						delete item;	    
					}
					break;
				case 1:
					break;
				default:
					break;
			}
		}
    }
}


void MainWindow::RVGTestAction_activated()
{
    AosBaseTreeItem* item = (AosBaseTreeItem*)mTorTree->selectedItem();
    if (item == NULL)
    {
	return;
    }
    if (item->getType() == eTreeItem_RVG)
    {   
	AosRVGTreeItem* rvgItem = (AosRVGTreeItem*)item;
	if (rvgItem &&  rvgItem->getRVG())
	{
	    RVGTestDlg dlg(this, "TestRVGDialog", TRUE);
	    dlg.mRVG = rvgItem->getRVG();
	    dlg.initialize();
	    dlg.exec();
	}
    }
}
