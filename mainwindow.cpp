#include "mainwindow.h"
#include <qgridlayout.h>
#include <qlabel.h>
#include <qstackedwidget.h>
#include "config.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    clientModbus(new ModbusConnect(this)),
    angelXLabel(nullptr),
    angelYLabel(nullptr),
    angelZLabel(nullptr),
    pollTimer(new QTimer(this))

{


    this->setFixedSize(1024,800);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QGridLayout *layout = new QGridLayout(centralWidget);

    QLabel *textX = new QLabel("Изменение угла по оси X",centralWidget);
    angelXLabel = new QLabel("0",centralWidget);

    QLabel *textY = new QLabel("Изменение угла по оси Y",centralWidget);
    angelYLabel = new QLabel(centralWidget);

    QLabel *textZ = new QLabel("Изменение угла по оси Z",centralWidget);
    angelZLabel = new QLabel(centralWidget);

     image = new QStackedWidget(centralWidget);

    for (const QString &path : std::as_const(Config::imagePaths)) {
        QLabel *label = new QLabel;
        label->setPixmap(QPixmap(path));
        label->setScaledContents(true); // чтобы картинка растягивалась
        label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        image->addWidget(label);
    }

    image->setCurrentIndex(0);


    layout->addWidget(textX, 0,0);
    layout->addWidget(angelXLabel, 1,0);
    layout->addWidget(textY, 2,0);
    layout->addWidget(angelYLabel, 3,0);
    layout->addWidget(textZ, 4,0);
    layout->addWidget(angelZLabel, 5,0);
    layout->addWidget(image,0,1,6,1);
   // layout->setColumnMinimumWidth(0, 200);
    layout->setColumnStretch(0, 2); // текстовая колонка
    layout->setColumnStretch(1, 5); // колонка с картинкой
    layout->setColumnMinimumWidth(0, 300);


    connect(clientModbus,&ModbusConnect::angelXReady, this, [this](double d){
        angelXLabel->setText(QString::number(d));
    });
    connect(clientModbus,&ModbusConnect::angelYReady, this, [this](double d){
        angelYLabel->setText(QString::number(d));
    });
    connect(clientModbus,&ModbusConnect::angelZReady, this, [this](double d){
        angelZLabel->setText(QString::number(d));
    });
    connect(clientModbus,&ModbusConnect::imageChange, this,[this](int i){
        image->setCurrentIndex(i);
    });

    clientModbus->startConnect();

    pollTimer->setInterval((Config::pollInterval));
    connect(pollTimer,&QTimer::timeout,this,[this](){
if(clientModbus->isConnected()){
            clientModbus->readRegister();
}
else{
    clientModbus->startConnect();
}
    });
    pollTimer->start();
}

MainWindow::~MainWindow()
{
}
