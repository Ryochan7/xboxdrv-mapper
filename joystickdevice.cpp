#include <unistd.h>
#include <fcntl.h>

#include <QDebug>

#include "joystickdevice.h"

JoystickDevice::JoystickDevice(QString devnode, int index, QObject *parent) : QObject(parent),
    devfile(devnode), index(index), initalized(false)
{
    fd = open(devfile.toStdString().c_str(), O_RDWR | O_NONBLOCK);
    int ret = libevdev_new_from_fd(fd, &m_dev);
    if (ret != 0)
    {
        initalized = false;
        close(fd);
    }
    else
    {
        initalized = true;
        grabDeviceData();
    }
}

JoystickDevice::~JoystickDevice()
{
    if (initalized)
    {
        libevdev_free(m_dev);
        close(fd);
    }
}

void JoystickDevice::addAxis(int index, int axis)
{
    axesValues.insert(index, axis);
}

void JoystickDevice::addButton(int index, int button)
{
    buttonValues.insert(index, button);
}

int JoystickDevice::getAxisValue(int index)
{
    int result = 0;
    if (index >= 0 && index < axesValues.size())
    {
        result = axesValues.value(index);
    }

    return result;
}

int JoystickDevice::getButtonValue(int index)
{
    int result = 0;
    if (index >= 0 && index < buttonValues.size())
    {
        result = buttonValues.value(index);
    }

    return result;
}

bool JoystickDevice::isRelevant()
{
    return initalized;
}

void JoystickDevice::grabDeviceData()
{
    const char *m_name = libevdev_get_name(m_dev);
    devname = QString::fromLocal8Bit(m_name);

    // Absolute axis (thumbsticks)
    int num_axis = 0;
    for (int axis = 0; axis < 0x100; axis++)
    {
        if (libevdev_has_event_code(m_dev, EV_ABS, axis))
        {
            addAxis(num_axis, axis);
            num_axis++;
        }
    }

    int num_buttons = 0;
    for (int key = 0; key < KEY_MAX; key++)
    {
        if (libevdev_has_event_code(m_dev, EV_KEY, key))
        {
            addButton(num_buttons, key);
            num_buttons++;
        }
    }
}

int JoystickDevice::getNumAxes()
{
    return axesValues.size();
}

int JoystickDevice::getNumButtons()
{
    return buttonValues.size();
}

QString JoystickDevice::getName()
{
    return devname;
}

QString JoystickDevice::getDeviceFilePath()
{
    return devfile;
}

int JoystickDevice::determineAxisValue(int axis, int evValue)
{
    int result = 0;
    int min = libevdev_get_abs_minimum(m_dev, axis);
    int max = libevdev_get_abs_maximum(m_dev, axis);
    int resolution = max - min;
    double temp = (evValue - min) / static_cast<double>(resolution);
    result = temp >= 0.5 ? ((temp - 0.5) * 2.0) * 32767.0 : ((0.5 - temp) * 2.0) * -32768.0;
    return result;
}

void JoystickDevice::pollEvents()
{
    struct input_event ev;
    int rc = LIBEVDEV_READ_STATUS_SUCCESS;
    while (rc >= 0)
    {
        rc = libevdev_next_event(m_dev, LIBEVDEV_READ_FLAG_NORMAL, &ev);
        if (rc == 0)
        {
            if (ev.type == EV_ABS)
            {
                int currentValue = determineAxisValue(ev.code, ev.value);
                emit axisEvent(ev.code, currentValue);
            }
            else if (ev.type == EV_KEY)
            {
                emit buttonEvent(ev.code, ev.value);
            }
        }
    }
}

void JoystickDevice::clearEventQueue()
{
    struct input_event ev;
    int rc = LIBEVDEV_READ_STATUS_SUCCESS;
    while (rc >= 0)
    {
        rc = libevdev_next_event(m_dev, LIBEVDEV_READ_FLAG_NORMAL, &ev);
    }
}
