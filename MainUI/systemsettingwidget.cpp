#include "systemsettingwidget.h"
#include "ui_systemsettingwidget.h"

SystemSettingWidget::SystemSettingWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SystemSettingWidget)
{
    ui->setupUi(this);

    pSettingValues=new SettingValues () ;

    this->setParent(parent);
    this->setHidden(true);
    this->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);

    /* 加载配置 */
    if(!jsonRead()){
        if(jsonWrite()){
            jsonRead();
        }
    }
    else {/* 外部更改配置,回写配置到UI */
        jsonWritetoUI();
    }

    if(pSettingValues->SaveImageOne){
        createImgPath(pSettingValues->ImgPathOne,pSettingValues->ImageFormatOne);
    }
    if(pSettingValues->SaveImageTow){
        createImgPath(pSettingValues->ImgPathTow,pSettingValues->ImageFormatTow);
    }
}

SystemSettingWidget::~SystemSettingWidget()
{
    delete  pSettingValues;
    delete ui;
}

void SystemSettingWidget::createImgPath(const QString &path,int currentindex)
{
    QDir  dir(QDir::toNativeSeparators(path));
    for(int i=1;i<=pSettingValues->ChannelNumber;i++){
        switch (currentindex) {
        case 0:
            dir.mkpath(QDir::toNativeSeparators(tr("%1/%2").arg(i).arg(QDateTime::currentDateTime().toString("yyyy/MM/dd"))));
            break;
        case 1:
            dir.mkpath(QDir::toNativeSeparators(tr("%1/%2").arg(i).arg(QDateTime::currentDateTime().toString("yyyy/MM"))));
            break;
        case 2:
            dir.mkpath(QDir::toNativeSeparators(tr("%1/%2").arg(i).arg(QDateTime::currentDateTime().toString("yyyy"))));
            break;
        case 3:
            dir.mkpath(QDir::toNativeSeparators(tr("%1").arg(i)));
            break;
        case 4:
            dir.mkpath(QDir::toNativeSeparators(tr("%1").arg(QDateTime::currentDateTime().toString("yyyy/MM/dd"))));
            break;
        case 5:
            dir.mkpath(QDir::toNativeSeparators(tr("%1").arg(QDateTime::currentDateTime().toString("yyyy/MM"))));
            break;
        case 6:
            dir.mkpath(QDir::toNativeSeparators(tr("%1").arg(QDateTime::currentDateTime().toString("yyyy"))));
            break;
        case 7:
            dir.mkpath(QDir::toNativeSeparators(path));
            break;
        }
    }
}

bool SystemSettingWidget::jsonWrite()
{
    /* 创建通道配置文件夹 */
    QDir mkPath(QCoreApplication::applicationDirPath());
    mkPath.mkdir("Json");
    mkPath.cd("Json");

    QFile file(QDir::toNativeSeparators(tr("%1/%2").arg(mkPath.path()).arg("SYSTEM.json")));

    if(!file.open(QIODevice::ReadWrite)){
        emit messageSignal(ZBY_LOG("ERROR"),tr("Failed to load the parameter, create the default parameter error<errorCode=%1>").arg(file.OpenError));
        return false;
    }

    QJsonDocument jsonDoc;
    QJsonObject jsonChild,jsonObjRoot;

    QJsonObject jsonObj;
    jsonObj.insert(tr("ChannelNumber"),ui->ChannelNumber->value());
    jsonObj.insert(tr("SaveImageOne"),int(ui->SaveImage_1->isChecked()));
    jsonObj.insert(tr("SaveImageTow"), int(ui->SaveImage_2->isChecked()));
    jsonObj.insert(tr("ImageFormatOne"),ui->ImageFormat_1->currentIndex());
    jsonObj.insert(tr("ImageFormatTow"),ui->ImageFormat_2->currentIndex());
    jsonObj.insert(tr("ImgPathOne"),ui->ImgPathlineEdit_1->text().toLocal8Bit().data());
    jsonObj.insert(tr("ImgPathTow"),ui->ImgPathlineEdit_2->text().toLocal8Bit().data());
    jsonChild.insert("Channel",QJsonValue(jsonObj));

    QJsonObject jsonObj1;
    jsonObj1.insert(tr("ColorDisplay"),int(ui->ColorDisplay->isChecked()));
    jsonObj1.insert(tr("AutomaticCorrection"),int(ui->AutomaticCorrection->isChecked()));
    jsonObj1.insert(tr("Server"),int(ui->Server->isChecked()));
    jsonObj1.insert(tr("ServerIP"),ui->ServerIP->text());
    jsonObj1.insert(tr("ServerPort"),ui->ServerPort->text());
    jsonObj1.insert(tr("Client"),int(ui->Client->isChecked()));
    jsonObj1.insert(tr("ClientIP"),ui->ClientIP->text());
    jsonObj1.insert(tr("ClientPort"),ui->ClientPort->text().toInt());
    jsonChild.insert("Recognizer",QJsonValue(jsonObj1));

    QJsonObject jsonObj2;
    jsonObj2.insert(tr("ProtocolVersion"),ui->ProtocolV->currentIndex());
    jsonObj2.insert(tr("CameraVersion"),ui->CameraV->currentIndex());
    jsonChild.insert("Agreement",QJsonValue(jsonObj2));

    QJsonObject jsonObj3;
    jsonObj3.insert(tr("FtpAddress"),ui->FtpAddress->text());
    jsonObj3.insert(tr("FtpPort"),ui->FtpPort->text());
    jsonObj3.insert(tr("FtpUser"), ui->FtpUser->text());
    jsonObj3.insert(tr("FtpPassword"),ui->FtpPassword->text());
    jsonObj3.insert(tr("FtpLocalImgPath"),ui->FtpLocalImgPath->text());
    jsonObj3.insert(tr("FtpRemoteImgPath"),ui->FtpRemoteImgPath->text());
    jsonChild.insert("FTP",QJsonValue(jsonObj3));

    QJsonObject jsonObj4;
    jsonObj4.insert(tr("Minimization"),int(ui->Minimization->isChecked()));
    //jsonObj4.insert(tr("SaveLog"),int(ui->SaveLog->isChecked()));
    jsonObj4.insert(tr("Language"), ui->Language->currentIndex());
    jsonChild.insert("Other",QJsonValue(jsonObj4));

    jsonObjRoot.insert("MAIN",QJsonValue(jsonChild));
    jsonDoc.setObject(jsonObjRoot);
    file.write(jsonDoc.toJson());
    file.close();

    return true;
}

bool SystemSettingWidget::jsonRead()
{
    QFile file(QDir::toNativeSeparators(tr("%1/%2/SYSTEM.json").arg(QCoreApplication::applicationDirPath()).arg("Json")));
    if(!file.open(QIODevice::ReadOnly)){
        emit messageSignal(ZBY_LOG("ERROR"),tr("Failed to load the parameter, create the default parameter error<errorCode=%1>").arg(file.OpenError));
        return false;
    }

    QByteArray arr=file.readAll();
    QJsonParseError jsonError;
    QJsonDocument jsonDoc=QJsonDocument::fromJson(arr,&jsonError);

    /* 加载文件 */
    if(!jsonDoc.isNull()&&jsonError.error==QJsonParseError::NoError){
        /* 读取根目录 */
        if(jsonDoc.isObject()){
            QJsonObject obj=jsonDoc.object();
            if(obj.contains(tr("MAIN"))){
                QJsonValue value=obj.value(tr("MAIN"));

                /* 读取子目录 */
                if(value.isObject()){
                    pSettingValues->CameraVersion= getJsonValue("Agreement","CameraVersion",value.toObject()).toInt();
                    pSettingValues->ProtocolVersion=getJsonValue("Agreement","ProtocolVersion",value.toObject()).toInt();

                    pSettingValues->ChannelNumber= getJsonValue("Channel","ChannelNumber",value.toObject()).toInt();
                    pSettingValues->ImageFormatOne= getJsonValue("Channel","ImageFormatOne",value.toObject()).toInt();
                    pSettingValues->ImageFormatTow= getJsonValue("Channel","ImageFormatTow",value.toObject()).toInt();
                    pSettingValues->SaveImageOne= getJsonValue("Channel","SaveImageOne",value.toObject()).toInt();
                    pSettingValues->SaveImageTow= getJsonValue("Channel","SaveImageTow",value.toObject()).toInt();
                    pSettingValues->ImgPathOne= getJsonValue("Channel","ImgPathOne",value.toObject()).toString();
                    pSettingValues->ImgPathTow= getJsonValue("Channel","ImgPathTow",value.toObject()).toString();

                    pSettingValues->FtpAddress= getJsonValue("FTP","FtpAddress",value.toObject()).toString();
                    pSettingValues->FtpLocalImgPath= getJsonValue("FTP","FtpLocalImgPath",value.toObject()).toString();
                    pSettingValues->FtpPassword= getJsonValue("FTP","FtpPassword",value.toObject()).toString();
                    pSettingValues->FtpPort= getJsonValue("FTP","FtpPort",value.toObject()).toString();
                    pSettingValues->FtpRemoteImgPath= getJsonValue("FTP","FtpRemoteImgPath",value.toObject()).toString();
                    pSettingValues->FtpUser= getJsonValue("FTP","FtpUser",value.toObject()).toString();

                    pSettingValues->Language= getJsonValue("Other","Language",value.toObject()).toInt();
                    pSettingValues->Minimization= getJsonValue("Other","Minimization",value.toObject()).toInt();
                    pSettingValues->SaveLog= getJsonValue("Other","SaveLog",value.toObject()).toInt();

                    pSettingValues->AutomaticCorrection= getJsonValue("Recognizer","AutomaticCorrection",value.toObject()).toInt();
                    pSettingValues->Client= getJsonValue("Recognizer","Client",value.toObject()).toInt();
                    pSettingValues->ClientIP= getJsonValue("Recognizer","ClientIP",value.toObject()).toString();
                    pSettingValues->ClientPort= getJsonValue("Recognizer","ClientPort",value.toObject()).toInt();
                    pSettingValues->Server= getJsonValue("Recognizer","Server",value.toObject()).toInt();
                    pSettingValues->ServerIP= getJsonValue("Recognizer","ServerIP",value.toObject()).toString();
                    pSettingValues->ServerPort= getJsonValue("Recognizer","ServerPort",value.toObject()).toInt();
                    pSettingValues->ColorDisplay= getJsonValue("Recognizer","ColorDisplay",value.toObject()).toInt();

                    return true;
                    }
                }
            }
        }
    else {
        emit messageSignal(ZBY_LOG("ERROR"),tr("load SYSTEM.json error<errorCode=%1>").arg(jsonError.errorString()));
    }
    return false;
}

void SystemSettingWidget::jsonWritetoUI()
{
    ui->ChannelNumber->setValue(pSettingValues->ChannelNumber);
    ui->SaveImage_1->setChecked(pSettingValues->SaveImageOne);
    ui->SaveImage_2->setChecked(pSettingValues->SaveImageTow);
    ui->ImageFormat_1->setCurrentIndex(pSettingValues->ImageFormatOne);
    ui->ImageFormat_2->setCurrentIndex(pSettingValues->ImageFormatTow);
    ui->ImgPathlineEdit_1->setText(pSettingValues->ImgPathOne);
    ui->ImgPathlineEdit_2->setText(pSettingValues->ImgPathTow);

    ui->ColorDisplay->setChecked(pSettingValues->ColorDisplay);
    ui->AutomaticCorrection->setChecked(pSettingValues->AutomaticCorrection);
    ui->Server->setChecked(pSettingValues->Server);
    ui->ServerIP->setText(pSettingValues->ServerIP);
    ui->ServerPort->setText(QString::number(pSettingValues->ServerPort));
    ui->Client->setChecked(pSettingValues->Client);
    ui->ClientIP->setText(pSettingValues->ClientIP);
    ui->ClientPort->setText(QString::number(pSettingValues->ClientPort));

    ui->ProtocolV->setCurrentIndex(pSettingValues->ProtocolVersion);
    ui->CameraV->setCurrentIndex(pSettingValues->CameraVersion);

    ui->FtpAddress->setText(pSettingValues->FtpAddress);
    ui->FtpPort->setText(pSettingValues->FtpPort);
    ui->FtpUser->setText(pSettingValues->FtpUser);
    ui->FtpPassword->setText(pSettingValues->FtpPassword);
    ui->FtpLocalImgPath->setText(pSettingValues->FtpLocalImgPath);
    ui->FtpRemoteImgPath->setText(pSettingValues->FtpRemoteImgPath);

    ui->Minimization->setChecked(pSettingValues->Minimization);
    //ui->SaveLog->setChecked(SaveLog);
    ui->Language->setCurrentIndex(pSettingValues->Language);
}

QVariant SystemSettingWidget::getJsonValue(const QString &child, const QString &key, QJsonObject obj)
{
    if(obj.contains(child)){
        QJsonValue jsonValue=obj.value(child);       
        /* 读取配置子项 */
        if(jsonValue.isObject()){
            obj=jsonValue.toObject();
            /* 读取配置值 */
            if(obj.contains(key)){
                jsonValue=obj.value(key);
                if(jsonValue.isString()){
                    return jsonValue.toString();
                }
                else {
                    return jsonValue.toInt();
                }
            }
        }
    }
    emit messageSignal(ZBY_LOG("ERROR"),tr("load SYSTEM.json value error:%1-%2").arg(child).arg(key));
    return  "NULL";
}

void SystemSettingWidget::on_buttonBox_clicked(QAbstractButton *button)
{
    if(button==ui->buttonBox->button(QDialogButtonBox::Save)){
        if(jsonWrite()){
            emit messageSignal(ZBY_LOG("INFO"),tr("Save System Json sucess"));
        }
        else {
            emit messageSignal(ZBY_LOG("ERROR"),tr("Save System Json error"));
        }
    }
    if(button==ui->buttonBox->button(QDialogButtonBox::Discard)){
        emit messageSignal(ZBY_LOG("INFO"),tr("Not Save System Json"));
    }
}

void SystemSettingWidget::on_CheckPathPushButton_1_clicked()
{
    QString path=QFileDialog::getExistingDirectory(this);
    ui->ImgPathlineEdit_1->setText(tr("%1").arg(path).toLocal8Bit());
}

void SystemSettingWidget::on_CheckPathPushButton_2_clicked()
{
    QString path=QFileDialog::getExistingDirectory(this);
    ui->ImgPathlineEdit_2->setText(tr("%1").arg(path).toLocal8Bit());
}