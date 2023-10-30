#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QMediaPlayer>
#include <QMediaPlaylist>

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
    void on_sound_valueChanged(int value);

    void on_open_file_triggered();

    void on_exit_triggered();

private:
    Ui::MainWindow *ui;
    QStandardItemModel  *user_data;// Модель данных от пользовотеля для отображения плейлиста
    QMediaPlayer    *audio_player;//Проигрыватель треков
    QMediaPlaylist  *playlist;//Плейлист проигрывателя
};
#endif // MAINWINDOW_H
