/****************************************************************************
** Form interface generated from reading ui file 'MDIWndRVG.ui'
**
** Created: Wed Jan 30 18:36:17 2008
**      by: The User Interface Compiler ($Id: MDIWndRVG.h,v 1.2 2015/01/06 08:57:50 andy Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef MDIWNDRVG_H
#define MDIWNDRVG_H

#include <qvariant.h>
#include <qmainwindow.h>
#include "MDIWindow.h"
#include "RVG/RVG.h"
#include "RVG/Ptrs.h"
#include "TorTree.h"

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

class MDIWndRVG : public MDIWindow
{
    Q_OBJECT

public:
    MDIWndRVG(AosRVGPtr& rvg,  QWidget* parent = 0, const char* name = 0, WFlags fl = WType_TopLevel );
    ~MDIWndRVG();
	void init();
	AosRVGPtr getRVG();
	AosRVGTreeItem* getTreeItem();
	void setTreeItem(AosRVGTreeItem* item);

	virtual void save();
	virtual void saveAs();

    QGroupBox* mCommonGroupBox;
    QLabel* mNameLabel;
    QLabel* mWeightLabel;
    QLabel* mValueTypeLabel;
    QLabel* mRVGTypeLabel;
    QLineEdit* mNamelineEdit;
    QLineEdit* mWeightlineEdit;
    QComboBox* mTypeLineEdit;
    QComboBox* mRVGTypeLineEdit;
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
    QLineEdit* mSeperatorComboBox;

public slots:
    virtual void ValuesTable_TextLabelChanged( bool );
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
	AosRVGPtr	mRVG;
	AosRVGTreeItem*	mTreeItem;

private:
	void initRIGValueSelectorTableHeader();


protected slots:
    virtual void languageChange();

};

#endif // MDIWNDRVG_H
