#pragma once

// Coding: UTF-8(BOM)
#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif

#include <QDialog>
#include <QLabel>
#include <QProgressBar>
#include <functional>
#include <type_traits>
#include <QThread>
#include <QDebug>

/*使用步骤：
1：设置处理的步骤总数：setSteps；
2：设置处理函数，并等待完成：exec；
3：接收进度消息：recvMessage（信号槽机制）；
5：关闭等待框：close（信号槽机制）。
*/
class DialogWait : public QDialog
{
    Q_OBJECT
private:
    //执行函数（目前只支持无返回值的函数）
    using ExecFunc = std::function < void()>;

    //执行线程
    class ExecThread:public QThread
    {
    public:
        void setExecFunc(ExecFunc execFunc) {this->execFunc = execFunc;}
    protected:
        ExecFunc execFunc;
        void run() { execFunc();}
    };

public:
    //movieGif：相对进程的gif文件路径
    explicit DialogWait(const QString& movieGif, QWidget *parent = 0);

public slots:
    /*设置处理的进度消息。
     * 设置一次，进度值变化一次，设置次数应该等于setSteps的值。
     * 对于耗时较久的处理，避免用户以为出问题，设置onlyInfo为true，则只更新进度消息，不改变进度值。
     * */
    void recvMessage(const QString & msgInfo,  bool onlyInfo = false);
    bool close(){
        if(execThread.isRunning())
        {
            execThread.quit();
        }
        return QDialog::close();
    }

public:
    QProgressBar* progressBar() {return pProgressBar;}

    //设置处理的步骤总数（即处理消息一共要几步，即调用setMessage的次数），0 表示不需要进度条，实际值为 配置值的两倍
    void setSteps(unsigned int steps = 0);

    int exec(){ return QDialog::exec();}

    //设置处理函数：接受function、函数对象、lamda和普通函数
    template< class Func, class... Args, class = typename std::enable_if<!std::is_member_function_pointer<Func>::value>::type>
    int exec(Func && func, Args && ... args)
    {
        execThread.setExecFunc([&]{ return func(args...); });
        execThread.start();
        return QDialog::exec();
    }
    //设置处理函数：接受返回void类型的const成员函数
    template<class ClassType, class... ArgsFormat, class ClassPobj, class... ArgsFactual>
    int exec(void(ClassType::*memFunc)(ArgsFormat...) const, ClassPobj && classPobj, ArgsFactual && ... argsFactual)
    {
        execThread.setExecFunc([&, memFunc]{return (*classPobj.*memFunc)( argsFactual...); });
        execThread.start();
        return QDialog::exec();
    }
    //设置处理函数：接受返回void类型的非const成员函数
    template<class ClassType, class... ArgsFormat, class ClassPobj, class... ArgsFactual >
    int exec(void(ClassType::*memFunc)(ArgsFormat...), ClassPobj && classPobj, ArgsFactual && ... argsFactual)
    {
        execThread.setExecFunc([&, memFunc]{return (*classPobj.*memFunc)( argsFactual...); });
        execThread.start();
        return QDialog::exec();
    }

protected:
    QLabel*       pProgressMsg; //消息显示
    QLabel*       pProgressPic; //等待图片
    QProgressBar* pProgressBar; //进行条
    unsigned int  currStep;     //当前进度
    ExecThread execThread;
};

