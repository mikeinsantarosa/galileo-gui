#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <QProcess>
#include <QTimer>
#include <QDebug>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

protected:
    QProcess *proc;
    QTimer *timer;

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

    QString getCmdOut(QString cmd);
    QString getVersion(QString name);
    void setup();

public slots:
    void procStart();
    void procTime();
    void procDone(int exitCode);
    void setConnections(QTimer *timer, QProcess *proc);
    void onStdoutAvailable();

private slots:
    void on_radioQuiet_clicked();

    void on_radioVerbose_clicked();

    void on_radioForce_clicked();

    void on_radioForceVerbose_clicked();

    void on_btnAbout_clicked();

    void on_btnHelp_clicked();

    void on_btnSync_clicked();

    void on_btnQuit_clicked();

private:
    Ui::Dialog *ui;
    QString options;
    QString sCommand;
    void Sync(QString option);
    QString getOption();
};

#endif // DIALOG_H
