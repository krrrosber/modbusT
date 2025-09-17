#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "modbusconnect.h"
#include <QMainWindow>
#include <QObject>
#include <qmodbustcpclient.h>
#include <qstackedwidget.h>

class MainWindow:public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent *event) override;


private:
    ModbusConnect *clientModbus;
    QLabel *angelXLabel;
    QLabel *angelYLabel;
    QLabel *angelZLabel;
    QTimer *pollTimer;
    QStackedWidget *image;
    QWidget *leftPanel;
    QVector<QPixmap> origPixmaps;
    void updateImages();

};

#endif // MAINWINDOW_H
