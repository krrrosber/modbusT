#include "mainwindow.h"
#include <qgridlayout.h>
#include <qlabel.h>
#include <qstackedwidget.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    this->setFixedSize(1024,800);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QGridLayout *layout = new QGridLayout(centralWidget);

    QLabel *textX = new QLabel("Изменение угла по оси X",centralWidget);
    QLabel *angelX = new QLabel(centralWidget);

    QLabel *textY = new QLabel("Изменение угла по оси Y",centralWidget);
    QLabel *angelY = new QLabel(centralWidget);

    QLabel *textZ = new QLabel("Изменение угла по оси Z",centralWidget);
    QLabel *angelZ = new QLabel(centralWidget);

    QStackedWidget *image = new QStackedWidget(centralWidget);

    layout->addWidget(textX, 0,0);
    layout->addWidget(angelX, 1,0);
    layout->addWidget(textY, 2,0);
    layout->addWidget(angelY, 3,0);
    layout->addWidget(textZ, 4,0);
    layout->addWidget(angelZ, 5,0);
    layout->addWidget(image,0,1,2,5);

}

MainWindow::~MainWindow()
{
}
