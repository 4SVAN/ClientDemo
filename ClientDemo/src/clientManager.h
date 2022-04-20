#ifndef CLIENTMANAGER_H
#define CLIENTMANAGER_H

#include <QObject>
#include <QQmlEngine>
#include <QtCore>
#include <QtGui>
#include <QAxObject>
#include <QDir>
#include <QFileDialog>


class DeviceInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString hostname READ getHostname WRITE setHostname NOTIFY hostnameChanged)
    Q_PROPERTY(QString macAddress READ getMacAddress WRITE setMacAddress NOTIFY macAddressChanged)
    Q_PROPERTY(bool state READ getState WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(int seatID READ getSeatID WRITE setSeatID NOTIFY seatIDChanged)
    Q_PROPERTY(QStringList camaraIPList READ getCamaraIPList WRITE setCamaraIPList NOTIFY camaraIPListChanged)
    Q_PROPERTY(QStringList camaraMacList READ getCamaraMacList WRITE setCamaraMacList NOTIFY camaraMacListChanged)
    Q_PROPERTY(QList<bool> camaraStateList READ getCamaraStateList WRITE setCamaraStateList NOTIFY camaraStateListChanged)
    Q_PROPERTY(bool checkState READ getCheckState WRITE setCheckState NOTIFY checkStateChanged)
public:
    explicit DeviceInfo(int seat);

    Q_INVOKABLE QString getHostname() {return _hostname;}
    Q_INVOKABLE QString getMacAddress() {return _macAddress;}
    Q_INVOKABLE bool getState() {return _state;}
    Q_INVOKABLE int getSeatID() {return _seatID;}
    Q_INVOKABLE QStringList getCamaraIPList() {return _camaraIPList;}
    Q_INVOKABLE QStringList getCamaraMacList() {return _camaraMacList;}
    Q_INVOKABLE QList<bool> getCamaraStateList() {return _camaraStateList;}
    Q_INVOKABLE bool getCheckState() {return _checkState;}

    Q_INVOKABLE void setHostname(const QString &ip) {_hostname = ip; Q_EMIT hostnameChanged();}
    Q_INVOKABLE void setMacAddress(const QString &addr) {_macAddress = addr; Q_EMIT macAddressChanged();}
    Q_INVOKABLE void setState(const bool &newState) {_state = newState; Q_EMIT stateChanged();}
    Q_INVOKABLE void setSeatID(const int &seat) {_seatID = seat; Q_EMIT seatIDChanged();}
    Q_INVOKABLE void setCamaraIPList(const QStringList &list) {_camaraIPList = list; Q_EMIT camaraIPListChanged();}
    Q_INVOKABLE void setCamaraMacList(const QStringList &list) {_camaraMacList = list; Q_EMIT camaraMacListChanged();}
    Q_INVOKABLE void setCamaraStateList(const QList<bool> &list) {_camaraStateList = list; Q_EMIT hostnameChanged();}
    Q_INVOKABLE void setCheckState(const bool &newState) {qDebug() << "hi"; _checkState = newState; Q_EMIT checkStateChanged();}

Q_SIGNALS:
    void hostnameChanged();
    void macAddressChanged();
    void stateChanged();
    void seatIDChanged();
    void camaraIPListChanged();
    void camaraMacListChanged();
    void camaraStateListChanged();
    void checkStateChanged();

private:
    QString _hostname;              // 本机主机名
    QString _macAddress;            // 本机mac地址
    bool _state;                    // 本机(MQTT)连接状态
    int _seatID;                    // 本机座位号
    QStringList _camaraIPList;      // 本机摄像头组ip地址
    QStringList _camaraMacList;     // 本机摄像头组mac地址
    QList<bool> _camaraStateList;   // 本机摄像头组连接状态
    bool _checkState;               // 本机选中状态
};

class ClientManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVector<DeviceInfo*> deviceInfo READ deviceInfo WRITE setDeviceInfo NOTIFY deviceInfoChanged)
    Q_PROPERTY(QList<bool> checkStateGroup READ checkStateGroup WRITE setCheckStateGroup NOTIFY checkStateGroupChanged)
public:
    static ClientManager *getInstance();
    static QObject *qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine);

    /// @brief initializing ClientManager
    void init();

    /// @brief get the count of devices
    Q_INVOKABLE int count() const;

    /**
     * @brief createSavePath and return absolute path
     * @param path - local path
     * @param filename
     * @return absolute path of this file
     */
    QString createSavePath(const QString &path, const QString &filename);

    /**
     * @brief save Device Info Settings
     * @param index - seat number
     * @param key - info name (ip,mac,etc.)
     * @param value - info
     */
    Q_INVOKABLE void saveDeviceInfoSettings(const int &index, const QString &key, QVariant value=" ");

    /**
     * @brief read Device Info Settings
     * @param index - seat number
     * @param key - info name (ip,mac,etc.)
     * @return
     */
    Q_INVOKABLE QVariant readDeviceInfoSettings(const int &index, const QString &key);

public slots:
    QVector<DeviceInfo*> deviceInfo() {return _deviceInfo;}
    QList<bool> checkStateGroup() {return _checkStateGroup;}
    void setDeviceInfo(const QVector<DeviceInfo*> &info) {_deviceInfo = info; Q_EMIT deviceInfoChanged();}
    void setCheckStateGroup(const QList<bool> &group) {_checkStateGroup = group; Q_EMIT checkStateGroupChanged();}

    /**
     * @brief check if deviceinfo is empty
     * @return
     */
    bool isEmpty();

    /**
     * @brief check if Current Seat is Connect
     * @param seat number
     * @return if current seat is connected, return true
     */
    bool isCurrentSeatConnect(const int &seat);


    /**
     * @brief get Hostname by indicated seat number
     * @param seat number
     */
    QString getSeatHostname(const int &seat);

    /**
     * @brief get Seat State
     * @param seat
     * @return true if this seat is connected
     */
    bool getSeatState(const int &seat);

    /**
     * @brief write to (filename.xls) file
     * @param filename
     */
    void writeXlsFile(const QString &filename);

    /**
     * @brief set Hostname of indicated seat
     * @param seat number
     * @param hostname address
     */
    void setSeatHostname(const int &seat, QString ip = "127.0.0.1");

    /**
     * @brief set Current Seat State
     * @param current seat
     * @param state (true means current seat is connected)
     */
    void setCurrentSeatState(const int &seat, const bool &state);

Q_SIGNALS:
    void deviceInfoChanged();
    void checkStateGroupChanged();
    void updateModelData(const int &row, const int &column, const QVariant &value);

private slots:
    /// @brief exchange from QList<QList<QVariant>> to QVariant
    void castListListVariant2Variant(const QList<QList<QVariant> > &cells, QVariant &res);

    /// @brief exchange from QVariant to QList<QList<QVariant>>
    void castVariant2ListListVariant(const QVariant &var, QList<QList<QVariant> > &res);

    /// @brief get the Cells string name by its row and column
    QString convertToColName(int row, int column);

    /// @brief exchange from int to QString in ASCII ( 1 -> A )
    QString to26AlphabetString(int data);

    /// @brief handle Heartbeat Package
    void handleHeartbeatPackage(const QJsonObject &jsonObj);


private:
    ClientManager();
    ~ClientManager();

    /// 用户信息
    QVector<DeviceInfo*> _deviceInfo;
    /// 选中状态
    QList<bool> _checkStateGroup;

    QTimer timer;
    QAxObject *sheet;
    QAxObject *excel;
    QAxObject *workbook;
    QAxObject *worksheet;
    QAxObject *user_range;

};

#endif // CLIENTMANAGER_H
