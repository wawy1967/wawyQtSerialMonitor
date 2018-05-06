#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QScrollBar>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    sb = ui->plainTextEdit->verticalScrollBar();
    arduino_is_available = false;
    QMainWindow::removeToolBar(ui->mainToolBar);
    ui->statusBar->hide();
    QStringList baudRate = {"9600 baud","19200 baud",
                            "38400 baud","57600 baud",
                            "115200 baud"};
    QStringList endLine = {"No line ending","Newline",
                          "Carriage return","Both NL & CR"};
    ui->comboBoxBaudRate->addItems(baudRate);
    ui->comboBoxEndLine->addItems(endLine);
    static const quint16 uno_vendor_id = 9025;
    static const quint16 uno_product_id = 67;
    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
    {

        if (serialPortInfo.hasVendorIdentifier() &&
            serialPortInfo.hasProductIdentifier())
        {
            serialPortName = serialPortInfo.portName();
            if ((serialPortInfo.vendorIdentifier()==uno_vendor_id) &&
                 (serialPortInfo.productIdentifier()==uno_product_id))
            {
                arduino_is_available = true;
            }
        }
    }
    if (arduino_is_available)
    {
        serial = new QSerialPort(this);
        serial->setDataBits(QSerialPort::Data8);
        serial->setParity(QSerialPort::NoParity);
        serial->setStopBits(QSerialPort::OneStop);
        serial->setFlowControl(QSerialPort::NoFlowControl);
        serial->setPortName(serialPortName);
        if (!serial->open(QIODevice::ReadWrite))
           {
                QMessageBox::information(0,"Error","Cannot open port!");
                return;
           }
    }
    this->setWindowTitle(serialPortName + " (Arduino/Genuino Uno");
    connect(ui->comboBoxBaudRate,SIGNAL(currentIndexChanged(int)),
            this,SLOT(updateBaudRate()));
    connect(serial,SIGNAL(readyRead()),this,SLOT(serialRead()));
    connect(ui->lineEdit,SIGNAL(returnPressed()),
            ui->pushButton,SLOT(click()));
}
MainWindow::~MainWindow()
{
    serial->close();
    delete ui;
}
void MainWindow::updateBaudRate()
{
    int i = ui->comboBoxBaudRate->currentIndex();
    switch(i)
    {
        case 0:
            serial->setBaudRate(QSerialPort::Baud9600);
            break;
        case 1:
            serial->setBaudRate(QSerialPort::Baud19200);
            break;
        case 2:
            serial->setBaudRate(QSerialPort::Baud38400);
            break;
        case 3:
            serial->setBaudRate(QSerialPort::Baud57600);
            break;
        case 4:
            serial->setBaudRate(QSerialPort::Baud115200);
            break;
        default:
            serial->setBaudRate(QSerialPort::Baud9600);
            break;
    }
}
void MainWindow::on_pushButton_clicked()
{
    QString s = ui->lineEdit->text();
    ui->lineEdit->clear();
    QByteArray str = s.toUtf8();
    if (serial->isWritable())
        serial->write(str);
    else if (serial->open(QIODevice::ReadWrite))
        serial->write(str);
    else
       {
            QMessageBox::information(0,"Error","Cannot open port!");
            return;
       }
}

void MainWindow::serialRead()
{
    QByteArray ba = "";
    while (serial->bytesAvailable())
        ba = serial->readAll();
    ba.replace("\r","");
    ui->plainTextEdit->insertPlainText(ba);
    if (ui->checkBoxAutoScroll->isChecked())
        sb->setValue(sb->maximumHeight());
    else
    {
        int a = sb->value();
        sb->setValue(a);
    }
}

