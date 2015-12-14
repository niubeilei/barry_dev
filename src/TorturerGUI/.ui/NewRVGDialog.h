/****************************************************************************
** Form interface generated from reading ui file 'UI/NewRVGDialog.ui'
**
** Created: Sun Mar 16 15:36:05 2008
**      by: The User Interface Compiler ($Id: NewRVGDialog.h,v 1.2 2015/01/06 08:57:51 andy Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef NEWRVGDIALOG_H
#define NEWRVGDIALOG_H

#include <qvariant.h>
#include <qdialog.h>
#include "RVG/Util.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QLabel;
class QPushButton;
class QComboBox;
class QIconView;
class QIconViewItem;

class NewRVGDialog : public QDialog
{
    Q_OBJECT

public:
    NewRVGDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~NewRVGDialog();

    QLabel* mRVGsLabel;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QComboBox* mProductNamecomboBox;
    QLabel* mProductNameLabel;
    QIconView* mRVGsIconView;

public slots:
    virtual void init();
    virtual std::string getProductName();
    virtual AosRVGType::E getRVGType();

protected:
    QHBoxLayout* Layout1;
    QSpacerItem* Horizontal_Spacing2;

protected slots:
    virtual void languageChange();

};

#endif // NEWRVGDIALOG_H
