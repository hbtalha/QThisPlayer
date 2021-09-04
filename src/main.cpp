#include "mainwindow.h"

#include <QApplication>
#include <QStyleFactory>
#include <QSettings>
#include <QList>
#include <QUrl>

#include "shared.h"

void associateFileExtensions()
{
    QString val;
    QString exePath = qApp->applicationFilePath();
    exePath.replace("/", "\\");

    for(auto const& e : supportedMediaFormats)
    {
        QSettings regType("HKEY_CURRENT_USER\\SOFTWARE\\Classes\\." + e,
                          QSettings::NativeFormat);
        QSettings regIcon("HKEY_CURRENT_USER\\SOFTWARE\\Classes\\." + e + "\\DefaultIcon",
                          QSettings::NativeFormat);
        QSettings regShell("HKEY_CURRENT_USER\\SOFTWARE\\Classes\\." + e + "\\Shell\\Open\\Command",
                           QSettings::NativeFormat);

        /** . means default value, you can also use the "Default" string */
        if("" != regType.value(".").toString())
        {
            regType.setValue(".","");
        }

        val = exePath + ",0";
        if(val != regIcon.value(".").toString())
        {
            regIcon.setValue(".",val);
        }

        val = exePath + " \"%1\"";
        if(val != regShell.value(".").toString())
        {
            regShell.setValue(".",val);
        }
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // associateFileExtensions();

//    QCoreApplication::setApplicationVersion("2021.08.10");
    QCoreApplication::setOrganizationName("HBatalha");
    QCoreApplication::setApplicationName("QThisPlayer");
    QSettings::setDefaultFormat(QSettings::IniFormat);

    QPalette darkPalette;
    QColor darkColor = QColor(27,27,27);
    QColor disabledColor = QColor(127,127,127);

    darkPalette.setColor(QPalette::Window, darkColor);
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(0,0,0));
    darkPalette.setColor(QPalette::AlternateBase, darkColor);
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::Text, disabledColor);
    darkPalette.setColor(QPalette::Button, darkColor);
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, disabledColor);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(83,173,203));
    darkPalette.setColor(QPalette::Highlight, QColor(83,173,203));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    darkPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, disabledColor);

    qApp->setStyle(QStyleFactory::create("fusion"));
    qApp->setPalette(darkPalette);

    MainWindow w;
    w.show();
    return a.exec();
}
