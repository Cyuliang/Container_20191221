#include "recognizerprocessing.h"

RecognizerProcessing::RecognizerProcessing(QObject *parent) : QObject(parent)
{
    this->imgPath1="";
    //this->imgPath2="";
}

void RecognizerProcessing::setChannelSlot(int channel)
{
    this->channel=channel;
}

void RecognizerProcessing::setSaveImgFormatOneSlot(const QString &path, int format)
{
    this->imgPath1=path;
    this->format1=format;
    dir.setPath(QDir::toNativeSeparators(path));
}

//void RecognizerProcessing::setSaveImgFormatTowSlot(const QString &path, int format)
//{
//    this->imgPath2=path;
//    this->format2=format;
//}

void RecognizerProcessing::pictureStreamSlot(const QByteArray &jpgStream, const int &imgNumber, const QString &imgTime)
{
    QMutexLocker locket(&mutex);
    if(imgPath1!=""){/* 保存路径不存在,图片不保存,不识别 */
        QString suffixPath="";
        switch (format1) {
        case 0:
            suffixPath=QDir::toNativeSeparators(tr("%1/%2").arg(channel).arg(QDateTime::currentDateTime().toString("yyyy/MM/dd")));
            break;
        case 1:
            suffixPath=QDir::toNativeSeparators(tr("%1/%2").arg(channel).arg(QDateTime::currentDateTime().toString("yyyy/MM")));
            break;
        case 2:
            suffixPath=QDir::toNativeSeparators(tr("%1/%2").arg(channel).arg(QDateTime::currentDateTime().toString("yyyy")));
            break;
        case 4:
            suffixPath=QDir::toNativeSeparators(tr("%1").arg(QDateTime::currentDateTime().toString("yyyy/MM/dd")));
            break;
        case 5:
            suffixPath=QDir::toNativeSeparators(tr("%1").arg(QDateTime::currentDateTime().toString("yyyy/MM")));
            break;
        case 6:
            suffixPath=QDir::toNativeSeparators(tr("%1").arg(QDateTime::currentDateTime().toString("yyyy")));
            break;
        }
        dir.mkpath(suffixPath);
        dir.cd(suffixPath);

        if(imgTime!="" && jpgStream!=nullptr){
            QPixmap *labelPix = new QPixmap();
            labelPix->loadFromData(jpgStream);
            labelPix->save(QDir::toNativeSeparators(tr("%1/%2%3%4.jpg").arg(dir.path()).arg(channel).arg(imgTime.split('-').join("").split(':').join("").split(" ").join("")).arg(imgNumber)));
            delete labelPix;
            labelPix=nullptr;
        }
        dir.setPath(imgPath1);
    }
}
