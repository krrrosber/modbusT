#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "modbusconnect.h"
#include <QMainWindow>
#include <QObject>

class MainWindow:public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    ModbusConnect *client;
};

#endif // MAINWINDOW_H
