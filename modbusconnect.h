#ifndef MODBUSCONNECT_H
#define MODBUSCONNECT_H

#include <qlabel.h>
#include <qmodbusclient.h>
#include <QTimer>

class ModbusConnect:public QObject
{ 
    Q_OBJECT
public:
    ModbusConnect();

    void connected();
    void disconnected();
    void bitsRead();

private:
    QModbusClient *client;
    QTimer pollTimer;
    QTimer reconnectTimer;
    bool pendingRequest;
};

#endif // MODBUSCONNECT_H
