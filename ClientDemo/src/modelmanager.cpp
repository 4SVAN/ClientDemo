#include <QJsonObject>

#include "modelmanager.h"
#include "mqtt.h"
#include "ftpcontroller.h"

class ModelManagerPrivate
{
public:
    ModelManagerPrivate(ModelManager *parent)
        : q_ptr(parent)
        , objectModel(NULL)
    {
    }

    void init(ObjectModel::ModelTypes types);
    void uninit();

private:
    ModelManager * const q_ptr;
    Q_DECLARE_PUBLIC(ModelManager)

    ObjectModel* objectModel;
};

ModelManager::ModelManager(ObjectModel::ModelTypes types, QObject *parent)
    : QObject(parent), d_ptr(new ModelManagerPrivate(this))
{
    Q_D(ModelManager);
    d->init(types);
}

ModelManager::~ModelManager()
{
    Q_D(ModelManager);
    d->uninit();
}

/**
 * @brief 获取整个模型
 * @return 模型数据
 */
ObjectModel *ModelManager::getModel()
{
    Q_D(ModelManager);
    return d->objectModel;
}

void ModelManager::initMqttModel(const int &nums)
{
    Q_D(ModelManager);
    QObjectList list;
    for(int i=0;i<nums;i++){
        list.append(new MqttElement(i));
    }
    if(setModel(&list)){
        QObject::connect(Mqtt::getInstance(), &Mqtt::seatStateOn, this, [=](const int &seat){
            MqttElement *tmp = dynamic_cast<MqttElement*>(d->objectModel->get(seat));
            tmp->setMqttOnlineState(true);
        });
    }
    else qWarning() << "Failure Initialization for Mqtt Model";
}

void ModelManager::initFtpModel()
{
    QObject::connect(FtpController::getInstance(), &FtpController::fileRecv, this, [=](const QString file){
        //"ftpFileName" : list[0],
        //"ftpFileLastModified" : list[1],
        //"ftpFileType" : list[2],
        //"ftpFileSize" : list[3]
//        QStringList list = file.split("  ");
//        qDebug() << list;
        ModelManager::append(new FtpElement(file));
    });
}

bool ModelManager::setModel(QList<QObject*> *itemList)
{
    Q_D(ModelManager);
    // setModel自带重置Model功能
    return d->objectModel->setModel(itemList);
}

void ModelManager::append(QObject *element)
{
    Q_D(ModelManager);
    d->objectModel->append(element);
}

void ModelManager::clear()
{
    Q_D(ModelManager);
    d->objectModel->clear();
}

int ModelManager::count()
{
    Q_D(ModelManager);
    return d->objectModel->rowCount();
}

void ModelManagerPrivate::init(ObjectModel::ModelTypes types)
{
    Q_Q(ModelManager);
    if (objectModel == NULL) {
        objectModel = new ObjectModel(types, q);
    }
}

void ModelManagerPrivate::uninit()
{
    if (objectModel) {
        objectModel->deleteLater();
        objectModel = NULL;
    }
}
