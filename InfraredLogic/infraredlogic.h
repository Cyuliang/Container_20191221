#ifndef INFRAREDLOGIC_H
#define INFRAREDLOGIC_H

#include "infraredlogic_global.h"
#include "infraredlogicinterface.h"

class INFRAREDLOGICSHARED_EXPORT InfraredLogic:public InfraredlogicInterface
{
    Q_OBJECT
    Q_INTERFACES(InfraredlogicInterface)
    Q_PLUGIN_METADATA(IID InfraredlogicInterfaceIID)

public:
    explicit InfraredLogic(QObject *parent = nullptr);
    ~InfraredLogic() Q_DECL_OVERRIDE;

private:

    ///
    /// \brief status 红外信号状态
    ///
    int status[6];

    ///
    /// \brief tmpStatus 临时红外信号
    ///
    int tmpStatus[6];

    /* 红外信号值 */
    int valueOne,valueTwo;

     /* 箱型逻辑条件 45尺,22尺,双22尺中间状态,双22尺 */
     bool _45G1,_22G1,_22G1_MID_22G1,_22G1_22G1;

     ///
     /// \brief _22G1_22G1_STATE 双箱状态
     ///
     bool  _22G1_22G1_STATE;

     ///
     /// \brief type 模拟抓拍类型
     ///
     int type;

     ///
     /// \brief exit 退出循环状态
     ///
     bool exit;

     ///
     /// \brief health 红外健康状态
     ///
     bool health;

     ///
     /// \brief doubleFrontPut 双22G1箱前面抓拍状态
     ///
     bool doubleFrontPut;

     ///
     /// \brief pTimerFront 前循环抓拍定时器
     ///
     QTimer* pTimerFront;

     ///
     /// \brief pTimerAfter 后循环抓拍定时器
     ///
     QTimer* pTimerAfter;

     ///
     /// \brief pDetectionTimer 检测红外状态定时器
     ///
     QTimer* pDetectionTimer;

     ///
     /// \brief pSerial1 串口1
     ///
     QSerialPort* pSerial1;

     ///
     /// \brief pSerial2 串口2
     ///
     QSerialPort* pSerial2;

     ///
     /// \brief com1 串口打开状态1
     ///
     bool com1=false;

     ///
     /// \brief com2 串口打开状态2
     ///
     bool com2=false;

     ///
     /// \brief port1 串口1
     ///
     QString port1;

     ///
     /// \brief port2 串口2
     ///
     QString port2;

private:

     ///
     /// \brief compareStatus 比较红外状态
     /// \param before 前一次状态
     /// \param after 后一次状态
     /// \return
     ///
     bool compareStatus(int* before,int* after);

     ///
     /// \brief serialLogicStatus 逻辑判断
     /// \param status 状态
     ///
      void serialLogic(int *status);

      ///
      /// \brief detectionLogicStatus 检测红外状态
      ///
      void detectionLogicStatus(bool com1,bool com2);

private slots:

      ///
      /// \brief delayAfterCaptureSlot 延时抓拍后3张图片.
      ///
      void delayAfterCaptureSlot();

      ///
      /// \brief DelayFrontCaptureSlot 延时抓拍前3张图片.
      ///
      void delayFrontCaptureSlot();

      ///
      /// \brief detectionLogicSlot 定时检测红外状态
      ///
      void detectionLogicSlot();

      ///
      /// \brief realyTheSerialport  串口打开失败重复打开
      ///
      void realyTheSerialport();

public:

     ///
     /// \brief startSlave 设置参数,启动串口
     /// \param portName1 串口1
     /// \param portName2 串口2
     ///
     void startSlaveSlot(const QString &portName1, const QString &portName2) Q_DECL_OVERRIDE;

     ///
     /// \brief simulateTriggerSlot 模拟触发
     /// \param type 逻辑类型
     /// type=1 22G1
     /// type=2 45G1
     /// type=3 Double 22G1_F
     /// type=4 Double 22G1_b
     /// type=5 while
     ///
      void simulateTriggerSlot(int type)Q_DECL_OVERRIDE;

     ///
     /// \brief setAlarmModeSlot 设置红外模式
     /// \param mode 模式(敞开|常闭)
     ///
     void setAlarmModeSlot(bool model)Q_DECL_OVERRIDE;

     ///
     /// \brief exitWhile 退出循环
     /// \param exit 条件
     ///
     void exitWhileSlot(bool EXIT)Q_DECL_OVERRIDE;
};

#endif // INFRAREDLOGIC_H
