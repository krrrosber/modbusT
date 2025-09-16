#include "modbusconnect.h"
#include <QtSerialBus/QModbusTcpClient>
#include <QtSerialBus/QModbusDataUnit>
#include <QtSerialBus/QModbusReply>
#include "config.h"

ModbusConnect::ModbusConnect(QObject *parent) :
    QObject(parent),
    modbusClient(new QModbusTcpClient(this))
{
}

void ModbusConnect::startConnect(){
    if(!modbusClient) return;
    modbusClient->setConnectionParameter(QModbusDevice::NetworkAddressParameter, Config::ipAddress);
    modbusClient->setConnectionParameter(QModbusDevice::NetworkPortParameter,Config::port);
    modbusClient->setTimeout(2000);
    modbusClient->setNumberOfRetries(1);
    modbusClient->connectDevice();
}

bool ModbusConnect::isConnected() const {
    return modbusClient && modbusClient->state() == QModbusDevice::ConnectedState;
}


 double ModbusConnect::registerToFloat(const QModbusDataUnit &du, int idxHigh, int idxLow,bool swapWords, bool swapBytes ){
    // читаем два регистра (они передаются как quint16)
    qDebug()<<du.value(idxHigh)<<" "<<du.value(idxLow);

    uint16_t w1 = static_cast<uint16_t>(du.value(idxHigh));
    uint16_t w2 = static_cast<uint16_t>(du.value(idxLow));

    // если нужно поменять байты внутри каждого регистра (A B -> B A)
    if (swapBytes) {
        auto swap16 = [](uint16_t x) -> uint16_t { return static_cast<uint16_t>((x << 8) | (x >> 8)); };
        w1 = swap16(w1);
        w2 = swap16(w2);
    }

    // составляем 32-битное слово: по умолчанию w1 = старшее слово, w2 = младшее
    uint32_t combined = 0;
    if (swapWords) {
        combined = (static_cast<uint32_t>(w2) << 16) | w1;
    } else {
        combined = (static_cast<uint32_t>(w1) << 16) | w2;
    }

    // безопасно копируем биты в float (избегаем UB с reinterpret_cast)
    float f;
    std::memcpy(&f, &combined, sizeof(f));
    return f;
}

 void ModbusConnect::readAngelX()
 {
     qDebug() << "readAngelX() called";

     if (!modbusClient) {
         qDebug() << "readAngelX: no modbus client";
         return;
     }

     // если уже есть незавершённый запрос — пропускаем
     if (pendingReadX && !pendingReadX->isFinished()) {
         qDebug() << "readAngelY: pending request exists, skip";
         return;
     }

     QModbusDataUnit read(QModbusDataUnit::HoldingRegisters, Config::angelX, 2);
     QModbusReply *reply = modbusClient->sendReadRequest(read, /*unitId*/ 1);

     if (!reply) {
         qWarning() << "readAngelY: sendReadRequest returned nullptr";
         return;
     }

     // сохраняем как pending для предотвращения дублей
     pendingReadX = reply;

     // если уже завершён (маловероятно), обработаем синхронно
     if (reply->isFinished()) {
         if (reply->error() == QModbusDevice::NoError) {
             auto result = reply->result();
             float v = registerToFloat(result, 0, 1, true, false);
             qDebug() << "readAngelY: immediate result =" << v;
             emit angelXReady(static_cast<double>(v));
         } else {
             qWarning() << "readAngelY immediate error:" << reply->errorString();
         }
         // очистка
         pendingReadX = nullptr;
         reply->deleteLater();
         return;
     }

     // асинхронная обработка результата
     connect(reply, &QModbusReply::finished, this, [this, reply]() {
         if (reply->error() == QModbusDevice::NoError) {
             auto result = reply->result();
             float v = registerToFloat(result, 0, 1, true, false);
             qDebug() << "readAngelX: got result =" << v;
             emit angelXReady(static_cast<double>(v));
         } else {
             qWarning() << "readAngelY error:" << reply->errorString();
         }
         // очистка pending и освобождение reply — делаем это ТУТ (только тут)
         if (reply == pendingReadX) pendingReadX = nullptr;
         reply->deleteLater();
     });
 }

void ModbusConnect::readAngelY()
{
        qDebug() << "readAngelY() called";

        if (!modbusClient) {
            qDebug() << "readAngelY: no modbus client";
            return;
        }

        // если уже есть незавершённый запрос — пропускаем
        if (pendingReadY && !pendingReadY->isFinished()) {
            qDebug() << "readAngelY: pending request exists, skip";
            return;
        }

        QModbusDataUnit read(QModbusDataUnit::HoldingRegisters, Config::angelY, 2);
        QModbusReply *reply = modbusClient->sendReadRequest(read, /*unitId*/ 1);

        if (!reply) {
            qWarning() << "readAngelY: sendReadRequest returned nullptr";
            return;
        }

        // сохраняем как pending для предотвращения дублей
        pendingReadY = reply;

        // если уже завершён (маловероятно), обработаем синхронно
        if (reply->isFinished()) {
            if (reply->error() == QModbusDevice::NoError) {
                auto result = reply->result();
                float v = registerToFloat(result, 0, 1, true, false);
                qDebug() << "readAngelY: immediate result =" << v;
                emit angelYReady(static_cast<double>(v));
            } else {
                qWarning() << "readAngelY immediate error:" << reply->errorString();
            }
            // очистка
            pendingReadY = nullptr;
            reply->deleteLater();
            return;
        }

        // асинхронная обработка результата
        connect(reply, &QModbusReply::finished, this, [this, reply]() {
            if (reply->error() == QModbusDevice::NoError) {
                auto result = reply->result();
                float v = registerToFloat(result, 0, 1, true, false);
                qDebug() << "readAngelY: got result =" << v;
                emit angelYReady(static_cast<double>(v));
            } else {
                qWarning() << "readAngelY error:" << reply->errorString();
            }
            // очистка pending и освобождение reply — делаем это ТУТ (только тут)
            if (reply == pendingReadY) pendingReadY = nullptr;
            reply->deleteLater();
        });
    }



    void ModbusConnect::readAngelZ()
    {
        qDebug() << "readAngelY() called";

        if (!modbusClient) {
            qDebug() << "readAngelY: no modbus client";
            return;
        }

        // если уже есть незавершённый запрос — пропускаем
        if (pendingReadZ && !pendingReadZ->isFinished()) {
            qDebug() << "readAngelY: pending request exists, skip";
            return;
        }

        QModbusDataUnit read(QModbusDataUnit::HoldingRegisters, Config::angelZ, 2);
        QModbusReply *reply = modbusClient->sendReadRequest(read, /*unitId*/ 1);

        if (!reply) {
            qWarning() << "readAngelY: sendReadRequest returned nullptr";
            return;
        }

        // сохраняем как pending для предотвращения дублей
        pendingReadZ = reply;

        // если уже завершён (маловероятно), обработаем синхронно
        if (reply->isFinished()) {
            if (reply->error() == QModbusDevice::NoError) {
                auto result = reply->result();
                float v = registerToFloat(result, 0, 1, true, false);
                qDebug() << "readAngelY: immediate result =" << v;
                emit angelZReady(static_cast<double>(v));
            } else {
                qWarning() << "readAngelY immediate error:" << reply->errorString();
            }
            // очистка
            pendingReadZ = nullptr;
            reply->deleteLater();
            return;
        }

        // асинхронная обработка результата
        connect(reply, &QModbusReply::finished, this, [this, reply]() {
            if (reply->error() == QModbusDevice::NoError) {
                auto result = reply->result();
                float v = registerToFloat(result, 0, 1, true, false);
                qDebug() << "Z: got result =" << v;
                emit angelZReady(static_cast<double>(v));
            } else {
                qWarning() << "readAngelZ error:" << reply->errorString();
            }
            // очистка pending и освобождение reply — делаем это ТУТ (только тут)
            if (reply == pendingReadZ) pendingReadZ = nullptr;
            reply->deleteLater();
        });
    }


