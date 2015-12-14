/****************************************************************************
** RVGTestDlg meta object code from reading C++ file 'RVGTestDlg.h'
**
** Created: Wed Feb 20 23:57:37 2008
**      by: The Qt MOC ($Id: moc_RVGTestDlg.cpp,v 1.2 2015/01/06 08:57:50 andy Exp $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "../.ui/RVGTestDlg.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *RVGTestDlg::className() const
{
    return "RVGTestDlg";
}

QMetaObject *RVGTestDlg::metaObj = 0;
static QMetaObjectCleanUp cleanUp_RVGTestDlg( "RVGTestDlg", &RVGTestDlg::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString RVGTestDlg::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "RVGTestDlg", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString RVGTestDlg::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "RVGTestDlg", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* RVGTestDlg::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QDialog::staticMetaObject();
    static const QUMethod slot_0 = {"startTestBtn_clicked", 0, 0 };
    static const QUMethod slot_1 = {"languageChange", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "startTestBtn_clicked()", &slot_0, QMetaData::Public },
	{ "languageChange()", &slot_1, QMetaData::Protected }
    };
    metaObj = QMetaObject::new_metaobject(
	"RVGTestDlg", parentObject,
	slot_tbl, 2,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_RVGTestDlg.setMetaObject( metaObj );
    return metaObj;
}

void* RVGTestDlg::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "RVGTestDlg" ) )
	return this;
    return QDialog::qt_cast( clname );
}

bool RVGTestDlg::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: startTestBtn_clicked(); break;
    case 1: languageChange(); break;
    default:
	return QDialog::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool RVGTestDlg::qt_emit( int _id, QUObject* _o )
{
    return QDialog::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool RVGTestDlg::qt_property( int id, int f, QVariant* v)
{
    return QDialog::qt_property( id, f, v);
}

bool RVGTestDlg::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
