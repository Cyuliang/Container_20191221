﻿#include "recognizerprocessing.h"

RecognizerProcessing::RecognizerProcessing(QObject *parent) : QObject(parent)
{
    encryption=false;
    this->imgPath1="";
    this->imgPath2="";
    this->channel=-1;
    containerNum=0;
    containertType=0;
    imgCounts=0;
}

RecognizerProcessing::~RecognizerProcessing()
{
    resulList.clear();
    //chanResulList.clear();
    queue.clear();
}

void RecognizerProcessing::setChannelSlot(int channel)
{
    this->channel=channel;
}

void RecognizerProcessing::setSaveImgFormatOneSlot(const QString &path, int format)
{
    this->imgPath1=path;
    this->format1=format;  
}

void RecognizerProcessing::setSaveImgFormatTowSlot(const QString &path, int format)
{
    this->imgPath2=path;
    this->format2=format;
}

void RecognizerProcessing::pictureStreamSlot(const QByteArray &jpgStream, const int &imgNumber, const QString &imgTime)
{
    if(imgNumber==0){/* 测试抓图,不保存,不识别 */
        return;
    }
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
        case 3:
            suffixPath=QDir::toNativeSeparators(tr("%1").arg(channel));
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
        case 7:
            suffixPath=QDir::toNativeSeparators("./");
            break;
        }

        QDir dir(imgPath1);
        dir.mkpath(suffixPath);
        dir.cd(suffixPath);

        qDebug()<<"encryption:"<<encryption;
        /* 加密不存在识别.给空结果,通用不需要单独读狗 */
        if(true && imgTime!="" && jpgStream!=nullptr){
            QPixmap *labelPix = new QPixmap();
            labelPix->loadFromData(jpgStream);            
            QPixmap labelPixFit=  labelPix->scaled(1280,720, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);/* 缩放图片 */
            QString image=QDir::toNativeSeparators(tr("%1/%2%3%4.jpg").arg(dir.path()).arg(QDateTime::fromString(imgTime,"yyyy-MM-dd hh:mm:ss").toString("yyyyMMddhhmmss")).arg(channel).arg(imgNumber));
            labelPixFit.save(image);
            delete labelPix;
            labelPix=nullptr;
            emit identifyImagesSignal(image);/* 识别图片 */
            /* 抓拍状态写入日志 */
            emit putCommantStateSignal(channel,tr("TIME:%1 ID:%2").arg(QDateTime::fromString(imgTime,"yyyy-MM-dd hh:mm:ss").toString("yyyyMMddhhmmss")).arg(QString::number(imgNumber)));
        }
        else {
            /* 没有图片直接给结果,不经过识别器 */
            QString image=QDir::toNativeSeparators(tr("%1/%2%3%4.jpg").arg(dir.path()).arg(QDateTime::fromString(imgTime,"yyyy-MM-dd hh:mm:ss").toString("yyyyMMddhhmmss")).arg(channel).arg(imgNumber));
            recognitionResultSlot("RESULT: ||0|0",image);
        }
    }
}

void RecognizerProcessing::saveImageTowSlot(const QByteArray &jpgStream, const int &imgNumber, const QString &imgTime)
{
    if(imgNumber==0){/* 测试抓图,不保存,不识别 */
        return;
    }
    QMutexLocker locket(&mutex);
    if(imgPath2!=""){/* 保存路径不存在,图片不保存 */
        QString suffixPath="";
        switch (format2) {
        case 0:
            suffixPath=QDir::toNativeSeparators(tr("%1/%2").arg(channel).arg(QDateTime::currentDateTime().toString("yyyy/MM/dd")));
            break;
        case 1:
            suffixPath=QDir::toNativeSeparators(tr("%1/%2").arg(channel).arg(QDateTime::currentDateTime().toString("yyyy/MM")));
            break;
        case 2:
            suffixPath=QDir::toNativeSeparators(tr("%1/%2").arg(channel).arg(QDateTime::currentDateTime().toString("yyyy")));
            break;
        case 3:
            suffixPath=QDir::toNativeSeparators(tr("%1").arg(channel));
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
        case 7:
            suffixPath=QDir::toNativeSeparators("./");
            break;
        }

        QDir dir(imgPath2);
        dir.mkpath(suffixPath);
        dir.cd(suffixPath);

        QString image="";
        if(imgTime!="" && jpgStream!=nullptr){
            QPixmap *labelPix = new QPixmap();
            labelPix->loadFromData(jpgStream);
            //QPixmap labelPixFit=  labelPix->scaled(1920,1080, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);/* 缩放图片 */
            image=QDir::toNativeSeparators(tr("%1/%2%3%4.jpg").arg(dir.path()).arg(QDateTime::fromString(imgTime,"yyyy-MM-dd hh:mm:ss").toString("yyyyMMddhhmmss")).arg(channel).arg(imgNumber));
            //labelPixFit.save(image);
            labelPix->save(image);
            delete labelPix;
            labelPix=nullptr;
        }
    }
}

void RecognizerProcessing::InfraredLogicStartSlot()
{    
//    resulList.clear();
//    imgList.clear();
//    timeList.clear();
//    containerNum=0;
//    containertType=0;
//    imgCounts=0;/* 置零 */
}

void RecognizerProcessing::infraredCompleteSlot(const int &type,int imgCount)
{
    /*  Type,集装箱类别:
     * -1 – 未知
     * 0 – 一个 20 集装箱
     * 1 – 一个 40 吋/45 吋集装箱
     * 2 – 两个 20 吋集装箱
     */

    if(type!=-1){
        queue.enqueue(type);
    }
    imgCounts+=imgCount;
    containerNum=0;/* 防止遗留逻辑类型 */
}

void RecognizerProcessing::recognitionResultSlot(const QString &result, const QString& imgName)
{
    imgNameLIst.append(imgName);/* 用作删除废弃的图片 */
    QStringList tmpList=imgName.split(QDir::toNativeSeparators("/"));
    QString img="";
    if(tmpList.count()>1){
        img=tmpList[tmpList.count()-1].split(".")[0];
    }

    qDebug()<<imgName;
    resulList<<result;
    imgList<<img;
    timeList<<img.mid(0,14);

    if(containerNum==0 && queue.count()!=0){
        containertType=queue.dequeue();
        switch (containertType) {
        case 0:
            containerNum=4;
            break;
        case 1:
            containerNum=6;
            break;
        case 2:
            containerNum=6;
            break;
        }
    }
    qDebug()<<"imgCounts"<<imgCounts;
    qDebug()<<"containerNum"<<containerNum;
    qDebug()<<"resulList.count"<<resulList.count();

    if(resulList.count()>=containerNum && resulList.count()==imgCounts && containerNum!=0){
        QSet<QString> set=timeList.toSet();
        QString setMax="";
        foreach (auto var, set) {/* 取最后时间戳 */
            if(var>setMax){
                setMax=var;
            }
        }
        QList<int> indList;
        QList<int> indRemoveList;/* 要删除的图片名下标 */
        for (int ind = timeList.count()-1; ind >= 0; --ind) {
            if(setMax==timeList[ind]){
                qDebug()<<"setMax"<<setMax<<"timeList:"<<timeList[ind];
                if(indList.count()==containerNum){
                    continue;
                }
                indList.append(ind);
            }
            else {
                indRemoveList.append(ind);
            }
        }
        if(indList.count()==containerNum){
            QStringList chanResultList,chanImgList;
            foreach (auto var, indList) {
                chanResultList<<resulList[var];
                chanImgList<<imgList[var];
                qDebug()<<"imgList"<<imgList[var];
            }
            emit resultsOfAnalysisSignal(chanResultList,containertType,chanImgList);/* 分析结果 */
            chanResultList.clear();
            chanImgList.clear();

            foreach (auto ind, indRemoveList) {/* 删除多余图片 */
                qDebug()<<imgNameLIst[ind];
                QFile::remove(imgNameLIst[ind]);
            }

            foreach (auto ind, indList) {/* 上传图片 */
                emit uploadDataSignal(imgNameLIst[ind]);
            }

            containerNum=0;
            containertType=0;
            resulList.clear();
            imgList.clear();
            timeList.clear();
            set.clear();
            indRemoveList.clear();
            imgNameLIst.clear();
            imgCounts=0;/* 置零 */
        }
        else {
            indList.clear();
        }
    }
}
/*    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!加密状态       */
void RecognizerProcessing::GetTheEncryptedStateSlot(bool state)
{
    encryption=state;
}
