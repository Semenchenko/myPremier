
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "portsettings.h"
#include "inputset.h"

#include <QMessageBox>
#include <QtSerialPort/QSerialPort>
//#include <QDate>
//#include <QThread>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("TermIn");

    serial = new QSerialPort(this);

    settings = new PortSettings;

    inpdelset = new InputSet;

    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionQuit->setEnabled(true);
    ui->actionCOM_Port->setEnabled(true);

    ui->send03Button->setEnabled(false);
    ui->synchButton->setEnabled(false);
    ui->inpSetButton->setEnabled(false);
    ui->pollingBox->setEnabled(false);

    initAction_COM_Port_Connections();
    init_Functions();

    timerPC = new QTimer(this);
    timerAV = new QTimer(this);
    timerRX = new QTimer(this);
    timerTX = new QTimer(this);
    timerTic = new QTimer(this);

    connect(timerPC, SIGNAL(timeout()), this, SLOT(out_TimerPC()));
    timerPC->stop();

    connect(timerAV, SIGNAL(timeout()), this, SLOT(out_TimerAV()));
    timerAV->stop();

    connect(timerRX, SIGNAL(timeout()), this, SLOT(out_TimerRX()));
    timerRX->stop();

    connect(timerTX, SIGNAL(timeout()), this, SLOT(out_TimerTX()));
    timerTX->stop();

    connect(timerTic, SIGNAL(timeout()), this, SLOT(out_TimerTic()));
    timerTic->start(1000);

    connect(serial, SIGNAL(error(QSerialPort::SerialPortError)), this,
            SLOT(handleError(QSerialPort::SerialPortError)));

    connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));

    connect(ui->okButton, SIGNAL(pressed()),
            this, SLOT(on_okButton_pressed()), Qt::UniqueConnection);

    connect(ui->okButton, SIGNAL(released()),
            this, SLOT(on_okButton_released()), Qt::UniqueConnection);

    connect(ui->addressBox, SIGNAL(valueChanged(int)),
            this, SLOT(on_addressBox_editingFinished()));

    connect(ui->pollingBox, SIGNAL(clicked()), this,
            SLOT(on_pollingBox_clicked()));

    connect(ui->send03Button, SIGNAL(clicked()), this,
            SLOT(on_send03Button_clicked()), Qt::UniqueConnection);

    connect(ui->synchButton, SIGNAL(clicked()), this,
            SLOT(on_synchButton_clicked()), Qt::UniqueConnection);

    connect(ui->inpSetButton, SIGNAL(toggled(bool)), this,
            SLOT(on_inpSetButton_clicked()), Qt::UniqueConnection);

//    QByteArray cmnd("+A?");

//    writeData(cmnd);

    ui->addressBox->setRange(1, 255);
    ui->addressBox->setValue(1);

}


MainWindow::~MainWindow()
{
    delete settings;
    delete ui;
}


void MainWindow::openSerialPort()
{
    PortSettings::Settings p = settings->settings();
    serial->setPortName(p.name);
    serial->setBaudRate(p.baudRate);
    serial->setDataBits(p.dataBits);
    serial->setParity(p.parity);
    serial->setStopBits(p.stopBits);
    serial->setFlowControl(p.flowControl);
    if (serial->open(QIODevice::ReadWrite)) {
            ui->actionConnect->setEnabled(false);
            ui->actionDisconnect->setEnabled(true);
            ui->actionCOM_Port->setEnabled(false);
            ui->statusBar->showMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
                                       .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                                       .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));
            QPixmap mypix (":/images/LedRed2.png");
            ui->readyLabel->setPixmap(mypix);
    } else {
        QMessageBox::critical(this, tr("Error"), serial->errorString());
        ui->statusBar->showMessage(tr("Open error"));
    }

    receiveDelay = false;

    ui->send03Button->setEnabled(true);
    ui->synchButton->setEnabled(true);
    ui->inpSetButton->setEnabled(true);
    ui->pollingBox->setEnabled(true);
}


void MainWindow::closeSerialPort()
{
    serial->close();
//    console->setEnabled(false);
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionCOM_Port->setEnabled(true);
    ui->statusBar->showMessage(tr("Disconnected"));

    QPixmap mypix (":/images/LedGrey2.png");
    ui->readyLabel->setPixmap(mypix);

    ui->send03Button->setEnabled(false);
    ui->synchButton->setEnabled(false);
    ui->inpSetButton->setEnabled(false);

    ui->pollingBox->setChecked(false);
    ui->pollingBox->setEnabled(false);
    // останавливаем таймер автоматического режима
    timerAV->stop();
    cmndAvto = false;

}


void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), serial->errorString());
        closeSerialPort();
    }
}


void MainWindow::writeData(const QByteArray &data)
{
    if(serial->isOpen()){

        QPixmap mypix (":/images/LedRed2.png");
        ui->txLabel->setPixmap(mypix);
        timerTX->start(250);

        serial->write(data);
        while(serial->waitForBytesWritten(100)) //ожидаем завершения записи в порт
        {
            if(serial->isDataTerminalReady())
                return;//Все в порядке
        }
    }
}


void MainWindow::readData()
{
    if(serial->isOpen()){

        QByteArray mesg = serial->readAll();

        receiveDelay = true;    // устанавливыем флаг задержки приёма

        rcvBuf += mesg;

        QPixmap mypix (":/images/LedRed2.png");
        ui->rxLabel->setPixmap(mypix);

        PortSettings::Settings pstng = settings->settings();
        timerRX->start(pstng.timeDelay);
    }
}


// функция обработки принятых данных
//
void MainWindow::processingRcvData()
{
    qint16 num = rcvBuf.size();


    if(!CalcCRC16(rcvBuf) || (num < 7))
    {
        ui->reply1Label->setText("Rsv Error");
        settingInputLeds(0);        // сброс входов
        return;
    }

    QString query;
    quint8 res;

    for (qint16 i=0; i<num; i++)
    {
        res = (quint8)rcvBuf[i];
        if(res < 0x10)
            query += '0';
        query += QString::number(res, 16).toUpper() + ' ';
    }

    res = (uchar)rcvBuf[2];

    if (res == 2)
    {
        if(cmndAvto)
            ui->reply1Label->clear();
        else
            ui->reply1Label->setText(query);

        res = (quint8)rcvBuf[4];
        settingInputLeds(res);     // индикация входов
    }
    else
    {
        ui->reply1Label->clear();

        // формирование и вывод записей событий
        if(!outputEventRecords())
            ui->reply1Label->setText("Rsv Error");
        // если принят пакет с записями, то ещё раз опрашиваем
        // контроллер для получения текущих состояний входов
        else
        {
            if(!cmndAvto)
            {
                PortSettings::Settings tdl = settings->settings();
                timerAV->start((tdl.timeDelay)*3);
            }
        }
    }

    timerRX->start(250);

    // не выводим принятую посылку, если режим автоопроса
    res = (uchar)rcvBuf[2];
    if((res==2)&&cmndAvto)
        return;

    query += '\n';
//        ui->text1Edit->setPlainText(query);
    ui->text1Edit->textCursor().movePosition(QTextCursor::Start);
    ui->text1Edit->insertPlainText("R: ");
    ui->text1Edit->insertPlainText(query);


}


void MainWindow::initAction_COM_Port_Connections()
{
    connect(ui->actionConnect, SIGNAL(triggered()), this, SLOT(openSerialPort()));
    connect(ui->actionDisconnect, SIGNAL(triggered()), this, SLOT(closeSerialPort()));
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionCOM_Port, SIGNAL(triggered()), settings, SLOT(exec()));
    connect(ui->actionInput_Setup, SIGNAL(triggered()), inpdelset, SLOT(exec()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(on_actionAbout()));
    connect(ui->actionAbout_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}


//*************************************************************************
//*** Функция по редактированию addressBox
//
void MainWindow::on_addressBox_editingFinished()
{
    quint8 addr = ui->addressBox->value();

    if(addr == 31)
    {
        ui->addressLabel->setText("Not Supported");
        return;
    }

    ui->addressLabel->setText("Address");

    QString query;
    quint8 mch;

    if(addr < 0x10)
    {
        query = '0';
    }

    // вычислим CRC
//    fnc_3[0] = QByteArray::number(addr, 16);
    fnc_3[0] = addr;
    CalcCRC16(fnc_3);

    // показываем команду
    query += QString::number(addr, 16).toUpper() + " 03 00 00 00 01 "; // hex

    mch = fnc_3[6];
    if (mch < 0x10)
        query += '0';

    query += QString::number(mch, 16).toUpper() + ' ';

    mch = fnc_3[7];
    if (mch < 0x10)
        query += '0';

    query += QString::number(mch, 16).toUpper();

    ui->query1Label->setText(query);

    // проверка проверка
//    QPixmap mypix (":/images/LedRed2.png");
//    ui->led7Label->setPixmap(mypix);
//    timerRX->start(500);

}


//*************************************************************************
//*** Функция по изменению состояния pollingBox
//
void MainWindow::on_pollingBox_clicked()
{
    if(ui->pollingBox->isChecked())
    {
        cmndAvto = true;

        // блокируем кнопки при автоматическом режиме
        ui->synchButton->setEnabled(false);
        ui->send03Button->setEnabled(false);
        ui->inpSetButton->setEnabled(false);

        // запускаем таймер автоматического режима
        PortSettings::Settings tdl = settings->settings();
        timerAV->start((tdl.timeDelay)*4);
    }
    else
    {
        cmndAvto = false;

        // останавливаем таймер автоматического режима
        timerAV->stop();

        // разблокируем кнопки при активном режиме COM порта
        if(serial->isOpen()){
            ui->synchButton->setEnabled(true);
            ui->send03Button->setEnabled(true);
            ui->inpSetButton->setEnabled(true);
        }
    }
}


//*************************************************************************
// Инициализация, заполнение команд
//
void MainWindow::init_Functions()
{
/*    fnc_3.resize(8);
    fnc_3[0] = 0x01;
    fnc_3[1] = 0x03;
    fnc_3[2] = 0x00;
    fnc_3[3] = 0x00;
    fnc_3[4] = 0x00;
    fnc_3[5] = 0x01;
    fnc_3[6] = 0x00;
    fnc_3[7] = 0x00;
*/
    fnc_3 = QByteArray::fromRawData("\x01\x03\x00\x00\x00\x01\x00\x00", 8);

    fnc_6 = QByteArray::fromRawData("\x01\x06\x00\x00\x00\x01\x00\x00", 8);

    rcvBuf.clear();

    cmndAvto = false;
}

//*************************************************************************
//*** Функция генерации CRC16
//
// на входе:
//  cbuf   - буфер с командой, подготовленной для вывода в магистраль
//           или с принятым из магистрали сообщением;
//  num    - количество байтов исключая CRC16;
//
// на выходе:
//  сbuf   - буфер с командой для передачи в Modbus RTU,
//           снабжённой рассчитанным CRC16
//           или с сообщением;
//
// Контрольная сумма CRC в сообщении передается младшим байтом вперед,
// т. е., сначала передаётся младший байт CL, а затем старший байт CH.
//
// функция возвращает:
//  true   - если CRC16 во входном буфере соответствует рассчитанному;
//  false  - если CRC16 не равно рассчитанному функцией значению.

bool MainWindow::CalcCRC16(QByteArray &cbuf) {

    unsigned char CH, CL, th, tl;
    unsigned short int CRC = 0xFFFF;
    unsigned short int cb;
    uint n, res;
    int i;

    QByteArray cmnd = cbuf;
    int num = cmnd.size() - 2;
    if(num <= 2)
        return false;

    for (i=0; i<num; i++) {
        res = (uchar)cbuf[i];

        CRC = CRC ^ res;

        for (n=0; n<8; n++) {
            cb = CRC & 0x0001;
            CRC = CRC>>1;
            if (cb == true) CRC = CRC ^ POLY;
        }
    }

    cb = CRC & 0x00FF;
    CL = (unsigned char)cb;
    cb = CRC>>8;
    CH = (unsigned char)cb;

    tl = cbuf[num];
    cbuf[num] = CL;

    num++;
    th = cbuf[num];
    cbuf[num] = CH;

    if ((th == CH)&&(tl == CL)) return true;

    return false;
}


//*************************************************************************
//*** Функция управления светодиодными индикаторами состояния Входов
//
void MainWindow::settingInputLeds(quint8 rin)
{
    QPixmap onLed  (":/images/LedRed2.png");
    QPixmap offLed (":/images/LedGrey2.png");

//    quint8 res = (quint8)rcvBuf[4];

    if(rin & (1<<0))
        ui->led1Label->setPixmap(onLed);
    else
        ui->led1Label->setPixmap(offLed);

    if(rin & (1<<1))
        ui->led2Label->setPixmap(onLed);
    else
        ui->led2Label->setPixmap(offLed);

    if(rin & (1<<2))
        ui->led3Label->setPixmap(onLed);
    else
        ui->led3Label->setPixmap(offLed);

    if(rin & (1<<3))
        ui->led4Label->setPixmap(onLed);
    else
        ui->led4Label->setPixmap(offLed);

    if(rin & (1<<4))
        ui->led5Label->setPixmap(onLed);
    else
        ui->led5Label->setPixmap(offLed);

    if(rin & (1<<5))
        ui->led6Label->setPixmap(onLed);
    else
        ui->led6Label->setPixmap(offLed);

    if(rin & (1<<6))
        ui->led7Label->setPixmap(onLed);
    else
        ui->led7Label->setPixmap(offLed);

    if(rin & (1<<7))
        ui->led8Label->setPixmap(onLed);
    else
        ui->led8Label->setPixmap(offLed);
}


//*************************************************************************
//*** Функция вывода информации о сохранённых событиях по изменениям
//
//             111111111122
//   0123456789012345678901
//  "dd.MM.yyyy hh:mm:ss:ms InpN"
//
//             1
//   01234567890
//  "hh:mm:ss:ms InpN On/Off"
//
bool MainWindow::outputEventRecords()
{
    uchar res = 0;

    uint num = (uchar)rcvBuf[2];

    if(num<4 || num%4>0)
    {
        settingInputLeds(0);
        return false;
    }

    // получаем текущие настройки входов
    InputSet::InpDelaySet tuning = inpdelset->inptuning();

    // получаем текущую дату
//    QString str = QDateTime::currentDateTime().toString("dd.MM.yyyy hh");
//    str += ':';

    // получаем текущее время
    QTime mtime = QTime::currentTime();
    int chour = mtime.hour();
    int prehour;
    int cmint = mtime.minute();
//    int csecn = mtime.second();

    QString str;

    if(chour<10)
        str = '0';
    str += QString::number(chour, 10) + ':';

    uint trc = num/4;
    uint i;
    uint n = 3;
    uchar crnt;
    uchar state, pscn;
    int tmp;

    ui->changeVewEdit->textCursor().movePosition(QTextCursor::Start);

    for (i=0; i < trc; i++, n++)
    {
        crnt = rcvBuf[n];       // определяем номер и состояние входа
        pscn = crnt&0x0F;
        if(crnt&0x10)
        {
            res |= (1<<(crnt&0x0F));
            state = 1;
        }
        else
        {
            res &= ~(1<<(crnt&0x0F));
            state = 0;
        }

        if (n==3)
        {
            n++;                            // определяем минуту фиксации записи
            crnt = conv_fromBDec(rcvBuf[n]);
            if(crnt<10)
                str += '0';
            str += QString::number(crnt, 10) + ':';

            tmp = crnt;                     // запоминаем минуту

            n++;                            // определяем секунду фиксации записи
            crnt = conv_fromBDec(rcvBuf[n]);
            if(crnt<10)
                str += '0';
            str += QString::number(crnt, 10) + ':';

            n++;                            // определяем сотые секунды фиксации записи
            crnt = conv_fromBDec(rcvBuf[n]);
            if(crnt<10)
                str += '0';
            str += QString::number(crnt, 10);
            str += " Inp";
        }
        else
        {
            n++;                            // определяем минуту фиксации записи
            crnt = conv_fromBDec(rcvBuf[n]);
            if(crnt<10)
            {
                str.replace(3, 1, '0');
                str.replace(4, 1, QString::number(crnt, 10));
            }
            else
                str.replace(3, 2, QString::number(crnt, 10));

            tmp = crnt;                     // запоминаем минуту

            n++;                            // определяем секунду фиксации записи
            crnt = conv_fromBDec(rcvBuf[n]);
            if(crnt<10)
            {
                str.replace(6, 1, '0');
                str.replace(7, 1, QString::number(crnt, 10));
            }
            else
                str.replace(6, 2, QString::number(crnt, 10));

            n++;                            // определяем сотые секунды фиксации записи
            crnt = conv_fromBDec(rcvBuf[n]);
            if(crnt<10)
            {
                str.replace(9, 1, '0');
                str.replace(10, 1, QString::number(crnt, 10));
            }
            else
                str.replace(9, 2, QString::number(crnt, 10));
        }

        // проверяем разрешение вывода строки с записью информацией
        // о изменении на входе
        switch (pscn) {
        case 0:
            if(tuning.Inp1==0)
                continue;
            break;
        case 1:
            if(tuning.Inp2==0)
                continue;
            break;
        case 2:
            if(tuning.Inp3==0)
                continue;
            break;
        case 3:
            if(tuning.Inp4==0)
                continue;
            break;
        case 4:
            if(tuning.Inp5==0)
                continue;
            break;
        case 5:
            if(tuning.Inp6==0)
                continue;
            break;
        case 6:
            if(tuning.Inp7==0)
                continue;
            break;
        case 7:
            if(tuning.Inp8==0)
                continue;
            break;
        default:
            continue;
        }

        // если минута фиксации записи больше текущей минуты,
        // то корректируем час фиксации записи
        if(tmp > cmint)
        {
            prehour = chour - 1;
            if(prehour<10)
            {
                str.replace(0, 1, '0');
                str.replace(1, 1, QString::number(prehour, 10));
            }
            else
                str.replace(0, 2, QString::number(prehour, 10));
        }

        // выводим строку
        ui->changeVewEdit->insertPlainText(str);
        ui->changeVewEdit->insertPlainText(QString::number(pscn, 10));
        if(state)
            ui->changeVewEdit->insertPlainText(" On\n");
        else
            ui->changeVewEdit->insertPlainText(" Off\n");
    }

    settingInputLeds(res);

    return true;
}


//*************************************************************************
//*** Функции по нажатию кнопок
//
void MainWindow::on_okButton_pressed()
{
    ui->text1Edit->clear();     //

    settingInputLeds(0);        // сброс индикации входов

//    QPixmap mypix (":/images/LedRed2.png");
//    ui->led1Label->setPixmap(mypix);

//    cmnd += '\n';

//    QByteArray cmnd2;
//    cmnd2.insert(0,cmnd);               // преобразование из QString в QByteArray

//    writeData(cmnd2);

//    QString cmnd3 = QString(cmnd2);     // преобразование из QByteArray в QString

//    ui->text1Label->setText(cmnd);
}

void MainWindow::on_okButton_released()
{
    ui->changeVewEdit->clear();

//    QPixmap mypix (":/images/LedGrey2.png");
//    ui->led1Label->setPixmap(mypix);

}


//*************************************************************************
//*** Функция по нажатию кнопки отправки запроса
//
void MainWindow::on_send03Button_clicked()
{
    if(!receiveDelay)
        writeData(fnc_3);

    // не выводим команду на экран, если режим автоопроса
    if(cmndAvto)
        return;

    // преобразование из QByteArray в QString
    uchar tmp;
    int num = fnc_3.size();
    QString cmnd = "S: ";
    for (int i = 0; i < num; i++)
    {
        tmp = fnc_3[i];
        if(tmp<0x10)
            cmnd += '0';
        cmnd += QString::number(tmp, 16).toUpper() + ' ';
    }

    cmnd += '\n';

    // выводим отправляемый запрос на экран
    ui->text1Edit->textCursor().movePosition(QTextCursor::Start);
    ui->text1Edit->insertPlainText(cmnd);
}


//*************************************************************************
//*** Функция по нажатию кнопки синхронизации времени
//
void MainWindow::on_synchButton_clicked()
{
    uchar tmp;

    // получаем текущее время для синхронизации
    QTime mtime = QTime::currentTime();
    int cmint = mtime.minute();
    int csecn = mtime.second();
    int cmsec = mtime.msec();

    // инициализируем команду
    QByteArray synch = QByteArray::fromRawData("\x1F\x06\x03\x00\x00\x00\x00\x00", 8);

    tmp = conv_toBDec(cmint);
    if ((tmp&0x0F)>0x09 || (tmp&0xF0)>0x50)
        tmp = 0;
    synch[3] = tmp;

    tmp = conv_toBDec(csecn);
    if ((tmp&0x0F)>0x09 || (tmp&0xF0)>0x50)
        tmp = 0;
    synch[4] = tmp;

    tmp = conv_toBDec(cmsec);
    if ((tmp&0x0F)>0x09 || (tmp&0xF0)>0x90)
        tmp = 0;
    synch[5] = tmp;

    // посылаем команду в контроллеры
    CalcCRC16(synch);
    if(!receiveDelay)
        writeData(synch);

    // преобразование из QByteArray в QString
    int num = synch.size();
    QString cmnd = "S: ";
    for (int i = 0; i < num; i++)
    {
        tmp = synch[i];
        if(tmp<0x10)
            cmnd += '0';
        cmnd += QString::number(tmp, 16).toUpper() + ' ';
    }

    cmnd += '\n';

    // выводим отправляемый запрос на экран
    ui->text1Edit->textCursor().movePosition(QTextCursor::Start);
    ui->text1Edit->insertPlainText(cmnd);
}


//*************************************************************************
//*** Функция по нажатию кнопки установки параметров дискретных входов
//
void MainWindow::on_inpSetButton_clicked()
{

    // блокируем кнопки до завершения всех посылок
    ui->synchButton->setEnabled(false);
    ui->send03Button->setEnabled(false);
    ui->inpSetButton->setEnabled(false);
    ui->okButton->setEnabled(false);
    ui->pollingBox->setEnabled(false);

    // предустановка счётчиков посылок
    countPC = 0;
    countFN = 1;

    // определяем время на одну посылку
    PortSettings::Settings tdl = settings->settings();
//    QThread::msleep(tdl.timeDelay);
    timerPC->start((tdl.timeDelay)*3);
}


//*************************************************************************
//*** Функции по срабатыванию таймеров
//
void MainWindow::out_TimerPC()
{
    if(countPC == 8)
    {
        timerPC->stop();

        // разблокируем кнопки после завершения всех посылок
        ui->synchButton->setEnabled(true);
        ui->send03Button->setEnabled(true);
        ui->inpSetButton->setEnabled(true);
        ui->okButton->setEnabled(true);
        ui->pollingBox->setEnabled(true);

        return;
    }

    // формируем посылку
    form_Function06(countFN, countPC);

    countFN++;
    if(countFN>3)
    {
        countFN = 1;
        countPC++;
    }
}

void MainWindow::out_TimerAV()
{
    if(!cmndAvto)
        timerAV->stop();
    on_send03Button_clicked();
}

void MainWindow::out_TimerRX()
{
    timerRX->stop();

    // если был активный режим приёма
    if(receiveDelay)
    {
        receiveDelay = false;   // сбрасываем флаг задержки
        processingRcvData();    // обрабатываем принятое сообщение
        rcvBuf.clear();
    }
    else
    {
        // отключаем индикатор приёма
        QPixmap mypix (":/images/LedGrey2.png");
        ui->rxLabel->setPixmap(mypix);
    }

    // проверка
//    QPixmap my2pix (":/images/LedGrey2.png");
//    ui->led7Label->setPixmap(my2pix);

}

void MainWindow::out_TimerTX()
{
    timerTX->stop();

    QPixmap mypix (":/images/LedGrey2.png");
    ui->txLabel->setPixmap(mypix);
}


void MainWindow::out_TimerTic()
{
//    QDate mdate = QDate::currentDate();
//    int dow = mdate.dayOfWeek();
//    QString str = QString::number(dow, 10) + ' ';
//    str += QDateTime::currentDateTime().toString("ddd ");

    QString str = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss");


//    ui->dateTimeLabel->setText(QDateTime::currentDateTime().
//                               toString("dd.MM.yyyy ddd hh:mm:ss"));

//    QTime mtime = QTime::currentTime();
//    int chour = mtime.hour();
//    int cmint = mtime.minute();
//    int csecn = mtime.second();
//    int cmsec = mtime.msec();

//    str.clear();
//    str = QString::number(chour, 10) + " : " + QString::number(cmint, 10) + " : "
//            + QString::number(csecn, 10) + " : " + QString::number(cmsec, 10);


    ui->dateTimeLabel->setText(str);
}


//*************************************************************************
//*** Функция преобразования числа в двоично-десятичную форму
//
uchar MainWindow::conv_toBDec(int rfc)
{
    uchar val = 0;
    bool ok;

    if(rfc < 10)
    {
        val = (uchar)rfc;
        return val;
    }

    QString mstr = QString::number(rfc, 10);
//    int nmb = mstr.size();

    QString ch0 = mstr.mid(0,1);
    QString ch1 = mstr.mid(1,1);

    val = ((ch0.toUInt(&ok, 10)<<4) & 0xF0) | (ch1.toUInt(&ok, 10) & 0x0F);

    return val;
}


//*************************************************************************
//*** Функция преобразования из двоично-десятичной формы в число
//
uchar MainWindow::conv_fromBDec(uchar rtm)
{
    uchar res = (uchar)(((rtm>>4)&0x0F)*10 + (rtm&0x0F));
    return res;
}


//*************************************************************************
//*** Функция вывода окна со справочной информацией о программе
//
void MainWindow::on_actionAbout()
{
    QMessageBox::about(this, tr("About TermIn"),
                       tr("The program <b>TermIn</b> demonstrates how to <br />"
                          "use the controller module DIC08 with a MODBUS RTU <br />"
                          "<br />"
                          "TermIn comes AS IS with ABSOLUTELY NO WARRANTY.  <br />"
                          "This is free software and you are welcome to redistribute it <br />"
                          "under certain conditions, see  http://www.gnu.org. <br />"
                          "<br />"
                          "TermIn: &copy; 2015 Company Name"));

}


//*************************************************************************
//*** Функция формирования команды записи
//
void MainWindow::form_Function06(uchar cmnd, uchar inp)
{
    // формируем команду
    quint16 val;
    uchar input = inp;
    bool setn = false;

    InputSet::InpDelaySet tuning = inpdelset->inptuning();

    switch (cmnd) {
    case 0x01:
        val = tuning.spanTR[inp];
        break;
    case 0x02:
        val = tuning.spanTF[inp];
        break;
    case 0x03:
        val = tuning.spanTP[inp];
        break;
    default:
        return;
    }

    switch (inp) {
    case 0:
        setn = tuning.Inp1;
        break;
    case 1:
        setn = tuning.Inp2;
        break;
    case 2:
        setn = tuning.Inp3;
        break;
    case 3:
        setn = tuning.Inp4;
        break;
    case 4:
        setn = tuning.Inp5;
        break;
    case 5:
        setn = tuning.Inp6;
        break;
    case 6:
        setn = tuning.Inp7;
        break;
    case 7:
        setn = tuning.Inp8;
        break;
    default:
        return;
    }

    if(setn)
        input |= 0x08;

    fnc_6[0] = ui->addressBox->value();

    fnc_6[1] = 0x06;

    fnc_6[2] = cmnd;

    fnc_6[3] = input;

    fnc_6[4] = (uchar)(val/256);

    fnc_6[5] = (uchar)(val%256);

    CalcCRC16(fnc_6);

    // выводим команду на экран
    QString query = "S: ";
    for (qint8 i=0; i<8; i++)
    {
        val = (quint8)fnc_6[i];
        if(val < 0x10)
            query += '0';
        query += QString::number(val, 16).toUpper() + ' ';
    }
    query += '\n';

    ui->text1Edit->textCursor().movePosition(QTextCursor::Start);
    ui->text1Edit->insertPlainText(query);

    // выводим команду в магистраль
    if(!receiveDelay)
        writeData(fnc_6);
}

