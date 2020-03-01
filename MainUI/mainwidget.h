#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QDir>
#include <QPluginLoader>
#include <QPixmap>
#include <QDebug>
#include <QThread>
#include <QTreeWidgetItem>
#include <QResizeEvent>
#include <QStatusBar>
#include <QCloseEvent>
#include<QLabel>

//------------------------------------------------------------------------------------------------------------Interface
#include "getimagesinterface.h"
#include "infraredlogicinterface.h"

//------------------------------------------------------------------------------------------------------------Headers
#include "setting.h"

//------------------------------------------------------------------------------------------------------------UI
#include "picturewidget.h"
#include "datawidget.h"
#include "channelsettingwidget.h"
#include "systemsettingwidget.h"
#include "servicewidget.h"
#include "databasewidget.h"

//------------------------------------------------------------------------------------------------------------Process
#include "imageprocessing.h"
#include "logicalprocessing.h"
#include "getsysinfo.h"

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = nullptr);
    ~MainWidget()override;

private:

    ///
    /// \brief statusBar 系统状态栏
    ///
    QStatusBar *statusBar;

    ///
    /// \brief pStatusBarLabel 状态栏文本
    ///
    QLabel*pStatusBarLabel;


private:
    Ui::MainWidget *ui;

    ///
    /// \brief pGetSysInfo CPU,MEM状态
    ///
    GetSysInfo* pGetSysInfo;

    ///
    /// \brief CamerNameList 通道相机列表
    ///
    QList<QString> CamerNameList;

    ///
    /// \brief ThreadList 插件线程池
    ///
    QList<QThread*> ThreadList;

    ///
    /// \brief Separators 路径分割符
    ///
    QString  separators;

    ///
    /// \brief channelCounnt 通道数
    ///
    int channelCounnt;

private:

    /*
     * 窗口字典
     */
    QHash<int,QObject*>CamerWidgetMap;
    QHash<int,QObject*>DataWidgetMap;

    ///
    /// \brief channelCamerMultiMap 通道对应的相机.
    ///
    QMultiHash<int,QObject*>channelCamerMultiMap;

    /*
     * treeWidget判断显示窗口
     */
    QHash<QTreeWidgetItem*,QObject*> ItemWidgetMap;

    ///
    /// \brief ImageProcessingMap 图片处理类
    ///
    QHash<int,QObject*> ImageProcessingMap;

    ///
    /// \brief LogicalProcessingMap 红外逻辑处理类
    ///
    QHash<int,QObject*> LogicalProcessingMap;

private:

    ///
    /// \brief bindingCamerObjects 绑定相机对象
    ///
    void bindingCamerObjects();

    ///
    /// \brief connetObject 初始化系统状态类
    ///
    void initSysInfo();

    ///
    /// \brief closeEvent 重新窗口关闭事件
    /// \param event
    ///
    void closeEvent(QCloseEvent *event)Q_DECL_OVERRIDE;

    ///------------------------------------------------------------------------------------------------------------MainUI
    /// \brief InitializeObject 初始化对象
    ///
    void InitializeObject();

    ///------------------------------------------------------------------------------------------------------------MainUI
    /// \brief InitializeOtherWindow 初始化其他窗口
    ///
    void InitializeOtherWindow();

    ///------------------------------------------------------------------------------------------------------------MainUI
    /// \brief InitializeDataWindow 初始化数据窗口
    ///
    void InitializeDataWindow();

    ///------------------------------------------------------------------------------------------------------------MainUI
    /// \brief InitializeCamerWindow 初始化相机窗口
    ///
    void InitializeCamerWindow();

    ///------------------------------------------------------------------------------------------------------------MainUI
    /// \brief loadPlugin 加载插件
    ///
    void loadPlugins();

    ///------------------------------------------------------------------------------------------------------------MainUI
    /// \brief ProcessingPlug 分析插件
    ///
    void processingPlugins(QDir path,int num);

    ///------------------------------------------------------------------------------------------------------------MainUI
    /// \brief camerPlugin 处理相机插件
    /// \param Camer 相机类
    ///
    void getImagePlugin(GetImagesInterface* pGetimagesInterface,int num);

    ///------------------------------------------------------------------------------------------------------------MainUI
    /// \brief serialportPlugin 处理串口插件
    /// \param SerialPort 串口类
    ///
    void serialportPlugin(InfraredlogicInterface* pInfraredlogicInterface,int num);

    ///------------------------------------------------------------------------------------------------------------MainUI
    /// \brief hideWindows 隐藏所有窗口
    ///
    void hideWindows();

    ///------------------------------------------------------------------------------------------------------------MainUI
    /// \brief setStatusBar 设置状态栏
    ///
    void setStatusBar();

private slots:

    ///------------------------------------------------------------------------------------------------------------MainUI
    /// \brief message 日志
    /// \param msg 信息体
    ///
    void message(const QString &msg);

    ///------------------------------------------------------------------------------------------------------------MainUI
    /// \brief resizeEvent 重写窗口改变大小事件
    /// \param size 窗口尺寸
    ///
    void resizeEvent(QResizeEvent *size)override;

    ///------------------------------------------------------------------------------------------------------------MainUI
    /// \brief on_treeWidget_itemActivated 导航栏选取事件
    /// \param item 选取项
    /// \param column 列
    ///
    void on_treeWidget_itemActivated(QTreeWidgetItem *item);

    ///
    /// \brief statusMsgSlot CPU,MEN状态
    /// \param msg 信息体
    ///
    void statusMsgSlot(const QString &msg);

signals:

    ///
    /// \brief closeStreamSignal 关闭视频流
    ///
    void closeStreamSignal();

    ///
    /// \brief exitWhileSignal 退出循环线程,系统状态循环和逻辑循环
    /// \param status 标志位
    ///
    void exitWhileSignal(bool status);

};

#endif // MAINWIDGET_H
