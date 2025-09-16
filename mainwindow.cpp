#include <qwidget.h>
#include "mainwindow.h"
#include <qgridlayout.h>
#include <qlabel.h>
#include <qstackedwidget.h>
#include "config.h"
#include "QSplitter"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    clientModbus(new ModbusConnect(this)),
    angelXLabel(nullptr),
    angelYLabel(nullptr),
    angelZLabel(nullptr),
    pollTimer(new QTimer(this))
{
    this->setFixedSize(1024,800); // стартовый размер окна

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // --- Splitter ---
    QSplitter *splitter = new QSplitter(Qt::Horizontal, centralWidget);

    // --- Левая панель ---
    leftPanel = new QWidget;
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(0, 150, 0, 50); // отступ сверху/снизу
    leftLayout->setSpacing(20);                    // промежуток между карточками

    // Функция создания карточки
    auto createCard = [](const QString &titleText, QLabel* &valueLabel) -> QFrame* {
        QFrame *card = new QFrame;
        QVBoxLayout *layout = new QVBoxLayout(card);

        QLabel *title = new QLabel(titleText);
        title->setAlignment(Qt::AlignCenter);
        title->setStyleSheet("font-size: 16px; color: black;");

        valueLabel = new QLabel("0");
        valueLabel->setAlignment(Qt::AlignCenter);
        valueLabel->setStyleSheet("font-size: 26px; font-weight: bold;");

        layout->addWidget(title);
        layout->addWidget(valueLabel);
        layout->addStretch(); // чтобы значение не растягивалось на всю высоту

        return card;
    };

    leftLayout->addWidget(createCard("Изменение угла по оси X", angelXLabel));
    leftLayout->addWidget(createCard("Изменение угла по оси Y", angelYLabel));
    leftLayout->addWidget(createCard("Изменение угла по оси Z", angelZLabel));
    leftLayout->addStretch(); // оставшееся пространство внизу

    leftPanel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    // --- Правая панель (картинка) ---
    image = new QStackedWidget;
    image->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    for (const QString &path : std::as_const(Config::imagePaths)) {
        QLabel *label = new QLabel;
        label->setAlignment(Qt::AlignCenter);
        label->setPixmap(QPixmap(path));
        image->addWidget(label);
    }
    image->setCurrentIndex(0);

    // --- Добавляем панели в splitter ---
    splitter->addWidget(leftPanel);
    splitter->addWidget(image);

    splitter->setStretchFactor(0, 1); // левая колонка ~1/3
    splitter->setStretchFactor(1, 2); // правая колонка ~2/3

    setCentralWidget(splitter);

    // --- Подключение сигналов Modbus ---
    connect(clientModbus, &ModbusConnect::angelXReady, this, [this](double d){
        angelXLabel->setText(QString::number(d, 'f', 3));
    });
    connect(clientModbus, &ModbusConnect::angelYReady, this, [this](double d){
        angelYLabel->setText(QString::number(d, 'f', 3));
    });
    connect(clientModbus, &ModbusConnect::angelZReady, this, [this](double d){
        angelZLabel->setText(QString::number(d, 'f', 3));
    });
    connect(clientModbus, &ModbusConnect::imageChange, this, [this](int i){
        image->setCurrentIndex(i);
    });

    clientModbus->startConnect();

    pollTimer->setInterval(Config::pollInterval);
    connect(pollTimer, &QTimer::timeout, this, [this](){
        if(clientModbus->isConnected()){
            clientModbus->readRegister();
        } else {
            clientModbus->startConnect();
        }
    });
    pollTimer->start();
}

// --- Масштабирование картинки при изменении размера окна ---
void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    for(int i = 0; i < image->count(); ++i) {
        QLabel* lbl = qobject_cast<QLabel*>(image->widget(i));
        if(lbl && !lbl->pixmap().isNull()) {
            QPixmap pix(Config::imagePaths[i]);
            lbl->setPixmap(pix.scaled(
                image->size(),
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
                ));
        }
    }
}
MainWindow::~MainWindow()
{
}
