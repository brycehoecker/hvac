/****************************************************************************
** VHVData meta object code from reading C++ file 'VHVData.h'
**
** Created: Thu Mar 28 07:58:31 2013
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.6   edited Mar 8 17:43 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "../include/VHVData.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *VHVData::className() const
{
    return "VHVData";
}

QMetaObject *VHVData::metaObj = 0;
static QMetaObjectCleanUp cleanUp_VHVData( "VHVData", &VHVData::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString VHVData::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "VHVData", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString VHVData::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "VHVData", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* VHVData::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QObject::staticMetaObject();
    static const QUParameter param_slot_0[] = {
	{ "chan", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_0 = {"restorePixel", 1, param_slot_0 };
    static const QMetaData slot_tbl[] = {
	{ "restorePixel(int)", &slot_0, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"VHVData", parentObject,
	slot_tbl, 1,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_VHVData.setMetaObject( metaObj );
    return metaObj;
}

void* VHVData::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "VHVData" ) )
	return this;
    return QObject::qt_cast( clname );
}

bool VHVData::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: restorePixel((int)static_QUType_int.get(_o+1)); break;
    default:
	return QObject::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool VHVData::qt_emit( int _id, QUObject* _o )
{
    return QObject::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool VHVData::qt_property( int id, int f, QVariant* v)
{
    return QObject::qt_property( id, f, v);
}

bool VHVData::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
