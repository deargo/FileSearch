#pragma once

// Coding: UTF-8(BOM)
#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif

#include <QString>
#include <QTimer>
#include <QMessageBox>
#include <QPushButton>
#include <QDebug>

/*
 * 消息框类
 * 工具交互时产生的消息框，设置显示时间，超时之后还没有点击确定，就自动关闭
 */
class CMsgBox
{
    CMsgBox();
public:
    enum Type{
        Info,
        Warn,
        Error
    };
    enum {DISAPPEAR_SECEND = 10};
    static void myBox(QWidget *parent, Type msgType,const QString &msg,int disappearSecend=CMsgBox::DISAPPEAR_SECEND){
        QMessageBox* msgBox = new QMessageBox(parent);
        QTimer* timer = new QTimer(parent);
        QObject::connect(timer,&QTimer::timeout,msgBox,&QMessageBox::close);
        QObject::connect(timer,&QTimer::timeout,msgBox,&QMessageBox::deleteLater);
        QObject::connect(msgBox,&QMessageBox::destroyed,timer,&QTimer::deleteLater);
        switch (msgType) {
        case Warn:
            msgBox->setWindowTitle(QObject::tr("警告！"));
            msgBox->setIcon(QMessageBox::Warning);
            qWarning() << msg;
            break;
        case Error:
            msgBox->setWindowTitle(QObject::tr("错误！"));
            msgBox->setIcon(QMessageBox::Warning);
            qCritical() << msg;
            break;
        default:
            msgBox->setWindowTitle(QObject::tr("信息！"));
            msgBox->setIcon(QMessageBox::Information);
            qInfo() << msg;
            break;
        }
        msgBox->setText(msg);
        //QAbstractButton* yesBtn = new QPushButton();
        //yesBtn->setText("OK");
        QPushButton* yesBtn = msgBox->addButton("OK", QMessageBox::YesRole);
        QObject::connect(yesBtn,&QPushButton::click,msgBox,&QMessageBox::close);
        QObject::connect(yesBtn,&QPushButton::click,msgBox,&QMessageBox::deleteLater);
        //yesBtn->deleteLater();
        timer->start(disappearSecend*1000);
        msgBox->show();
    }
    inline static void myBox(QWidget *parent, Type msgType,const char* msg,int disappearSecend=CMsgBox::DISAPPEAR_SECEND){ myBox(parent,msgType,QString(msg),disappearSecend); }
    inline static void myBox(QWidget *parent, Type msgType,const std::string &msg,int disappearSecend=CMsgBox::DISAPPEAR_SECEND){ myBox(parent,msgType,QString::fromStdString(msg),disappearSecend); }
    //QMessageBox::question
    inline static bool question(QWidget *parent, const QString &title, const QString &msg){
        return QMessageBox::question(parent,title,msg,QMessageBox::Ok|QMessageBox::No,QMessageBox::No)==QMessageBox::Ok;
    }
    inline static bool question(QWidget *parent, const QString &title, const char* msg){ return question(parent,title,QString(msg)); }
    inline static bool question(QWidget *parent, const QString &title, const std::string &msg){ return question(parent,title,QString::fromStdString(msg));}
    inline static bool question(QWidget *parent, const char* title, const char* msg){ return question(parent,QString(title),QString(msg)); }
    inline static bool question(QWidget *parent, const char* title, const QString &msg){ return question(parent,QString(title),msg);}
    inline static bool question(QWidget *parent, const char* title, const std::string &msg){ return question(parent,QString(title),QString::fromStdString(msg));}
    inline static bool question(QWidget *parent, const std::string &title, const char* msg){ return question(parent,QString::fromStdString(title),QString(msg)); }
    inline static bool question(QWidget *parent, const std::string &title, const QString &msg){ return question(parent,QString::fromStdString(title),msg);}
    inline static bool question(QWidget *parent, const std::string &title, const std::string &msg){ return question(parent,QString::fromStdString(title),QString::fromStdString(msg));}
    //QMessageBox::about
    inline static void about(QWidget *parent, const QString &title, const QString &msg){
        QMessageBox::about(parent,title,msg);
    }
    inline static void about(QWidget *parent, const QString &title, const char* msg){ about(parent,title,QString(msg)); }
    inline static void about(QWidget *parent, const QString &title, const std::string &msg){ about(parent,title,QString::fromStdString(msg));}
    inline static void about(QWidget *parent, const char* title, const char* msg){ about(parent,QString(title),QString(msg)); }
    inline static void about(QWidget *parent, const char* title, const QString &msg){ about(parent,QString(title),msg);}
    inline static void about(QWidget *parent, const char* title, const std::string &msg){ about(parent,QString(title),QString::fromStdString(msg));}
    inline static void about(QWidget *parent, const std::string &title, const char* msg){ about(parent,QString::fromStdString(title),QString(msg)); }
    inline static void about(QWidget *parent, const std::string &title, const QString &msg){ about(parent,QString::fromStdString(title),msg);}
    inline static void about(QWidget *parent, const std::string &title, const std::string &msg){ about(parent,QString::fromStdString(title),QString::fromStdString(msg));}
};

//传入参数只能是char*,QString和std::string类型
#define MSG_BOX_about(title,msg) CMsgBox::about(this,(title),(msg))
#define MSG_BOX_askRtnTrue(title,msg) (CMsgBox::question(this,(title),(msg)))
#define MSG_BOX_askRtnFalse(title,msg) (!CMsgBox::question(this,(title),(msg)))
#define MSG_BOX_info(msg) CMsgBox::myBox(this,CMsgBox::Info,(msg))
#define MSG_BOX_warn(msg) CMsgBox::myBox(this,CMsgBox::Warn,(msg))
#define MSG_BOX_error(msg) CMsgBox::myBox(this,CMsgBox::Error,(msg))
