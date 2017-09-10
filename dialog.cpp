/*****************************************************************************
 * dialog.cpp
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

#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    options = "";
    sCommand = QString("galileo");
    setup();
}

Dialog::~Dialog()
{
    delete ui;
}

QString Dialog::getCmdOut(QString cmd)
{

    proc = new QProcess(this);
    proc->start("/bin/bash", QStringList() << "-c" << cmd);
    proc->setReadChannel(QProcess::StandardOutput);
    proc->setReadChannelMode(QProcess::MergedChannels);
    proc->waitForFinished(-1);
    return proc->readAllStandardOutput().trimmed();

}

QString Dialog::getVersion(QString name)
{
    QString cmd = QString("dpkg -l %1 | awk 'NR==6 {print $3}'").arg(name);
    return getCmdOut(cmd);
}

void Dialog::setup()
{
    QString opt;

    proc = new QProcess(this);
    timer = new QTimer(this);
    proc->setReadChannel(QProcess::StandardOutput);
    proc->setReadChannelMode(QProcess::MergedChannels);
    ui->stackedWidget->setCurrentIndex(0);
    ui->btnQuit->setEnabled(true);
    ui->btnSync->setEnabled(true);
    ui->progressBar->setValue(0);

    // Save the last option the user selected.
    //------------------------------------------------
    opt = loadOptionSetting();


    if(opt!="")
    {
        setSelectedRunOption(opt);
    }
    else
    {
        ui->radioForceVerbose->setChecked(true);
    }

    //------------------------------------------------

    if (ui->btnSync->icon().isNull())
    {
        ui->btnSync->setIcon(QIcon::fromTheme("dialog-ok"));
    }

}

void Dialog::procStart()
{
    timer->start(100);
}

void Dialog::procTime()
{
    int i = ui->progressBar->value() + 1;
    if (i > 100) {
        i = 0;
    }
    ui->progressBar->setValue(i);
}

void Dialog::procDone(int exitCode)
{



    timer->stop();
    ui->progressBar->setValue(100);
    setCursor(QCursor(Qt::ArrowCursor));

    if (exitCode == 0)
    {
        if(ui->radioHelp->isChecked())
        {
           ui->outputLabel->setText(tr("CLI Help for galileo"));
        }
        else
        {
        ui->outputLabel->setText(tr("Finished synchronizing your tracker."));
        }
    } else {
        QMessageBox::critical(this, tr("Error"),
                              tr("Process finished. Errors have occurred."));
    }

    ui->btnSync->setEnabled(true);
    ui->btnSync->setText(tr("Back"));
    ui->btnSync->setIcon(QIcon::fromTheme("go-previous"));
}

void Dialog::setConnections(QTimer *timer, QProcess *proc)
{

    disconnect(timer, SIGNAL(timeout()), 0, 0);
    connect(timer, SIGNAL(timeout()), SLOT(procTime()));
    disconnect(proc, SIGNAL(started()), 0, 0);
    connect(proc, SIGNAL(started()), SLOT(procStart()));
    disconnect(proc, SIGNAL(finished(int)), 0, 0);
    connect(proc, SIGNAL(finished(int)),SLOT(procDone(int)));
    disconnect(proc, SIGNAL(readyReadStandardOutput()), 0, 0);
    connect(proc, SIGNAL(readyReadStandardOutput()), SLOT(onStdoutAvailable()));
}

void Dialog::onStdoutAvailable()
{


    QByteArray output = proc->readAllStandardOutput();
    QString out = ui->outputBox->toPlainText() + QString::fromUtf8(output);
    ui->outputBox->setPlainText(out);
    //QScrollBar *sb = ui->outputBox->verticalScrollBar();
}

void Dialog::on_radioQuiet_clicked()
{
    // save this setting
    saveOptionSetting("quiet");
}

void Dialog::on_radioVerbose_clicked()
{
    // save this setting
    saveOptionSetting("verbose");
}

void Dialog::on_radioForce_clicked()
{
    // save this setting
    saveOptionSetting("force");
}

void Dialog::on_radioForceVerbose_clicked()
{
    // save this setting
    saveOptionSetting("forceverbose");
}

void Dialog::on_btnAbout_clicked()
{
    this->hide();
    QMessageBox msgBox(QMessageBox::NoIcon,
                       tr("About Galileo GUI"), "<p align=\"center\"><b><h2>" +
                       tr("Galileo GUI") + "</h2></b></p><p align=\"center\">" + tr("Version: ") +
                       getVersion("galileo-gui") + "</p><p align=\"center\"><h3>" +
                       tr("GUI to run fitbit tracker sync applicaiton galileo") + "</h3><h4><br/>" +
                       tr("This program is composed of 2 packages:") +
                       "<p>galileo, the CLI utility: Copyright (c) Benoît Allard</p>" +
                       "<p>galileo-gui, the GUI wrapper: " +
                       tr("Copyright (c) MX Linux\n") + "<br /><br /></p>", 0, this);
    msgBox.addButton(tr("License"), QMessageBox::AcceptRole);
    msgBox.addButton(tr("Cancel"), QMessageBox::NoRole);
    if (msgBox.exec() == QMessageBox::AcceptRole) {
        system("xdg-open file:///usr/share/doc/galileo-gui/license.html");
    }
    this->show();
}


void Dialog::on_btnSync_clicked()
{
    // on first page
    QString opt;

    if (ui->stackedWidget->currentIndex() == 0) {
        ui->btnSync->setEnabled(false);
        ui->outputLabel->setText("Attempting to update database...");

        opt = getOption();

        Sync(opt);

    } else if (ui->stackedWidget->currentWidget() == ui->pageResult) {
        ui->stackedWidget->setCurrentIndex(0);
        // restore Start button
        ui->btnSync->setText(tr("Sync"));
        ui->btnSync->setIcon(QIcon::fromTheme("dialog-ok"));
        if (ui->btnSync->icon().isNull()) {
            ui->btnSync->setIcon(QIcon(":/icons/dialog-ok.svg"));
        }
 //       ui->buttonSave->setEnabled(false);
        ui->outputBox->clear();
        ui->outputLabel->setText("");
    } else {
        qApp->exit(0);
    }

}

void Dialog::on_btnQuit_clicked()
{
    qApp->exit(0);
}



void Dialog::Sync(QString option)
{


    setCursor(QCursor(Qt::WaitCursor));

    QString cmd = QString(sCommand + " " + option);

    ui->stackedWidget->setCurrentWidget(ui->pageResult);
    setConnections(timer, proc);

    proc->start(cmd);
}

QString Dialog::getOption()
{
    if(ui->radioQuiet->isChecked()){
        return QString("");
    }
    else
        if(ui->radioVerbose->isChecked()){
            return QString("-v");
        }
    else
        if(ui->radioForce->isChecked())
        {
            return QString("--force");
        }
    else
            if(ui->radioForceVerbose->isChecked())
        {
            return QString("--force -v");
        }
    else
    {
        return QString("--help");
    }
}

void Dialog::saveOptionSetting(QString opt)
{
    QString thisPath = QDir::homePath() + "/.config/galileo-gui/galileo-gui.conf";

    config_file.setFileName(thisPath);
    QSettings settings(config_file.fileName(), QSettings::IniFormat);
    settings.beginGroup("selectedRunOption");
    settings.setValue("runOption",opt);
    settings.endGroup();

}

QString Dialog::loadOptionSetting()
{

    QString opt;

    QString thisPath = QDir::homePath() + "/.config/galileo-gui/galileo-gui.conf";
    config_file.setFileName(thisPath);
    QSettings settings(config_file.fileName(), QSettings::IniFormat);
    settings.beginGroup("selectedRunOption");
    opt = settings.value("runOption").toString();
    settings.endGroup();

    return opt;
}

void Dialog::setSelectedRunOption(QString opt)
{
    if(opt=="quiet")
    {
        ui->radioQuiet->setChecked(true);
    }
    if(opt=="verbose")
    {
        ui->radioVerbose->setChecked(true);
    }
    if(opt=="force")
    {
        ui->radioForce->setChecked(true);
    }
    if(opt=="forceverbose")
    {
        ui->radioForceVerbose->setChecked(true);
    }
    if(opt=="help")
    {
        ui->radioHelp->setChecked(true);
    }

}



void Dialog::on_radioHelp_clicked()
{
    // save this setting
    saveOptionSetting("help");
}
