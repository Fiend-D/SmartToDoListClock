/****************************************************************************
** Meta object code from reading C++ file 'clockwidget.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../clockwidget.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'clockwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
namespace {
struct qt_meta_stringdata_ClockWidget_t {
    uint offsetsAndSizes[34];
    char stringdata0[12];
    char stringdata1[16];
    char stringdata2[1];
    char stringdata3[15];
    char stringdata4[16];
    char stringdata5[17];
    char stringdata6[12];
    char stringdata7[5];
    char stringdata8[8];
    char stringdata9[15];
    char stringdata10[14];
    char stringdata11[14];
    char stringdata12[13];
    char stringdata13[11];
    char stringdata14[5];
    char stringdata15[17];
    char stringdata16[14];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_ClockWidget_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_ClockWidget_t qt_meta_stringdata_ClockWidget = {
    {
        QT_MOC_LITERAL(0, 11),  // "ClockWidget"
        QT_MOC_LITERAL(12, 15),  // "pomodoroStarted"
        QT_MOC_LITERAL(28, 0),  // ""
        QT_MOC_LITERAL(29, 14),  // "pomodoroPaused"
        QT_MOC_LITERAL(44, 15),  // "pomodoroStopped"
        QT_MOC_LITERAL(60, 16),  // "pomodoroFinished"
        QT_MOC_LITERAL(77, 11),  // "modeChanged"
        QT_MOC_LITERAL(89, 4),  // "Mode"
        QT_MOC_LITERAL(94, 7),  // "newMode"
        QT_MOC_LITERAL(102, 14),  // "togglePomodoro"
        QT_MOC_LITERAL(117, 13),  // "startPomodoro"
        QT_MOC_LITERAL(131, 13),  // "pausePomodoro"
        QT_MOC_LITERAL(145, 12),  // "stopPomodoro"
        QT_MOC_LITERAL(158, 10),  // "switchMode"
        QT_MOC_LITERAL(169, 4),  // "mode"
        QT_MOC_LITERAL(174, 16),  // "pomodoroProgress"
        QT_MOC_LITERAL(191, 13)   // "glowIntensity"
    },
    "ClockWidget",
    "pomodoroStarted",
    "",
    "pomodoroPaused",
    "pomodoroStopped",
    "pomodoroFinished",
    "modeChanged",
    "Mode",
    "newMode",
    "togglePomodoro",
    "startPomodoro",
    "pausePomodoro",
    "stopPomodoro",
    "switchMode",
    "mode",
    "pomodoroProgress",
    "glowIntensity"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_ClockWidget[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       2,   88, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   74,    2, 0x06,    3 /* Public */,
       3,    0,   75,    2, 0x06,    4 /* Public */,
       4,    0,   76,    2, 0x06,    5 /* Public */,
       5,    0,   77,    2, 0x06,    6 /* Public */,
       6,    1,   78,    2, 0x06,    7 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       9,    0,   81,    2, 0x0a,    9 /* Public */,
      10,    0,   82,    2, 0x0a,   10 /* Public */,
      11,    0,   83,    2, 0x0a,   11 /* Public */,
      12,    0,   84,    2, 0x0a,   12 /* Public */,
      13,    1,   85,    2, 0x0a,   13 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 7,    8,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 7,   14,

 // properties: name, type, flags
      15, QMetaType::QReal, 0x00015103, uint(-1), 0,
      16, QMetaType::QReal, 0x00015103, uint(-1), 0,

       0        // eod
};

Q_CONSTINIT const QMetaObject ClockWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_ClockWidget.offsetsAndSizes,
    qt_meta_data_ClockWidget,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_ClockWidget_t,
        // property 'pomodoroProgress'
        QtPrivate::TypeAndForceComplete<qreal, std::true_type>,
        // property 'glowIntensity'
        QtPrivate::TypeAndForceComplete<qreal, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ClockWidget, std::true_type>,
        // method 'pomodoroStarted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'pomodoroPaused'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'pomodoroStopped'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'pomodoroFinished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'modeChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<Mode, std::false_type>,
        // method 'togglePomodoro'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'startPomodoro'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'pausePomodoro'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'stopPomodoro'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'switchMode'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<Mode, std::false_type>
    >,
    nullptr
} };

void ClockWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ClockWidget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->pomodoroStarted(); break;
        case 1: _t->pomodoroPaused(); break;
        case 2: _t->pomodoroStopped(); break;
        case 3: _t->pomodoroFinished(); break;
        case 4: _t->modeChanged((*reinterpret_cast< std::add_pointer_t<Mode>>(_a[1]))); break;
        case 5: _t->togglePomodoro(); break;
        case 6: _t->startPomodoro(); break;
        case 7: _t->pausePomodoro(); break;
        case 8: _t->stopPomodoro(); break;
        case 9: _t->switchMode((*reinterpret_cast< std::add_pointer_t<Mode>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ClockWidget::*)();
            if (_t _q_method = &ClockWidget::pomodoroStarted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ClockWidget::*)();
            if (_t _q_method = &ClockWidget::pomodoroPaused; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ClockWidget::*)();
            if (_t _q_method = &ClockWidget::pomodoroStopped; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ClockWidget::*)();
            if (_t _q_method = &ClockWidget::pomodoroFinished; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (ClockWidget::*)(Mode );
            if (_t _q_method = &ClockWidget::modeChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
    }else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<ClockWidget *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< qreal*>(_v) = _t->pomodoroProgress(); break;
        case 1: *reinterpret_cast< qreal*>(_v) = _t->glowIntensity(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<ClockWidget *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setPomodoroProgress(*reinterpret_cast< qreal*>(_v)); break;
        case 1: _t->setGlowIntensity(*reinterpret_cast< qreal*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
}

const QMetaObject *ClockWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ClockWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ClockWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int ClockWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 10;
    }else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void ClockWidget::pomodoroStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ClockWidget::pomodoroPaused()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void ClockWidget::pomodoroStopped()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void ClockWidget::pomodoroFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void ClockWidget::modeChanged(Mode _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
