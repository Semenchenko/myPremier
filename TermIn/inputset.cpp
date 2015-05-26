#include "inputset.h"
#include "ui_inputset.h"

InputSet::InputSet(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InputSet)
{
    ui->setupUi(this);
    this->setWindowTitle("Input Setup");

    connect(ui->okButton, SIGNAL(clicked()), this,
            SLOT(on_okButton_clicked()));

    connect(ui->cancelButton, SIGNAL(clicked()), this,
            SLOT(on_cancelButton_clicked()));

    connect(ui->checkIn1Box, SIGNAL(clicked()), this,
            SLOT(on_checkIn1Box_clicked()));

    updateTuning();

}

InputSet::~InputSet()
{
    delete ui;
}

InputSet::InpDelaySet InputSet::inptuning() const
{
    return InputsTuning;
}


//*************************************************************************
//*** Функции нажатия на кнопки
//
void InputSet::on_okButton_clicked()
{
    updateTuning();
    hide();
}

void InputSet::on_cancelButton_clicked()
{
    resetTuning();
    hide();
}


//*************************************************************************
//*** Функция обновления окна настроек
//
void InputSet::resetTuning()
{
    ui->spinTr1Box->setValue(InputsTuning.spanTR[0]);
    ui->spinTr2Box->setValue(InputsTuning.spanTR[1]);
    ui->spinTr3Box->setValue(InputsTuning.spanTR[2]);
    ui->spinTr4Box->setValue(InputsTuning.spanTR[3]);
    ui->spinTr5Box->setValue(InputsTuning.spanTR[4]);
    ui->spinTr6Box->setValue(InputsTuning.spanTR[5]);
    ui->spinTr7Box->setValue(InputsTuning.spanTR[6]);
    ui->spinTr8Box->setValue(InputsTuning.spanTR[7]);

    ui->spinTp1Box->setValue(InputsTuning.spanTP[0]);
    ui->spinTp2Box->setValue(InputsTuning.spanTP[1]);
    ui->spinTp3Box->setValue(InputsTuning.spanTP[2]);
    ui->spinTp4Box->setValue(InputsTuning.spanTP[3]);
    ui->spinTp5Box->setValue(InputsTuning.spanTP[4]);
    ui->spinTp6Box->setValue(InputsTuning.spanTP[5]);
    ui->spinTp7Box->setValue(InputsTuning.spanTP[6]);
    ui->spinTp8Box->setValue(InputsTuning.spanTP[7]);

    ui->spinTf1Box->setValue(InputsTuning.spanTF[0]);
    ui->spinTf2Box->setValue(InputsTuning.spanTF[1]);
    ui->spinTf3Box->setValue(InputsTuning.spanTF[2]);
    ui->spinTf4Box->setValue(InputsTuning.spanTF[3]);
    ui->spinTf5Box->setValue(InputsTuning.spanTF[4]);
    ui->spinTf6Box->setValue(InputsTuning.spanTF[5]);
    ui->spinTf7Box->setValue(InputsTuning.spanTF[6]);
    ui->spinTf8Box->setValue(InputsTuning.spanTF[7]);

    ui->checkIn1Box->setChecked(InputsTuning.Inp1);
    ui->checkIn2Box->setChecked(InputsTuning.Inp2);
    ui->checkIn3Box->setChecked(InputsTuning.Inp3);
    ui->checkIn4Box->setChecked(InputsTuning.Inp4);
    ui->checkIn5Box->setChecked(InputsTuning.Inp5);
    ui->checkIn6Box->setChecked(InputsTuning.Inp6);
    ui->checkIn7Box->setChecked(InputsTuning.Inp7);
    ui->checkIn8Box->setChecked(InputsTuning.Inp8);

    on_checkIn1Box_clicked();
    on_checkIn2Box_clicked();
    on_checkIn3Box_clicked();
    on_checkIn4Box_clicked();
    on_checkIn5Box_clicked();
    on_checkIn6Box_clicked();
    on_checkIn7Box_clicked();
    on_checkIn8Box_clicked();
}


//*************************************************************************
//*** Функция обновления параметров из окна настроек
//
void InputSet::updateTuning()
{
    InputsTuning.spanTR[0] = ui->spinTr1Box->value();
    InputsTuning.spanTR[1] = ui->spinTr2Box->value();
    InputsTuning.spanTR[2] = ui->spinTr3Box->value();
    InputsTuning.spanTR[3] = ui->spinTr4Box->value();
    InputsTuning.spanTR[4] = ui->spinTr5Box->value();
    InputsTuning.spanTR[5] = ui->spinTr6Box->value();
    InputsTuning.spanTR[6] = ui->spinTr7Box->value();
    InputsTuning.spanTR[7] = ui->spinTr8Box->value();

    InputsTuning.spanTP[0] = ui->spinTp1Box->value();
    InputsTuning.spanTP[1] = ui->spinTp2Box->value();
    InputsTuning.spanTP[2] = ui->spinTp3Box->value();
    InputsTuning.spanTP[3] = ui->spinTp4Box->value();
    InputsTuning.spanTP[4] = ui->spinTp5Box->value();
    InputsTuning.spanTP[5] = ui->spinTp6Box->value();
    InputsTuning.spanTP[6] = ui->spinTp7Box->value();
    InputsTuning.spanTP[7] = ui->spinTp8Box->value();

    InputsTuning.spanTF[0] = ui->spinTf1Box->value();
    InputsTuning.spanTF[1] = ui->spinTf2Box->value();
    InputsTuning.spanTF[2] = ui->spinTf3Box->value();
    InputsTuning.spanTF[3] = ui->spinTf4Box->value();
    InputsTuning.spanTF[4] = ui->spinTf5Box->value();
    InputsTuning.spanTF[5] = ui->spinTf6Box->value();
    InputsTuning.spanTF[6] = ui->spinTf7Box->value();
    InputsTuning.spanTF[7] = ui->spinTf8Box->value();

    InputsTuning.Inp1 = ui->checkIn1Box->isChecked();
    InputsTuning.Inp2 = ui->checkIn2Box->isChecked();
    InputsTuning.Inp3 = ui->checkIn3Box->isChecked();
    InputsTuning.Inp4 = ui->checkIn4Box->isChecked();
    InputsTuning.Inp5 = ui->checkIn5Box->isChecked();
    InputsTuning.Inp6 = ui->checkIn6Box->isChecked();
    InputsTuning.Inp7 = ui->checkIn7Box->isChecked();
    InputsTuning.Inp8 = ui->checkIn8Box->isChecked();
}


//*************************************************************************
//*** Функции обработки изменения состояния checkInNBox
//
void InputSet::on_checkIn1Box_clicked()
{
    if(ui->checkIn1Box->isChecked())
    {
        ui->spinTr1Box->setEnabled(true);
        ui->spinTp1Box->setEnabled(true);
        ui->spinTf1Box->setEnabled(true);
    }
    else
    {
        ui->spinTr1Box->setEnabled(false);
        ui->spinTp1Box->setEnabled(false);
        ui->spinTf1Box->setEnabled(false);
    }
}

void InputSet::on_checkIn2Box_clicked()
{
    if(ui->checkIn2Box->isChecked())
    {
        ui->spinTr2Box->setEnabled(true);
        ui->spinTp2Box->setEnabled(true);
        ui->spinTf2Box->setEnabled(true);
    }
    else
    {
        ui->spinTr2Box->setEnabled(false);
        ui->spinTp2Box->setEnabled(false);
        ui->spinTf2Box->setEnabled(false);
    }
}

void InputSet::on_checkIn3Box_clicked()
{
    if(ui->checkIn3Box->isChecked())
    {
        ui->spinTr3Box->setEnabled(true);
        ui->spinTp3Box->setEnabled(true);
        ui->spinTf3Box->setEnabled(true);
    }
    else
    {
        ui->spinTr3Box->setEnabled(false);
        ui->spinTp3Box->setEnabled(false);
        ui->spinTf3Box->setEnabled(false);
    }
}

void InputSet::on_checkIn4Box_clicked()
{
    if(ui->checkIn4Box->isChecked())
    {
        ui->spinTr4Box->setEnabled(true);
        ui->spinTp4Box->setEnabled(true);
        ui->spinTf4Box->setEnabled(true);
    }
    else
    {
        ui->spinTr4Box->setEnabled(false);
        ui->spinTp4Box->setEnabled(false);
        ui->spinTf4Box->setEnabled(false);
    }
}

void InputSet::on_checkIn5Box_clicked()
{
    if(ui->checkIn5Box->isChecked())
    {
        ui->spinTr5Box->setEnabled(true);
        ui->spinTp5Box->setEnabled(true);
        ui->spinTf5Box->setEnabled(true);
    }
    else
    {
        ui->spinTr5Box->setEnabled(false);
        ui->spinTp5Box->setEnabled(false);
        ui->spinTf5Box->setEnabled(false);
    }
}

void InputSet::on_checkIn6Box_clicked()
{
    if(ui->checkIn6Box->isChecked())
    {
        ui->spinTr6Box->setEnabled(true);
        ui->spinTp6Box->setEnabled(true);
        ui->spinTf6Box->setEnabled(true);
    }
    else
    {
        ui->spinTr6Box->setEnabled(false);
        ui->spinTp6Box->setEnabled(false);
        ui->spinTf6Box->setEnabled(false);
    }
}

void InputSet::on_checkIn7Box_clicked()
{
    if(ui->checkIn7Box->isChecked())
    {
        ui->spinTr7Box->setEnabled(true);
        ui->spinTp7Box->setEnabled(true);
        ui->spinTf7Box->setEnabled(true);
    }
    else
    {
        ui->spinTr7Box->setEnabled(false);
        ui->spinTp7Box->setEnabled(false);
        ui->spinTf7Box->setEnabled(false);
    }
}

void InputSet::on_checkIn8Box_clicked()
{
    if(ui->checkIn8Box->isChecked())
    {
        ui->spinTr8Box->setEnabled(true);
        ui->spinTp8Box->setEnabled(true);
        ui->spinTf8Box->setEnabled(true);
    }
    else
    {
        ui->spinTr8Box->setEnabled(false);
        ui->spinTp8Box->setEnabled(false);
        ui->spinTf8Box->setEnabled(false);
    }
}
