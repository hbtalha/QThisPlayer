#include "shared.h"

#include <QTime>
#include <QIcon>

const QStringList supportedSubtitlesFormats = {"cdg", "idx", "srt", "ssa", "aqt", "rt", "sami", "smi", "txt", "sub", "utf", "ass",
                                               "jss", "psb", "dks", "pjs", "mpl2", "mks", "smil", "stl", "usf", "vtt", "tt", "ttml",
                                               "dfxp", "scc"
                                              };

QList<QFileInfo> filterSupportedMediaFormats(const QList<QUrl>& urls)
{
    QList<QFileInfo> files;

    for(auto const& url : urls)
    {
        if(url.isLocalFile())
        {
            QFileInfo localFile(url.toLocalFile());

            if(supportedMediaFormats.contains(localFile.suffix()))
                files.append(localFile);
        }
    }

    return files;
}

bool areAllSubtitleFiles(const QList<QUrl> &urls)
{
    for(auto const& url : urls)
    {
        if(url.isLocalFile())
        {
            QFileInfo localFile(url.toLocalFile());

            if( ! supportedSubtitlesFormats.contains(localFile.suffix()))
                return false;
        }
        else
            return false;
    }

    return true;
}

QString formattedTime(int millSec)
{
    QTime time = QTime(0, 0);
    time = time.addMSecs(millSec);

    QString displayFormat = "mm:ss";
    if (time.hour() > 0)
        displayFormat = "hh:mm:ss";

    return  time.toString(displayFormat);;
}

QIcon invertedColorIcon(QIcon icon)
{
    /* ---- For all the states ----------*/
//    QIcon invertedIcon;
//    for(auto singleMode : {QIcon::Normal,QIcon::Disabled,QIcon::Active,QIcon::Selected}){
//    for(auto singleState : {QIcon::On,QIcon::Off}){
//    const auto avalSize = myIcon.availableSizes(singleMode ,singleState );
//    for(auto& singleSize : avalSize){
//    QImage tempImage = myIcon.pixmap(singleSize,singleMode ,singleState).toImage();
//    tempImage.invertPixels();
//    invertedIcon.addPixmap(QPixmap::fromImage(std::move(tempImage)),singleMode,singleState);
//    }
//    }
//    }

    const auto avalSize = icon.availableSizes();

    QIcon invertedIcon;

    for(auto& singleSize : avalSize)
    {
        QImage tempImage = icon.pixmap(singleSize).toImage();
        tempImage.invertPixels();
        invertedIcon.addPixmap(QPixmap::fromImage(std::move(tempImage)));
    }

    return invertedIcon;
}
