#ifndef NEWORDER_H
#define NEWORDER_H

#include "hintcombobox.h"
#include "maininclude.h"
#include "messenger.h"
#include <QWidget>

namespace Ui {
class NewOrder;
}

class NewOrder : public QWidget {
    Q_OBJECT

public:
    explicit NewOrder(QWidget* parent = nullptr);
    void changeOrder(OneOrder* order); //修改某订单（订单id不变）
    void addnewOrder(); //新建订单（新建id）
    void addnewOrder_WithModel(OneOrder order); //作为模板，新建订单（新建id）
    void showOrder(OneOrder order); //展示某订单
    ~NewOrder();
signals:
    void finishChange();

protected slots:
    void afterConfirmReply(QNetworkReply*);
    void finishPost(QNetworkReply*);

    void flushBox(int);

    void on_resBox_currentTextChanged(const QString& arg1);
    void on_nameBox_currentTextChanged(const QString& arg1);
    void on_typeBox_currentTextChanged(const QString& arg1);
    void on_addItem_clicked();
    void on_deleteItem_clicked();
    void on_cancel_clicked();
    void on_confirm_clicked();
    void on_changeItem_clicked();

protected:
    void addOneItem(OneItem item);
    void setOrder(OneOrder order);
    //void postOn(QJsonObject json);

    //bool isInNewOrder = true;
    int dowhat;
    bool isInAddItem = false;
    OneOrder* theOrder = nullptr;
    QPushButton* confirm;
    Messenger* messenger;

private:
    Ui::NewOrder* ui;
};

#endif // NEWORDER_H
