#include <linux/uinput.h>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <QMessageBox>

#include "mainwindow.h"
#include "ui_mainwindow.h"



#define xstr(s) str(s)
#define str(s) #s

static std::string axesNames[ABS_HAT3Y+6] = {
    str(ABS_X), str(ABS_Y), str(ABS_Z), str(ABS_RX), str(ABS_RY), str(ABS_RZ),
    str(ABS_THROTTLE), str(ABS_RUDDER), str(ABS_WHEEL), str(ABS_GAS), str(ABS_BRAKE),
    "NONE", "NONE", "NONE", "NONE", "NONE",
    str(ABS_HAT0X), str(ABS_HAT0Y), str(ABS_HAT1X), str(ABS_HAT1Y), str(ABS_HAT2X), str(ABS_HAT2Y),
    str(ABS_HAT3X), str(ABS_HAT3Y),
};

static std::string buttonNames[BTN_THUMBR-BTN_SOUTH+1] = {
    str(BTN_A), str(BTN_B), str(BTN_C), str(BTN_X),
    str(BTN_Y), str(BTN_Z), str(BTN_TL), str(BTN_TR), str(BTN_TL2), str(BTN_TR2),
    str(BTN_SELECT), str(BTN_START), str(BTN_MODE), str(BTN_THUMBL), str(BTN_THUMBR),
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    currentBox = 0;
    ui->recordLTriggerPushButton->setText("\u2022");
    ui->recordRTriggerPushButton->setText("\u2022");
    ui->recordLBumperPushButton->setText("\u2022");
    ui->recordRBumperPushButton->setText("\u2022");
    ui->recordBackPushButton->setText("\u2022");
    ui->recordStartPushButton->setText("\u2022");
    ui->recordGuidePushButton->setText("\u2022");
    ui->recordAPushButton->setText("\u2022");
    ui->recordBPushButton->setText("\u2022");
    ui->recordXPushButton->setText("\u2022");
    ui->recordYPushButton->setText("\u2022");
    ui->recordDPadXPushButton->setText("\u2022");
    ui->recordDPadYPushButton->setText("\u2022");
    ui->recordLStickXPushButton->setText("\u2022");
    ui->recordLStickYPushButton->setText("\u2022");
    ui->recordRStickXPushButton->setText("\u2022");
    ui->recordRStickYPushButton->setText("\u2022");
    ui->recordLSBPushButton->setText("\u2022");
    ui->recordRSBPushButton->setText("\u2022");

    pollEventTimer.setInterval(10);

    purgeControllerList();
    EvdevDeviceFinder finder(this);
    finder.openDevices(controllerList);

    QList<int> axes;
    QList<int> buttons;

    if (!controllerList.isEmpty())
    {
        JoystickDevice *tempDevice = controllerList.first();
        for (int i=0; i < tempDevice->getNumAxes(); i++)
        {
            axes.append(tempDevice->getAxisValue(i));
        }

        for (int i=0; i < tempDevice->getNumButtons(); i++)
        {
            buttons.append(tempDevice->getButtonValue(i));
        }
    }

    for (int i=0; i < axes.size(); i++)
    {
        int tempValue = axes.at(i);
        QString tempString;
        if (tempValue <= ABS_HAT3Y)
        {
            tempString = QString::fromStdString(axesNames[tempValue]);
        }
        else
        {
            tempString = QString("ABS_#%1").arg(tempValue);
        }

        ui->lTriggerComboBox->addItem(QString("%1").arg(tempString), tempValue);
        ui->rTriggerComboBox->addItem(QString("%1").arg(tempString), tempValue);
        ui->dpadXComboBox->addItem(QString("%1").arg(tempString), tempValue);
        ui->dpadYComboBox->addItem(QString("%1").arg(tempString), tempValue);
        ui->lStickXComboBox->addItem(QString("%1").arg(tempString), tempValue);
        ui->lStickYComboBox->addItem(QString("%1").arg(tempString), tempValue);
        ui->rStickXComboBox->addItem(QString("%1").arg(tempString), tempValue);
        ui->rStickYComboBox->addItem(QString("%1").arg(tempString), tempValue);
    }

    for (int i=0; i < buttons.size(); i++)
    {
        int tempValue = buttons.at(i) - BTN_SOUTH;

        ui->lBumperComboBox->addItem(QString("%1").arg(buttonNames[tempValue].c_str()), tempValue);
        ui->rBumperComboBox->addItem(QString("%1").arg(buttonNames[tempValue].c_str()), tempValue);
        ui->backComboBox->addItem(QString("%1").arg(buttonNames[tempValue].c_str()), tempValue);
        ui->startComboBox->addItem(QString("%1").arg(buttonNames[tempValue].c_str()), tempValue);
        ui->aButtonComboBox->addItem(QString("%1").arg(buttonNames[tempValue].c_str()), tempValue);
        ui->bButtonComboBox->addItem(QString("%1").arg(buttonNames[tempValue].c_str()), tempValue);
        ui->xButtonComboBox->addItem(QString("%1").arg(buttonNames[tempValue].c_str()), tempValue);
        ui->yButtonComboBox->addItem(QString("%1").arg(buttonNames[tempValue].c_str()), tempValue);
        ui->guideComboBox->addItem(QString("%1").arg(buttonNames[tempValue].c_str()), tempValue);
        ui->lsbComboBox->addItem(QString("%1").arg(buttonNames[tempValue].c_str()), tempValue);
        ui->rsbComboBox->addItem(QString("%1").arg(buttonNames[tempValue].c_str()), tempValue);
    }

    ui->tabWidget->setTabText(0, "Command");
    ui->tabWidget->setTabText(1, "Config");

    generateOutputString();

    connect(ui->lTriggerComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(generateOutputString()));
    connect(ui->rTriggerComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(generateOutputString()));
    connect(ui->dpadXComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(generateOutputString()));
    connect(ui->dpadYComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(generateOutputString()));
    connect(ui->lStickXComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(generateOutputString()));
    connect(ui->lStickYComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(generateOutputString()));
    connect(ui->rStickXComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(generateOutputString()));
    connect(ui->rStickYComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(generateOutputString()));

    connect(ui->lBumperComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(generateOutputString()));
    connect(ui->rBumperComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(generateOutputString()));
    connect(ui->backComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(generateOutputString()));
    connect(ui->startComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(generateOutputString()));
    connect(ui->aButtonComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(generateOutputString()));
    connect(ui->bButtonComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(generateOutputString()));
    connect(ui->xButtonComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(generateOutputString()));
    connect(ui->yButtonComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(generateOutputString()));
    connect(ui->guideComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(generateOutputString()));
    connect(ui->lsbComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(generateOutputString()));
    connect(ui->rsbComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(generateOutputString()));

    connect(ui->mimicXpadCheckBox, SIGNAL(clicked(bool)), this, SLOT(generateOutputString()));
    connect(ui->invertLYCheckBox, SIGNAL(clicked(bool)), this, SLOT(generateOutputString()));
    connect(ui->invertRYCheckBox, SIGNAL(clicked(bool)), this, SLOT(generateOutputString()));

    connect(ui->recordAPushButton, SIGNAL(clicked(bool)), this, SLOT(setupRecordAButton()));
    connect(ui->recordBPushButton, SIGNAL(clicked(bool)), this, SLOT(setupRecordBButton()));
    connect(ui->recordXPushButton, SIGNAL(clicked(bool)), this, SLOT(setupRecordXButton()));
    connect(ui->recordYPushButton, SIGNAL(clicked(bool)), this, SLOT(setupRecordYButton()));

    connect(ui->recordLBumperPushButton, SIGNAL(clicked(bool)), this, SLOT(setupRecordLBButton()));
    connect(ui->recordRBumperPushButton, SIGNAL(clicked(bool)), this, SLOT(setupRecordRBButton()));
    connect(ui->recordLSBPushButton, SIGNAL(clicked(bool)), this, SLOT(setupRecordLSBButton()));
    connect(ui->recordRSBPushButton, SIGNAL(clicked(bool)), this, SLOT(setupRecordRSBButton()));

    connect(ui->recordBackPushButton, SIGNAL(clicked(bool)), this, SLOT(setupRecordBackButton()));
    connect(ui->recordStartPushButton, SIGNAL(clicked(bool)), this, SLOT(setupRecordStartButton()));
    connect(ui->recordGuidePushButton, SIGNAL(clicked(bool)), this, SLOT(setupRecordGuideButton()));

    connect(ui->recordDPadXPushButton, SIGNAL(clicked(bool)), this, SLOT(setupRecordDPadXAxis()));
    connect(ui->recordDPadYPushButton, SIGNAL(clicked(bool)), this, SLOT(setupRecordDPadYAxis()));

    connect(ui->recordLTriggerPushButton, SIGNAL(clicked(bool)), this, SLOT(setupRecordLTriggerAxis()));
    connect(ui->recordRTriggerPushButton, SIGNAL(clicked(bool)), this, SLOT(setupRecordRTriggerAxis()));

    connect(ui->recordLStickXPushButton, SIGNAL(clicked(bool)), this, SLOT(setupRecordLStickXAxis()));
    connect(ui->recordLStickYPushButton, SIGNAL(clicked(bool)), this, SLOT(setupRecordLStickYAxis()));

    connect(ui->recordRStickXPushButton, SIGNAL(clicked(bool)), this, SLOT(setupRecordRStickXAxis()));
    connect(ui->recordRStickYPushButton, SIGNAL(clicked(bool)), this, SLOT(setupRecordRStickYAxis()));
}

MainWindow::~MainWindow()
{
    purgeControllerList();

    delete ui;
}

void MainWindow::generateOutputString()
{
    generateCommandOutputString();
    generateConfigOutputString();
}

void MainWindow::generateCommandOutputString()
{
    QString temp;
    QStringList tempList;
    tempList.append(QString("--evdev-absmap %1=x1,%2=y1")
                    .arg(axesNames[ui->lStickXComboBox->currentData().toInt()].c_str())
                    .arg(axesNames[ui->lStickYComboBox->currentData().toInt()].c_str()));

    tempList.append(QString("--evdev-absmap %1=x2,%2=y2")
                    .arg(axesNames[ui->rStickXComboBox->currentData().toInt()].c_str())
                    .arg(axesNames[ui->rStickYComboBox->currentData().toInt()].c_str()));

    tempList.append(QString("--evdev-absmap %1=lt,%2=rt")
                    .arg(axesNames[ui->lTriggerComboBox->currentData().toInt()].c_str())
                    .arg(axesNames[ui->rTriggerComboBox->currentData().toInt()].c_str()));

    tempList.append(QString("--evdev-absmap %1=dpad_x,%2=dpad_y")
                    .arg(axesNames[ui->dpadXComboBox->currentData().toInt()].c_str())
                    .arg(axesNames[ui->dpadYComboBox->currentData().toInt()].c_str()));

    tempList.append(QString("--evdev-keymap %1=tl,%2=tr")
                    .arg(buttonNames[ui->lsbComboBox->currentData().toInt()].c_str())
                    .arg(buttonNames[ui->rsbComboBox->currentData().toInt()].c_str()));

    tempList.append(QString("--evdev-keymap %1=lb,%2=rb")
                    .arg(buttonNames[ui->lBumperComboBox->currentData().toInt()].c_str())
                    .arg(buttonNames[ui->rBumperComboBox->currentData().toInt()].c_str()));

    tempList.append(QString("--evdev-keymap %1=a,%2=b,%3=x,%4=y")
                    .arg(buttonNames[ui->aButtonComboBox->currentData().toInt()].c_str())
                    .arg(buttonNames[ui->bButtonComboBox->currentData().toInt()].c_str())
                    .arg(buttonNames[ui->xButtonComboBox->currentData().toInt()].c_str())
                    .arg(buttonNames[ui->yButtonComboBox->currentData().toInt()].c_str()));

    tempList.append(QString("--evdev-keymap %1=back,%2=guide,%3=start")
                    .arg(buttonNames[ui->backComboBox->currentData().toInt()].c_str())
                    .arg(buttonNames[ui->guideComboBox->currentData().toInt()].c_str())
                    .arg(buttonNames[ui->startComboBox->currentData().toInt()].c_str()));

    if (ui->invertLYCheckBox->isChecked() || ui->invertRYCheckBox->isChecked())
    {
        QStringList invertTempList;
        if (ui->invertLYCheckBox->isChecked())
        {
            invertTempList.append("-y1=y1");
        }

        if (ui->invertRYCheckBox->isChecked())
        {
            invertTempList.append("-y2=y2");
        }

        tempList.append(QString("--axismap %1").arg(invertTempList.join(",")));
    }

    /*tempList.append(QString("--evdev-keymap %1=a,%2=b,%3=x,%4=y")
                    .arg(buttonNames[ui->lTriggerComboBox->currentData().toInt()].c_str())
                    .arg(buttonNames[ui->rTriggerComboBox->currentData().toInt()].c_str())
                    .arg(buttonNames[ui->rTriggerComboBox->currentData().toInt()].c_str())
                    .arg(buttonNames[ui->rTriggerComboBox->currentData().toInt()].c_str()));
    */

    if (ui->mimicXpadCheckBox->isChecked())
    {
        tempList.append(QString("--mimic-xpad"));
    }

    temp.append(tempList.join(" \\\n"));
    ui->outputTextEdit->setText(temp);
}

void MainWindow::generateConfigOutputString()
{
    QString temp;
    QStringList tempList;

    if (ui->mimicXpadCheckBox->isChecked())
    {
        tempList.append(QString("[xboxdrv]"));
        tempList.append(QString("mimic-xpad=true"));
        tempList.append(QString(""));
    }

    tempList.append(QString("[evdev-absmap]"));
    tempList.append(QString("%1 = dpad_x").arg(axesNames[ui->dpadXComboBox->currentData().toInt()].c_str()));
    tempList.append(QString("%1 = dpad_y").arg(axesNames[ui->dpadYComboBox->currentData().toInt()].c_str()));

    tempList.append(QString(""));
    tempList.append(QString("%1 = X1").arg(axesNames[ui->lStickXComboBox->currentData().toInt()].c_str()));
    tempList.append(QString("%1 = Y1").arg(axesNames[ui->lStickYComboBox->currentData().toInt()].c_str()));

    tempList.append(QString(""));
    tempList.append(QString("%1 = X2").arg(axesNames[ui->rStickXComboBox->currentData().toInt()].c_str()));
    tempList.append(QString("%1 = Y2").arg(axesNames[ui->rStickYComboBox->currentData().toInt()].c_str()));

    tempList.append(QString(""));
    tempList.append(QString("%1 = LT").arg(axesNames[ui->lTriggerComboBox->currentData().toInt()].c_str()));
    tempList.append(QString("%1 = RT").arg(axesNames[ui->lTriggerComboBox->currentData().toInt()].c_str()));

    if (ui->invertLYCheckBox->isChecked() || ui->invertRYCheckBox->isChecked())
    {
        tempList.append(QString(""));

        if (ui->invertLYCheckBox->isChecked())
        {
            tempList.append("-Y1=Y1");
        }

        if (ui->invertRYCheckBox->isChecked())
        {
            tempList.append("-Y2=Y2");
        }
    }

    tempList.append(QString(""));
    tempList.append(QString("[evdev-keymap]"));
    tempList.append(QString("%1 = LB").arg(buttonNames[ui->lBumperComboBox->currentData().toInt()].c_str()));
    tempList.append(QString("%1 = RB").arg(buttonNames[ui->rBumperComboBox->currentData().toInt()].c_str()));

    tempList.append(QString(""));
    tempList.append(QString("%1 = A").arg(buttonNames[ui->aButtonComboBox->currentData().toInt()].c_str()));
    tempList.append(QString("%1 = B").arg(buttonNames[ui->bButtonComboBox->currentData().toInt()].c_str()));
    tempList.append(QString("%1 = X").arg(buttonNames[ui->xButtonComboBox->currentData().toInt()].c_str()));
    tempList.append(QString("%1 = Y").arg(buttonNames[ui->yButtonComboBox->currentData().toInt()].c_str()));

    tempList.append(QString(""));
    tempList.append(QString("%1 = TL").arg(buttonNames[ui->lsbComboBox->currentData().toInt()].c_str()));
    tempList.append(QString("%1 = TR").arg(buttonNames[ui->rsbComboBox->currentData().toInt()].c_str()));

    tempList.append(QString(""));
    tempList.append(QString("%1 = start").arg(buttonNames[ui->startComboBox->currentData().toInt()].c_str()));
    tempList.append(QString("%1 = back").arg(buttonNames[ui->backComboBox->currentData().toInt()].c_str()));
    tempList.append(QString("%1 = guide").arg(buttonNames[ui->guideComboBox->currentData().toInt()].c_str()));

    temp.append(tempList.join("\n"));
    ui->outputConfigTextEdit->setText(temp);
}


void MainWindow::purgeControllerList()
{
    QListIterator<JoystickDevice*> temp(controllerList);
    temp.toBack();
    while (temp.hasPrevious())
    {
        JoystickDevice *tempDevice = temp.previous();
        if (tempDevice)
        {
            delete tempDevice;
            tempDevice = 0;
        }
    }

    controllerList.clear();
}

void MainWindow::setupRecordAButton()
{
    currentBox = ui->aButtonComboBox;
    setupRecordTest();
}

void MainWindow::setupRecordBButton()
{
    currentBox = ui->bButtonComboBox;
    setupRecordTest();
}

void MainWindow::setupRecordXButton()
{
    currentBox = ui->xButtonComboBox;
    setupRecordTest();
}

void MainWindow::setupRecordYButton()
{
    currentBox = ui->yButtonComboBox;
    setupRecordTest();
}

void MainWindow::setupRecordLBButton()
{
    currentBox = ui->lBumperComboBox;
    setupRecordTest();
}

void MainWindow::setupRecordRBButton()
{
    currentBox = ui->rBumperComboBox;
    setupRecordTest();
}

void MainWindow::setupRecordLSBButton()
{
    currentBox = ui->lsbComboBox;
    setupRecordTest();
}

void MainWindow::setupRecordRSBButton()
{
    currentBox = ui->rsbComboBox;
    setupRecordTest();
}

void MainWindow::setupRecordBackButton()
{
    currentBox = ui->backComboBox;
    setupRecordTest();
}

void MainWindow::setupRecordStartButton()
{
    currentBox = ui->startComboBox;
    setupRecordTest();
}

void MainWindow::setupRecordGuideButton()
{
    currentBox = ui->guideComboBox;
    setupRecordTest();
}


void MainWindow::setupRecordDPadXAxis()
{
    currentBox = ui->dpadXComboBox;
    setupRecordTest();
}

void MainWindow::setupRecordDPadYAxis()
{
    currentBox = ui->dpadYComboBox;
    setupRecordTest();
}

void MainWindow::setupRecordLTriggerAxis()
{
    currentBox = ui->lTriggerComboBox;
    setupRecordTest();
}

void MainWindow::setupRecordRTriggerAxis()
{
    currentBox = ui->rTriggerComboBox;
    setupRecordTest();
}

void MainWindow::setupRecordLStickXAxis()
{
    currentBox = ui->lStickXComboBox;
    setupRecordTest();
}

void MainWindow::setupRecordLStickYAxis()
{
    currentBox = ui->lStickYComboBox;
    setupRecordTest();
}

void MainWindow::setupRecordRStickXAxis()
{
    currentBox = ui->rStickXComboBox;
    setupRecordTest();
}

void MainWindow::setupRecordRStickYAxis()
{
    currentBox = ui->rStickYComboBox;
    setupRecordTest();
}


void MainWindow::setupRecordTest()
{
    if (controllerList.first())
    {
        QMessageBox *msg = new QMessageBox(this);
        msg->setWindowTitle(QString("Record Action"));
        msg->setText(QString("Please move an axis or press a button on your controller."));
        msg->setStandardButtons(QMessageBox::Close);
        establishRecordingConnections();
        msg->show();
        connect(this, SIGNAL(joystickEventAssigned()), msg, SLOT(close()));
        connect(msg, SIGNAL(finished(int)), this, SLOT(disconnectRecordingConnections()));
        connect(msg, SIGNAL(finished(int)), msg, SLOT(deleteLater()));
    }
}

void MainWindow::establishRecordingConnections()
{
    JoystickDevice *tempDevice = controllerList.first();
    if (tempDevice)
    {
        tempDevice->clearEventQueue();
        connect(tempDevice, SIGNAL(axisEvent(int,int)), this, SLOT(determineAxisAction(int,int)));
        connect(tempDevice, SIGNAL(buttonEvent(int,bool)), this, SLOT(determineButtonAction(int,bool)));
        connect(&pollEventTimer, SIGNAL(timeout()), tempDevice, SLOT(pollEvents()));
        pollEventTimer.start();
    }
}

void MainWindow::disconnectRecordingConnections()
{
    JoystickDevice *tempDevice = controllerList.first();
    if (tempDevice)
    {
        disconnect(tempDevice, SIGNAL(axisEvent(int,int)), this, 0);
        disconnect(tempDevice, SIGNAL(buttonEvent(int,bool)), this, 0);
        disconnect(&pollEventTimer, SIGNAL(timeout()), tempDevice, 0);
        disconnect(this, SIGNAL(joystickEventAssigned()), 0, 0);
        pollEventTimer.stop();
        currentBox = 0;
    }
}

void MainWindow::determineAxisAction(int code, int value)
{
    if (qAbs(value) >= 20000 && currentBox)
    {
        int index = currentBox->findData(code);
        if (index > -1)
        {
            currentBox->setCurrentIndex(index);
        }

        emit joystickEventAssigned();
    }
}

void MainWindow::determineButtonAction(int code, bool pressed)
{
    if (!pressed && currentBox)
    {
        int tempValue = code - BTN_SOUTH;
        int index = currentBox->findData(tempValue);
        if (index > -1)
        {
            currentBox->setCurrentIndex(index);
        }

        emit joystickEventAssigned();
    }
}
