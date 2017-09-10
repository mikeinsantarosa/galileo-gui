/*****************************************************************************
 * dialog.h
 *****************************************************************************
 * Copyright (C) 2014 MX Authors
 *
 * Authors: mmikeinsantarosa/Adrian
 *          Originally released for
 *          MX Linux <http://mxlinux.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Galileo GUI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Galileo GUI.  If not, see <http://www.gnu.org/licenses/>.
 **********************************************************************/

#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <QProcess>
#include <QTimer>
#include <QDebug>
#include <QSettings>
#include <QFile>
#include <QDir>

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
    void on_btnSync_clicked();
    void on_btnQuit_clicked();



    void on_radioHelp_clicked();

private:
    Ui::Dialog *ui;
    QString options;
    QString sCommand;
    void Sync(QString option);
    QString getOption();
    void saveOptionSetting(QString opt);
    QString loadOptionSetting();
    void setSelectedRunOption(QString opt);
    QFile config_file;
};

#endif // DIALOG_H
