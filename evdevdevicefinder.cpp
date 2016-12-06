#include <libudev.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/joystick.h>
#include <libevdev-1.0/libevdev/libevdev.h>

#include "evdevdevicefinder.h"

EvdevDeviceFinder::EvdevDeviceFinder(QObject *parent) : QObject(parent)
{

}

void EvdevDeviceFinder::openDevices(QList<JoystickDevice*> &controllerList)
{
    struct udev *udev;
    struct udev_enumerate *enumerate;
    struct udev_list_entry *devices, *dev_list_entry;
    struct udev_device *dev;
    int num_devices = 0;

    udev = udev_new();

    enumerate = udev_enumerate_new(udev);
    udev_enumerate_add_match_subsystem(enumerate, "input");
    udev_enumerate_scan_devices(enumerate);
    devices = udev_enumerate_get_list_entry(enumerate);

    udev_list_entry_foreach(dev_list_entry, devices) {
        const char *path;

        path = udev_list_entry_get_name(dev_list_entry);
        dev = udev_device_new_from_syspath(udev, path);
        const char* devnode = udev_device_get_devnode(dev);

        if (access(devnode, W_OK) == 0)
        {
            const char* value = NULL;
            value = udev_device_get_property_value(dev, "ID_INPUT_JOYSTICK");

            if (value && strcmp(value, "1") == 0)
            {
                JoystickDevice *tempDevice = new JoystickDevice(devnode, num_devices);
                if (tempDevice->isRelevant())
                {
                    controllerList.append(tempDevice);
                    num_devices++;
                }
                else
                {
                    delete tempDevice;
                    tempDevice = 0;
                }
            }
        }

        udev_device_unref(dev);
    }

    udev_enumerate_unref(enumerate);
    udev_unref(udev);
}
