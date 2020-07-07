#include "QDBC.h"
#include "maininclude.h"

#include <QJsonDocument>
#include <QJsonObject>

/*-----------sql查询相关------------------*/
QString getOrderSql(int onwhich, QVector<int> idenCheckFor)
{
    QString sql = "";
    if (onwhich == ONDealOrder || onwhich == ONNowOrder) {
        Database* base = new Database(config.ip, config.dataPort, config.basename, thisUser.useName, thisUser.usePassword);
        QSqlDatabase database = base->getDatabase();
        QSqlQuery query(database);
        QSqlQuery itemQuery(database);
        QString iden = thisUser.identity;
        for (QChar c : iden) {
            if (!c.isLetter())
                return nullptr;
        }
        QString fstSql = "select status from authority"; //" where " + iden + "=1 or " + iden + "=3;";
        QString sndSql = ";";
        if (idenCheckFor.size() > 0) {
            sndSql = " where " + iden + "=" + QString::number(idenCheckFor[0]);
            for (int i = 1; i < idenCheckFor.size(); i++) {
                sndSql = sndSql + " or " + iden + "=" + QString::number(idenCheckFor[i]);
            }
            sndSql = sndSql + ";";
        }
        query.exec(fstSql + sndSql);
        QString idCheck, statusCheck = "";
        if (onwhich == ONDealOrder) {
            //TODO 待处理订单的人员检查
            idCheck = "( true )";
            if (thisUser.identity == QString("teacher"))
                idCheck = "( teacher='" + thisUser.truename + "' )";
            if (thisUser.identity == QString("header"))
                idCheck = "( workshop='" + thisUser.workshop + "' )";
        } else {
            QString iden = thisUser.identity + "='" + thisUser.truename + "'";
            QString teacher = "teacher='" + thisUser.truename + "'";
            idCheck = " ( " + iden + " or " + teacher + " ) ";
        }
        while (query.next()) {
            if (onwhich == ONDealOrder)
                statusCheck = statusCheck + " or status='" + query.value(0).toString() + "'";
            else
                statusCheck = statusCheck + "and status!='" + query.value(0).toString() + "' ";
        }
        statusCheck.remove(0, 3);
        statusCheck = " ( " + statusCheck + " ) ";
        sql = idCheck + " and " + statusCheck;
        base->close();
        delete base;
    } else if (onwhich == ONHistory) {
        if (thisUser.identity != QString("teacher")) {
            QString iden = thisUser.identity + "='" + thisUser.truename + "'";
            QString teacher = "teacher='" + thisUser.truename + "'";
            sql = " " + iden + " or " + teacher + " ";
        } else {
            sql = " teacher='" + thisUser.truename + "' ";
        }
    } else {
        sql = " false ";
    }
    return sql;
}
QString getOrderSql(int onwhich, QVector<int> idenCheckFor, QDate start, QDate end)
{
    QString pre = getOrderSql(onwhich, idenCheckFor);
    end = end.addDays(1);
    QString after = " starttime between '" + start.toString("yyyy-MM-dd") + "' and '" + end.toString("yyyy-MM-dd") + "' ";
    if (onwhich == ONHistory)
        return QString(" (" + pre + ") and (" + after + ") limit ") + QString::number(config.MaxHistoryOrders);
    return QString(" (" + pre + ") and (" + after + ") ");
}
bool formOrders(QString MainSql, QString itemMainSql, QString whereSql, QVector<OneOrder>& orders)
{
    orders.clear();
    Database* base = new Database(config.ip, config.dataPort, config.basename, thisUser.useName, thisUser.usePassword);
    if (!base->check())
        return false;
    QSqlDatabase database = base->getDatabase();
    QSqlQuery query(database);
    QSqlQuery itemQuery(database);
    if (MainSql == nullptr)
        MainSql = "select id,workshop,useclass,starttime,usetime,more,teacher,header,admin,keeper,accountant,status from orders";
    if (itemMainSql == nullptr)
        itemMainSql = "select pid,cnt,status,more from orderitems";
    QString sql = MainSql + " where " + whereSql + ";";
    //qDebug() << sql;
    if (!query.exec(sql))
        return false;
    while (query.next()) {
        OneOrder* order = new OneOrder;
        order->id = query.value(0).toInt();
        order->workshop = query.value(1).toString();
        order->useclass = query.value(2).toString();
        order->starttime = query.value(3).toDate();
        order->usetime = query.value(4).toDateTime();
        order->more = query.value(5).toString();
        order->teacher = query.value(6).toString();
        order->header = query.value(7).toString();
        order->admin = query.value(8).toString();
        order->keeper = query.value(9).toString();
        order->accountant = query.value(10).toString();
        order->status = query.value(11).toString();
        sql = itemMainSql + " where orderid=" + QString::number(order->id);
        //qDebug() << sql;
        itemQuery.exec(sql);
        while (itemQuery.next()) {
            OneItem item;
            item.pid = itemQuery.value(0).toInt();
            item.number = itemQuery.value(1).toDouble();
            item.status = itemQuery.value(2).toString();
            item.more = itemQuery.value(3).toString();
            order->items.push_back(item);
        }
        orders.push_back(*order);
    }
    base->close();
    delete base;
    std::sort(orders.begin(), orders.end());
    return true;
}
/*-----------sql查询相关end---------------*/

/*--------------初始化相关---------------*/
bool initNewItems()
{
    //TODO 入库订单读取待写
    QVector<int> tmp;
    tmp.push_back(1);
    tmp.push_back(3);
    QString sql = getOrderSql(ONDealOrder, tmp);
    return formOrders(nullptr, nullptr, sql, newitems);
}

bool initDealOrders()
{
    QVector<int> tmp;
    tmp.push_back(1);
    tmp.push_back(3);
    QString sql = getOrderSql(ONDealOrder, tmp);
    return formOrders(nullptr, nullptr, sql, dealorders);
}

bool initNowOrders()
{
    QVector<int> tmp;
    tmp.push_back(1);
    tmp.push_back(3);
    QString sql = getOrderSql(ONNowOrder, tmp);
    return formOrders(nullptr, nullptr, sql, noworders);
}

bool initSatus()
{
    Database* base = new Database(config.ip, config.dataPort, config.basename, thisUser.useName, thisUser.usePassword);
    if (!base->check())
        return false;
    QSqlDatabase database = base->getDatabase();
    QSqlQuery query(database);
    if (!query.exec("select status from authority;"))
        return false;
    config.statusList.clear();
    config.statusList << QString("0");
    while (query.next()) {
        QString now = query.value(0).toString();
        config.statusList << now;
    }
    base->close();
    delete base;
    return true;
}
/*--------------初始化相关end------------*/

/*--------------功能函数-----------------*/
bool flushDealOrders(QDate start, QDate end) //刷新待处理订单
{
    QVector<int> tmp;
    tmp.push_back(1);
    tmp.push_back(3);
    QString sql = getOrderSql(ONDealOrder, tmp, start, end);
    bool res = formOrders(nullptr, nullptr, sql, dealorders);
    return res;
}
bool flushNowOrders(QDate start, QDate end) //刷新当前订单
{
    QVector<int> tmp;
    tmp.push_back(1);
    tmp.push_back(3);
    QString sql = getOrderSql(ONNowOrder, tmp, start, end);
    bool res = formOrders(nullptr, nullptr, sql, noworders);
    return res;
}

bool flushHistoryOrders(QDate start, QDate end) //刷新历史订单
{
    QVector<int> tmp;
    QString whereSql = getOrderSql(ONHistory, tmp, start, end);
    QString MainSql = "select id,workshop,useclass,starttime,usetime,more,teacher,header,admin,keeper,accountant,status from historyorder ";
    QString itemMainSql = "select pid,cnt,status,more from historyitems";
    bool res = formOrders(MainSql, itemMainSql, whereSql, historys);
    return res;
}

OneOrder* getOrder(int id, QVector<OneOrder>& orders) //按id查找订单order
{
    OneOrder order;
    order.id = id;
    QVector<OneOrder>::iterator it = std::lower_bound(orders.begin(), orders.end(), order);
    if (it == noworders.end() || it->id != id) {
        return nullptr;
    }
    return &(*it);
}

/*----------物品相关函数----------*/
uint qHash(const OneResItem key) //物品set的哈希映射
{
    return uint(key.pid);
}

bool initResItems()
{
    allResItem.clear();
    resItemsTrie.clear();
    Database base(config.ip, config.dataPort, config.basename, thisUser.useName, thisUser.usePassword);
    QSqlDatabase database = base.getDatabase();
    QSqlQuery query(database);
    query.exec("select pid,res,name,type,units,cnt from allitems;");
    while (query.next()) {
        OneResItem aResItem;
        aResItem.pid = query.value(0).toInt();
        aResItem.res = query.value(1).toString();
        aResItem.name = query.value(2).toString();
        aResItem.type = query.value(3).toString();
        aResItem.units = query.value(4).toString();
        aResItem.cnt = query.value(5).toDouble();
        addResItem(aResItem, false);
    }
    base.close();
    return true;
}
void addResItem(OneResItem atype, bool forCheck = false) //添加物品
{
    if (forCheck == true) {
        if (hasResItem(atype.res, atype.name, atype.type))
            return;
    }
    QSet<OneResItem>::iterator it = allResItem.insert(atype);
    const OneResItem* ret = (&(*it));
    resItemsTrie.insert(ret);
}
bool hasResItem(QString res, QString name, QString type) //判断是否含有该物品
{
    /*for (OneResItem atype : allResItem) {
        if (atype.res.compare(res) == 0 && atype.name.compare(name) == 0 && atype.type.compare(type) == 0)
            return true;
    }
    return false;*/
    return resItemsTrie.find(res, name, type) != nullptr;
}
int getResItemPid(QString res, QString name, QString type) //获取物品的pid
{
    /*for (OneResItem atype : allResItem) {
        if (atype.res.compare(res) == 0 && atype.name.compare(name) == 0 && atype.type.compare(type) == 0)
            return atype.pid;
    }
    return 0;*/
    const OneResItem* tmp = resItemsTrie.find(res, name, type);
    if (tmp == nullptr)
        return 0;
    return tmp->pid;
}
const OneResItem* getResItem(int pid) //按pid获取到物品
{
    OneResItem type;
    type.pid = pid;
    QSet<OneResItem>::iterator it = allResItem.find(type);
    if (it == allResItem.end())
        return nullptr;
    return &(*it);
}
QString getUnits(QString res, QString name, QString type) //获取物品的单位
{
    /*for (OneResItem atype : allResItem) {
        if (atype.res.compare(res) == 0 && atype.name.compare(name) == 0 && atype.type.compare(type) == 0)
            return atype.units;
    }
    return nullptr;*/
    const OneResItem* tmp = resItemsTrie.find(res, name, type);
    if (tmp == nullptr)
        return nullptr;
    return tmp->units;
}
QStringList getResList()
{
    return QStringList(resItemsTrie.nodes[0].mp.keys());
}
QStringList getNameList(QString res)
{
    int nodeid = 0;
    QHash<QString, int>::iterator mpit;
    mpit = resItemsTrie.nodes[nodeid].mp.find(res);
    if (mpit == resItemsTrie.nodes[nodeid].mp.end())
        return QStringList();
    nodeid = mpit.value();
    return QStringList(resItemsTrie.nodes[nodeid].mp.keys());
}
QStringList getTypeList(QString res, QString name)
{
    int nodeid = 0;
    QHash<QString, int>::iterator mpit;
    mpit = resItemsTrie.nodes[nodeid].mp.find(res);
    if (mpit == resItemsTrie.nodes[nodeid].mp.end())
        return QStringList();
    nodeid = mpit.value();
    mpit = resItemsTrie.nodes[nodeid].mp.find(name);
    if (mpit == resItemsTrie.nodes[nodeid].mp.end())
        return QStringList();
    nodeid = mpit.value();
    return QStringList(resItemsTrie.nodes[nodeid].mp.keys());
}
QSet<OneResItem> getResItemsByRes(QString res)
{
    if (config.itemsList.size() < 1)
        return QSet<OneResItem>();
    char start = '0';
    int i = 0;
    for (QString tmp : config.itemsList) {
        if (tmp == res)
            start = config.itemStartWith[i];
        i++;
    }
    QSet<OneResItem> st;
    for (OneResItem item : allResItem) {
        if (QString::number(item.pid).at(0).toLatin1() == start) {
            st.insert(item);
        }
    }
    return st;
}

ResItemsTrie::ResItemsTrie()
{
    clear();
}
void ResItemsTrie::clear()
{
    itemsSum = 0;
    nodes.clear();
    OneResItem tmp;
    tmp.pid = 0;
    tmp.mp.clear();
    nodes.push_back(tmp);
}
const OneResItem* ResItemsTrie::find(QString res, QString name, QString type)
{
    int nodeid = 0;
    QHash<QString, int>::iterator mpit;
    mpit = nodes[nodeid].mp.find(res);
    if (mpit == nodes[nodeid].mp.end())
        return nullptr;
    nodeid = mpit.value();
    mpit = nodes[nodeid].mp.find(name);
    if (mpit == nodes[nodeid].mp.end())
        return nullptr;
    nodeid = mpit.value();
    mpit = nodes[nodeid].mp.find(type);
    if (mpit == nodes[nodeid].mp.end())
        return nullptr;
    nodeid = mpit.value();
    return nodes[nodeid].p;
}
bool ResItemsTrie::insert(const OneResItem* item)
{
    int nodeid = 0;
    QHash<QString, int>::iterator mpit;
    mpit = nodes[nodeid].mp.find(item->res);
    if (mpit == nodes[nodeid].mp.end()) {
        OneResItem tmp;
        tmp.pid = 1;
        tmp.res = item->res;
        tmp.mp.clear();
        itemsSum = nodes.size();
        tmp.nodeid = itemsSum;
        nodes.push_back(tmp);
        mpit = nodes[nodeid].mp.insert(item->res, tmp.nodeid);
    }
    nodeid = mpit.value();
    mpit = nodes[nodeid].mp.find(item->name);
    if (mpit == nodes[nodeid].mp.end()) {
        OneResItem tmp;
        tmp.pid = 2;
        tmp.res = item->res;
        tmp.name = item->name;
        tmp.mp.clear();
        itemsSum = nodes.size();
        tmp.nodeid = itemsSum;
        nodes.push_back(tmp);
        mpit = nodes[nodeid].mp.insert(item->name, tmp.nodeid);
    }
    nodeid = mpit.value();
    mpit = nodes[nodeid].mp.find(item->type);
    if (mpit == nodes[nodeid].mp.end()) {
        OneResItem tmp;
        tmp.pid = 3;
        tmp.res = item->res;
        tmp.name = item->name;
        tmp.type = item->type;
        tmp.mp.clear();
        itemsSum = nodes.size();
        tmp.nodeid = itemsSum;
        nodes.push_back(tmp);
        mpit = nodes[nodeid].mp.insert(item->type, tmp.nodeid);
    }
    nodeid = mpit.value();
    nodes[nodeid].mp.clear(); //当前认为同res、name、type的物品唯一
    nodes[nodeid].p = item;
    return true;
}
/*----------物品相关函数end----------*/

/*----------检查相关函数------------*/
bool regCheck(QString reg, QString s) //正则表达式格式化检查 用于检查用户名和密码格式
{
    QRegExp rx(reg);
    QRegExpValidator v(rx);
    int pos = 0;
    if (v.validate(s, pos) == QValidator::Acceptable)
        return true;
    return false;
}
bool orderCheck(OneOrder order)
{
    initResItems();
    std::unordered_map<int, double> mp;
    for (OneItem item : order.items) {
        mp[item.pid] = mp[item.pid] + item.number;
    }
    for (auto num : mp) {
        if ((getResItem(num.first)->cnt) < num.second)
            return false;
    }
    return true;
}
/*----------检查相关函数end----------*/

/*--------------功能函数end--------------*/

/*--------安全相关----------------*/
QString toSHA256(QString s)
{
    QByteArray qb = QCryptographicHash::hash(s.toUtf8(), QCryptographicHash::Sha256);
    int n = qb.size();
    QString res;
    unsigned int tmp;
    char cc;
    for (int i = 0; i < n; i++) {
        tmp = (unsigned char)qb[i];
        cc = (tmp & (0xf0)) >> 4;
        if (cc > 9)
            cc = 'a' + cc - 10;
        else
            cc = '0' + cc;
        res.append(cc);
        cc = tmp & (0xf);
        if (cc > 9)
            cc = 'a' + cc - 10;
        else
            cc = '0' + cc;
        res.append(cc);
    }
    return res;
}
/*--------安全相关end-------------*/

/*----------配置文件相关-----------*/
Config::Config(QString filename)
{
    QSettings* ini = new QSettings(filename, QSettings::IniFormat);

    ip = ini->value("server/ip", "127.0.0.1").toString();
    serverPort = ini->value("server/serverPort", 2333).toInt();
    dataPort = ini->value("server/dataPort", 3306).toInt();
    basename = ini->value("server/basename", "finaltest").toString();
    MaxHistoryOrders = ini->value("client/MaxHistoryOrders", 100).toInt();
    TableOnePageRows = ini->value("client/TableOnePageRows", 10).toInt();
    QString date = ini->value("client/orderStartDay", "2020/01/01").toString();
    orderStartDay = QDate::fromString(date, "yyyy/MM/dd");

    if (!ini->contains("server/ip"))
        ini->setValue("server/ip", "127.0.0.1");
    if (!ini->contains("server/serverPort"))
        ini->setValue("server/serverPort", 2333);
    if (!ini->contains("server/dataPort"))
        ini->setValue("server/dataPort", 3306);
    if (!ini->contains("server/basename"))
        ini->setValue("server/basename", "finaltest");
    if (!ini->contains("client/MaxHistoryOrders"))
        ini->setValue("client/MaxHistoryOrders", 100);
    if (!ini->contains("client/TableOnePageRows"))
        ini->setValue("client/TableOnePageRows", 10);
    if (!ini->contains("client/orderStartDay"))
        ini->setValue("client/orderStartDay", "2020/01/01");

    UserAgent = "ResClient";
    nowClientVersion = "0.0.1";
    statusList << QString("")
               << QString("待一级审核") << QString("一审不通过") << QString("待二级审核") << QString("二审不通过")
               << QString("审核通过(待出库)") << QString("已出库") << QString("仓库无法完成") << QString("已完成");
    itemsList << QString("耗材类") << QString("租赁类");
    itemStartWith[0] = '1';
    itemStartWith[1] = '2';
}
/*----------配置文件相关end---------*/
