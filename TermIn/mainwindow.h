
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QtGlobal>

#include <QMainWindow>

#include <QMessageBox>
#include <QtSerialPort/QSerialPort>

QT_BEGIN_NAMESPACE

namespace Ui {
class MainWindow;
}

QT_END_NAMESPACE

class PortSettings;
class InputSet;

#define POLY        0xA001      // константа для проверки CRC16


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void openSerialPort();
    void closeSerialPort();
    void writeData(const QByteArray &data);
    void readData();
//    void about();

    void handleError(QSerialPort::SerialPortError error);

    void on_okButton_pressed();

    void on_okButton_released();

    void on_addressBox_editingFinished();

    void on_pollingBox_clicked();

    void on_send03Button_clicked();

    void on_synchButton_clicked();

    void on_inpSetButton_clicked();

    void on_actionAbout();

    void out_TimerPC();
    void out_TimerAV();
    void out_TimerRX();
    void out_TimerTX();
    void out_TimerTic();

private:
    void initAction_COM_Port_Connections();
    void init_Functions();
    void processingRcvData();
    void settingInputLeds(quint8 rin);
    bool outputEventRecords();
    bool CalcCRC16(QByteArray &cbuf);
    void form_Function06(uchar cmnd, uchar inp);
    uchar conv_toBDec(int rfc);
    uchar conv_fromBDec(uchar rtm);

private:
    Ui::MainWindow *ui;
    PortSettings *settings;
    QSerialPort *serial;
    InputSet *inpdelset;

    QByteArray fnc_3;
    QByteArray fnc_6;
    QByteArray rcvBuf;

    QTimer *timerPC;
    QTimer *timerAV;
    QTimer *timerRX;
    QTimer *timerTX;
    QTimer *timerTic;

    bool receiveDelay;
    bool cmndAvto;

    quint8 countPC;
    quint8 countFN;
};

#endif // MAINWINDOW_H
