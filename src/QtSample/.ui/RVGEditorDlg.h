/****************************************************************************
** Form interface generated from reading ui file 'UI/RVGEditorDlg.ui'
**
** Created: Wed Feb 20 23:57:06 2008
**      by: The User Interface Compiler ($Id: RVGEditorDlg.h,v 1.2 2015/01/06 08:57:51 andy Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef RVGEDITORDLG_H
#define RVGEDITORDLG_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QGroupBox;
class QPushButton;
class QTable;
class QLabel;
class QCheckBox;
class QLineEdit;
class QComboBox;

class RVGEditorDlg : public QDialog
{
    Q_OBJECT

public:
    RVGEditorDlg( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~RVGEditorDlg();

    QGroupBox* mValuesGroupBox;
    QPushButton* mValueEditBtn;
    QPushButton* mAddValueBtn;
    QPushButton* mDelValueBtn;
    QTable* mValuesTable;
    QGroupBox* mOtherGroupBox;
    QLabel* mMinLabel;
    QLabel* mMaxLabel;
    QCheckBox* mUniqueCheckBox;
    QCheckBox* mOrderedCheckBox;
    QLineEdit* mMinLineEdit;
    QLineEdit* mMaxLineEdit;
    QGroupBox* mSpecialGroupBox;
    QLabel* mSeperatorLabel;
    QLineEdit* mSeperatorLineEdit;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QGroupBox* mCommonGroupBox;
    QLabel* mNameLabel;
    QLabel* mWeightLabel;
    QLabel* mValueTypeLabel;
    QLabel* mRVGTypeLabel;
    QLineEdit* mNamelineEdit;
    QLineEdit* mWeightlineEdit;
    QComboBox* mTypeComboBox;
    QComboBox* mRVGTypeComboBox;

protected:
    QHBoxLayout* Layout1;
    QSpacerItem* Horizontal_Spacing2;

protected slots:
    virtual void languageChange();

};

#endif // RVGEDITORDLG_H
