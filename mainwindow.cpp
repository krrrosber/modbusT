// в начале файла убедитесь, что подключены нужные заголовки:
#include <QTimer>
#include <QVector>
#include <QPixmap>
#include <QDebug>
#include <qwidget.h>
#include "mainwindow.h"
#include <qgridlayout.h>
#include <qlabel.h>
#include <qstackedwidget.h>
#include "config.h"
#include "QSplitter"

// ------------------ Конструктор ------------------
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    clientModbus(new ModbusConnect(this)),
    angelXLabel(nullptr),
    angelYLabel(nullptr),
    angelZLabel(nullptr),
    pollTimer(new QTimer(this)),
    image(nullptr)
{
    // стартовый  размер окна
    this->resize(1280, 720);

    // Splitter как центральный виджет
    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
    setCentralWidget(splitter);

    // --- Левая панель ---
    leftPanel = new QWidget;
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(20, 0, 20, 0);
    leftLayout->setSpacing(60);

    auto createCard = [](const QString &titleText, QLabel* &valueLabel) -> QFrame* {
        QFrame *card = new QFrame;
        card->setFrameShape(QFrame::StyledPanel);
        card->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

        QVBoxLayout *layout = new QVBoxLayout(card);
        layout->setContentsMargins(12, 8, 12, 8);
        layout->setSpacing(6);

        QLabel *title = new QLabel(titleText);
        title->setAlignment(Qt::AlignCenter);
        title->setStyleSheet("font-size: 26px; color: black;");

        valueLabel = new QLabel("0");
        valueLabel->setAlignment(Qt::AlignCenter);
        valueLabel->setStyleSheet("font-size: 30px; font-weight: bold;");

        layout->addWidget(title);
        layout->addWidget(valueLabel);

        return card;
    };

    // вертикальное центрирование между растяжками
    leftLayout->addStretch();

    // горизонтально — слева (AlignLeft), чтобы карточки не были по центру экрана
    leftLayout->addWidget(createCard("Изменение угла по оси X", angelXLabel), 0, Qt::AlignLeft);
    leftLayout->addWidget(createCard("Изменение угла по оси Y", angelYLabel), 0, Qt::AlignLeft);
    leftLayout->addWidget(createCard("Изменение угла по оси Z", angelZLabel), 0, Qt::AlignLeft);

    leftLayout->addStretch();

    leftPanel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    // --- Правая панель (картинка) ---
    image = new QStackedWidget;
    image->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Загружаем оригинальные картинки в кеш и создаём QLabel'ы
    origPixmaps.clear();

    for (const QString &path : std::as_const(Config::imagePaths)) {

        QPixmap orig(path);
        origPixmaps.append(orig); // кешируем

        QLabel *label = new QLabel;
        label->setAlignment(Qt::AlignCenter);
        label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        label->setMinimumSize(100, 100);

        QWidget *page = new QWidget;
        QHBoxLayout *hl = new QHBoxLayout(page);
        hl->setContentsMargins(0, 0, 20, 0);
        hl->setSpacing(0);
        hl->addWidget(label);

        //
        image->addWidget(page);
    }

    image->setCurrentIndex(0);

    // --- добавляем панели в splitter ---
    splitter->addWidget(leftPanel);
    splitter->addWidget(image);

    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 2);

    // делаем initial update после того, как лэйауты отработают (все размеры будут известны)
    QTimer::singleShot(0, this, &MainWindow::updateImages);

    // --- Подключения Modbus (как было у вас) ---
    connect(clientModbus, &ModbusConnect::angelXReady, this, [this](double d){
        if (angelXLabel) angelXLabel->setText(QString::number(d, 'f', 3));
    });
    connect(clientModbus, &ModbusConnect::angelYReady, this, [this](double d){
        if (angelYLabel) angelYLabel->setText(QString::number(d, 'f', 3));
    });
    connect(clientModbus, &ModbusConnect::angelZReady, this, [this](double d){
        if (angelZLabel) angelZLabel->setText(QString::number(d, 'f', 3));
    });
    connect(clientModbus, &ModbusConnect::imageChange, this, [this](int i){
        if (i >= 0 && i < image->count()) image->setCurrentIndex(i);
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

// ------------------ масштабирование картинок из кеша ------------------
void MainWindow::updateImages()
{
    for (int i = 0; i < image->count(); ++i) {

        QWidget *page = qobject_cast<QWidget*>(image->widget(i));
        if (!page) continue;

        // ищем QLabel на странице
        QLabel* lbl = page->findChild<QLabel*>();
        if (!lbl) continue;

        if (i >= 0 && i < origPixmaps.size() && !origPixmaps[i].isNull()) {
            QSize target = lbl->size();                  // размер доступной области для картинки (учитывает отступы)
            if (target.isEmpty()) target = image->size();
            if (target.isEmpty()) continue;

            QPixmap scaled = origPixmaps[i].scaled(target, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            lbl->setPixmap(scaled);
        } else {
            lbl->setPixmap(QPixmap());
        }
    }
}


// ------------------ resizeEvent ------------------
void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    // при изменении размера — обновляем масштабированные картинки
    updateImages();
}
MainWindow::~MainWindow()
{
}

