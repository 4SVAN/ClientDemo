#ifndef MODELMANAGER_H
#define MODELMANAGER_H

#include <QObject>
#include "listElements.h"
#include "mymodels.h"

class ModelManagerPrivate;

class ModelManager : public QObject
{
    Q_OBJECT
public:
    ModelManager(ObjectModel::ModelTypes types, QObject *parent = nullptr);
//    ModelManager(TableModel::ModelTypes types, QObject *parent = nullptr);
    virtual ~ModelManager();

    ObjectModel *getModel();
    void initMqttModel(const int &nums);
    void initFtpModel();
    bool setModel(QList<QObject*> *itemList);
    void append(QObject *element);
    void clear();
    int count();

private:
    QScopedPointer<ModelManagerPrivate> d_ptr;
    Q_DECLARE_PRIVATE(ModelManager);
};


#endif // MODELMANAGER_H
