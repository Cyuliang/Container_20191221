#ifndef SOCKETSERVICE_H
#define SOCKETSERVICE_H

#include "socketservice_global.h"
#include "socketserverinterface.h"
#include "socketserver.h"

class SOCKETSERVICESHARED_EXPORT SocketService:public SocketServerInterface
{
    Q_OBJECT
    Q_INTERFACES(SocketServerInterface)
    Q_PLUGIN_METADATA(IID SocketServerInterfaceIID)

public:
    SocketService(QObject* parent=nullptr);
    ~SocketService()Q_DECL_OVERRIDE;

private:

    ///
    /// \brief connected 客户端链接状态
    ///
    bool isConnected;

    ///
    /// \brief heartBeat 心跳包状态
    ///
    int heartBeat;

    ///
    /// \brief address 地址
    ///
    QString address;

    ///
    /// \brief port 端口
    ///
    quint16 port;

    ///
    /// \brief tcpSocket 客户端
    ///
    QTcpSocket *pTcpSocket;

    ///
    /// \brief pTcpServer 服务器
    ///
    SocketServer* pTcpServer;

    ///
    /// \brief pTimerLink 定时心跳包检测
    ///
    QTimer *pTimerLink;

private slots:

    ///
    /// \brief startLink 链接到服务器,客户端模式
    ///
    void startLink();

    ///
    /// \brief startListen 开始监听,服务器模式
    ///
    void startListen();

    ///
    /// \brief heartbeatSlot 心跳检测
    ///
    void heartbeatSlot();

    ///
    /// \brief connected 链接成功
    ///
    void connected();

    ///
    /// \brief readFortune 接收数据
    ///
    void readFortune();

    ///
    /// \brief disconnected 链接断开
    ///
    void disconnected();

    ///
    /// \brief displayError 链接错误
    /// \param socketError 链接状态码
    ///
    void displayError(QAbstractSocket::SocketError socketError);

public:

    ///
    /// \brief InitializationParameterSlot 初始化参数
    /// \param address 地址
    /// \param port 端口
    /// \param serviceType 服务类型
    /// \param heartBeat 心跳包 状态
    /// \param serviceMode 服务模式
    ///
    void  InitializationParameterSlot(const QString& address,const quint16& port,const int& serviceType,const int& serviceMode,const int& heartBeat)Q_DECL_OVERRIDE;

    ///
    /// \brief socketSendDataSlot 发送数据
    /// \param data 数据体
    ///
    void socketSendDataSlot(const QString& data)Q_DECL_OVERRIDE;  

    ///
    /// \brief releaseResourcesSlot 释放动资源
    ///
    void releaseResourcesSlot()Q_DECL_OVERRIDE;

    ///
    /// \brief sendResultSlot 发送识别结果
    /// \param channel 通道号
    /// \param result 识别结果
    ///
    void sendResultSlot(int channel,const QString& result);
};

#endif // SOCKETSERVICE_H
