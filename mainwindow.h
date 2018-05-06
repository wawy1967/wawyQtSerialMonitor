#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QScrollBar>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void updateBaudRate();
    void serialRead();

private:
    Ui::MainWindow *ui;
    QSerialPort *serial;
    QScrollBar *sb;
    QString serialPortName;
    bool arduino_is_available;
};

#endif // MAINWINDOW_H
