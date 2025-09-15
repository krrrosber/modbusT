#ifndef CONFIG_H
#define CONFIG_H

#include <qcontainerfwd.h>
#include <qcoreapplication.h>
#include <QMap>

class Config
{
public:
    Config();
    static Config loadJson(const QString &path);
    bool validate();

private:
    QString ipAddress = "192.168.1.10";
    int port = 502;
    int pollInterval = 200;
    double backoffBaseS = 0.5;
    double backoffMaxs=60;
    QStringList imagePaths {"image/0.png","image/1.png","image/2.png","image/3.png","image/4.png","image/5.png","image/6.png","image/7.png"};
    //QMap<int,int> bitToImageMap;
    double angelX = 840;
    double angelY = 856;
    double angelZ = 872;

};

#endif // CONFIG_H
