#include "listElements.h"
#include "mqtt.h"
#include "clientManager.h"
#include "ftpClient.h"
#include "ftpcontroller.h"


/*-----------------------------------------------------------------------------|
 |                                 MqttElement                                 |
 |----------------------------------------------------------------------------*/

MqttElement::MqttElement(const int &seat, QString color, bool state, QObject *parent)
    : QObject(parent), m_seatNumber(seat), m_indicatorColor(color), m_state(state)
{
    init();
}

void MqttElement::init()
{
    setIndicatorColor(m_indicatorColor);
    online = false;
    timer.setInterval(10000);
    connect(&timer, &QTimer::timeout, this, &MqttElement::mqttTimeout);
    connect(this, &MqttElement::currentSeatStateChanged, ClientManager::getInstance(), &ClientManager::setCurrentSeatState);
}

// 若超时则重置状态
void MqttElement::mqttTimeout()
{
    timer.stop();
    setIndicatorColor("gold");
    online = false;
    emit currentSeatStateChanged(m_seatNumber, false);
}

// 更新当前Mqtt状态
void MqttElement::setMqttOnlineState(bool state)
{
    timer.stop();
    if(online!=state){
        qDebug() << "原始状态: " << online << "新状态: " << state;
        state ? setIndicatorColor("green") : setIndicatorColor("gold");
        online = state;
        emit currentSeatStateChanged(m_seatNumber, online);
    }
    timer.start();
}




/*-----------------------------------------------------------------------------|
 |                                 FtpElement                                  |
 |----------------------------------------------------------------------------*/




FtpElement::FtpElement(const QString &filename, bool state, QObject *parent) :
    QObject(parent),
    m_filename(filename),
    m_state(state)
{
}


/*-----------------------------------------------------------------------------|
 |                                 InfoItem                                    |
 |----------------------------------------------------------------------------*/

InfoItem::InfoItem(const QString &newSeat, const QString &newIP, const QString &newMac, const QStringList &ipList, const QStringList &macList, bool newState, QList<bool> stateList, QObject *parent) :
    QObject(parent),
    _seat(newSeat),
    _stuIP(newIP),
    _mac(newMac),
    _camaraIPList(ipList),
    _camaraMacList(macList),
    _state(newState),
    _camaraStateList(stateList)
{

}
