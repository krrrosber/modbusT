#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "modbusconnect.h"
#include <QMainWindow>
#include <QObject>
#include <qmodbustcpclient.h>

class MainWindow:public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    ModbusConnect *clientModbus;
    QLabel *angelXLabel;
    QLabel *angelYLabel;
    QLabel *angelZLabel;
    QTimer *pollTimer;
};

#endif // MAINWINDOW_H
