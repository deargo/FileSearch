#include "dialogWait.h"
#include <QMovie>
#include <QVBoxLayout>
#include <QApplication>
#include <QPixmap>
#include <QImage>

//movieGif：相对进程的gif文件路径
DialogWait::DialogWait(const QString& movieGif, QWidget *parent /*= 0*/): QDialog(parent)
{
    pProgressPic = new QLabel(this);
    QString path = QApplication::applicationDirPath()+ "/"+movieGif;

    QMovie * mv = new QMovie(pProgressPic);
    mv->setFileName(path);
    mv->start();
    pProgressPic->setMovie(mv);
    pProgressPic->setAlignment(Qt::AlignCenter);

    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->addWidget(pProgressPic);
    pProgressBar = new QProgressBar(this);
    pProgressBar->setFixedWidth(400);
    layout->addWidget(pProgressBar);
    pProgressMsg = new QLabel(tr("Proccessing data..."),this);
    pProgressMsg->setFixedWidth(400);
    layout->addWidget(pProgressMsg);
    this->setLayout(layout);
    setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint);
}

/*设置处理的进度消息。
 * 设置一次，进度值变化一次，设置次数应该等于setSteps的值。
 * 对于耗时较久的处理，避免用户以为出问题，设置onlyInfo为true，则只更新进度消息，不改变进度值。
 * */
void DialogWait::recvMessage(const QString & msgInfo,  bool onlyInfo /*= false*/)
{
    this->pProgressMsg->setText(msgInfo);
    if(onlyInfo) return;
    if(pProgressBar->isHidden()) return;
    if(pProgressBar->value() >= pProgressBar->maximum()-1){
        pProgressBar->setValue(pProgressBar->maximum());
        return;
    }
    //实际处理的进度为/2-1
    pProgressBar->setValue(2*(currStep+1)-1);
    currStep ++;
    return;
}

//设置处理的步骤总数（即处理消息一共要几步，即调用setMessage的次数），0 表示不需要进度条，实际值为 配置值的两倍
void DialogWait::setSteps(unsigned int steps /*= 0*/)
{
    currStep = 0;
    pProgressBar->setHidden(false);
    if(steps<=0){
        pProgressBar->setHidden(true);
        return;
    }
    pProgressBar->setRange(0,2*steps);
    pProgressBar->setValue(0);
}

