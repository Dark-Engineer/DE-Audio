#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDir>
#include <QToolButton>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QSettings>

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

    //min и max значения звука
    ui->sound->setMinimum(1);
    ui->sound->setMaximum(100);
    ui->sound->setValue(30);

    //подключение кнопок управления от сигнала к слотам
    connect(ui->back, &QToolButton::clicked, playlist, &QMediaPlaylist::previous);
    connect(ui->stop, &QToolButton::clicked, audio_player, &QMediaPlayer::stop);
    connect(ui->play, &QToolButton::clicked, audio_player, &QMediaPlayer::play);
    connect(ui->pause, &QToolButton::clicked, audio_player, &QMediaPlayer::pause);
    connect(ui->forward, &QToolButton::clicked, playlist, &QMediaPlaylist::next);

    //При двойном клике по треку устанавливаем трек
    connect(ui->playlistView, &QTableView::doubleClicked, [this](const QModelIndex &i){
        playlist->setCurrentIndex(i.row());
    });

    //установка названия текущего трека в специальной рамке
    connect(playlist, &QMediaPlaylist::currentIndexChanged, [this](int index){
        ui->currentTrack->setText(user_data->data(user_data->index(index, 0)).toString());
    });

    audio_player->setVolume(ui->sound->value());
    //настройка слотов для прогресс бара плеера
    connect(audio_player, &QMediaPlayer::durationChanged, this, &MainWindow::durationChanged);
    connect(audio_player, &QMediaPlayer::positionChanged, this, [this](qint64 progress) {
        if (!ui->lengthMusic->isSliderDown()) {  // Обновляем только если ползунок не двигается пользователем
            ui->lengthMusic->setValue(progress / 1000);
        }
        updateduration(progress / 1000);
    });

    // Загрузка последнего плейлиста при запуске
    QSettings settings;
    QString lastPlaylist = settings.value("lastPlaylist").toString();
    if (!lastPlaylist.isEmpty() && QFile::exists(lastPlaylist)) {
        loadPlaylistFromFile(lastPlaylist);
    }
}
//деструктор для очищения выделеной памяти
MainWindow::~MainWindow(){
    // Сохраняем последний плейлист перед выходом
    QSettings settings;
    settings.setValue("lastPlaylist", currentPlaylistFile);
    delete ui;
    delete user_data;
    delete playlist;
    delete audio_player;
}
//Обновление продолжительности трека
void MainWindow::updateduration(qint64 duration){
    QString timestr;
    if(duration || Mduration){
        QTime CurrentTime((duration / 3600) % 60, (duration / 60) % 60, duration % 60, (duration * 1000) % 1000);
        QTime totalTime((Mduration / 3600) % 60, (Mduration / 60) % 60, Mduration % 60, (Mduration * 1000) % 1000);
        QString format = "mm:ss";
        if(Mduration > 3600){
            format = "hh:mm:ss";
        }
        ui->timeStart->setText(CurrentTime.toString(format));
        ui->timeEnd->setText(totalTime.toString(format));
    }
}
// Сохранение плейлиста в файл
void MainWindow::savePlaylistToFile(const QString &filename){
    QJsonArray tracksArray;

    // Сохраняем все треки из модели
    for (int i = 0; i < user_data->rowCount(); ++i) {
        QJsonObject track;
        track["title"] = user_data->data(user_data->index(i, 0)).toString();
        track["path"] = user_data->data(user_data->index(i, 1)).toString();
        tracksArray.append(track);
    }

    QJsonObject playlistObject;
    playlistObject["version"] = "1.0";
    playlistObject["tracks"] = tracksArray;
    playlistObject["currentIndex"] = playlist->currentIndex();
    playlistObject["volume"] = audio_player->volume();
    playlistObject["muted"] = audio_player->isMuted();

    QFile file(filename);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(playlistObject).toJson());
        file.close();
        currentPlaylistFile = filename; // Сохраняем путь к текущему плейлисту
    }
}

// Загрузка плейлиста из файла
void MainWindow::loadPlaylistFromFile(const QString &filename){
    QFile file(filename);
        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "Не удалось открыть файл плейлиста:" << filename;
            return;
        }

        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);

        if (doc.isNull()) {
            qWarning() << "Неверный формат JSON в файле:" << filename;
            return;
        }

        QJsonObject playlistObject = doc.object();

        // Очищаем текущий плейлист
        user_data->clear();
        user_data->setHorizontalHeaderLabels(QStringList() << tr("Плейлист") << tr("Файл"));
        playlist->clear();

        // Загружаем треки
        QJsonArray tracksArray = playlistObject["tracks"].toArray();
        for (const QJsonValue& value : tracksArray) {
            QJsonObject track = value.toObject();
            QString path = track["path"].toString();

            // Проверяем существует ли файл
            if (QFile::exists(path)) {
                QList<QStandardItem*> items;
                items.append(new QStandardItem(track["title"].toString()));
                items.append(new QStandardItem(path));
                user_data->appendRow(items);
                playlist->addMedia(QUrl(path));
            } else {
                qWarning() << "Файл не найден:" << path;
            }
        }

        // Восстанавливаем состояние плеера
        if (playlistObject.contains("currentIndex")) {
            int index = playlistObject["currentIndex"].toInt();
            if (index >= 0 && index < playlist->mediaCount()) {
                playlist->setCurrentIndex(index);
            }
        }

        if (playlistObject.contains("volume")) {
            int volume = playlistObject["volume"].toInt();
            audio_player->setVolume(volume);
            ui->sound->setValue(volume);
        }

        if (playlistObject.contains("muted")) {
            bool muted = playlistObject["muted"].toBool();
            audio_player->setMuted(muted);
            ui->soundlessly->setIcon(style()->standardIcon(
                muted ? QStyle::SP_MediaVolumeMuted : QStyle::SP_MediaVolume));
            IS_Muted = muted;
        }

        file.close();
        currentPlaylistFile = filename; // Сохраняем путь к текущему плейлисту
}

//Определение продолжительности трека
void MainWindow::durationChanged(qint64 duration){
    Mduration = duration / 1000;
    ui->lengthMusic->setMaximum(Mduration);
}

void MainWindow::positionChanged(qint64 progress){
    if(!ui->lengthMusic->isSliderDown()){
        ui->lengthMusic->setValue(progress / 1000);
    }
    updateduration(progress / 1000);
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
void MainWindow::on_soundlessly_clicked(){
    if(IS_Muted == false){
        ui->soundlessly->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
        IS_Muted = true;
        audio_player->setMuted(true);
    }
    else{
        ui->soundlessly->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
        IS_Muted = false;
        audio_player->setMuted(false);
    }
}

void MainWindow::on_lengthMusic_valueChanged(int value){
    // Устанавливаем позицию только если ползунок отпущен
    if (ui->lengthMusic->isSliderDown()) {
        audio_player->setPosition(value * 1000);
    }
}

//сохранить плейлист
void MainWindow::on_savePlaylist_triggered(){
    QString filename = QFileDialog::getSaveFileName(this,
        tr("Сохранить плейлист"),
        currentPlaylistFile.isEmpty() ? QDir::homePath() + "/playlist.json" : currentPlaylistFile,
        tr("Плейлист (*.json);;Все файлы (*)"));

    if (!filename.isEmpty()) {
        savePlaylistToFile(filename);
    }
}

//импортировать плейлист
void MainWindow::on_loadPlaylist_triggered(){
    QString filename = QFileDialog::getOpenFileName(this,
        tr("Загрузить плейлист"),
        QDir::homePath(),
        tr("Плейлист (*.json);;Все файлы (*)"));

    if (!filename.isEmpty()) {
        loadPlaylistFromFile(filename);
    }
}

