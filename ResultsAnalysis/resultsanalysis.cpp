#include "resultsanalysis.h"

ResultsAnalysis::ResultsAnalysis(QObject *parent)
{
    this->setParent(parent);
    this->correct=false;
    this->channel=-1;
    initCheckMap();
}

ResultsAnalysis::~ResultsAnalysis()
{
    checkMap.clear();
}

void ResultsAnalysis::setCheckTheResultsSlot(bool correct)
{
    this->correct=correct;
}

void ResultsAnalysis::setChannelSlot(int channel)
{
    this->channel=channel;
}

void ResultsAnalysis::initCheckMap()
{
    char ch = 'A';
    for (int i = 10; i <= 38; i++)
    {
        if ((i == 11) || (i == 22) || (i == 33))
        {
            i++;
        }
        checkMap.insert(ch,i);
        ch++;
    }
}

int ResultsAnalysis::computeQuadraticPower(int variable)
{
    int result;
    if (variable == 0)
    {
        return 1;
    }
    result = 2 * computeQuadraticPower(variable - 1);
    return result;
}

bool ResultsAnalysis::numberCheck(QString &number)
{
    if(number.length()<10)/* 最后一位可以计算出来 */
    {
        return false;
    }

    int sum=0;
    int i = 1;
    foreach (QChar ch , number)
    {/* 计算校验总值 */
        if (i == 11)
        {
            break;
        }
        if (i > 4)
        {
            sum += (ch.toLatin1()-'0') * (computeQuadraticPower(i-1));
        }
        else
        {
            if (checkMap.value(ch)!=0)
            {
                sum += checkMap.value(ch) * computeQuadraticPower(i - 1);
            }
            else
            {/* 箱号前4位必须是英文,否则箱号格式错误 */
                return false;
            }
        }
        i++;
    }

    int die=sum % 11;

    if(number.count()==10){
        if(correct){
            number=number.append(QString::number(die));
            return true;
        }
        return false;
    }
    if (number.count()==11 && die!=number[10].toLatin1()-'0')
    {
        if(0<=die && die <=9 && correct)
        {
            number[10]=die+'0';/* 矫正结果 */
            return true;
        }
        return false;
    }
    return true;
}

void ResultsAnalysis::resultsOfAnalysisSlot(QStringList resultList, int type, const QString &imgTime)
{
    /* 1:22G1 */
    /* 2:45G1 */
    /* 3:双22G1 */
    this->imgTime=imgTime;

    QList<uint32_t> conProbabilityTemp;/* 箱号置信度 */
    QList<uint32_t> isoProbabilityTemp;/* 箱型置信度 */

    for(auto var:resultList){
        QString con="";        QString iso="";        uint32_t Cprobability=0;                      uint32_t Iprobability=0;        int check=0;
        if(var.startsWith("RESULT")){
            QStringList tmp=var.split(":")[1].split("|");
            if(tmp.count()==4){
                QString conT=tmp[0].trimmed();
                check=numberCheck(conT);
                con=conT;
                iso=tmp[1];
                Cprobability=tmp[2].toUInt();
                Iprobability=tmp[3].toUInt();
            }
        }
        conTemp.append(con);/* 箱号 */
        isoTemp.append(iso);/* 箱型 */
        checkConList.append(check);/* 校验结果 */
        conProbabilityTemp.append(Cprobability);/* 箱号置信度 */
        isoProbabilityTemp.append(Iprobability);/* 箱型置信度 */
    }

    /* 双箱，分前3个结果和后3个结果独立处理,前箱下标,前箱型下标,后箱下标,后箱型下标 */
    int Cindex1=-1;    int Iindex1=-1;    int Cindex2=-1;    int Iindex2=-1;    uint32_t Cprobability=0;    uint32_t Iprobability=0;
    if(type==3 && conProbabilityTemp.count()==6){
        for (int var = 0; var < 3; ++var) {
            if(conProbabilityTemp[var]>Cprobability){
                Cprobability=conProbabilityTemp[var];
                Cindex1=var;
            }
            if(isoProbabilityTemp[var]>Iprobability){
                Iprobability=isoProbabilityTemp[var];
                Iindex1=var;
            }
        }
        Cprobability=0; Iprobability=0;
        for (int var = 3; var < 6; ++var) {
            if(conProbabilityTemp[var]>Cprobability){
                Cprobability=conProbabilityTemp[var];
                Cindex2=var;
            }
            if(isoProbabilityTemp[var]>Iprobability){
                Iprobability=isoProbabilityTemp[var];
                Iindex2=var;
            }
        }
        emit containerSignal(type,conTemp[Cindex1],checkConList[Cindex1],isoTemp[Iindex1],conTemp[Cindex2],checkConList[Cindex2],isoTemp[Iindex2]);
    }
    else {
        for (int var = 0; var < conProbabilityTemp.count(); ++var) {
            if(conProbabilityTemp[var]>Cprobability){
                Cprobability=conProbabilityTemp[var];
                Cindex1=var;
            }
            if(isoProbabilityTemp[var]>Iprobability){
                Iprobability=isoProbabilityTemp[var];
                Iindex1=var;
            }
        }
        emit containerSignal(type,conTemp[Cindex1],checkConList[Cindex1],isoTemp[Iindex1]);
    }
    updateDataBase(type,Cindex1,Iindex1,Cindex2,Iindex2);

    conTemp.clear();
    isoTemp.clear();
    checkConList.clear();
    conProbabilityTemp.clear();
    isoProbabilityTemp.clear();
}

void ResultsAnalysis::updateDataBase(int type, int Cindex1,int Iindex1, int Cindex2, int Iindex2)
{
    QStringList tmp=imgTime.split(QDir::toNativeSeparators("/"));
    QString dateTime="";
    QString time="";
    if(tmp.count()>0){
        time=tmp[tmp.count()-1].split(".")[0].mid(0,14);
        dateTime=QDateTime::fromString(time,"yyyyMMddhhmmss").toString("yyyy-MM-dd hh:mm:ss");
    }

    emit resultsAnalysisStateSignal(channel,tr("%1 start").arg(dateTime));/* 日志起始 */

    for (int var = 0; var < conTemp.count(); ++var) {
        /* 识别结果写入日志,标志|时间戳|通道号|箱号|箱型|校验 */
        emit resultsAnalysisStateSignal(channel,tr("[%1|%2|%3|%4|%5|%6]").arg("I").arg(time).arg(channel).arg(conTemp[var]).arg(isoTemp[var]).arg(QString::number(checkConList[var])));
    }

    if(Cindex1!=-1){
        if(Cindex2!=-1){
            /* 识别结果写入日志,标志|时间戳|通道号|逻辑|箱号|箱型|箱号|箱型|校验|校验 */
            emit resultsAnalysisStateSignal(channel,tr("[%1|%2|%3|%4|%5|%6|%7|%8|%9|%10]").arg("C").arg(time).arg(channel).arg(type).arg(conTemp[Cindex1]).arg(isoTemp[Iindex1]).arg(conTemp[Cindex2]).arg(isoTemp[Iindex2]).arg(QString::number(checkConList[Cindex1])).arg(QString::number(checkConList[Cindex2])));
        }
        else {
            /* 识别结果写入日志,标志|时间戳|通道号|逻辑|箱号|箱型|校验*/
            emit resultsAnalysisStateSignal(channel,tr("[%1|%2|%3|%4|%5|%6|%7]").arg("C").arg(time).arg(channel).arg(type).arg(conTemp[Cindex1]).arg(isoTemp[Iindex1]).arg(QString::number(checkConList[Cindex1])));
        }
    }

    QMap<QString,QString> data;
    data.insert("Timer",dateTime);
    if(channel!=-1){
         data.insert("Channel",QString::number(channel));
    }

    if(Cindex1!=-1){
        data["ContainerFront"]=conTemp[Cindex1];
        data["ISOFront"]=isoTemp[Iindex1];
        data["CheckFront"]=QString::number(checkConList[Cindex1]);

        if(Cindex2!=-1){
            data["ContainerAfter"]=conTemp[Cindex2];
            data["ISOAfter"]=isoTemp[Iindex2];
            data["CheckAfter"]=QString::number(checkConList[Cindex2]);
        }
    }

    if(conTemp.count()==4){
        data["ImgFrontNumber"]=conTemp[0];
        data["ImgFrontCheck"]=QString::number(checkConList[0]);
        data["ImgLeftFrontNumber"]=conTemp[1];
        data["ImgLeftFrontCheck"]=QString::number(checkConList[1]);
        data["ImgRightFrontNumber"]=conTemp[2];
        data["ImgRightFrontCheck"]=QString::number(checkConList[2]);
        data["ImgAfterNumber"]=conTemp[3];
        data["ImgAfterCheck"]=QString::number(checkConList[3]);
    }
    else if (conTemp.count()==6) {
        data["ImgFrontNumber"]=conTemp[0];
        data["ImgFrontCheck"]=QString::number(checkConList[0]);
        data["ImgLeftFrontNumber"]=conTemp[1];
        data["ImgLeftFrontCheck"]=QString::number(checkConList[1]);
        data["ImgRightFrontNumber"]=conTemp[2];
        data["ImgRightFrontCheck"]=QString::number(checkConList[2]);
        data["ImgLeftAfterNumber"]=conTemp[3];
        data["ImgLeftAfterCheck"]=QString::number(checkConList[3]);
        data["ImgRightAfterNumber"]=conTemp[4];
        data["ImgRightAfterCheck"]=QString::number(checkConList[4]);
        data["ImgAfterNumber"]=conTemp[5];
        data["ImgAfterCheck"]=QString::number(checkConList[5]);
    }

    emit updateDataBaseSignal(data);
    data.clear();
    tmp.clear();
}
