#ifndef PORTSETTINGS_H
#define PORTSETTINGS_H

#include <QDialog>
#include <QtSerialPort/QtSerialPort>

QT_USE_NAMESPACE

QT_BEGIN_NAMESPACE

namespace Ui {
class PortSettings;
}

QT_END_NAMESPACE


class PortSettings : public QDialog
{
    Q_OBJECT

public:
    struct Settings {
        QString name;
        qint32 baudRate;
        QString stringBaudRate;
        QSerialPort::DataBits dataBits;
        QString stringDataBits;
        QSerialPort::Parity parity;
        QString stringParity;
        QSerialPort::StopBits stopBits;
        QString stringStopBits;
        QSerialPort::FlowControl flowControl;
        QString stringFlowControl;
        quint16 timeDelay;
        bool localEchoEnabled;
    };

    explicit PortSettings(QWidget *parent = 0);
    ~PortSettings();

    Settings settings() const;


private slots:
    void showPortNumberInfo(int idx);
    void apply();
    void cancel();

private:
    void fillPortsParameters();
    void fillPortsInfo();
    void updateSettings();

private:
    Ui::PortSettings *ui;
    Settings currentSettings;
};

#endif // PORTSETTINGS_H
