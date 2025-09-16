#ifndef MODBUSCONNECT_H
#define MODBUSCONNECT_H

#include <qlabel.h>
#include <qmodbusclient.h>
#include <QTimer>
#include <qmodbustcpclient.h>

class ModbusConnect:public QObject
{ 
    Q_OBJECT
public:
    explicit ModbusConnect(QObject *parent =nullptr);

    void startConnect();
    bool isConnected() const;
    void readRegister();
    static double registerToFloat(const QModbusDataUnit &d, int index1, int index2, bool swapWords, bool swapBytes);

signals:
    void angelXReady(double value);
    void angelYReady(double value);
    void angelZReady(double value);
    void imageChange(int i);

private:
    QModbusTcpClient *modbusClient;
    QModbusReply *pendingRead = nullptr;

};

#endif // MODBUSCONNECT_H
