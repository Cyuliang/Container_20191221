#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H

#include <QObject>

class ImageProcessing : public QObject
{
    Q_OBJECT
public:
    explicit ImageProcessing(QObject *parent = nullptr);   

signals:

    ///
    /// \brief InitializationSignal 初始化参数
    ///
    bool InitializationSignal();

    ///
    /// \brief initCamer 初始化相机
    /// \param camerIP 相机地址
    /// \param camerPort 相机端口
    ///
    void initCamerSignal(const QString &camerIP, const int &camerPort,const QString &CamerUser,const QString &CamerPow,const QString& alias);

    ///
    /// \brief putCommand 相机指令
    /// \param command 指令
    /// \return  返回执行状态
    ///
    bool putCommandSignal(const int &command);

    ///
    /// \brief pictureStreamSignals 图片流信号
    /// \param jpgStream 图片流
    /// \param imgNumber 图片编号
    ///
    void pictureStreamSignal(const QByteArray &jpgStream,const int &imgNumber);

};

#endif // IMAGEPROCESSING_H
