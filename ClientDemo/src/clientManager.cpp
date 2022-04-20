#include "clientManager.h"
#include "mqtt.h"
#include "xlsxdocument.h"

DeviceInfo::DeviceInfo(int seat):
    _hostname(" "),
    _macAddress(" "),
    _state(false),
    _seatID(seat),
    _camaraIPList({" "," "," "}),
    _camaraMacList({"","",""}),
    _camaraStateList({false,false,false})
{

}

ClientManager::ClientManager()
{

}


ClientManager::~ClientManager()
{
    qDebug() << "deleted ClientManager";
}

ClientManager *ClientManager::getInstance()
{
    static ClientManager clientManager;
    return &clientManager;
}

QObject *ClientManager::qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);
    return ClientManager::getInstance();
}

void ClientManager::init()
{ 
//    qDebug()<<"current thread : ClientManager, ID:"<<QThread::currentThreadId();
    if(_deviceInfo.isEmpty()){
        qDebug() << "设备数据未初始化，正在初始化设备信息";
//        const int &column = INFO_LENGTH;
        const int &row = 24;
//        _deviceInfo.resize(row);
        for(int i=0;i<row;i++){
            DeviceInfo *info = new DeviceInfo(i);
            _deviceInfo.append(info);
            _checkStateGroup.append(false);
        }
        qDebug()<< "初始化完成！设备数量 :" <<_deviceInfo.size();
    }

    QSettings settings;


    QObject::connect(Mqtt::getInstance(), &Mqtt::heartbeatPackageReceived, this, &ClientManager::handleHeartbeatPackage);
    qDebug() << "Client Manager has been initialized";
}

QString ClientManager::createSavePath(const QString &path, const QString &filename)
{
    QString filepath = QCoreApplication::applicationDirPath().replace("\\", "/") + path;
    QDir dir(filepath);

    // 若当前文件夹路径不存在，则新建文件夹
    if(!dir.exists()){
        qInfo() << "指定文件夹路径不存在，正在新建文件夹...";
        dir.mkpath(filepath);
        qInfo() << "新建文件夹成功！文件夹路径：" << filepath;
        filepath += "/" + filename;
        qDebug() << filepath;
    }
    else{
        filepath += "/" + filename;
        qDebug() << filepath;
        QFileInfo fileTemp(filepath);

        // 若excel文件已存在，则删除该文件
        if(fileTemp.isFile()){
            qInfo() << "excel文件已存在，正在删除当前文件...";
            QFile file(filepath);
            file.remove();
            qInfo() << "删除文件成功！";
        }
    }

    return filepath;
}

int ClientManager::count() const
{
    return _deviceInfo.size();
}

QString ClientManager::convertToColName(int row, int column)
{
    Q_ASSERT(row*column>0 && row*column<65535);
    QString res;
    res.append(to26AlphabetString(column) + QString::number(row));
    return res;
}

QString ClientManager::to26AlphabetString(int data)
{
    QChar ch = data + 0x40;//A对应0x41
    return QString(ch);
}

void ClientManager::handleHeartbeatPackage(const QJsonObject &jsonObj)
{
    int seatNumber = jsonObj.value("seatNumber").toInt();
    QString studentIP = jsonObj.value("studentIP").toString();
    QString macAddress = jsonObj.value("macAddr").toString();
    qInfo() << "座位号：" << seatNumber << " 学生端IP：" << studentIP << " 学生端MAC地址：" << macAddress;

    _deviceInfo[seatNumber]->setHostname(studentIP);
    _deviceInfo[seatNumber]->setMacAddress(macAddress);
    _deviceInfo[seatNumber]->setSeatID(seatNumber);
    saveDeviceInfoSettings(seatNumber, "hostname", studentIP);
    saveDeviceInfoSettings(seatNumber, "macAddr", macAddress);
    _checkStateGroup[seatNumber] = true;
}

QVariant ClientManager::readDeviceInfoSettings(const int &index, const QString &key)
{
//    qDebug() << key;
    QSettings settings;
    settings.beginReadArray("deviceInfo");
    settings.setArrayIndex(index);
    QVariant value = settings.value(key);
//    qDebug() << value;
    return value;
}


void ClientManager::saveDeviceInfoSettings(const int &index, const QString &key, QVariant value)
{
//    qDebug() << key << " : " << value;
    QSettings settings;
    settings.beginWriteArray("deviceInfo");
    settings.setArrayIndex(index);
    settings.setValue(key, value);
    settings.endArray();
}

void ClientManager::setCurrentSeatState(const int &seat, const bool &state)
{
    _deviceInfo[seat]->setState(state);
}

void ClientManager::writeXlsFile(const QString &filename)
{
    QXlsx::Document xlsx;

    QString filepath = createSavePath("/data", filename);
    QString cellstr;
    QXlsx::Format rowFormat;

    rowFormat.setFontBold(true);
//    rowFormat.setFontColor(QColor(Qt::blue));
    rowFormat.setFontSize(18);
    rowFormat.setHorizontalAlignment(QXlsx::Format::AlignHCenter);

    QXlsx::Format columnFormat;
//    columnFormat.setFontBold(true);
//    rowFormat.setFontColor(QColor(Qt::blue));
    columnFormat.setFontSize(16);
    columnFormat.setHorizontalAlignment(QXlsx::Format::AlignHCenter);

    xlsx.write("A1", "座位号");
    xlsx.write("B1", "IP地址");
    xlsx.write("C1", "MAC地址");

    xlsx.setColumnWidth(1,15.0);
    xlsx.setColumnWidth(2,20.0);
    xlsx.setColumnWidth(3,30.0);
    xlsx.setRowHeight(1,30.0);
    xlsx.setRowHeight(2,_deviceInfo.size()+1,25.0);
    xlsx.setRowFormat(1,rowFormat);
    xlsx.setColumnFormat(1,3,columnFormat);

    for(int i = 0; i < _deviceInfo.size(); i++)
    {
        // 座位在第一列第i+2行（i从0开始）
        cellstr = convertToColName(i + 2, 1);
//        qDebug() << cellstr << ": " << i;
        // 座位号为i
        xlsx.write(cellstr, i);

        cellstr = convertToColName(i + 2, 2);
        xlsx.write(cellstr, _deviceInfo[i]->getHostname());

        cellstr = convertToColName(i + 2, 3);
        xlsx.write(cellstr, _deviceInfo[i]->getMacAddress());
    }

    xlsx.saveAs(filepath);
}

void ClientManager::setSeatHostname(const int &seat, QString ip)
{
    _deviceInfo[seat]->setHostname(ip);
    ClientManager::saveDeviceInfoSettings(seat, "hostname", _deviceInfo[seat]->getHostname());
}

bool ClientManager::isEmpty()
{
    return _deviceInfo.isEmpty();
}

bool ClientManager::isCurrentSeatConnect(const int &seat)
{
    return _deviceInfo[seat]->getState();
}

QString ClientManager::getSeatHostname(const int &seat)
{
    qDebug() << "当前座位ip：" << _deviceInfo[seat]->getHostname();
    return _deviceInfo[seat]->getHostname();
}

bool ClientManager::getSeatState(const int &seat)
{
    return _deviceInfo[seat]->getState();
}

//把QVariant转为QList<QList<QVariant>>,用于快速读出的
void ClientManager::castVariant2ListListVariant(const QVariant &var, QList<QList<QVariant> > &res)
{
    QVariantList varRows = var.toList();
    if(varRows.isEmpty())
    {
        return;
    }

    const int rowCount = varRows.size();
    QVariantList rowData;

    for(int i=0;i<rowCount;++i)
    {
        rowData = varRows[i].toList();
        res.push_back(rowData);
    }
}

//把QList<QList<QVariant> > 转为QVariant,用于快速写入的
void ClientManager::castListListVariant2Variant(const QList<QList<QVariant> > &cells, QVariant &res)
{
    QVariantList vars;
    const int rows = cells.size();
    for(int i=0;i<rows;++i)
    {
        vars.append(QVariant(cells[i]));
    }
    res = QVariant(vars);
}


