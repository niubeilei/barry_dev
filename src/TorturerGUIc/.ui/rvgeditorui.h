/****************************************************************************
** Form interface generated from reading ui file 'UI/rvgeditorui.ui'
**
** Created: Sun Mar 16 15:36:05 2008
**      by: The User Interface Compiler ($Id: rvgeditorui.h,v 1.2 2015/01/06 08:57:52 andy Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef RVGEDITORUI_H
#define RVGEDITORUI_H

#include <qvariant.h>
#include <qmainwindow.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QAction;
class QActionGroup;
class QToolBar;
class QPopupMenu;
class QGroupBox;
class QLabel;
class QLineEdit;
class QTable;
class QPushButton;
class QCheckBox;
class QComboBox;

class RVGEditorUI : public QMainWindow
{
    Q_OBJECT

public:
    RVGEditorUI( QWidget* parent = 0, const char* name = 0, WFlags fl = WType_TopLevel );
    ~RVGEditorUI();

    QGroupBox* mCommonGroupBox;
    QLabel* mNameLabel;
    QLabel* mWeightLabel;
    QLabel* mValueTypeLabel;
    QLabel* mRVGTypeLabel;
    QLineEdit* mNamelineEdit;
    QLineEdit* mWeightlineEdit;
    QLineEdit* mTypeLineEdit;
    QLineEdit* mRVGTypeLineEdit;
    QGroupBox* mValuesGroupBox;
    QTable* mValuesTable;
    QPushButton* mAddValueBtn;
    QPushButton* mDelValueBtn;
    QPushButton* mValueEditBtn;
    QGroupBox* mOtherGroupBox;
    QLabel* mMinLabel;
    QLabel* mMaxLabel;
    QCheckBox* mUniqueCheckBox;
    QCheckBox* mOrderedCheckBox;
    QLineEdit* mMinLineEdit;
    QLineEdit* mMaxLineEdit;
    QGroupBox* mSpecialGroupBox;
    QLabel* mSeperatorLabel;
    QComboBox* mSeperatorComboBox;

public slots:
    virtual void RVGEditorUI_usesTextLabelChanged( bool );
    virtual void AddValueBtn_clicked();
    virtual void DelValueBtn_clicked();
    virtual void ValueEditBtn_clicked();
    virtual void NamelineEdit_textChanged( const QString & );
    virtual void WeightlineEdit_textChanged( const QString & );
    virtual void TypeLineEdit_textChanged( const QString & );
    virtual void RVGTypeLineEdit_textChanged( const QString & );
    virtual void MinLineEdit_textChanged( const QString & );
    virtual void SeperatorComboBox_textChanged( const QString & );
    virtual void MaxLineEdit_textChanged( const QString & );

protected:

protected slots:
    virtual void languageChange();

};

#endif // RVGEDITORUI_H
