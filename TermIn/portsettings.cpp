#include "portsettings.h"
#include "ui_portsettings.h"

#include <QtSerialPort/QSerialPortInfo>
#include <QLineEdit>

QT_USE_NAMESPACE

PortSettings::PortSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PortSettings)
{
    ui->setupUi(this);
    this->setWindowTitle("COM Port Setup");

    ui->baudRateBox->setInsertPolicy(QComboBox::NoInsert);

    connect(ui->applyButton, SIGNAL(clicked()),
            this, SLOT(apply()));

    connect(ui->cancelButton, SIGNAL(clicked()),
            this, SLOT(cancel()));

    connect(ui->serialPortBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(showPortNumberInfo(int)));

    fillPortsParameters();
    fillPortsInfo();

    updateSettings();
}


PortSettings::~PortSettings()
{
    delete ui;
}


PortSettings::Settings PortSettings::settings() const
{
    return currentSettings;
}


void PortSettings::showPortNumberInfo(int idx)
{
    if (idx != -1) {
        QStringList list = ui->serialPortBox->itemData(idx).toStringList();
        ui->descriptionLabel->setText(tr("Description: %1").arg(list.at(1)));
        ui->manufacturerLabel->setText(tr("Manufacturer: %1").arg(list.at(2)));
        ui->serialNumberLabel->setText(tr("Serial number: %1").arg(list.at(3)));
        ui->locationLabel->setText(tr("Location: %1").arg(list.at(4)));
        ui->vendorLabel->setText(tr("Vendor Identifier: %1").arg(list.at(5)));
        ui->productLabel->setText(tr("Product Identifier: %1").arg(list.at(6)));
        ui->infotmationLabel->setText(tr("Information: %1").arg(list.at(7)));
    }
}


void PortSettings::apply()
{
    updateSettings();
    hide();
}


void PortSettings::cancel()
{
    ui->serialPortBox->setCurrentText(currentSettings.name);
    ui->baudRateBox->setCurrentText(currentSettings.stringBaudRate);
    ui->dataBitBox->setCurrentText(currentSettings.stringDataBits);
    ui->parityBitBox->setCurrentText(currentSettings.stringParity);
    ui->stopBitBox->setCurrentText(currentSettings.stringStopBits);
    ui->flowControlBox->setCurrentText(currentSettings.stringFlowControl);
    ui->transmitDelayBox->setValue(currentSettings.timeDelay);

    hide();
}

void PortSettings::fillPortsParameters()
{
    ui->baudRateBox->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    ui->baudRateBox->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    ui->baudRateBox->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    ui->baudRateBox->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
    ui->baudRateBox->addItem(QStringLiteral("230400"),230400);
    ui->baudRateBox->addItem(QStringLiteral("460800"),460800);
    ui->baudRateBox->addItem(QStringLiteral("921600"),921600);

    ui->dataBitBox->addItem(QStringLiteral("5"), QSerialPort::Data5);
    ui->dataBitBox->addItem(QStringLiteral("6"), QSerialPort::Data6);
    ui->dataBitBox->addItem(QStringLiteral("7"), QSerialPort::Data7);
    ui->dataBitBox->addItem(QStringLiteral("8"), QSerialPort::Data8);
    ui->dataBitBox->setCurrentIndex(3);

    ui->parityBitBox->addItem(QStringLiteral("None"), QSerialPort::NoParity);
    ui->parityBitBox->addItem(QStringLiteral("Even"), QSerialPort::EvenParity);
    ui->parityBitBox->addItem(QStringLiteral("Odd"), QSerialPort::OddParity);
    ui->parityBitBox->addItem(QStringLiteral("Mark"), QSerialPort::MarkParity);
    ui->parityBitBox->addItem(QStringLiteral("Space"), QSerialPort::SpaceParity);

    ui->stopBitBox->addItem(QStringLiteral("1"), QSerialPort::OneStop);
#ifdef Q_OS_WIN
    ui->stopBitBox->addItem(QStringLiteral("1.5"), QSerialPort::OneAndHalfStop);
#endif
    ui->stopBitBox->addItem(QStringLiteral("2"), QSerialPort::TwoStop);

    ui->flowControlBox->addItem(QStringLiteral("None"), QSerialPort::NoFlowControl);
    ui->flowControlBox->addItem(QStringLiteral("RTS/CTS"), QSerialPort::HardwareControl);
    ui->flowControlBox->addItem(QStringLiteral("XON/XOFF"), QSerialPort::SoftwareControl);

    ui->transmitDelayBox->setRange(0, 10000);
    ui->transmitDelayBox->setSuffix(" ms");
    ui->transmitDelayBox->setValue(50);

    ui->dataBitBox->setEnabled(false);
    ui->parityBitBox->setEnabled(false);
    ui->stopBitBox->setEnabled(false);
    ui->flowControlBox->setEnabled(false);

//    ui->transmitDelayBox->setEnabled(false);
}


void PortSettings::fillPortsInfo()
{
    ui->serialPortBox->clear();
    static const QString blankString = QObject::tr("N/A");
    static const QString portBusy = QObject::tr("Busy");
    static const QString portFree = QObject::tr("Free");
    QString description;
    QString manufacturer;
    QString serialNumber;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QStringList list;
        description = info.description();
        manufacturer = info.manufacturer();
        serialNumber = info.serialNumber();
        list << info.portName()
             << (!description.isEmpty() ? description : blankString)
             << (!manufacturer.isEmpty() ? manufacturer : blankString)
             << (!serialNumber.isEmpty() ? serialNumber : blankString)
             << info.systemLocation()
             << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : blankString)
             << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : blankString)
             << (info.isBusy() ? portBusy : portFree);

        ui->serialPortBox->addItem(list.first(), list);
    }
}


void PortSettings::updateSettings()
{
    currentSettings.name = ui->serialPortBox->currentText();

    if (ui->baudRateBox->currentIndex() == 4) {
        currentSettings.baudRate = ui->baudRateBox->currentText().toInt();
    } else {
        currentSettings.baudRate = static_cast<QSerialPort::BaudRate>(
                    ui->baudRateBox->itemData(ui->baudRateBox->currentIndex()).toInt());
    }
    currentSettings.stringBaudRate = QString::number(currentSettings.baudRate);

    currentSettings.dataBits = static_cast<QSerialPort::DataBits>(
                ui->dataBitBox->itemData(ui->dataBitBox->currentIndex()).toInt());
    currentSettings.stringDataBits = ui->dataBitBox->currentText();

    currentSettings.parity = static_cast<QSerialPort::Parity>(
                ui->parityBitBox->itemData(ui->parityBitBox->currentIndex()).toInt());
    currentSettings.stringParity = ui->parityBitBox->currentText();

    currentSettings.stopBits = static_cast<QSerialPort::StopBits>(
                ui->stopBitBox->itemData(ui->stopBitBox->currentIndex()).toInt());
    currentSettings.stringStopBits = ui->stopBitBox->currentText();

    currentSettings.flowControl = static_cast<QSerialPort::FlowControl>(
                ui->flowControlBox->itemData(ui->flowControlBox->currentIndex()).toInt());
    currentSettings.stringFlowControl = ui->flowControlBox->currentText();

    currentSettings.timeDelay = ui->transmitDelayBox->value();

    //currentSettings.localEchoEnabled = ui->localEchoCheckBox->isChecked();
}
