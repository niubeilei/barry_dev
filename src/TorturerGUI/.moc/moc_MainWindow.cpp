/****************************************************************************
** MainWindow meta object code from reading C++ file 'MainWindow.h'
**
** Created: Sun Mar 16 15:36:32 2008
**      by: The Qt MOC ($Id: moc_MainWindow.cpp,v 1.2 2015/01/06 08:57:51 andy Exp $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "../.ui/MainWindow.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *MainWindow::className() const
{
    return "MainWindow";
}

QMetaObject *MainWindow::metaObj = 0;
static QMetaObjectCleanUp cleanUp_MainWindow( "MainWindow", &MainWindow::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString MainWindow::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "MainWindow", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString MainWindow::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "MainWindow", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* MainWindow::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QMainWindow::staticMetaObject();
    static const QUMethod slot_0 = {"helpIndex", 0, 0 };
    static const QUMethod slot_1 = {"helpContents", 0, 0 };
    static const QUMethod slot_2 = {"helpAbout", 0, 0 };
    static const QUMethod slot_3 = {"initTorTree", 0, 0 };
    static const QUMethod slot_4 = {"initWorkspace", 0, 0 };
    static const QUParameter param_slot_5[] = {
	{ "item", &static_QUType_ptr, "QListViewItem", QUParameter::In }
    };
    static const QUMethod slot_5 = {"newMDIWnd", 1, param_slot_5 };
    static const QUMethod slot_6 = {"windowCloseAction_activated", 0, 0 };
    static const QUMethod slot_7 = {"windowClose_AllAction_activated", 0, 0 };
    static const QUMethod slot_8 = {"windowCascadeAction_activated", 0, 0 };
    static const QUMethod slot_9 = {"windowTitleAction_activated", 0, 0 };
    static const QUMethod slot_10 = {"windowTitle_HorizontallyAction_activated", 0, 0 };
    static const QUParameter param_slot_11[] = {
	{ "rvg", &static_QUType_ptr, "AosRVGPtr", QUParameter::InOut },
	{ "item", &static_QUType_ptr, "AosRVGTreeItem", QUParameter::In }
    };
    static const QUMethod slot_11 = {"newRVGWindow", 2, param_slot_11 };
    static const QUMethod slot_12 = {"newRVGAction", 0, 0 };
    static const QUMethod slot_13 = {"SaveObjectAction", 0, 0 };
    static const QUMethod slot_14 = {"RVGDeleteAction_activated", 0, 0 };
    static const QUMethod slot_15 = {"RVGTestAction_activated", 0, 0 };
    static const QUMethod slot_16 = {"languageChange", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "helpIndex()", &slot_0, QMetaData::Public },
	{ "helpContents()", &slot_1, QMetaData::Public },
	{ "helpAbout()", &slot_2, QMetaData::Public },
	{ "initTorTree()", &slot_3, QMetaData::Public },
	{ "initWorkspace()", &slot_4, QMetaData::Public },
	{ "newMDIWnd(QListViewItem*)", &slot_5, QMetaData::Public },
	{ "windowCloseAction_activated()", &slot_6, QMetaData::Public },
	{ "windowClose_AllAction_activated()", &slot_7, QMetaData::Public },
	{ "windowCascadeAction_activated()", &slot_8, QMetaData::Public },
	{ "windowTitleAction_activated()", &slot_9, QMetaData::Public },
	{ "windowTitle_HorizontallyAction_activated()", &slot_10, QMetaData::Public },
	{ "newRVGWindow(AosRVGPtr&,AosRVGTreeItem*)", &slot_11, QMetaData::Public },
	{ "newRVGAction()", &slot_12, QMetaData::Public },
	{ "SaveObjectAction()", &slot_13, QMetaData::Public },
	{ "RVGDeleteAction_activated()", &slot_14, QMetaData::Public },
	{ "RVGTestAction_activated()", &slot_15, QMetaData::Public },
	{ "languageChange()", &slot_16, QMetaData::Protected }
    };
    metaObj = QMetaObject::new_metaobject(
	"MainWindow", parentObject,
	slot_tbl, 17,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_MainWindow.setMetaObject( metaObj );
    return metaObj;
}

void* MainWindow::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "MainWindow" ) )
	return this;
    return QMainWindow::qt_cast( clname );
}

bool MainWindow::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: helpIndex(); break;
    case 1: helpContents(); break;
    case 2: helpAbout(); break;
    case 3: initTorTree(); break;
    case 4: initWorkspace(); break;
    case 5: newMDIWnd((QListViewItem*)static_QUType_ptr.get(_o+1)); break;
    case 6: windowCloseAction_activated(); break;
    case 7: windowClose_AllAction_activated(); break;
    case 8: windowCascadeAction_activated(); break;
    case 9: windowTitleAction_activated(); break;
    case 10: windowTitle_HorizontallyAction_activated(); break;
    case 11: newRVGWindow((AosRVGPtr&)*((AosRVGPtr*)static_QUType_ptr.get(_o+1)),(AosRVGTreeItem*)static_QUType_ptr.get(_o+2)); break;
    case 12: newRVGAction(); break;
    case 13: SaveObjectAction(); break;
    case 14: RVGDeleteAction_activated(); break;
    case 15: RVGTestAction_activated(); break;
    case 16: languageChange(); break;
    default:
	return QMainWindow::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool MainWindow::qt_emit( int _id, QUObject* _o )
{
    return QMainWindow::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool MainWindow::qt_property( int id, int f, QVariant* v)
{
    return QMainWindow::qt_property( id, f, v);
}

bool MainWindow::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
