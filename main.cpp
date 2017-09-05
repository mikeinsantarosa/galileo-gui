#include "dialog.h"
#include <QApplication>

#include <QTranslator>
#include <QLocale>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialog w;
    w.show();

    return a.exec();
}
