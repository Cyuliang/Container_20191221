#include "captureimages.h"

#if defined(Q_OS_WIN32)
    #define OS 0
#elif defined(Q_OS_LINUX)
    #define OS 1
#endif

CaptureImages* CaptureImages::pThis=nullptr;

CaptureImages::CaptureImages(QObject *parent)
{
    this->setParent(parent);

    CaptureImages::pThis=this;

    /* 登录ID,登录状态,视频流状态 */
    lUserID=-1;dwResult=0;streamID=-1;

    if(OS){
        pDLLHCNet= new QLibrary("./plugins/HCNetSDK/libhcnetsdk.so");
    }
    else {
        pDLLHCNet = new QLibrary("./plugins/HCNetSDK/libhcnetsdk.dll");
    }
}

CaptureImages::~CaptureImages()
{
    //delete  CaptureImages::pThis;
    pDLLHCNet->unload();
    delete pDLLHCNet;
}

void CaptureImages::initCamerSlot(const QString &camerIP, quint16 camerPort,const QString &CamerUser,const QString &CamerPow)
{
    this->ip=camerIP;
    this->port=camerPort;
    this->name=CamerUser;
    this->pow=CamerPow;

    if(pDLLHCNet->load()){
        NET_DVR_SetExceptionCallBack_V30_L=reinterpret_cast<NET_DVR_SetExceptionCallBack_V30FUN>(pDLLHCNet->resolve("NET_DVR_SetExceptionCallBack_V30"));
        NET_DVR_SetSDKInitCfg_L=reinterpret_cast<NET_DVR_SetSDKInitCfgFUN>(pDLLHCNet->resolve("NET_DVR_SetSDKInitCfg"));
        NET_DVR_Cleanup_L=reinterpret_cast<NET_DVR_CleanupFUN>(pDLLHCNet->resolve("NET_DVR_Cleanup"));
        NET_DVR_Init_L=reinterpret_cast<NET_DVR_InitFUN>(pDLLHCNet->resolve("NET_DVR_Init"));
        NET_DVR_Login_V40_L=reinterpret_cast<NET_DVR_Login_V40FUN>(pDLLHCNet->resolve("NET_DVR_Login_V40"));
        NET_DVR_Logout_L=reinterpret_cast<NET_DVR_LogoutFUN>(pDLLHCNet->resolve("NET_DVR_Logout"));
        NET_DVR_ChangeWndResolution_L=reinterpret_cast<NET_DVR_ChangeWndResolutionFUN>(pDLLHCNet->resolve("NET_DVR_ChangeWndResolution"));
        NET_DVR_CaptureJPEGPicture_NEW_L=reinterpret_cast<NET_DVR_CaptureJPEGPicture_NEWFUN>(pDLLHCNet->resolve("NET_DVR_CaptureJPEGPicture_NEW"));
        NET_DVR_RealPlay_V40_L=reinterpret_cast<NET_DVR_RealPlay_V40FUN>(pDLLHCNet->resolve("NET_DVR_RealPlay_V40"));
        NET_DVR_GetLastError_L=reinterpret_cast<NET_DVR_GetLastErrorFUN>(pDLLHCNet->resolve("NET_DVR_GetLastError"));
        NET_DVR_SetLogToFile_L=reinterpret_cast<NET_DVR_SetLogToFileFUN>(pDLLHCNet->resolve("NET_DVR_SetLogToFile"));
        NET_DVR_GetLocalIP_L=reinterpret_cast<NET_DVR_GetLocalIPFUN>(pDLLHCNet->resolve("NET_DVR_GetLocalIP"));
        NET_DVR_SetValidIP_L=reinterpret_cast<NET_DVR_SetValidIPFUN>(pDLLHCNet->resolve("NET_DVR_SetValidIP"));
        NET_DVR_StopRealPlay_L=reinterpret_cast<NET_DVR_StopRealPlayFUN>(pDLLHCNet->resolve("NET_DVR_StopRealPlay"));
        NET_DVR_RemoteControl_L=reinterpret_cast<NET_DVR_RemoteControlFUN>(pDLLHCNet->resolve("NET_DVR_RemoteControl"));
        NET_DVR_GetRealPlayerIndex_L=reinterpret_cast<NET_DVR_GetRealPlayerIndexFUN>(pDLLHCNet->resolve("NET_DVR_GetRealPlayerIndex"));

        loginCamer();
    }
    else {
        emit pThis->messageSignal(tr("ERROR:Failed to load the dynamic pDLLHCNet"));
    }
}


bool CaptureImages::getDeviceStatus(LONG lUserID)
{
    if(NET_DVR_RemoteControl_L(lUserID,NET_DVR_CHECK_USER_STATUS,nullptr,4)){
        return true;
    }
    else {
        return false;
    }
}

void CaptureImages::loginCamer()
{
    NET_DVR_LOCAL_SDK_PATH SDKPath={};
    NET_SDK_INIT_CFG_TYPE cfgType=NET_SDK_INIT_CFG_SDK_PATH;
    QString path= QDir::toNativeSeparators(tr("%1/%2").arg(QCoreApplication::applicationDirPath()).arg("plugins/HCNetSDK"));
    strcpy(SDKPath.sPath,path.toLocal8Bit().data());
    NET_DVR_SetSDKInitCfg_L(cfgType,&SDKPath);

    NET_DVR_USER_LOGIN_INFO LoginInfo={};
    NET_DVR_DEVICEINFO_V40 DeviceInfo={};
    strcpy(LoginInfo.sDeviceAddress,this->ip.toLatin1().data());
    strcpy(LoginInfo.sUserName,this->name.toLatin1().data());
    strcpy(LoginInfo.sPassword,this->pow.toLatin1().data());
    LoginInfo.wPort=static_cast<uint16_t>(this->port);
    LoginInfo.bUseAsynLogin=1;
    LoginInfo.cbLoginResult=CaptureImages::loginResultCallBack;
    LoginInfo.pUser=nullptr;

    if(NET_DVR_Init_L()){
        NET_DVR_SetExceptionCallBack_V30_L(0,nullptr,CaptureImages::exceptionCallBack_V30,nullptr);
        //NET_DVR_SetLogToFile_L(3, QString(".\\sdkLog").toLatin1().data(), true);
        NET_DVR_Login_V40_L(&LoginInfo,&DeviceInfo);
    }
    emit pThis->messageSignal(tr("IP:%1,Camera login failed.").arg(ip));
}

void CaptureImages::exceptionCallBack_V30(DWORD dwType, LONG lUserID, LONG lHandle, void *pUser)
{
    //HCNetSDK* pThis=reinterpret_cast<HCNetSDK*>(pUser);
    emit pThis->messageSignal(tr("ID:%1,ERROR:%2,HANDLE:%3").arg(lUserID).arg(dwType).arg(lHandle));
}

void CaptureImages::loginResultCallBack(LONG lUserID, DWORD dwResult, LPNET_DVR_DEVICEINFO_V30 lpDeviceInfo, void *pUser)
{   
    //HCNetSDK* pThis=static_cast<HCNetSDK*>(pUser);
    pThis->lUserID=lUserID;
    pThis->dwResult=dwResult;
    emit pThis->messageSignal(tr("ID:%1,STATUS:%2").arg(lUserID).arg(dwResult));
}

bool CaptureImages::putCommandSlot(const int &imgNumber)
{
    NET_DVR_JPEGPARA   pJpegFile={};

    uint32_t charLen=300000;
    LPDWORD dataLen=nullptr;
    char* buff=static_cast<char*>(malloc( charLen* sizeof(char)));
    pJpegFile.wPicSize=0xff;
    pJpegFile.wPicQuality=0;

    if(dwResult){
        if(!NET_DVR_CaptureJPEGPicture_NEW_L(lUserID,1,&pJpegFile,buff,charLen,dataLen)){
            emit messageSignal(tr("put command Error:%1").arg(NET_DVR_GetLastError_L()));
            dataLen=nullptr;    delete  dataLen;
            free(buff);    buff=nullptr;    delete buff;
            return false;
        }
        else {
            QByteArray arrayJpg(buff,300000);
            //arrayJpg.insert(0,buff,charLen);
            //QByteArray bye(buff, charLen);
            emit pictureStreamSignal(arrayJpg,imgNumber);
            arrayJpg.clear();
            //arrayJpg.resize(0);
        }
    }
    dataLen=nullptr; delete  dataLen;
    free(buff);  buff=nullptr; delete buff;
    return true;
}

void CaptureImages::playStreamSlot(uint winID,bool play)
{
    if(dwResult){
        if(play){
            NET_DVR_PREVIEWINFO struPlayInfo = {};
            struPlayInfo.hPlayWnd    =reinterpret_cast<HWND>(winID);        //需要SDK解码时句柄设为有效值，仅取流不解码时可设为空
            struPlayInfo.lChannel     = 1;       //预览通道号
            struPlayInfo.dwStreamType = 0;       //0-主码流，1-子码流，2-码流3，3-码流4，以此类推
            struPlayInfo.dwLinkMode   = 1;       //0- TCP方式，1- UDP方式，2- 多播方式，3- RTP方式，4-RTP/RTSP，5-RSTP/HTTP
            struPlayInfo.bBlocked     = 1;       //0- 非阻塞取流，1- 阻塞取流

            streamID =NET_DVR_RealPlay_V40_L(lUserID,&struPlayInfo,nullptr,nullptr);

            if(streamID==-1){
                 emit messageSignal(tr("open stream Error:%1").arg(NET_DVR_GetLastError_L()));
            }
        }
        else {
            if(!NET_DVR_StopRealPlay_L(streamID)){
                emit messageSignal(tr("stop stream Error:%1").arg(NET_DVR_GetLastError_L()));
            }
        }
    }
}

void CaptureImages::resizeEventSlot()
{
    if(dwResult){
         NET_DVR_ChangeWndResolution_L(this->streamID);
    }
}

void CaptureImages::closeStreamSlot()
{
    if(streamID!=-1||lUserID!=-1){
        NET_DVR_StopRealPlay_L(streamID);
        NET_DVR_Logout_L(lUserID);
        NET_DVR_Cleanup_L();
    }
}
