#include "dialogHelp.h"
#include "ui_dialogHelp.h"
#include <QDate>
#include <QTime>
#include <QUrl>
#include <QRect>
#include <QFile>
#include <QTextStream>
#include <windows.h>

DialogHelp::DialogHelp(QWidget *parent, Type type) :
    QDialog(parent),
    ui(new Ui::DialogHelp)
{
    ui->setupUi(this);
    //设置水平滚动条不可见
    this->ui->textBrowser->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //不可编辑
    this->ui->textBrowser->setReadOnly(true);
    //使用系统默认浏览器打开
    this->ui->textBrowser->setOpenLinks(false);
    QObject::connect(this->ui->textBrowser, &QTextBrowser::anchorClicked,[&](const QUrl& url)
    {
        ShellExecuteA(NULL, "open", url.toString().toStdString().c_str(), "", "", SW_SHOW);
    });
    //设置最小和最大窗口
    QRect rect = parent->geometry();
    this->setMinimumSize(this->width(),this->height());
    this->setMaximumSize(rect.width()-20,rect.height()-20);

    //设置窗体只有关闭按钮
    Qt::WindowFlags windowFlag  = Qt::Dialog;
    windowFlag |= Qt::WindowCloseButtonHint;
    this->setWindowFlags(windowFlag);

    if(Type::ABOUT == type)
    {
        showAbout();
    } else if(Type::USAGE == type) {
        showUsage();
    } else {
    }
    //默认滚动到最上面
    this->ui->textBrowser->moveCursor(QTextCursor::Start);
}

DialogHelp::~DialogHelp()
{
    delete ui;
}

void DialogHelp::showAbout()
{
    this->setWindowTitle("关于工具");
    static const QDate buildDate = QLocale( QLocale::English ).toDate( QString(__DATE__).replace("  ", " 0"), "MMM dd yyyy");
    static const QTime buildTime = QTime::fromString(__TIME__, "hh:mm:ss");
    addText("更新时间",buildDate.toString("yyyy-MM-dd")+" "+buildTime.toString("hh:mm:ss"));
    addBlank();
    addBrief();
    addBlank();
    addAuthor();
}
void DialogHelp::showUsage()
{
    //禁止缩放
    this->setWindowTitle("使用帮助");
    addBrief();
    addBlank();
    addText("搜索目标","支持选择文件夹，或者目录；支持手动输入（按入Enter键后会重置搜索条件）。");
    addText("搜索条件","包含目录、排除目录、包含文件和排除文件，不支持路径。文件名文件内容搜索时，支持大小写匹配、全字匹配和正则表达式匹配的规则");
    addText("列表选择","支持单击、双击、Ctrl和Shift键操作。");
    addText("列表操作","支持Ctrl+C复制路径、拖动列宽、表头排序、表头双击自适应列宽。");
    addText("列表右键","拷贝路径、文件属性、打开所在目录、删除选项");
    addText("列表删除","支持列表选择项的：从列表删除、彻底删除，和删除到回收站。");
    addText("列表导出","导出列表中的全部数据为CSV文件，到指定目录。");
    addText("其他功能","支持多个目标，多个条件的操作，但需使用英文分号[;]隔开。");
    addBlank();
    addAuthor();
}

void DialogHelp::addText(const QString& flage, const QString& text)
{
    QString textInfo = flage;
    if(textInfo.isEmpty())
    {
        textInfo += text;
    }else {
        textInfo += "：";
        textInfo += "<font color='#5500ff' size='+1' face='Sans'>";
        textInfo += text;
        textInfo += "</font>";
    }
    this->ui->textBrowser->append(textInfo);
}

void DialogHelp::addUrl(const QString& flage, const QString& url)
{
    QString urlInfo = "<a href = \""+url+ "\">"+url+"</a>";
    this->ui->textBrowser->append(flage+ "："+urlInfo);
}

void DialogHelp::addBlank()
{
    addText("","");
}
void DialogHelp::addBrief()
{
    addText("工具简介","Windows文件搜索工具。支持指定目录和文件，并按照文件名，或文件内容的进行搜索。对于搜索结果，支持删除列表、彻底删除、删除到回收、排序和导出CSV操作。对展示列表支持单击、双击、右键和表头排序操作。");
}

void DialogHelp::addAuthor()
{
    addUrl("源码地址","https://github.com/deargo/FileSearch");
    addUrl("博客地址","https://blog.csdn.net/aguoxin/article/details/104700809");
    addBlank();
    addText("联系作者","欢迎指出bug、提新需求，请前往博客留言。");
}

