/****************************************************************************
** Form implementation generated from reading ui file 'UI/NewRVGDialog.ui'
**
** Created: Wed Feb 20 23:57:23 2008
**      by: The User Interface Compiler ($Id: NewRVGDialog.cpp,v 1.2 2015/01/06 08:57:50 andy Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "NewRVGDialog.h"

#include <qvariant.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qiconview.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "Torturer/Product.h"
#include "../UI/NewRVGDialog.ui.h"
/*
 *  Constructs a NewRVGDialog as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
NewRVGDialog::NewRVGDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "NewRVGDialog" );
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)2, (QSizePolicy::SizeType)2, 0, 0, sizePolicy().hasHeightForWidth() ) );
    setBaseSize( QSize( 400, 300 ) );
    setSizeGripEnabled( TRUE );
    setModal( TRUE );

    mRVGsLabel = new QLabel( this, "mRVGsLabel" );
    mRVGsLabel->setGeometry( QRect( 20, 60, 66, 20 ) );

    QWidget* privateLayoutWidget = new QWidget( this, "Layout1" );
    privateLayoutWidget->setGeometry( QRect( 60, 280, 360, 33 ) );
    Layout1 = new QHBoxLayout( privateLayoutWidget, 0, 6, "Layout1"); 
    Horizontal_Spacing2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout1->addItem( Horizontal_Spacing2 );

    buttonOk = new QPushButton( privateLayoutWidget, "buttonOk" );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    Layout1->addWidget( buttonOk );

    buttonCancel = new QPushButton( privateLayoutWidget, "buttonCancel" );
    buttonCancel->setAutoDefault( TRUE );
    Layout1->addWidget( buttonCancel );

    mProductNamecomboBox = new QComboBox( FALSE, this, "mProductNamecomboBox" );
    mProductNamecomboBox->setGeometry( QRect( 90, 20, 200, 22 ) );

    mProductNameLabel = new QLabel( this, "mProductNameLabel" );
    mProductNameLabel->setGeometry( QRect( 20, 20, 66, 20 ) );

    mRVGsIconView = new QIconView( this, "mRVGsIconView" );
    mRVGsIconView->setGeometry( QRect( 90, 60, 330, 200 ) );
    mRVGsIconView->setArrangement( QIconView::TopToBottom );
    mRVGsIconView->setResizeMode( QIconView::Adjust );
    mRVGsIconView->setAutoArrange( TRUE );
    mRVGsIconView->setItemsMovable( FALSE );
    mRVGsIconView->setWordWrapIconText( FALSE );
    languageChange();
    resize( QSize(444, 321).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
NewRVGDialog::~NewRVGDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void NewRVGDialog::languageChange()
{
    setCaption( tr( "New RVG - select from the list" ) );
    mRVGsLabel->setText( tr( "RVG" ) );
    buttonOk->setText( tr( "&OK" ) );
    buttonOk->setAccel( QKeySequence( QString::null ) );
    buttonCancel->setText( tr( "&Cancel" ) );
    buttonCancel->setAccel( QKeySequence( QString::null ) );
    mProductNameLabel->setText( tr( "Product" ) );
    mRVGsIconView->clear();
    (void) new QIconViewItem( mRVGsIconView, tr( "New Item" ) );
}

