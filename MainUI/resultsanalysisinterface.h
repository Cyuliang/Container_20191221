#ifndef RESULTSANALYSISINTERFACE_H
#define RESULTSANALYSISINTERFACE_H

#define ZBY_LOG(type)  tr("ZBY_LOG_%1(Func[%2](Line[%3]))").arg(type).arg(Q_FUNC_INFO).arg(__LINE__)
#include <QObject>
#include <QMap>

class ResultsAnalysisInterface : public QObject
{
    Q_OBJECT

public:
    virtual ~ResultsAnalysisInterface() {}

signals:

    ///
    /// \brief messageSignal 日志信息
    /// \param type 日志类型
    /// \param msg 信息体
    ///
    void messageSignal(const QString &type,const QString &msg);

    ///
    /// \brief containerSignal 箱号结果
    /// \param result 箱号
    ///
    void containerSignal(const QString &result1,const QString &iso1,const QString &result2="",const QString &iso2="");

    ///
    /// \brief updateDataBaseSignal 更新箱号数据
    /// \param data 数据字典
    ///
    void updateDataBaseSignal(QMap<QString, QString> data);


public slots:

    ///
    /// \brief resultsOfAnalysisSlot 分析箱号
    /// \param resultList 识别结果集
    /// \param type 逻辑类型
    /// \param imgTime 箱号时间戳
    ///
    virtual void resultsOfAnalysisSlot(QStringList resultList, int type,const QString& imgTime)=0;

    ///
    /// \brief setCheckTheResultsSlot 是否校验识别结果
    /// \param correct 校验标示
    ///
    virtual void setCheckTheResultsSlot(bool correct=false)=0;

    ///
    /// \brief setChannelSlot 设定通道号
    ///
    virtual void setChannelSlot(int channel)=0;
};

#define ResultsAnalysisInterfaceIID "ZBY.ContainerServer.ResultsAnalysisInterface/1.0"
Q_DECLARE_INTERFACE(ResultsAnalysisInterface,ResultsAnalysisInterfaceIID);

#endif // RESULTSANALYSISINTERFACE_H
