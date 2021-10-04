#include "about.h"
#include "ui_about.h"

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);

    this->setWindowTitle(tr("About"));
    this->setFixedSize(this->size());
    this->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);// remove question mark from the title bar

    ui->logo_label->setPixmap(QPixmap(":/images/icons/app_icon.png"));
    ui->name_label->setPixmap(QPixmap(":/images/icons/app_name.png"));

    ui->logo_label->setScaledContents(true);
    ui->name_label->setScaledContents(true);

    ui->logo_label->move((this->width() - ui->logo_label->width()) / 2, ui->logo_label->y());
    ui->name_label->move((this->width() - ui->name_label->width()) / 2, ui->name_label->y());

    ui->version_label->setText(tr("Version ") + qApp->applicationVersion());
    ui->version_label->setStyleSheet("QLabel { color: #3498DB;}");

    ui->vlc_label->setText(tr("<a style=color:#3498DB href=https://www.videolan.org/vlc/libvlc.html>libVLC</a> powered"));
    ui->vlc_label->setTextFormat(Qt::RichText);
    ui->vlc_label->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->vlc_label->setOpenExternalLinks(true);

    ui->reportBugs_label->setText(tr("<a style=color:#3498DB href=https://github.com/hbtalha/QThisPlayer/issues/new>"
                                     "Report problems</a>"));
    ui->reportBugs_label->setTextFormat(Qt::RichText);
    ui->reportBugs_label->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->reportBugs_label->setOpenExternalLinks(true);

    ui->version_label->adjustSize();
    ui->version_label->move((this->width() - ui->version_label->width()) / 2, ui->version_label->y());

    ui->vlc_label->adjustSize();
    ui->vlc_label->move((this->width() - ui->vlc_label->width()) / 2, ui->vlc_label->y());

    ui->reportBugs_label->adjustSize();
    ui->reportBugs_label->move((this->width() - ui->reportBugs_label->width()) / 2, ui->reportBugs_label->y());

    ui->pushButton->move((this->width() - ui->pushButton->width()) / 2, ui->pushButton->y());

    connect(ui->pushButton, &QPushButton::clicked, [this]()
    {
        this->close();
    });
}

About::~About()
{
    delete ui;
}
