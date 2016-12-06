#ifndef JOYSTICKDEVICE_H
#define JOYSTICKDEVICE_H

#include <libevdev-1.0/libevdev/libevdev.h>

#include <QObject>
#include <QString>
#include <QList>

class JoystickDevice : public QObject
{
    Q_OBJECT
public:
    explicit JoystickDevice(QString devnode, int index, QObject *parent = 0);
    ~JoystickDevice();
    bool isRelevant();
    void addAxis(int index, int axis);
    void addButton(int index, int button);
    int getAxisValue(int index);
    int getButtonValue(int index);
    int getNumAxes();
    int getNumButtons();
    QString getName();
    QString getDeviceFilePath();

protected:
    void grabDeviceData();
    int determineAxisValue(int axis, int evValue);

    QList<int> axesValues;
    QList<int> buttonValues;
    QString devname;
    QString devfile;
    int index;
    int fd;
    struct libevdev* m_dev;
    bool initalized;

signals:
    void buttonEvent(int button, bool pressed);
    void axisEvent(int axis, int value);

public slots:
    void pollEvents();
    void clearEventQueue();
};

#endif // JOYSTICKDEVICE_H
