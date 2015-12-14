/****************************************************************************
** Form interface generated from reading ui file 'UI/RVGTestDlg.ui'
**
** Created: Sun Mar 16 15:36:05 2008
**      by: The User Interface Compiler ($Id: RVGTestDlg.h,v 1.2 2015/01/06 08:57:52 andy Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef RVGTESTDLG_H
#define RVGTESTDLG_H

#include <qvariant.h>
#include <qdialog.h>
#include <qlistview.h>
#include "RVG/Ptrs.h"
#include "RVG/RVG.h"
#include "TorTree.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QListBox;
class QListBoxItem;

class RVGTestDlg : public QDialog
{
    Q_OBJECT

public:
    RVGTestDlg( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~RVGTestDlg();

    QGroupBox* mRVGGroupBox;
    QLabel* mRVGNameLabel;
    QLineEdit* mProductNameLineEdit;
    QLineEdit* mRVGNamelineEdit;
    QLabel* mProductNameLabel;
    QPushButton* buttonOk;
    QGroupBox* mPolicyGroupBox;
    QLabel* mTestTimes;
    QLabel* mValuesLabel;
    QListBox* mValuesListBox;
    QLineEdit* mTestTimesLineEdit;
    QPushButton* mStartTestBtn;

    AosRVGPtr mRVG;

    virtual void initialize();

public slots:
    virtual void startTestBtn_clicked();

protected:
    QHBoxLayout* Layout1;
    QSpacerItem* Horizontal_Spacing2;

protected slots:
    virtual void languageChange();

};

#endif // RVGTESTDLG_H
