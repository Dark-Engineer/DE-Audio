#include "mainwindow.h"

#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QFile file(":/style.css");
    file.open(QFile::ReadOnly);
    w.setStyleSheet(file.readAll());
    w.setWindowTitle("DE Audio");//название приложения
    w.setToolTip("DE Audio");//всплывающая подсказка
    w.show();// выводим виджет на экран
    w.resize(1000, 700);
    return a.exec();
}
