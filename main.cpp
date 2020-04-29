#define ISMAINCPP 1
#include "login.h"
#include "maininclude.h"
#include "mainwindow.h"
#include <QApplication>

QSet<OneType> allType;
QVector<OneOrder> dealorders;
QVector<OneOrder> noworders;
QVector<OneOrder> historys;
User thisUser;
Config config("");

/*
int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    //    if (!initItems()) {
    //        return -1;
    //    }
    //Login* login = new Login();
    //login->show();
    MainWindow w;
    //w.setLoginWindow(login);
    //w.hide();
    w.show();
    return a.exec();
}
*/

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    Login* login = new Login();
    login->show();
    MainWindow w;
    w.setLoginWindow(login);
    w.hide();

    return a.exec();
}
