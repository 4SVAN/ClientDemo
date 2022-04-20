#ifndef LISTELEMENTS_H
#define LISTELEMENTS_H
#include <QObject>
#include <QTimer>
#include <QDebug>
#include <QMutexLocker>


/*-----------------------------------------------------------------------------|
 |                                 MqttElement                                 |
 |----------------------------------------------------------------------------*/

class MqttElement : public QObject
{
    Q_OBJECT
    /// 指示器颜色
    Q_PROPERTY(QString indicatorColor READ indicatorColor WRITE setIndicatorColor NOTIFY indicatorColorChanged)
    /// 当前学生机选择状态
    Q_PROPERTY(bool state READ state WRITE setState NOTIFY stateChanged)

public:
    MqttElement(const int &seat, QString color = "gold", bool state = false, QObject *parent = 0);
    void init();
    int getSeat() {return m_seatNumber;}
    bool isOnline() {return online;}

    Q_INVOKABLE QString indicatorColor() {return m_indicatorColor;}
    Q_INVOKABLE bool state() {return m_state;}

    Q_INVOKABLE void setIndicatorColor(const QString &color) { m_indicatorColor = color; emit indicatorColorChanged();}
    Q_INVOKABLE void setState(const bool &newstate) {m_state = newstate; emit stateChanged();}

Q_SIGNALS:
    void indicatorColorChanged();
    void stateChanged();
    void currentSeatStateChanged(const int &seat, const bool &state);

public slots:
    void mqttTimeout();
    void setMqttOnlineState(bool state);

private:
    int m_seatNumber;
    QString m_indicatorColor;
    bool m_state;   // 保存当前学生机选择状态
    bool online;    // 保存当前学生机是否在线(mqtt服务连接状态)

    QTimer timer;
};




/*-----------------------------------------------------------------------------|
 |                                 FtpElement                                  |
 |----------------------------------------------------------------------------*/
//  Note:
// "ftpFileName" : strArray[0],
// "ftpFileLastModified" : strArray[1],
// "ftpFileType" : strArray[2],
// "ftpFileSize" : strArray[3]

class FtpElement : public QObject
{
    Q_OBJECT
    /// 文件选择状态
    Q_PROPERTY(bool state READ state WRITE setState NOTIFY stateChanged)
    /// 文件名
    Q_PROPERTY(QString filename READ filename WRITE setFilename NOTIFY filenameChanged)
//    Q_PROPERTY(QString lastModified READ lastModified WRITE setLastModified NOTIFY lastModifiedChanged) // 最后修改日期
//    Q_PROPERTY(QString fileType READ fileType WRITE setFileType NOTIFY fileTypeChanged)                 // 文件类型
//    Q_PROPERTY(QString fileSize READ fileSize WRITE setFileSize NOTIFY fileSizeChanged)                  // 文件大小

public:
    FtpElement(const QString &filename,
               bool state = false, QObject *parent = 0);

    Q_INVOKABLE bool state() {return m_state;}
    Q_INVOKABLE QString filename() {return m_filename;}
//    Q_INVOKABLE QString lastModified() {return m_fileLastModified;}
//    Q_INVOKABLE QString fileType() {return m_fileType;}
//    Q_INVOKABLE QString fileSize() {return m_fileSize;}

    Q_INVOKABLE void setState(const bool &newState) {m_state = newState; qDebug() << m_state;emit stateChanged();}
    Q_INVOKABLE void setFilename(const QString &str) {m_filename = str; emit filenameChanged();}
//    Q_INVOKABLE void setLastModified(const QString &time) {m_fileLastModified = time; emit lastModifiedChanged();}
//    Q_INVOKABLE void setFileType(const QString &type) {m_fileType = type; emit fileTypeChanged();}
//    Q_INVOKABLE void setFileSize(const QString &size) {m_fileSize = size; emit fileSizeChanged();}

Q_SIGNALS:
    void stateChanged();
    void filenameChanged();
//    void lastModifiedChanged();
//    void fileTypeChanged();
//    void fileSizeChanged();

private:
    QString m_filename;
//    QString m_fileLastModified;
//    QString m_fileType;
//    QString m_fileSize;

    bool m_state;
};


/*-----------------------------------------------------------------------------|
 |                                 InfoItem                                   |
 |----------------------------------------------------------------------------*/


class InfoItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString seat READ seat WRITE setSeat NOTIFY seatChanged)                 // 学生端座位号
    Q_PROPERTY(QString stuIP READ stuIP WRITE setStuIP NOTIFY stuIPChanged)             // 学生端ip地址
    Q_PROPERTY(QString macAddr READ macAddr WRITE setMacAddr NOTIFY macAddrChanged)     // 学生端mac地址
    Q_PROPERTY(bool stuState READ stuState WRITE setStuState NOTIFY stuStateChanged)    // 学生端连接状态
    Q_PROPERTY(QStringList camaraIPList READ camaraIPList WRITE setCamaraIPList NOTIFY camaraIPListChanged)                 // 摄像头ip地址列表（主摄，正侧摄，右侧）
    Q_PROPERTY(QStringList camaraMacList READ camaraMacList WRITE setCamaraMacList NOTIFY camaraMacListChanged)             // 摄像头mac地址列表（主摄，正侧摄，右侧）
    Q_PROPERTY(QList<bool> camaraStateList READ camaraStateList WRITE setCamaraStateList NOTIFY camaraStateListChanged) // 摄像头连接状态列表（主摄，正侧摄，右侧）

public:
    InfoItem(const QString &newSeat,
                       const QString &newIP,
                       const QString &newMac,
                       const QStringList &ipList,
                       const QStringList &macList,
                       bool newState = false,
                       QList<bool> stateList = {false,false,false},
                       QObject *parent = 0);

public slots:
    QString seat() {return _seat;}
    QString stuIP() {return _stuIP;}
    QString macAddr() {return _mac;}
    bool stuState() {return _state;}
    QStringList camaraIPList() {return _camaraIPList;}
    QStringList camaraMacList() {return _camaraMacList;}
    QList<bool> camaraStateList() {return _camaraStateList;}

    void setSeat(QString newSeat) {_seat = newSeat; Q_EMIT seatChanged();}
    void setStuIP(QString newIP) {_stuIP = newIP; Q_EMIT stuIPChanged();}
    void setMacAddr(QString newMac) {_mac = newMac; Q_EMIT macAddrChanged();}
    void setStuState(bool newState) {_state = newState; Q_EMIT stuStateChanged();}
    void setCamaraIPList(QStringList newList) {_camaraIPList = newList; Q_EMIT camaraIPListChanged();}
    void setCamaraMacList(QStringList newList) {_camaraMacList = newList; Q_EMIT camaraMacListChanged();}
    void setCamaraStateList(QList<bool> newList) {_camaraStateList = newList; Q_EMIT camaraStateListChanged();}

Q_SIGNALS:
    void seatChanged();
    void stuIPChanged();
    void macAddrChanged();
    void stuStateChanged();
    void camaraIPListChanged();
    void camaraMacListChanged();
    void camaraStateListChanged();

private:
    QString _seat;
    QString _stuIP;
    QString _mac;
    QStringList _camaraIPList;
    QStringList _camaraMacList;
    bool _state;
    QList<bool> _camaraStateList;
};

#endif // LISTELEMENTS_H
