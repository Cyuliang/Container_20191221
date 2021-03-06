#ifndef RECOGNIZERPROCESSING_H
#define RECOGNIZERPROCESSING_H

#include <QObject>
#include <QPixmap>
#include <QMap>
#include <QMutexLocker>
#include <QMutex>
#include <QDir>
#include <QDateTime>
#include <QQueue>
#include <QDebug>

class RecognizerProcessing : public QObject
{
    Q_OBJECT
public:
    explicit RecognizerProcessing(QObject *parent = nullptr);
    ~RecognizerProcessing();

private:

    ///
    /// \brief encryption 加密狗状态
    ///
    bool encryption;

    ///
    /// \brief imgCounts 图片数据
    ///
    int imgCounts;

     ///
     /// \brief imgList 图片名列表
     ///
     QStringList imgList;

     ///
     /// \brief timeList 时间戳列表
     ///
     QStringList timeList;

     ///
     /// \brief imgNameLIst 识别图片列表
     ///
     QStringList imgNameLIst;

    ///
    /// \brief mutex 锁
    ///
    QMutex mutex;

    ///
    /// \brief queue 箱型队列
    ///
    QQueue<int> queue;

    ///
    /// \brief resulList 所有识别结果列表
    ///
    QStringList resulList;

    ///
    /// \brief containerNum 识别集装箱数量
    ///
    int containerNum;

    ///
    /// \brief containertType 箱型
    ///
    int containertType;

    ///
    /// \brief channel 通道号
    ///
    int channel;

    ///
    /// \brief imgPath1 图片路径1
    ///
    QString imgPath1;

    ///
    /// \brief imgPath2 图片路径2
    ///
    QString imgPath2;

    ///
    /// \brief format1 保存协议1
    ///
    int format1;

    ///
    /// \brief format2 保存协议2
    ///
    int format2;

signals:

    ///
    /// \brief identifyImagesSignal 识别图片
    /// \param image  图片名
    ///
    void identifyImagesSignal(const QString &image);

    ///
    /// \brief resultsOfAnalysisSignal 分析箱号
    /// \param resultList 识别结果集
    /// \param type 逻辑类型
    /// \param imgList 图片名
    ///
    void resultsOfAnalysisSignal(QStringList resultList, int type,QStringList imgList);

    ///
    /// \brief putCommantStateSignal 抓拍信息写入日志
    /// \param msg 信息体
    ///
    void putCommantStateSignal(const int& channel, const QString& msg);

    ///
    /// \brief uploadDataSlot 上传数据(上传图片)
    /// \param data 数据
    ///
   void uploadDataSignal(const QString& data);

public slots:

    ///
    /// \brief setChannelSlot 设定通道号
    ///
    void setChannelSlot(int channel);

    ///
    /// \brief setSaveImgFormatOneSlot 设置图片保存路径1
    /// \param path
    /// \param format
    ///
    void setSaveImgFormatOneSlot(const QString &path,int format);

    ///
    /// \brief setSaveImgFormatTowSlot 设置图片保存路径2
    /// \param path
    /// \param format
    ///
    void setSaveImgFormatTowSlot(const QString &path,int format);
    ///
    /// \brief pictureStreamSlot 图片流
    /// \param jpgStream 图片流
    /// \param imgNumber 图片编号
    ///
    void pictureStreamSlot(const QByteArray &jpgStream, const int &imgNumber,const QString &imgTime);

    ///
    /// \brief saveImageTowSlot 保存图片2
    /// \param jpgStream 图片流
    /// \param imgNumber 图片编号
    /// \param imgTime 时间戳
    ///
    void saveImageTowSlot(const QByteArray &jpgStream, const int &imgNumber,const QString &imgTime);

    ///
    /// \brief infraredCompleteSlot 逻辑抓拍完成
    /// \param type 逻辑类型
    /// \param imgCount 图片数量
    ///
    void infraredCompleteSlot(const int &type,int imgCount);

    ///
    /// \brief InfraredLogicStartSlot 逻辑抓拍开始
    ///
    void InfraredLogicStartSlot();

    ///
    /// \brief recognitionResultSlot 识别结果
    /// \param result 识别结果
    /// \param imgName 图片名
    ///
    void recognitionResultSlot(const QString &result,const QString& imgName);    

    ///
    /// \brief GetTheEncryptedState 获取加密状态
    /// \param state
    ///
    void GetTheEncryptedStateSlot(bool state);
};

#endif // RECOGNIZERPROCESSING_H
