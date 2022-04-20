#ifndef MYMODELS_H
#define MYMODELS_H

#include <QObject>
#include <QAbstractListModel>
#include <QMutex>
#include "listElements.h"

/*-----------------------------------------------------------------------------|
 |                                 ObjectModel                                 |
 |----------------------------------------------------------------------------*/


class ObjectModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum ModelRoles{
        ElementRole = Qt::UserRole + 1
    };
    Q_ENUM(ModelRoles)

    enum ModelTypes{
        Mqtt = -1,
        Ftp,
        Unknown
    };
    Q_ENUM(ModelTypes)

    enum ModelCheckStates {
        Unchecked = 0,
        PartiallyChecked,
        Checked
    };
    Q_ENUM(ModelCheckStates)

    explicit ObjectModel(ObjectModel::ModelTypes types = Unknown, QObject *parent = 0);
    ~ObjectModel();
    void init();

    ModelTypes getElementTypes() {return type;}
    void setElementTypes(ModelTypes newType) {type = newType;}
    int selected() {return select_ctr;}

    Q_INVOKABLE void decSelected(const int &num = 1);

    Q_INVOKABLE void addSelected(const int &num = 1);

    Q_INVOKABLE QList<QObject*> *getModel() {return &m_data;}

    Q_INVOKABLE bool setModel(QList<QObject*> *list);

    Q_INVOKABLE int rowCount(const QModelIndex & parent = QModelIndex()) const;

    Q_INVOKABLE void insert(const int &index, QObject *element);

    Q_INVOKABLE void append(QObject *element);

    Q_INVOKABLE QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    Q_INVOKABLE bool setData(const QModelIndex & index, const QVariant &value,int role = Qt::DisplayRole);

    Q_INVOKABLE void clear();

    Q_INVOKABLE int count();

    Q_INVOKABLE QObject *get(int index);

    Q_INVOKABLE void downloadFile(const int &seat);

    Q_INVOKABLE void batchDownload();

    Q_INVOKABLE void selectAll(int checkState);

    Q_INVOKABLE ObjectModel::ModelCheckStates checkState();

Q_SIGNALS:
    void startDownload(const QString &server_path, const QString local_file);

protected:
    QHash<int, QByteArray> roleNames() const;

private:
    QList<QObject*> m_data;
    ModelTypes type;
    int select_ctr;
    QMutex mutex;
};


/*-----------------------------------------------------------------------------|
 |                                 TableModel                                  |
 |----------------------------------------------------------------------------*/


class TableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit TableModel(QObject *parent);
    ~TableModel();

    enum ModelTypes{
        Info = -1,
        Ftp,
        Unknown
    };
    Q_ENUM(ModelTypes)

    /**
     * @brief 更新表格
     * @param newList
     */
    void updateData(QList<InfoItem*> newList);
    /**
     * @brief 表格行数
     * @param parent（默认为空）
     * @return
     */
    int rowCount(const QModelIndex & parent = QModelIndex()) const
    {Q_UNUSED(parent); return _infoTable.count();}
    /**
     * @brief 表格列数——默认有18列
     * @param parent（默认为空）
     * @return
     */
    int columnCount(const QModelIndex & parent = QModelIndex()) const
    {Q_UNUSED(parent); return 18;}
    /**
     * @brief 设置表格特定项数据
     * @param index——行数
     * @param value——修改值
     * @param role——指定操作
     * @return
     */
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    /**
     * @brief 获取表格特定项数据
     * @param index——行数
     * @param role——指定操作
     * @return
     */
    QVariant data(const QModelIndex &index, int role) const;
    /**
     * @brief 获取表头数据
     * @param section——指定列
     * @param orientation——定位（水平行/垂直列）表头
     * @param role——指定操作
     * @return
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

private:
    QList<InfoItem*> _infoTable;

};

#endif // MYMODELS_H
