#ifndef EVDEVDEVICEFINDER_H
#define EVDEVDEVICEFINDER_H

#include <QObject>
#include <QList>

#include "joystickdevice.h"

class EvdevDeviceFinder : public QObject
{
    Q_OBJECT
public:
    explicit EvdevDeviceFinder(QObject *parent = 0);
    void openDevices(QList<JoystickDevice*> &controllerList);

signals:

public slots:
};

#endif // EVDEVDEVICEFINDER_H
