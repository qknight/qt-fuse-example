/****************************************************************************
** Meta object code from reading C++ file 'MyDaemon.hh'
**
** Created: Fri Jul 12 22:25:27 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "MyDaemon.hh"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MyDaemon.hh' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MyDaemon[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      10,    9,    9,    9, 0x05,
      20,    9,    9,    9, 0x05,
      29,    9,    9,    9, 0x05,

 // slots: signature, parameters, type, tag, flags
      38,    9,    9,    9, 0x0a,
      53,    9,    9,    9, 0x0a,
      68,    9,    9,    9, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_MyDaemon[] = {
    "MyDaemon\0\0sigTERM()\0sigHUP()\0sigINT()\0"
    "handleSigInt()\0handleSigHup()\0"
    "handleSigTerm()\0"
};

void MyDaemon::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MyDaemon *_t = static_cast<MyDaemon *>(_o);
        switch (_id) {
        case 0: _t->sigTERM(); break;
        case 1: _t->sigHUP(); break;
        case 2: _t->sigINT(); break;
        case 3: _t->handleSigInt(); break;
        case 4: _t->handleSigHup(); break;
        case 5: _t->handleSigTerm(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData MyDaemon::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MyDaemon::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_MyDaemon,
      qt_meta_data_MyDaemon, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MyDaemon::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MyDaemon::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MyDaemon::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MyDaemon))
        return static_cast<void*>(const_cast< MyDaemon*>(this));
    return QObject::qt_metacast(_clname);
}

int MyDaemon::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void MyDaemon::sigTERM()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void MyDaemon::sigHUP()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void MyDaemon::sigINT()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}
QT_END_MOC_NAMESPACE
