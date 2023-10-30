#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDir>
#include <QToolButton>

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow){
    ui->setupUi(this);
    //настройка плейлиста
    //инициализация модели данных от пользователя
    user_data = new QStandardItemModel(this);

    //установка модели данных в таблицу
    ui->playlistView->setModel(user_data);

    //прописываем заголовки таблицы
    user_data->setHorizontalHeaderLabels(QStringList()  << tr("Плейлист") << tr("Файл"));

    //Скрываем колонку с каталогом к файлу
    ui->playlistView->hideColumn(1);

    // Включаем выделение строк
    ui->playlistView->setSelectionBehavior(QAbstractItemView::SelectRows);

    // Разрешаем выделять только одну строку
    ui->playlistView->setSelectionMode(QAbstractItemView::SingleSelection);

    // Отключаем редактирование названия музыки
    ui->playlistView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //подгоняем размер колонок в плейлисте
    ui->playlistView->horizontalHeader()->setStretchLastSection(true);

    // Инициализируем аудиоплеер
    audio_player = new QMediaPlayer(this);
    // Инициализируем плейлист
    playlist = new QMediaPlaylist(audio_player);
    // Устанавливаем плейлист в аудиоплеер
    audio_player->setPlaylist(playlist);
    // Устанавливаем циклический режим проигрывания плейлиста
    playlist->setPlaybackMode(QMediaPlaylist::Loop);

    //подключение кнопок управления от сигнала к слотам
    connect(ui->back, &QToolButton::clicked, playlist, &QMediaPlaylist::previous);
    connect(ui->stop, &QToolButton::clicked, audio_player, &QMediaPlayer::stop);
    connect(ui->play, &QToolButton::clicked, audio_player, &QMediaPlayer::play);
    connect(ui->pause, &QToolButton::clicked, audio_player, &QMediaPlayer::pause);
    connect(ui->forward, &QToolButton::clicked, playlist, &QMediaPlaylist::next);

    //определяем позицию песни в процентах
    connect(audio_player, &QMediaPlayer::positionChanged, [&](qint64 pos){
        ui->length_music->setValue(pos);
    });
    connect(audio_player, &QMediaPlayer::durationChanged, [&](qint64 dur){
        ui->length_music->setMaximum(dur);
    });

    //При двойном клике по треку устанавливаем трек
    connect(ui->playlistView, &QTableView::doubleClicked, [this](const QModelIndex &i){
        playlist->setCurrentIndex(i.row());
    });

    //установка названия текущего трека в специальной рамке
    connect(playlist, &QMediaPlaylist::currentIndexChanged, [this](int index){
        ui->currentTrack->setText(user_data->data(user_data->index(index, 0)).toString());
    });

}
//деструктор для очищения выделеной памяти
MainWindow::~MainWindow(){
    delete ui;
    delete user_data;
    delete playlist;
    delete audio_player;
}
void MainWindow::on_open_file_triggered(){
    QStringList file = QFileDialog::getOpenFileNames(this,
           tr("Открыть файл"), QString());
       //установка данных в плейлист через цыкл. Данный цыкл аналогичен цыклу for
       foreach (QString filePath, file) {
           QList<QStandardItem *> items;
           //передаем название файла
           items.append(new QStandardItem(QDir(filePath).dirName()));
           //каталог файла
           items.append(new QStandardItem(filePath));
           //плейлист модель
           user_data->appendRow(items);
           //устанавливаем медиа источник для воспроизведения
           playlist->addMedia(QUrl(filePath));
       }
}
void MainWindow::on_sound_valueChanged(int value){
    audio_player->setVolume(value);//громкость музыки
}
void MainWindow::on_exit_triggered(){
    QApplication::quit();//кнопка выхода
}

