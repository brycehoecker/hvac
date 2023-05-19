/****************************************************************************
** VHVFrame meta object code from reading C++ file 'VHVFrame.h'
**
** Created: Thu Mar 28 07:58:30 2013
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.6   edited Mar 8 17:43 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "../include/VHVFrame.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *VHVFrame::className() const
{
    return "VHVFrame";
}

QMetaObject *VHVFrame::metaObj = 0;
static QMetaObjectCleanUp cleanUp_VHVFrame( "VHVFrame", &VHVFrame::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString VHVFrame::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "VHVFrame", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString VHVFrame::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "VHVFrame", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* VHVFrame::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QMainWindow::staticMetaObject();
    static const QUMethod slot_0 = {"help", 0, 0 };
    static const QUMethod slot_1 = {"helpBrowser", 0, 0 };
    static const QUMethod slot_2 = {"about", 0, 0 };
    static const QUMethod slot_3 = {"notYet", 0, 0 };
    static const QUMethod slot_4 = {"init", 0, 0 };
    static const QUParameter param_slot_5[] = {
	{ "id", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_5 = {"fileMenu", 1, param_slot_5 };
    static const QUParameter param_slot_6[] = {
	{ "id", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_6 = {"dbMenu", 1, param_slot_6 };
    static const QUParameter param_slot_7[] = {
	{ "id", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_7 = {"pmtMenu", 1, param_slot_7 };
    static const QUParameter param_slot_8[] = {
	{ "id", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_8 = {"hvParamsMenu", 1, param_slot_8 };
    static const QUParameter param_slot_9[] = {
	{ "id", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_9 = {"starMenu", 1, param_slot_9 };
    static const QUParameter param_slot_10[] = {
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_10 = {"optionButtonClicked", 1, param_slot_10 };
    static const QUParameter param_slot_11[] = {
	{ 0, &static_QUType_ptr, "VPMTIcon", QUParameter::In },
	{ 0, &static_QUType_ptr, "QMouseEvent", QUParameter::In },
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_11 = {"pmtSelect", 3, param_slot_11 };
    static const QUParameter param_slot_12[] = {
	{ "id", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_12 = {"contextAction", 1, param_slot_12 };
    static const QUMethod slot_13 = {"textInputVoltage", 0, 0 };
    static const QUMethod slot_14 = {"textInputChannel", 0, 0 };
    static const QUMethod slot_15 = {"updateAll", 0, 0 };
    static const QUMethod slot_16 = {"scaleVoltages", 0, 0 };
    static const QUMethod slot_17 = {"offsetVoltages", 0, 0 };
    static const QUMethod slot_18 = {"setAllVoltages", 0, 0 };
    static const QUMethod slot_19 = {"markDisabled", 0, 0 };
    static const QUMethod slot_20 = {"unmarkDisabled", 0, 0 };
    static const QUMethod slot_21 = {"markAsNoAuto", 0, 0 };
    static const QUMethod slot_22 = {"unmarkAsNoAuto", 0, 0 };
    static const QUParameter param_slot_23[] = {
	{ "text", &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod slot_23 = {"sendMessage", 1, param_slot_23 };
    static const QUMethod slot_24 = {"setStatusPeriod", 0, 0 };
    static const QUMethod slot_25 = {"checkCurrentsChanged", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "help()", &slot_0, QMetaData::Public },
	{ "helpBrowser()", &slot_1, QMetaData::Public },
	{ "about()", &slot_2, QMetaData::Public },
	{ "notYet()", &slot_3, QMetaData::Public },
	{ "init()", &slot_4, QMetaData::Public },
	{ "fileMenu(int)", &slot_5, QMetaData::Public },
	{ "dbMenu(int)", &slot_6, QMetaData::Public },
	{ "pmtMenu(int)", &slot_7, QMetaData::Public },
	{ "hvParamsMenu(int)", &slot_8, QMetaData::Public },
	{ "starMenu(int)", &slot_9, QMetaData::Public },
	{ "optionButtonClicked(int)", &slot_10, QMetaData::Public },
	{ "pmtSelect(VPMTIcon*,QMouseEvent*,int)", &slot_11, QMetaData::Public },
	{ "contextAction(int)", &slot_12, QMetaData::Public },
	{ "textInputVoltage()", &slot_13, QMetaData::Public },
	{ "textInputChannel()", &slot_14, QMetaData::Public },
	{ "updateAll()", &slot_15, QMetaData::Public },
	{ "scaleVoltages()", &slot_16, QMetaData::Public },
	{ "offsetVoltages()", &slot_17, QMetaData::Public },
	{ "setAllVoltages()", &slot_18, QMetaData::Public },
	{ "markDisabled()", &slot_19, QMetaData::Public },
	{ "unmarkDisabled()", &slot_20, QMetaData::Public },
	{ "markAsNoAuto()", &slot_21, QMetaData::Public },
	{ "unmarkAsNoAuto()", &slot_22, QMetaData::Public },
	{ "sendMessage(const QString&)", &slot_23, QMetaData::Public },
	{ "setStatusPeriod()", &slot_24, QMetaData::Public },
	{ "checkCurrentsChanged()", &slot_25, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"VHVFrame", parentObject,
	slot_tbl, 26,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_VHVFrame.setMetaObject( metaObj );
    return metaObj;
}

void* VHVFrame::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "VHVFrame" ) )
	return this;
    return QMainWindow::qt_cast( clname );
}

bool VHVFrame::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: help(); break;
    case 1: helpBrowser(); break;
    case 2: about(); break;
    case 3: notYet(); break;
    case 4: init(); break;
    case 5: fileMenu((int)static_QUType_int.get(_o+1)); break;
    case 6: dbMenu((int)static_QUType_int.get(_o+1)); break;
    case 7: pmtMenu((int)static_QUType_int.get(_o+1)); break;
    case 8: hvParamsMenu((int)static_QUType_int.get(_o+1)); break;
    case 9: starMenu((int)static_QUType_int.get(_o+1)); break;
    case 10: optionButtonClicked((int)static_QUType_int.get(_o+1)); break;
    case 11: pmtSelect((VPMTIcon*)static_QUType_ptr.get(_o+1),(QMouseEvent*)static_QUType_ptr.get(_o+2),(int)static_QUType_int.get(_o+3)); break;
    case 12: contextAction((int)static_QUType_int.get(_o+1)); break;
    case 13: textInputVoltage(); break;
    case 14: textInputChannel(); break;
    case 15: updateAll(); break;
    case 16: scaleVoltages(); break;
    case 17: offsetVoltages(); break;
    case 18: setAllVoltages(); break;
    case 19: markDisabled(); break;
    case 20: unmarkDisabled(); break;
    case 21: markAsNoAuto(); break;
    case 22: unmarkAsNoAuto(); break;
    case 23: sendMessage((const QString&)static_QUType_QString.get(_o+1)); break;
    case 24: setStatusPeriod(); break;
    case 25: checkCurrentsChanged(); break;
    default:
	return QMainWindow::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool VHVFrame::qt_emit( int _id, QUObject* _o )
{
    return QMainWindow::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool VHVFrame::qt_property( int id, int f, QVariant* v)
{
    return QMainWindow::qt_property( id, f, v);
}

bool VHVFrame::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES


const char *VChannelReport::className() const
{
    return "VChannelReport";
}

QMetaObject *VChannelReport::metaObj = 0;
static QMetaObjectCleanUp cleanUp_VChannelReport( "VChannelReport", &VChannelReport::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString VChannelReport::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "VChannelReport", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString VChannelReport::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "VChannelReport", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* VChannelReport::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QGroupBox::staticMetaObject();
    static const QUParameter param_slot_0[] = {
	{ "value", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_0 = {"voltageChanged", 1, param_slot_0 };
    static const QUMethod slot_1 = {"powerChanged", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "voltageChanged(int)", &slot_0, QMetaData::Private },
	{ "powerChanged()", &slot_1, QMetaData::Private }
    };
    metaObj = QMetaObject::new_metaobject(
	"VChannelReport", parentObject,
	slot_tbl, 2,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_VChannelReport.setMetaObject( metaObj );
    return metaObj;
}

void* VChannelReport::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "VChannelReport" ) )
	return this;
    return QGroupBox::qt_cast( clname );
}

bool VChannelReport::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: voltageChanged((int)static_QUType_int.get(_o+1)); break;
    case 1: powerChanged(); break;
    default:
	return QGroupBox::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool VChannelReport::qt_emit( int _id, QUObject* _o )
{
    return QGroupBox::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool VChannelReport::qt_property( int id, int f, QVariant* v)
{
    return QGroupBox::qt_property( id, f, v);
}

bool VChannelReport::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES


const char *VBoardReport::className() const
{
    return "VBoardReport";
}

QMetaObject *VBoardReport::metaObj = 0;
static QMetaObjectCleanUp cleanUp_VBoardReport( "VBoardReport", &VBoardReport::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString VBoardReport::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "VBoardReport", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString VBoardReport::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "VBoardReport", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* VBoardReport::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QGroupBox::staticMetaObject();
    static const QUParameter param_slot_0[] = {
	{ "value", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_0 = {"voltageChanged", 1, param_slot_0 };
    static const QUParameter param_slot_1[] = {
	{ "value", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_1 = {"currentChanged", 1, param_slot_1 };
    static const QUParameter param_slot_2[] = {
	{ "value", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_2 = {"rampUpChanged", 1, param_slot_2 };
    static const QUParameter param_slot_3[] = {
	{ "value", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_3 = {"rampDownChanged", 1, param_slot_3 };
    static const QUMethod slot_4 = {"powerChanged", 0, 0 };
    static const QUMethod slot_5 = {"checkboxChanged", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "voltageChanged(int)", &slot_0, QMetaData::Private },
	{ "currentChanged(int)", &slot_1, QMetaData::Private },
	{ "rampUpChanged(int)", &slot_2, QMetaData::Private },
	{ "rampDownChanged(int)", &slot_3, QMetaData::Private },
	{ "powerChanged()", &slot_4, QMetaData::Private },
	{ "checkboxChanged()", &slot_5, QMetaData::Private }
    };
    metaObj = QMetaObject::new_metaobject(
	"VBoardReport", parentObject,
	slot_tbl, 6,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_VBoardReport.setMetaObject( metaObj );
    return metaObj;
}

void* VBoardReport::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "VBoardReport" ) )
	return this;
    return QGroupBox::qt_cast( clname );
}

bool VBoardReport::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: voltageChanged((int)static_QUType_int.get(_o+1)); break;
    case 1: currentChanged((int)static_QUType_int.get(_o+1)); break;
    case 2: rampUpChanged((int)static_QUType_int.get(_o+1)); break;
    case 3: rampDownChanged((int)static_QUType_int.get(_o+1)); break;
    case 4: powerChanged(); break;
    case 5: checkboxChanged(); break;
    default:
	return QGroupBox::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool VBoardReport::qt_emit( int _id, QUObject* _o )
{
    return QGroupBox::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool VBoardReport::qt_property( int id, int f, QVariant* v)
{
    return QGroupBox::qt_property( id, f, v);
}

bool VBoardReport::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES


const char *VTelStatus::className() const
{
    return "VTelStatus";
}

QMetaObject *VTelStatus::metaObj = 0;
static QMetaObjectCleanUp cleanUp_VTelStatus( "VTelStatus", &VTelStatus::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString VTelStatus::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "VTelStatus", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString VTelStatus::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "VTelStatus", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* VTelStatus::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QGroupBox::staticMetaObject();
    metaObj = QMetaObject::new_metaobject(
	"VTelStatus", parentObject,
	0, 0,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_VTelStatus.setMetaObject( metaObj );
    return metaObj;
}

void* VTelStatus::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "VTelStatus" ) )
	return this;
    return QGroupBox::qt_cast( clname );
}

bool VTelStatus::qt_invoke( int _id, QUObject* _o )
{
    return QGroupBox::qt_invoke(_id,_o);
}

bool VTelStatus::qt_emit( int _id, QUObject* _o )
{
    return QGroupBox::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool VTelStatus::qt_property( int id, int f, QVariant* v)
{
    return QGroupBox::qt_property( id, f, v);
}

bool VTelStatus::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES


const char *VStarPanel::className() const
{
    return "VStarPanel";
}

QMetaObject *VStarPanel::metaObj = 0;
static QMetaObjectCleanUp cleanUp_VStarPanel( "VStarPanel", &VStarPanel::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString VStarPanel::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "VStarPanel", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString VStarPanel::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "VStarPanel", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* VStarPanel::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QGroupBox::staticMetaObject();
    static const QUParameter param_slot_0[] = {
	{ "value", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_0 = {"changedMaxMag", 1, param_slot_0 };
    static const QUMethod slot_1 = {"changedRA", 0, 0 };
    static const QUMethod slot_2 = {"changedDec", 0, 0 };
    static const QUMethod slot_3 = {"showStars", 0, 0 };
    static const QUMethod slot_4 = {"showArcs", 0, 0 };
    static const QUParameter param_slot_5[] = {
	{ 0, &static_QUType_bool, 0, QUParameter::Out }
    };
    static const QUMethod slot_5 = {"starsOn", 1, param_slot_5 };
    static const QUParameter param_slot_6[] = {
	{ 0, &static_QUType_bool, 0, QUParameter::Out }
    };
    static const QUMethod slot_6 = {"arcsOn", 1, param_slot_6 };
    static const QUParameter param_slot_7[] = {
	{ "on", &static_QUType_bool, 0, QUParameter::In }
    };
    static const QUMethod slot_7 = {"setStarsOn", 1, param_slot_7 };
    static const QUParameter param_slot_8[] = {
	{ "on", &static_QUType_bool, 0, QUParameter::In }
    };
    static const QUMethod slot_8 = {"setArcsOn", 1, param_slot_8 };
    static const QUParameter param_slot_9[] = {
	{ "id", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_9 = {"optionButtonClicked", 1, param_slot_9 };
    static const QMetaData slot_tbl[] = {
	{ "changedMaxMag(int)", &slot_0, QMetaData::Private },
	{ "changedRA()", &slot_1, QMetaData::Private },
	{ "changedDec()", &slot_2, QMetaData::Private },
	{ "showStars()", &slot_3, QMetaData::Private },
	{ "showArcs()", &slot_4, QMetaData::Private },
	{ "starsOn()", &slot_5, QMetaData::Private },
	{ "arcsOn()", &slot_6, QMetaData::Private },
	{ "setStarsOn(bool)", &slot_7, QMetaData::Private },
	{ "setArcsOn(bool)", &slot_8, QMetaData::Private },
	{ "optionButtonClicked(int)", &slot_9, QMetaData::Private }
    };
    metaObj = QMetaObject::new_metaobject(
	"VStarPanel", parentObject,
	slot_tbl, 10,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_VStarPanel.setMetaObject( metaObj );
    return metaObj;
}

void* VStarPanel::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "VStarPanel" ) )
	return this;
    return QGroupBox::qt_cast( clname );
}

bool VStarPanel::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: changedMaxMag((int)static_QUType_int.get(_o+1)); break;
    case 1: changedRA(); break;
    case 2: changedDec(); break;
    case 3: showStars(); break;
    case 4: showArcs(); break;
    case 5: static_QUType_bool.set(_o,starsOn()); break;
    case 6: static_QUType_bool.set(_o,arcsOn()); break;
    case 7: setStarsOn((bool)static_QUType_bool.get(_o+1)); break;
    case 8: setArcsOn((bool)static_QUType_bool.get(_o+1)); break;
    case 9: optionButtonClicked((int)static_QUType_int.get(_o+1)); break;
    default:
	return QGroupBox::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool VStarPanel::qt_emit( int _id, QUObject* _o )
{
    return QGroupBox::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool VStarPanel::qt_property( int id, int f, QVariant* v)
{
    return QGroupBox::qt_property( id, f, v);
}

bool VStarPanel::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES


const char *VAutoResponseDialog::className() const
{
    return "VAutoResponseDialog";
}

QMetaObject *VAutoResponseDialog::metaObj = 0;
static QMetaObjectCleanUp cleanUp_VAutoResponseDialog( "VAutoResponseDialog", &VAutoResponseDialog::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString VAutoResponseDialog::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "VAutoResponseDialog", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString VAutoResponseDialog::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "VAutoResponseDialog", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* VAutoResponseDialog::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QDialog::staticMetaObject();
    metaObj = QMetaObject::new_metaobject(
	"VAutoResponseDialog", parentObject,
	0, 0,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_VAutoResponseDialog.setMetaObject( metaObj );
    return metaObj;
}

void* VAutoResponseDialog::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "VAutoResponseDialog" ) )
	return this;
    return QDialog::qt_cast( clname );
}

bool VAutoResponseDialog::qt_invoke( int _id, QUObject* _o )
{
    return QDialog::qt_invoke(_id,_o);
}

bool VAutoResponseDialog::qt_emit( int _id, QUObject* _o )
{
    return QDialog::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool VAutoResponseDialog::qt_property( int id, int f, QVariant* v)
{
    return QDialog::qt_property( id, f, v);
}

bool VAutoResponseDialog::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES


const char *VFlatFieldDialog::className() const
{
    return "VFlatFieldDialog";
}

QMetaObject *VFlatFieldDialog::metaObj = 0;
static QMetaObjectCleanUp cleanUp_VFlatFieldDialog( "VFlatFieldDialog", &VFlatFieldDialog::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString VFlatFieldDialog::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "VFlatFieldDialog", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString VFlatFieldDialog::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "VFlatFieldDialog", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* VFlatFieldDialog::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QDialog::staticMetaObject();
    static const QUMethod slot_0 = {"flatField", 0, 0 };
    static const QUMethod slot_1 = {"setValues", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "flatField()", &slot_0, QMetaData::Public },
	{ "setValues()", &slot_1, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"VFlatFieldDialog", parentObject,
	slot_tbl, 2,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_VFlatFieldDialog.setMetaObject( metaObj );
    return metaObj;
}

void* VFlatFieldDialog::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "VFlatFieldDialog" ) )
	return this;
    return QDialog::qt_cast( clname );
}

bool VFlatFieldDialog::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: flatField(); break;
    case 1: setValues(); break;
    default:
	return QDialog::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool VFlatFieldDialog::qt_emit( int _id, QUObject* _o )
{
    return QDialog::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool VFlatFieldDialog::qt_property( int id, int f, QVariant* v)
{
    return QDialog::qt_property( id, f, v);
}

bool VFlatFieldDialog::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES


const char *VFeedbackDialog::className() const
{
    return "VFeedbackDialog";
}

QMetaObject *VFeedbackDialog::metaObj = 0;
static QMetaObjectCleanUp cleanUp_VFeedbackDialog( "VFeedbackDialog", &VFeedbackDialog::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString VFeedbackDialog::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "VFeedbackDialog", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString VFeedbackDialog::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "VFeedbackDialog", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* VFeedbackDialog::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QDialog::staticMetaObject();
    metaObj = QMetaObject::new_metaobject(
	"VFeedbackDialog", parentObject,
	0, 0,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_VFeedbackDialog.setMetaObject( metaObj );
    return metaObj;
}

void* VFeedbackDialog::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "VFeedbackDialog" ) )
	return this;
    return QDialog::qt_cast( clname );
}

bool VFeedbackDialog::qt_invoke( int _id, QUObject* _o )
{
    return QDialog::qt_invoke(_id,_o);
}

bool VFeedbackDialog::qt_emit( int _id, QUObject* _o )
{
    return QDialog::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool VFeedbackDialog::qt_property( int id, int f, QVariant* v)
{
    return QDialog::qt_property( id, f, v);
}

bool VFeedbackDialog::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
