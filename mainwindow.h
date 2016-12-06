#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QListIterator>
#include <QTimer>
#include <QComboBox>

#include "evdevdevicefinder.h"
#include "joystickdevice.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void generateCommandOutputString();
    void generateConfigOutputString();
    void purgeControllerList();
    void establishRecordingConnections();

    QList<JoystickDevice*> controllerList;
    QTimer pollEventTimer;
    QComboBox *currentBox;

signals:
    void joystickEventAssigned();

public slots:
    void disconnectRecordingConnections();
    void determineAxisAction(int code, int value);
    void determineButtonAction(int code, bool pressed);

protected slots:
    void generateOutputString();
    void setupRecordTest();

    void setupRecordAButton();
    void setupRecordBButton();
    void setupRecordXButton();
    void setupRecordYButton();
    void setupRecordLBButton();
    void setupRecordRBButton();
    void setupRecordLSBButton();
    void setupRecordRSBButton();
    void setupRecordBackButton();
    void setupRecordStartButton();
    void setupRecordGuideButton();
    void setupRecordDPadXAxis();
    void setupRecordDPadYAxis();
    void setupRecordLTriggerAxis();
    void setupRecordRTriggerAxis();
    void setupRecordLStickXAxis();
    void setupRecordLStickYAxis();
    void setupRecordRStickXAxis();
    void setupRecordRStickYAxis();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
