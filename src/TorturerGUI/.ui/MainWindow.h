/****************************************************************************
** Form interface generated from reading ui file 'MainWindow.ui'
**
** Created: Sun Mar 16 15:36:05 2008
**      by: The User Interface Compiler ($Id: MainWindow.h,v 1.2 2015/01/06 08:57:51 andy Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qvariant.h>
#include <qmainwindow.h>
#include <qlistview.h>
#include "RVG/Ptrs.h"
#include "RVG/RVG.h"
#include "TorTree.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QAction;
class QActionGroup;
class QToolBar;
class QPopupMenu;
class QWorkspace;
class AosTorTree;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow( QWidget* parent = 0, const char* name = 0, WFlags fl = WType_TopLevel );
    ~MainWindow();

    QMenuBar *MenuBar;
    QPopupMenu *Project;
    QPopupMenu *RVG;
    QPopupMenu *Table;
    QPopupMenu *Command;
    QPopupMenu *Module;
    QPopupMenu *Torturer;
    QPopupMenu *Window;
    QPopupMenu *helpMenu;
    QToolBar *toolBar;
    QAction* RVGNewAction;
    QAction* SaveAction;
    QAction* helpContentsAction;
    QAction* helpIndexAction;
    QAction* helpAboutAction;
    QAction* mainOpen_projectAction;
    QAction* mainCreate_projectAction;
    QAction* projectExitAction;
    QAction* windowCloseAction;
    QAction* windowClose_AllAction;
    QAction* window_Action;
    QAction* windowTitleAction;
    QAction* windowCascadeAction;
    QAction* windowTitle_HorizontallyAction;
    QAction* RVGDeleteAction;
    QAction* RVGTestAction;

public slots:
    virtual void helpIndex();
    virtual void helpContents();
    virtual void helpAbout();
    virtual void initTorTree();
    virtual void initWorkspace();
    virtual void newMDIWnd( QListViewItem * item );
    virtual void windowCloseAction_activated();
    virtual void windowClose_AllAction_activated();
    virtual void windowCascadeAction_activated();
    virtual void windowTitleAction_activated();
    virtual void windowTitle_HorizontallyAction_activated();
    virtual void newRVGWindow( AosRVGPtr & rvg, AosRVGTreeItem * item );
    virtual void newRVGAction();
    virtual void SaveObjectAction();
    virtual void RVGDeleteAction_activated();
    virtual void RVGTestAction_activated();

protected:

protected slots:
    virtual void languageChange();

private:
    QWorkspace* mWorkSpace;
    AosTorTree* mTorTree;

};

#endif // MAINWINDOW_H
