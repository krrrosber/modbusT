#ifndef CONFIG_H
#define CONFIG_H

#include <qcontainerfwd.h>
#include <qcoreapplication.h>
#include <QMap>

struct Config
{

    inline static QString ipAddress = "192.168.1.10";
    inline static int port = 502;
    inline static int pollInterval = 500;
    inline static QStringList imagePaths {"image/0.png","image/1.png","image/2.png","image/3.png","image/4.png","image/5.png","image/6.png","image/7.png"};
    inline static double angelX = 348;
    inline static double angelY = 358;
    inline static double angelZ = 368;

};

#endif // CONFIG_H
