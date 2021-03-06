﻿#include "datawidget.h"
#include "ui_datawidget.h"

DataWidget::DataWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataWidget)
{
    ui->setupUi(this);

    this->setParent(parent);
    this->setHidden(true);
    this->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);

    pTimer1=new QTimer(this);
    pTimer2=new QTimer(this);
    //QTimer::singleShot(6000,this,SLOT(timeOutSendPlate()));
    pTimer1->setSingleShot(true);
    pTimer2->setSingleShot(true);
    connect(pTimer1,SIGNAL(timeout()),this,SLOT(logicStateSlot()));
    connect(pTimer2,SIGNAL(timeout()),this,SLOT(timeOutSendPlate()));

}

DataWidget::~DataWidget()
{
    if(pTimer1->isActive()){
        pTimer1->stop();
    }
    if(pTimer2->isActive()){
        pTimer2->stop();
    }

    delete ui;
}

void DataWidget::resizeEvent(QResizeEvent *size)
{
//    qDebug()<<size->size().width()-ui->Img_After_label->width()-ui->Img_RightAfter_label->width()-ui->Img_RightFront_label->width();
//    qDebug()<<size->size().height()-ui->Img_After_label->height()-ui->Img_LeftAfter_label->height();

    if(size->oldSize().height()!=-1){
        int W=(size->size().width()-10) /3;/* 10 */
        int H=(size->size().height()-69)/2;/* 69 */
        if(W>0&&H>0){
            ui->Img_After_label->setFixedSize(W,H);
            ui->Img_After_label->size().scale(W,H,Qt::IgnoreAspectRatio);
            ui->Img_LeftAfter_label->setFixedSize(W,H);
            ui->Img_LeftAfter_label->size().scale(W,H,Qt::IgnoreAspectRatio);
            ui->Img_Front_label->setFixedSize(W,H);
            ui->Img_Front_label->size().scale(W,H,Qt::IgnoreAspectRatio);
            ui->Img_RightAfter_label->setFixedSize(W,H);
            ui->Img_RightAfter_label->size().scale(W,H,Qt::IgnoreAspectRatio);
            ui->Img_LeftFront_label->setFixedSize(W,H);
            ui->Img_LeftFront_label->size().scale(W,H,Qt::IgnoreAspectRatio);
            ui->Img_RightFront_label->setFixedSize(W,H);
            ui->Img_RightFront_label->size().scale(W,H,Qt::IgnoreAspectRatio);
        }
    }
    QWidget::resizeEvent(size);
}

void DataWidget::hideEvent(QHideEvent *event)
{
    //ui->tabWidget->setTabText(1,tr("Result"));
}

void DataWidget::setDataWidgetPar(int channelNum)
{
    this->channelNum=channelNum;
}

void DataWidget::logicStatusSlot(int *status)
{
    /* A1.A2.D1.B1.B2.D2 */
    ui->checkBox_2->setChecked(status[0]);
    ui->checkBox_3->setChecked(status[1]);
    ui->checkBox_4->setChecked(status[3]);
    ui->checkBox_5->setChecked(status[4]);
    ui->checkBox_6->setChecked(status[2]);
    ui->checkBox_7->setChecked(status[5]);

    logicList.clear();

    for(int i=0;i<5;i++){
        if(i==2){
            continue;
        }
        logicList<<status[i];
    }
}

void DataWidget::logicStateSlot()
{
    isConCar=false;

    /*****************************
    * @brief:记录红外状态
    ******************************/
    QList<int> tmpList;
    tmpList<<ui->checkBox_2->checkState()<<ui->checkBox_3->checkState()<<ui->checkBox_4->checkState()<<ui->checkBox_5->checkState();
    for (int var = 0; var < logicList.count(); ++var) {
        if(logicList[var]!=tmpList[var]){
            isConCar=true;
        }
    }

    logicList.clear();
    tmpList.clear();

    /*****************************
    * @brief: 不是集卡车通过，直接写入数据库。
    ******************************/
    if(!isConCar && ui->lineEdit_13->text()!=""){

        QMap<QString ,QString> data;

        data.insert("Channel",QString::number(channelNum));
        data.insert("PlateTimer",QString("%1").arg(QDateTime::fromString(ui->lineEdit_13->text(),"yyyy-M-d h:m:s").toString("yyyy-MM-dd hh:mm:ss")));
        data.insert("Plate",ui->lineEdit_11->text());
        data.insert("PlateImg",QString("%1%2%3.jpg").arg(QDateTime::fromString(ui->lineEdit_13->text(),"yyyy-M-d h:m:s").toString("yyyyMMddhhmmss")).arg(7).arg(channelNum));

        emit insertDataBaseSignal(data);
        data.clear();

        int color=1;
        if(ui->lineEdit_12->text()=="蓝"){
            color=0;
        }

        /*****************************
        * @brief:发送车牌数据
        ******************************/
        emit sendResultSignal(channelNum,QString("[U|%1|%2|%3]").arg(QDateTime::fromString(ui->lineEdit_13->text(),"yyyy-M-d h:m:s").toString("yyyyMMddhhmmss")).arg(channelNum,2,10,QLatin1Char('0')).arg(ui->lineEdit_11->text()));

        ui->lineEdit_11->clear();
        ui->lineEdit_12->clear();
        ui->lineEdit_13->clear();
    }

    if(isConCar && ui->lineEdit_13->text()!=""){
        if(pTimer2->isActive()){
            pTimer2->stop();
        }
        pTimer2->start(15000);
    }
}

void DataWidget::timeOutSendPlate()
{
    /*****************************
    * @brief:超时没有收到集装箱数据，直接发送车牌
    ******************************/
    if(isConCar && ui->lineEdit_13->text()!=""){
        QMap<QString ,QString> data;

        data.insert("Channel",QString::number(channelNum));
        data.insert("PlateTimer",QString("%1").arg(QDateTime::fromString(ui->lineEdit_13->text(),"yyyy-M-d h:m:s").toString("yyyy-MM-dd hh:mm:ss")));
        data.insert("Plate",ui->lineEdit_11->text());
        data.insert("PlateImg",QString("%1%2%3.jpg").arg(QDateTime::fromString(ui->lineEdit_13->text(),"yyyy-M-d h:m:s").toString("yyyyMMddhhmmss")).arg(7).arg(channelNum));

        emit insertDataBaseSignal(data);
        data.clear();

        /*****************************
        * @brief:发送车牌数据
        ******************************/

        int color=1;
        if(ui->lineEdit_12->text()=="蓝"){
            color=0;
        }

        emit sendResultSignal(channelNum,QString("[U|%1|%2|%3]").arg(QDateTime::fromString(ui->lineEdit_13->text(),"yyyy-M-d h:m:s").toString("yyyyMMddhhmmss")).arg(channelNum,2,10,QLatin1Char('0')).arg(ui->lineEdit_11->text()));

        ui->lineEdit_11->clear();
        ui->lineEdit_12->clear();
        ui->lineEdit_13->clear();
    }
}

void DataWidget::pictureStreamSlot(const QByteArray &jpgStream, const int &imgNumber)
{   
    if(imgNumber==0){
        return;
    }
    QMutexLocker locker(&mutex);
    QPixmap *labelPix = new QPixmap();
    QPixmap  labelPixFit;
    if(jpgStream!=nullptr){
        labelPix->loadFromData(jpgStream);
        /* 防止图片发生偏移 */
        //labelPixFit=labelPix->scaled((ui->tab->width()-4)/3-4,(ui->tab->height()-36)/2-4,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    }
    else{
        delete labelPix;
        labelPix=nullptr;
    }

    switch (imgNumber) {
    case -1:
        /* 清除图片 */
        ui->Img_Front_label->clear();
        ui->Img_LeftFront_label->clear();
        ui->Img_RightFront_label->clear();
        ui->Img_LeftAfter_label->clear();
        ui->Img_RightAfter_label->clear();
        ui->Img_After_label->clear();

        /* 清除箱号结果 */
        ui->lineEdit->clear();
        ui->lineEdit_2->clear();
        ui->lineEdit_3->clear();
        ui->lineEdit_4->clear();

        ui->lineEdit_5->clear();
        ui->lineEdit_9->clear();
        ui->lineEdit_7->clear();
        ui->lineEdit_8->clear();

//        ui->lineEdit_10->clear();
//        ui->lineEdit_11->clear();
//        ui->lineEdit_12->clear();
//        ui->lineEdit_13->clear();

        ui->Infrared_logic_lineEdit->clear();


        ui->lineEdit->setStyleSheet("background-color: rgb(255, 255, 255);color: rgb(0, 170, 0);");
        ui->lineEdit_2->setStyleSheet("background-color: rgb(255, 255, 255);color: rgb(0, 170, 0);");
        ui->lineEdit_3->setStyleSheet("background-color: rgb(255, 255, 255);color: rgb(0, 170, 0);");
        ui->lineEdit_4->setStyleSheet("background-color: rgb(255, 255, 255);color: rgb(0, 170, 0);");

        ui->lineEdit_5->setStyleSheet("background-color: rgb(255, 255, 255);color: rgb(0, 170, 0);");
        ui->lineEdit_7->setStyleSheet("background-color: rgb(255, 255, 255);color: rgb(0, 170, 0);");
        ui->lineEdit_9->setStyleSheet("background-color: rgb(255, 255, 255);color: rgb(0, 170, 0);");
        ui->lineEdit_8->setStyleSheet("background-color: rgb(255, 255, 255);color: rgb(0, 170, 0);");

        //ui->tabWidget->setTabText(1,tr("Result"));
        //ui->tab->setStyleSheet("color: #2c2c2c;");

        break;

    case 1:
        if(labelPix!=nullptr){
            labelPixFit=labelPix->scaled(ui->Img_Front_label->width(),ui->Img_Front_label->height(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
            ui->Img_Front_label->setPixmap(labelPixFit);
        }
        break;
    case 2:
        if(labelPix!=nullptr){
            labelPixFit=labelPix->scaled(ui->Img_RightFront_label->width(),ui->Img_RightFront_label->height(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
            ui->Img_RightFront_label->setPixmap(labelPixFit);
        }
        break;
    case 3:
        if(labelPix!=nullptr){
            labelPixFit=labelPix->scaled(ui->Img_LeftFront_label->width(),ui->Img_LeftFront_label->height(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
            ui->Img_LeftFront_label->setPixmap(labelPixFit);
        }
        break;
    case 4:
        if(labelPix!=nullptr){
            labelPixFit=labelPix->scaled(ui->Img_RightAfter_label->width(),ui->Img_RightAfter_label->height(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
            ui->Img_RightAfter_label->setPixmap(labelPixFit);
        }
        break;
    case 5:
        if(labelPix!=nullptr){
            labelPixFit=labelPix->scaled(ui->Img_LeftAfter_label->width(),ui->Img_LeftAfter_label->height(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
            ui->Img_LeftAfter_label->setPixmap(labelPixFit);
        }
        break;
    case 6:
        if(labelPix!=nullptr){
            labelPixFit=labelPix->scaled(ui->Img_After_label->width(),ui->Img_After_label->height(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
            ui->Img_After_label->setPixmap(labelPixFit);
        }
        break;
    }

    if(labelPix!=nullptr){
        delete labelPix;
        labelPix=nullptr;
    }
}

void DataWidget::containerSlot(const int& type,const QString &result1,const int& resultCheck1,const QString &iso1,const QString &result2,const int& resultCheck2,const QString &iso2)
{

    /*****************************
    * @brief: 停止等待箱号超时
    ******************************/
    if(pTimer1->isActive()){
        pTimer1->stop();
    }
    if(pTimer2->isActive()){
        pTimer2->stop();
    }

    /* Tupe,集装箱类别:
     * -1 – 未知
     * 0 – 一个 20 集装箱
     * 1 – 一个 40 吋/45 吋集装箱
     * 2 – 两个 20 吋集装箱
     */

    /*****************************
    * @brief: 前箱结果后期附加时间戳,通道号
    ******************************/
    QStringList tmp=result1.split('@');


    if(tmp.count()==3){
        dateTime=tmp[1];
        //channelNum=tmp[2].toInt();
    }

    ui->lineEdit->setText(tmp[0]);
    ui->lineEdit_2->setText(iso1);
    ui->lineEdit_3->setText(result2);
    ui->lineEdit_4->setText(iso2);

    ui->lineEdit_5->setText(tmp[0]);
    ui->lineEdit_7->setText(iso1);
    ui->lineEdit_9->setText(result2);
    ui->lineEdit_8->setText(iso2);

    if(resultCheck1){
        ui->lineEdit->setStyleSheet("background-color: rgb(0, 170, 0);color: rgb(255, 255, 255);");
        ui->lineEdit_5->setStyleSheet("background-color: rgb(0, 170, 0);color: rgb(255, 255, 255);");
    }
    else {
        ui->lineEdit->setStyleSheet("background-color: rgb(255, 0, 0);color: rgb(255, 255, 255);");
        ui->lineEdit_5->setStyleSheet("background-color: rgb(255, 0, 0);color: rgb(255, 255, 255);");
    }
    if(type==2){
        if(resultCheck2){
            ui->lineEdit_3->setStyleSheet("background-color: rgb(0, 170, 0);color: rgb(255, 255, 255);");
            ui->lineEdit_9->setStyleSheet("background-color: rgb(0, 170, 0);color: rgb(255, 255, 255);");
        }
        else {
            ui->lineEdit_3->setStyleSheet("background-color: rgb(255, 0, 0);color: rgb(255, 255, 255);");
            ui->lineEdit_9->setStyleSheet("background-color: rgb(255, 0, 0);color: rgb(255, 255, 255);");
        }
    }

    QString logic="";
    ui->Infrared_logic_lineEdit->setStyleSheet("background-color: rgb(0, 170, 0);color: rgb(255, 255, 255);");

    switch (type) {
    case -1:
        logic=tr("No container");
        break;
    case 0:
        logic=tr("A small container");
        break;
    case 1:
        logic=tr("A large container");
        break;
    case 2:
        logic=tr("Two small containersr");
        break;
    }
    ui->Infrared_logic_lineEdit->setText(logic);

    /*****************************
    * @brief:更新车牌
    ******************************/
    if(ui->lineEdit_13->text()!=""){
        QMap<QString ,QString> data;

        data.insert("Timer",dateTime);
        data.insert("Channel",QString::number(channelNum));
        data.insert("PlateTimer",QString("%1").arg(QDateTime::fromString(ui->lineEdit_13->text(),"yyyy-M-d h:m:s").toString("yyyy-MM-dd hh:mm:ss")));
        data.insert("Plate",ui->lineEdit_11->text());
        data.insert("PlateImg",QString("%1%2%3.jpg").arg(QDateTime::fromString(ui->lineEdit_13->text(),"yyyy-M-d h:m:s").toString("yyyyMMddhhmmss")).arg(7).arg(channelNum));

        emit updateDataBaseSignal(data);
        data.clear();

        int color=1;
        if(ui->lineEdit_12->text()=="蓝"){
            color=0;
        }

        //----------------------------------------------
        /*****************************
        * @brief:发送车牌数据
        ******************************/
        //emit sendResultSignal(channelNum,QString("[U|%1|%2|%3|%4]").arg(QDateTime::fromString(ui->lineEdit_13->text(),"yyyy-M-d h:m:s").toString("yyyyMMddhhmmss")).arg(channelNum).arg(ui->lineEdit_10->text()).arg(color));
        //emit sendResultSignal(channelNum,QString("[U|%1|%2|%3|%4]").arg(QDateTime::fromString(dateTime,"yyyy-MM-dd hh:mm:ss").toString("yyyyMMddhhmmss")).arg(channelNum,2,10,QLatin1Char('0')).arg(ui->lineEdit_11->text()).arg(color));


//        QString time=QString("%1").arg(QDateTime::fromString(dateTime,"yyyy-MM-dd hh:mm:ss").toString("yyyyMMddhhmmss"));
//        if(type==2){
//            /* 识别结果写入日志,[标志|时间戳|通道号(2位)|逻辑|箱号|校验|箱号|校验|箱型|箱型|车牌|颜色] *//* 颜色暂时不需要 */
//            QString result=QString("[%1|%2|%3|%4|%5|%6|%7|%8|%9|%10|%11]").arg("C").arg(time).arg(channelNum,2,10,QLatin1Char('0')).arg(type).arg(tmp[0]).arg(resultCheck1?"Y":"N").arg(result2).arg(resultCheck2?"Y":"N").arg(iso1).arg(iso2).arg(ui->lineEdit_11->text());
//            emit sendResultSignal(channelNum,result);
//        }
//        else {
//            /* 识别结果写入日志,[标志|时间戳|通道号(2位)|逻辑|箱号|校验|箱型|车牌|颜色]*//* 颜色暂时不需要 */
//            QString result=QString("[%1|%2|%3|%4|%5|%6|%7|%8]").arg("C").arg(time).arg(channelNum,2,10,QLatin1Char('0')).arg(type).arg(tmp[0]).arg(resultCheck1?"Y":"N").arg(iso1).arg(ui->lineEdit_11->text());
//            emit sendResultSignal(channelNum,result);
//        }


        //---------------------------------------------

        ui->lineEdit_11->clear();
        ui->lineEdit_12->clear();
        ui->lineEdit_13->clear();

    }
//    else {
//        QString time=QString("%1").arg(QDateTime::fromString(dateTime,"yyyy-MM-dd hh:mm:ss").toString("yyyyMMddhhmmss"));
//        if(type==2){
//            /* 识别结果写入日志,[标志|时间戳|通道号(2位)|逻辑|箱号|校验|箱号|校验|箱型|箱型|] */
//            QString result=QString("[%1|%2|%3|%4|%5|%6|%7|%8|%9|%10|%11]").arg("C").arg(time).arg(channelNum,2,10,QLatin1Char('0')).arg(type).arg(tmp[0]).arg(resultCheck1?"Y":"N").arg(result2).arg(resultCheck2?"Y":"N").arg(iso1).arg(iso2);
//            emit sendResultSignal(channelNum,result);
//        }
//        else {
//            /* 识别结果写入日志,[标志|时间戳|通道号(2位)|逻辑|箱号|校验|箱型]*/
//            QString result=QString("[%1|%2|%3|%4|%5|%6|%7|%8]").arg("C").arg(time).arg(channelNum,2,10,QLatin1Char('0')).arg(type).arg(tmp[0]).arg(resultCheck1?"Y":"N").arg(iso1);
//            emit sendResultSignal(channelNum,result);
//        }
//    }
}

void DataWidget::camerIDstatesSlot(const QString &camerIP, bool state, const QString &alisa)
{
    if(alisa=="Front"){
        if(state){
            ui->Front_Camera_checkBox->setChecked(true);
        }
        else {
            ui->Front_Camera_checkBox->setChecked(false);
        }
    }
    else if(alisa=="After"){
        if(state){
            ui->After_Camera_checkBox->setChecked(true);
        }
        else {
            ui->After_Camera_checkBox->setChecked(false);
        }
    }
    else if(alisa=="Left"){
        if(state){
            ui->Left_Camera_checkBox->setChecked(true);
        }
        else {
            ui->Left_Camera_checkBox->setChecked(false);
        }
    }
    else if(alisa=="Right"){
        if(state){
            ui->Right_Camera_checkBox->setChecked(true);
        }
        else {
            ui->Right_Camera_checkBox->setChecked(false);
        }
    }
}

void DataWidget::imageFlowSlot(QByteArray img)
{
    QPixmap pix;
    pix.loadFromData(img);
    ui->label_4->setPixmap(pix);

    img.clear();
}

void DataWidget::theVideoStreamSlot(QByteArray arrImg)
{    
    if(openStream){
        QPixmap pix;
        pix.loadFromData(arrImg);
        ui->label_3->setPixmap(pix);
    }
    arrImg.clear();
}

void DataWidget::resultsTheLicensePlateSlot(const QString &plate, const QString &color, const QString &time, QByteArray arrImg)
{     
    ui->label_3->setText(plate+color+time);
    if(ui->lineEdit_13->text()!=""){
        QMap<QString ,QString> data;

        data.insert("Channel",QString::number(channelNum));
        data.insert("PlateTimer",QString("%1").arg(QDateTime::fromString(ui->lineEdit_13->text(),"yyyy-M-d h:m:s").toString("yyyy-MM-dd hh:mm:ss")));
        data.insert("Plate",ui->lineEdit_11->text());
        data.insert("PlateImg",QString("%1%2%3.jpg").arg(QDateTime::fromString(ui->lineEdit_13->text(),"yyyy-M-d h:m:s").toString("yyyyMMddhhmmss")).arg(7).arg(channelNum));

        emit insertDataBaseSignal(data);
        data.clear();

        /*****************************
        * @brief:发送车牌数据
        ******************************/

        int color=1;
        if(ui->lineEdit_12->text()=="蓝"){
            color=0;
        }

        emit sendResultSignal(channelNum,QString("[U|%1|%2|%3]").arg(QDateTime::fromString(ui->lineEdit_13->text(),"yyyy-M-d h:m:s").toString("yyyyMMddhhmmss")).arg(channelNum,2,10,QLatin1Char('0')).arg(ui->lineEdit_11->text()));

        ui->lineEdit_11->clear();
        ui->lineEdit_12->clear();
        ui->lineEdit_13->clear();
    }

    /*****************************
    * @brief:记录红外状态
    ******************************/
    logicList.clear();
    logicList<<ui->checkBox_2->checkState()<<ui->checkBox_3->checkState()<<ui->checkBox_4->checkState()<<ui->checkBox_5->checkState();

    ui->lineEdit_6->setText(plate);
    ui->lineEdit_11->setText(plate);
    ui->lineEdit_10->setText(plate);
    ui->lineEdit_12->setText(color);
    ui->lineEdit_13->setText(time);

    QPixmap pix;
    if(arrImg!=nullptr){
        pix.loadFromData(arrImg);
        ui->label_4->setPixmap(pix);
    }

    arrImg.clear();

    if(color=="黄" || color==""){
        pTimer1->start(10000);
    }
    else {
        isConCar=false;
        pTimer1->start(1);
    }
    //QTimer::singleShot(5000,this,SLOT(logicStateSlot()));
}

void DataWidget::equipmentStateSlot(bool state)
{
    ui->Plate_Camera_checkBox->setChecked(state);
}


void DataWidget::on_while_cycle_capture_checkBox_stateChanged(int arg1)
{
    if(arg1==Qt::Checked){
        ui->Capture_pushButton->setEnabled(false);
    }
    else {
        ui->Capture_pushButton->setEnabled(true);
    }
    emit simulateTriggerSignal(5);
}

void DataWidget::on_Capture_pushButton_clicked()
{
    switch (ui->Simulatiion_comboBox->currentIndex()) {
    case 0:
        emit simulateTriggerSignal(1);
        break;
    case 1:
        emit simulateTriggerSignal(2);
        break;
    case 2:
        emit simulateTriggerSignal(3);
        break;
    case 3:
        emit simulateTriggerSignal(4);
        break;
    case 4:
        emit simulateTriggerSignal(0);
        break;
    }
}

void DataWidget::on_pushButton_clicked()
{
    emit simulationCaptureSignal();
}

void DataWidget::on_pushButton_2_clicked(bool checked)
{
    //emit openTheVideoSignal(checked);
    openStream=checked;
    emit openTheVideoSignal(openStream,ui->label_3->winId());
}
