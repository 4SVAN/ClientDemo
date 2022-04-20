#include <QDebug>
#include <QMutexLocker>
#include "ftpcontroller.h"
#include "mymodels.h"

#define CHECK_BOX_COLUMN 0
#define STU_IP_COLUMN 2

/*-----------------------------------------------------------------------------|
 |                                 ObjectModel                                 |
 |----------------------------------------------------------------------------*/


ObjectModel::ObjectModel(ObjectModel::ModelTypes types, QObject *parent): QAbstractListModel(parent), type(types)
{
    qDebug() << "create" << type << "Model";
    init();
}

ObjectModel::~ObjectModel()
{
    qDebug() << "delete Model";
    qDeleteAll(m_data.begin(), m_data.end());
    m_data.clear();
}

void ObjectModel::init()
{
    m_data.clear();
    select_ctr = 0;

    qDebug() << "end init";
}

void ObjectModel::decSelected(const int &num)
{
    if(num<=select_ctr){
        select_ctr -= num;
    }
    else{
        qDebug() << "underFlow! Current select counter: " << select_ctr << ", input num: " << num;
        select_ctr = 0;
        return;
    }
}

void ObjectModel::addSelected(const int &num)
{
    select_ctr += num;
    if(select_ctr>m_data.size()){
        qWarning() << "Overflow! Current select counter: " << select_ctr << ", model data size: " << m_data.size();
        select_ctr = m_data.size();
    }
}

bool ObjectModel::setModel(QObjectList *list)
{
    QMutexLocker locker(&mutex);
    beginResetModel();
    m_data = *list;
    select_ctr = 0;
    endResetModel();
    if(m_data == *list) return true;
    return false;
}

int ObjectModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_data.count();
}

void ObjectModel::insert(const int &index, QObject *element)
{
    if (index >= 0 && index <= m_data.size()){
        QMutexLocker locker(&mutex);
        beginInsertRows(QModelIndex(), rowCount(), rowCount());
        m_data.insert(index, element);
        endInsertRows();
    }
}

void ObjectModel::append(QObject *element)
{
    insert(m_data.size(), element);
}

QVariant ObjectModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_data.count())
        return QVariant();
    else{
        if(role == ElementRole){
            QObject *element = m_data.at(index.row());
            return QVariant::fromValue(element);
        }
    }
    return QVariant();
}

bool ObjectModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.row() >= 0 && index.row() < m_data.count()){
        if(role == ElementRole){
            QMutexLocker locker(&mutex);
            m_data.replace(index.row(), value.value<QObject *>());
            return true;
        }
    }
    return false;
}

void ObjectModel::clear()
{
    QMutexLocker locker(&mutex);
    beginResetModel();
    qDeleteAll(m_data.begin(), m_data.end());
    m_data.clear();
    select_ctr=0;
    endResetModel();
}

int ObjectModel::count()
{
    return m_data.size();
}

QObject *ObjectModel::get(int index)
{
    if (index >= 0 && index < m_data.size()){
        QMutexLocker locker(&mutex);
        return m_data.at(index);
    }
    return NULL;
}

void ObjectModel::downloadFile(const int &seat)
{
    qDebug() << seat;
    if(type == Ftp){
        // 若当前处于文件下载界面，则遍历每一项
        // 若当前项处于被选中状态，则发送download信号
        for(int i=0;i<m_data.size();i++){
            FtpElement *tmp = dynamic_cast<FtpElement*>(m_data.at(i));
            // 被选中
            if(tmp->state()){
                QString dir = "/Seat/" + QString::number(seat);
                QString filename = tmp->filename().split("  ").at(0);
                qDebug() << "ftp file name:" << filename
                         << ", local save dir:" << dir;
                // seat为当前服务器(学生端)的座位号(保存文件路径用到)
                // filename为想要下载的文件名(不是绝对路径)
                // dir为想要保存的本地文件夹路径
                emit FtpController::getInstance()->download(seat, filename, dir);
            }
        }
    }
    else if(type == Mqtt){
        QString dir = "/Seat/" + QString::number(seat);
        emit FtpController::getInstance()->batchDownload(seat, dir);
    }
    else{
        qDebug() << "current Object Model is not Ftp Model! "
                 << "Current Model Type:" << type;
    }

}

void ObjectModel::batchDownload()
{
    if(type == Mqtt){
        for(int i=0;i<m_data.size();i++){
            MqttElement *tmp = dynamic_cast<MqttElement*>(m_data.at(i));
            // 若当前学生端被选中且连接状态为已连接时，下载最新文件
            if(tmp->state() && tmp->indicatorColor() == "green"){
                qDebug() << "download seat at:" << i;
                ObjectModel::downloadFile(i);
            }
        }
    }
    else{
        qDebug() << "current Object Model is not Ftp Model! "
                 << "Current Model Type:" << type;
    }
}

void ObjectModel::selectAll(int checkState)
{
    qDebug() << checkState;

    if(checkState==2) select_ctr = m_data.size();
    else select_ctr = 0;

    switch (type) {
    case Ftp:{
        for(int i=0;i<m_data.size();i++){
            FtpElement *tmp = dynamic_cast<FtpElement*>(m_data.at(i));
            if(checkState==2){
                tmp->setState(true);
            }
            else {
                tmp->setState(false);
            }
        }
        break;
    }
    case Mqtt:{
        for(int i=0;i<m_data.size();i++){
            MqttElement *tmp = dynamic_cast<MqttElement*>(m_data.at(i));
            if(checkState==2){
                tmp->setState(true);
            }
            else {
                tmp->setState(false);
            }
        }
        break;
    }
    default:{
        qDebug() << "Model未初始化！当前模型类型:" << type;
        break;
    }
    }
}

ObjectModel::ModelCheckStates ObjectModel::checkState()
{
    qDebug() << select_ctr << ":" << m_data.size();
    if(select_ctr == 0) return Unchecked;
    else if(select_ctr == m_data.size()) return Checked;
    else return PartiallyChecked;
}

QHash<int, QByteArray> ObjectModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ElementRole] = "ElementRole";
    return roles;
}





/*-----------------------------------------------------------------------------|
 |                                 TableModel                                  |
 |----------------------------------------------------------------------------*/


TableModel::TableModel(QObject *parent) : QAbstractTableModel(parent)
{
    qDebug() << "hi";
}

TableModel::~TableModel()
{
    qDebug() << "hi";
}

void TableModel::updateData(QList<InfoItem*> newList)
{
    beginResetModel();
    _infoTable = newList;
    endResetModel();
}

bool TableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    int nColumn = index.column();
    InfoItem *info = _infoTable.at(index.row());
    switch (role) {
    case Qt::CheckStateRole:
    {
        if(nColumn == CHECK_BOX_COLUMN){
            info->setStuState(value.toInt() == Qt::Checked);
            qDebug() << info->stuState();
            _infoTable.replace(index.row(), info);
            return true;
        }
    }
    default:{
        qDebug() << "current role: " << role;
        return false;
    }
    }
    return false;
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
            return QVariant();

        int nRow = index.row();
        int nColumn = index.column();
        InfoItem *info = _infoTable.at(nRow);
        switch (role)
            {
            case Qt::TextAlignmentRole:
                return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
            case Qt::DisplayRole:
            {
                if (nColumn == STU_IP_COLUMN){
                    qDebug() << "hi";
                    return info->stuIP();
                }
                return "";
            }
            case Qt::CheckStateRole:
            {
                if (nColumn == CHECK_BOX_COLUMN){
                    qDebug() << "hi";
                    return info->stuState() ? Qt::Checked : Qt::Unchecked;
                }
                return Qt::Unchecked;
            }
            default:
                return QVariant();
            }

        return QVariant();
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (role)
        {
        case Qt::TextAlignmentRole:
            return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
        case Qt::DisplayRole:
        {
            if (orientation == Qt::Horizontal)
            {
                if (section == CHECK_BOX_COLUMN)
                    return QStringLiteral("状态");

                if (section == STU_IP_COLUMN)
                    return QStringLiteral("学生端IP");
            }
        }
        default:
            return QVariant();
        }

    return QVariant();
}

Qt::ItemFlags TableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
            return QAbstractItemModel::flags(index);

    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    if (index.column() == CHECK_BOX_COLUMN)
        flags |= Qt::ItemIsUserCheckable;

    return flags;
}
