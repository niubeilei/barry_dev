/****************************************************************************
** NewRVGDialog meta object code from reading C++ file 'NewRVGDialog.h'
**
** Created: Wed Feb 20 23:57:36 2008
**      by: The Qt MOC ($Id: moc_NewRVGDialog.cpp,v 1.2 2015/01/06 08:57:50 andy Exp $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "../.ui/NewRVGDialog.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *NewRVGDialog::className() const
{
    return "NewRVGDialog";
}

QMetaObject *NewRVGDialog::metaObj = 0;
static QMetaObjectCleanUp cleanUp_NewRVGDialog( "NewRVGDialog", &NewRVGDialog::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString NewRVGDialog::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "NewRVGDialog", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString NewRVGDialog::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "NewRVGDialog", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* NewRVGDialog::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QDialog::staticMetaObject();
    static const QUMethod slot_0 = {"init", 0, 0 };
    static const QUMethod slot_1 = {"getProductName", 0, 0 };
    static const QUMethod slot_2 = {"getRVGType", 0, 0 };
    static const QUMethod slot_3 = {"languageChange", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "init()", &slot_0, QMetaData::Public },
	{ "getProductName()", &slot_1, QMetaData::Public },
	{ "getRVGType()", &slot_2, QMetaData::Public },
	{ "languageChange()", &slot_3, QMetaData::Protected }
    };
    metaObj = QMetaObject::new_metaobject(
	"NewRVGDialog", parentObject,
	slot_tbl, 4,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_NewRVGDialog.setMetaObject( metaObj );
    return metaObj;
}

void* NewRVGDialog::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "NewRVGDialog" ) )
	return this;
    return QDialog::qt_cast( clname );
}

bool NewRVGDialog::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: init(); break;
    case 1: getProductName(); break;
    case 2: getRVGType(); break;
    case 3: languageChange(); break;
    default:
	return QDialog::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool NewRVGDialog::qt_emit( int _id, QUObject* _o )
{
    return QDialog::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool NewRVGDialog::qt_property( int id, int f, QVariant* v)
{
    return QDialog::qt_property( id, f, v);
}

bool NewRVGDialog::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
