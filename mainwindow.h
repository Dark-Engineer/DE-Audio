#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QtMultimedia>
#include <QtCore>
#include <QtWidgets>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QSettings>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void durationChanged(qint64 duration);//продолжительности позиции трека в прогресс баре
    void positionChanged(qint64 progress);//прогресса позиции трека в прогресс баре
    void on_sound_valueChanged(int value);
    void on_open_file_triggered();
    void on_exit_triggered();
    void on_soundlessly_clicked();
    void on_lengthMusic_valueChanged(int value);
    void on_savePlaylist_triggered();
    void on_loadPlaylist_triggered();

private:
    void updateduration(qint64 duration);//обновление продолжительности трека
    Ui::MainWindow *ui;
    QStandardItemModel  *user_data;// Модель данных от пользовотеля для отображения плейлиста
    QMediaPlayer    *audio_player;//Проигрыватель треков
    QMediaPlaylist  *playlist;//Плейлист проигрывателя
    bool IS_Muted = false;//Значение состояния без звука
    qint64 Mduration;//Продолжительность трека

    QString currentPlaylistFile;   // Путь к текущему плейлисту
    // Методы для работы с файлами плейлистов
    void savePlaylistToFile(const QString& filename);
    void loadPlaylistFromFile(const QString& filename);

};
#endif // MAINWINDOW_H
