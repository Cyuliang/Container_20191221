#ifndef DATAWIDGET_H
#define DATAWIDGET_H

#include <QWidget>
#include <QMutex>
#include <QMutexLocker>
#include <QResizeEvent>
#include <QHideEvent>
#include <QPainter>
#include <QtDebug>

namespace Ui {
class DataWidget;
}

class DataWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DataWidget(QWidget *parent = nullptr);
    ~DataWidget()Q_DECL_OVERRIDE;

    ///
    /// \brief resizeEvent 重写窗口调整事件
    /// \param size
    ///
    void resizeEvent(QResizeEvent* size)Q_DECL_OVERRIDE;

    ///
    /// \brief hideEvent 重写窗口隐藏事件
    /// \param event
    ///
    void hideEvent(QHideEvent *event)Q_DECL_OVERRIDE;

private:

    Ui::DataWidget *ui;

    QMutex mutex;

public slots:

    ///
    /// \brief logicStatusSlot 红外状态
    /// \param status 状态值
    ///
    void logicStatusSlot(int* status);

    ///
    /// \brief pictureStreamSlot 图片流数据
    /// \param jpgStream 图片流
    /// \param imgNumber 编号
    ///
    void pictureStreamSlot(const QByteArray &jpgStream,const int &imgNumber);

    ///
    /// \brief containerSlot 箱号结果
    /// \param result1 前箱
    /// \param iso1 箱型
    /// \param result2 后箱
    /// \param iso2 箱型
    ///
    void containerSlot(const int& type,const QString &result1, const int &resultCheck1, const QString &iso1, const QString &result2, const int &resultCheck2, const QString &iso2);

signals:

    ///
    /// \brief simulateTriggerSignal 模拟抓拍
    /// \param type 逻辑类型
    ///
    void simulateTriggerSignal(int type);

private slots:

    void on_test_22_pushButton_clicked();
    void on_test_45_pushButton_clicked();
    void on_test_double_22_front_pushButton_clicked();
    void on_test_double_22_before_pushButton_clicked();
    void on_while_cycle_capture_checkBox_stateChanged(int arg1);
    void on_clearn_image_pushButton_clicked();
};

#endif // DATAWIDGET_H
