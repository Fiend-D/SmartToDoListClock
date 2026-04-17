#ifndef X11GLOBALSHORTCUT_H
#define X11GLOBALSHORTCUT_H

#include <QObject>
#include <QKeySequence>
#include <QThread>
#include <QMap>

typedef struct _XDisplay Display;
typedef union _XEvent XEvent;

class X11EventReader : public QThread {
    Q_OBJECT
public:
    explicit X11EventReader(Display *display, QObject *parent = nullptr);
    void stop();
    
signals:
    void keyPressed(int keycode);
    
protected:
    void run() override;
    
private:
    Display *m_display;
    bool m_running = true;
};

class X11GlobalShortcut : public QObject {
    Q_OBJECT
public:
    explicit X11GlobalShortcut(QObject *parent = nullptr);
    ~X11GlobalShortcut();
    
    bool registerShortcut(const QKeySequence &key, int id);
    void unregisterShortcut(int id);
    void unregisterAll();
    
signals:
    void activated(int id);
    
private:
    struct Shortcut {
        quint32 keycode = 0;
        quint32 modifiers = 0;
        bool registered = false;
    };
    
    quint32 nativeKeycode(Qt::Key key);
    quint32 nativeModifiers(Qt::KeyboardModifiers modifiers);
    
    QMap<int, Shortcut> m_shortcuts;
    Display *m_display = nullptr;
    X11EventReader *m_reader = nullptr;
};

#endif