#ifndef INPUTSET_H
#define INPUTSET_H

#include <QDialog>

namespace Ui {
class InputSet;
}

class InputSet : public QDialog
{
    Q_OBJECT

public:
    struct InpDelaySet {
        quint16 spanTR[8];
        quint16 spanTF[8];
        quint16 spanTP[8];
        bool Inp1;
        bool Inp2;
        bool Inp3;
        bool Inp4;
        bool Inp5;
        bool Inp6;
        bool Inp7;
        bool Inp8;
    };

    explicit InputSet(QWidget *parent = 0);
    ~InputSet();

    InpDelaySet inptuning() const;

private slots:
    void on_okButton_clicked();
    void on_cancelButton_clicked();

    void on_checkIn1Box_clicked();
    void on_checkIn2Box_clicked();
    void on_checkIn3Box_clicked();
    void on_checkIn4Box_clicked();
    void on_checkIn5Box_clicked();
    void on_checkIn6Box_clicked();
    void on_checkIn7Box_clicked();
    void on_checkIn8Box_clicked();

private:
    void resetTuning();
    void updateTuning();


private:
    Ui::InputSet *ui;
    InpDelaySet InputsTuning;
};

#endif // INPUTSET_H
