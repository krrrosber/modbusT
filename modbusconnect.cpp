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


float regsToFloat(quint16 reg1, quint16 reg2,
                                 bool swapWords, bool swapBytes)
{
    // меняем местами слова
    quint16 hi = swapWords ? reg1 : reg2;
    quint16 lo = swapWords ? reg2 : reg1;

    // меняем байты внутри слов
    auto swap16 = [](quint16 w) {
        return quint16((w >> 8) | (w << 8));
    };
    if (swapBytes) {
        hi = swap16(hi);
        lo = swap16(lo);
    }

    quint32 raw = (quint32(hi) << 16) | lo;
    float f;
    memcpy(&f, &raw, sizeof(f));
    return f;
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

    auto r0 = du.value(idxHigh);
    auto r1 = du.value(idxLow);

    float f1 = regsToFloat(r0, r1, false, false);
    float f2 = regsToFloat(r0, r1, true,  false);
    float f3 = regsToFloat(r0, r1, false, true);
    float f4 = regsToFloat(r0, r1, true,  true);

    qDebug() << "try (no swap):" << f1;
    qDebug() << "try (swap words):" << f2;
    qDebug() << "try (swap bytes):" << f3;
    qDebug() << "try (both swaps):" << f4;


    quint16 word1 = du.value(idxHigh);
    quint16 word2 = du.value(idxLow);
    qDebug() << "Raw registers:"
             << QString("0x%1").arg(du.value(idxHigh), 4, 16, QLatin1Char('0'))
             << QString("0x%1").arg(du.value(idxLow), 4, 16, QLatin1Char('0'));


    // Меняем местами слова (если нужно)
    quint16 hi = swapWords ? word1 : word2;
    quint16 lo = swapWords ? word2 : word1;

    // Меняем байты внутри слова (если нужно)
    if (swapBytes) {
        hi = (hi >> 8) | (hi << 8);
        lo = (lo >> 8) | (lo << 8);
    }

    quint32 raw = (quint32(hi) << 16) | lo;
    float f;
    memcpy(&f, &raw, sizeof(float));
    return f;
}

    void ModbusConnect::readRegister(){
        {
            qDebug() << "readAngels() called";

            if (!modbusClient) {
                qDebug() << "readAngelY: no modbus client";
                return;
            }

            // если уже есть незавершённый запрос — пропускаем
            if (pendingRead && !pendingRead->isFinished()) {
                qDebug() << "readAngels: pending request exists, skip";
                return;
            }

            QModbusDataUnit read(QModbusDataUnit::HoldingRegisters, 347, 7);
            QModbusReply *reply = modbusClient->sendReadRequest(read, /*unitId*/ 1);

            if (!reply) {
                qWarning() << "readAngels: sendReadRequest returned nullptr";
                return;
            }

            // сохраняем как pending для предотвращения дублей
            pendingRead = reply;

            // если уже завершён (маловероятно), обработаем синхронно
            if (reply->isFinished()) {
                if (reply->error() == QModbusDevice::NoError) {
                    auto result = reply->result();

                    float v1 = registerToFloat(result, 1, 2, false, false);
                    qDebug() << "readAngelX: immediate result =" << v1;
                    emit angelXReady(static_cast<double>(v1));

                    float v2 = registerToFloat(result, 3, 4, false, false);
                    qDebug() << "readAngelY: immediate result =" << v2;
                    emit angelYReady(static_cast<double>(v2));

                    float v3 = registerToFloat(result, 5, 6, false, false);
                    qDebug() << "readAngelZ: immediate result =" << v3;
                    emit angelZReady(static_cast<double>(v3));
                } else {
                    qWarning() << "readAngelY immediate error:" << reply->errorString();
                }
                // очистка
                pendingRead = nullptr;
                reply->deleteLater();
                return;
            }

            // асинхронная обработка результата
            connect(reply, &QModbusReply::finished, this, [this, reply]() {
                if (reply->error() == QModbusDevice::NoError) {
                    auto result = reply->result();

                    int im = result.value(0);
                      qDebug() << "image number immediate result =" << im;
                    emit imageChange(im);

                    float v1 = registerToFloat(result, 1, 2, false, false);
                    qDebug() << "readAngelX: immediate result =" << v1;
                    emit angelXReady(static_cast<double>(v1));

                    float v2 = registerToFloat(result, 3, 4, false, false);
                    qDebug() << "readAngelY: immediate result =" << v2;
                    emit angelYReady(static_cast<double>(v2));

                    float v3 = registerToFloat(result, 5, 6, false, false);
                    qDebug() << "readAngelZ: immediate result =" << v3;
                    emit angelZReady(static_cast<double>(v3));
                } else {
                    qWarning() << "readAngelZ error:" << reply->errorString();
                }
                // очистка pending и освобождение reply — делаем это ТУТ (только тут)
                if (reply == pendingRead) pendingRead = nullptr;
                reply->deleteLater();
            });
        }
    }
