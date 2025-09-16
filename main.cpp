#include <QApplication>
#include "config.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

   MainWindow window;
    /*QModbusTcpClient *client = new QModbusTcpClient();

    client->setConnectionParameter(QModbusDevice::NetworkAddressParameter, "192.168.1.10");
    client->setConnectionParameter(QModbusDevice::NetworkPortParameter, 502);

    // сигнал на изменение состояния соединения
    QObject::connect(client, &QModbusDevice::stateChanged, [client](QModbusDevice::State state){
        if(state == QModbusDevice::ConnectedState){
            qDebug() << "Подключение успешное";

            QModbusDataUnit readUnit(QModbusDataUnit::HoldingRegisters, 348, 2);
            QModbusReply *reply = client->sendReadRequest(readUnit, 1); // slave id = 1

            if(!reply){
                qDebug() << "Ошибка при отправке запроса";
                QCoreApplication::quit();
                return;
            }

            QObject::connect(reply, &QModbusReply::finished, [reply](){
                auto result = reply->result();
                qDebug() << "X =" << result.value(0);
                qDebug() << "X =" << result.value(1);
                reply->deleteLater();
                QCoreApplication::quit();
            });
        }
        else if(state == QModbusDevice::UnconnectedState){
            qDebug() << "Устройство не подключено";
        }
    });

    client->connectDevice();
   */
   window.show();
    return app.exec();
}
