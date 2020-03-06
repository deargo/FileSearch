#include "searchWindow.h"
#include "ui_searchWindow.h"
#include <windows.h>
#include "msgBox.hpp"
#include "dialogDelete.h"
#include "searchConst.hpp"

#include <QFont>
#include <QKeyEvent>
#include <QTextStream>
#include <QFileDialog>
#include <QStandardPaths>
#include <QSet>
#include <QList>
#include <QtAlgorithms>
#include <QProcess>


SearchWindow::SearchWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SearchWindow)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(QApplication::applicationDirPath()+"/img/icon.png"));
    this->setWindowTitle("文件搜索工具");

    //监控enter按钮事件
    this->ui->lineEdit_search_target->installEventFilter(this);

    pDialogWait = new DialogWait("img/loading.gif",this);

    initSignal();
    showProperty(false);
    searchReset("",false);
}

SearchWindow::~SearchWindow()
{
    delete ui;
    pDialogWait->close();
    delete pDialogWait;
}

void SearchWindow::initSignal()
{
    QObject::connect(this->ui->pushButton_search_browse_dir,&QPushButton::clicked,this,&SearchWindow::recvBtnBrowseDir);
    QObject::connect(this->ui->pushButton_search_browse_file,&QPushButton::clicked,this,&SearchWindow::recvBtnBrowseFile);
    QObject::connect(this->ui->pushButton_search_reset,&QPushButton::clicked,this,&SearchWindow::recvBtnSearchReset);
    QObject::connect(this->ui->pushButton_search_begin,&QPushButton::clicked,this,&SearchWindow::recvBtnSearchBegin);
    QObject::connect(this->ui->pushButton_delete,&QPushButton::clicked,this,&SearchWindow::recvBtnSearchDelete);
    QObject::connect(this->ui->pushButton_export,&QPushButton::clicked,this,&SearchWindow::recvBtnSearchExport);

    //列表单击处理
    QObject::connect(this->ui->tableWidget,&QTableWidget::clicked,[&](const QModelIndex &){
        this->ui->pushButton_delete->setEnabled(true);
        this->ui->pushButton_export->setEnabled(true);
    });
    //列表双击处理
    QObject::connect(this->ui->tableWidget,&QTableWidget::doubleClicked,[&](const QModelIndex &){
        this->recvBtnSearchDelete();
    });
    //等待框消息
    QObject::connect(this,&SearchWindow::sendWaitMessage,pDialogWait,&DialogWait::recvMessage);
    QObject::connect(this,&SearchWindow::sendWaitClose,pDialogWait,&DialogWait::close);
    //互斥条件（全字匹配和正则表达式，只能一个条件选到）
    QObject::connect(this->ui->checkBox_filename_include_whole_word,&QCheckBox::clicked,[&](bool isChecked){
        if(isChecked) this->ui->checkBox_filename_include_regex->setCheckState(Qt::Unchecked);
    });
    QObject::connect(this->ui->checkBox_filename_include_regex,&QCheckBox::clicked,[&](bool isChecked){
        if(isChecked) this->ui->checkBox_filename_include_whole_word->setCheckState(Qt::Unchecked);
    });
    QObject::connect(this->ui->checkBox_filename_exclude_whole_word,&QCheckBox::clicked,[&](bool isChecked){
        if(isChecked) this->ui->checkBox_filename_exclude_regex->setCheckState(Qt::Unchecked);
    });
    QObject::connect(this->ui->checkBox_filename_exclude_regex,&QCheckBox::clicked,[&](bool isChecked){
        if(isChecked) this->ui->checkBox_filename_exclude_whole_word->setCheckState(Qt::Unchecked);
    });
    QObject::connect(this->ui->checkBox_content_include_whole_word,&QCheckBox::clicked,[&](bool isChecked){
        if(isChecked) this->ui->checkBox_content_include_regex->setCheckState(Qt::Unchecked);
    });
    QObject::connect(this->ui->checkBox_content_include_regex,&QCheckBox::clicked,[&](bool isChecked){
        if(isChecked) this->ui->checkBox_content_include_whole_word->setCheckState(Qt::Unchecked);
    });
    //文件菜单
    QObject::connect(this->ui->action_file_window_new,&QAction::triggered,[&](){
        QProcess newWind;
        newWind.startDetached(QCoreApplication::applicationFilePath());
    });
    QObject::connect(this->ui->action_file_browse_dir,&QAction::triggered,this,&SearchWindow::recvBtnBrowseDir);
    QObject::connect(this->ui->action_file_browse_file,&QAction::triggered,this,&SearchWindow::recvBtnBrowseFile);
    QObject::connect(this->ui->action_file_window_exit,&QAction::triggered,[&](){return this->close(); });
    //搜索菜单
    QObject::connect(this->ui->action_search_begin,&QAction::triggered,this,&SearchWindow::recvBtnSearchBegin);
    QObject::connect(this->ui->action_search_reset,&QAction::triggered,this,&SearchWindow::recvBtnSearchReset);
    QObject::connect(this->ui->action_search_export,&QAction::triggered,this,&SearchWindow::recvBtnSearchExport);
    QObject::connect(this->ui->action_search_delete,&QAction::triggered,this,&SearchWindow::recvBtnSearchDelete);

}

bool SearchWindow::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == this->ui->lineEdit_search_target && event->type()==QEvent::KeyPress)
    {
        //监测到按键消息
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if(keyEvent->key() == Qt::Key_Return)
        {
            //监测到回车键消息
            QString target = this->ui->lineEdit_search_target->text();
            searchReset(target,false);
            return true;
        }
    }
    return QMainWindow::eventFilter(watched,event);
}

void SearchWindow::recvFocusChanged(QWidget *old, QWidget *now)
{
    if(old && now && old->objectName()=="tableWidget")
    {
        //表格失去焦点，则屏蔽着两个按钮
        this->ui->pushButton_delete->setEnabled(false);
        this->ui->pushButton_export->setEnabled(false);
    }
    if(now)
    {
        //按钮事件，触发一个信号
        if(now->objectName()== "pushButton_delete") emit this->ui->pushButton_delete->clicked(true);
        if(now->objectName()== "pushButton_export") emit this->ui->pushButton_export->clicked(true);
    }
    return;
}

void SearchWindow::recvBtnBrowseDir()
{
    QString search_browse_dir = QFileDialog::getExistingDirectory(this,"请选择搜索目录");
    if(search_browse_dir.isEmpty()) return;
    searchReset(search_browse_dir,false);
}

void SearchWindow::recvBtnBrowseFile()
{
    QStringList search_browse_files = QFileDialog::getOpenFileNames(this,"请选择搜索文件");
    if(search_browse_files.isEmpty()) return;
    searchReset(search_browse_files.join(SearchConst::multi_split),false);
}

void SearchWindow::searchReset(const QString& target, bool bAsk)
{
    if(bAsk)
    {
        bool isReset = MSG_BOX_askRtnTrue("重置提示","确定重置条件和结果？");
        if(false == isReset)
        {
            return;
        }
    }
    //重置表数据
    showProperty(true);

    bool targetValid = checkTargets(target,false);

    SearchImpl::CTarget targetObj(target);
    bool targetNotEmpty = targetObj.allCount()>0;
    bool targetHasDir = targetObj.dirCount()>0;

    this->ui->lineEdit_search_target->setText(target);
    if(this->ui->lineEdit_search_target->isEnabled() && this->ui->lineEdit_search_target->text().isEmpty())
    {
        this->ui->lineEdit_search_target->setPlaceholderText("文件或文件夹（多个则分号分割，但不能同时包含）");
    }

    this->ui->lineEdit_search_dir_include->setEnabled(targetHasDir && targetValid);
    this->ui->lineEdit_search_dir_include->setText("");
    if(this->ui->lineEdit_search_dir_include->isEnabled() && this->ui->lineEdit_search_dir_include->text().isEmpty())
    {
        this->ui->lineEdit_search_dir_include->setPlaceholderText("文件夹名");
    }

    this->ui->lineEdit_search_dir_exclude->setEnabled(targetHasDir && targetValid);
    this->ui->lineEdit_search_dir_exclude->setText("");
    if(this->ui->lineEdit_search_dir_exclude->isEnabled() && this->ui->lineEdit_search_dir_exclude->text().isEmpty())
    {
        this->ui->lineEdit_search_dir_exclude->setPlaceholderText("文件夹名");
    }

    this->ui->lineEdit_filename_exclude->setEnabled(targetHasDir && targetValid);
    this->ui->lineEdit_filename_exclude->setText("");
    this->ui->checkBox_filename_exclude_casesensitive->setEnabled(targetHasDir && targetValid);
    this->ui->checkBox_filename_exclude_casesensitive->setChecked(false);
    this->ui->checkBox_filename_exclude_whole_word->setEnabled(targetHasDir && targetValid);
    this->ui->checkBox_filename_exclude_whole_word->setChecked(false);
    this->ui->checkBox_filename_exclude_regex->setEnabled(targetHasDir && targetValid);
    this->ui->checkBox_filename_exclude_regex->setChecked(false);
    if(this->ui->lineEdit_filename_exclude->isEnabled() && this->ui->lineEdit_filename_exclude->text().isEmpty())
    {
        this->ui->lineEdit_filename_exclude->setPlaceholderText("文件名");
    }

    this->ui->lineEdit_filename_include->setEnabled(targetHasDir && targetValid);
    this->ui->lineEdit_filename_include->setText("");
    this->ui->checkBox_filename_include_casesensitive->setEnabled(targetHasDir && targetValid);
    this->ui->checkBox_filename_include_casesensitive->setChecked(false);
    this->ui->checkBox_filename_include_whole_word->setEnabled(targetHasDir && targetValid);
    this->ui->checkBox_filename_include_whole_word->setChecked(false);
    this->ui->checkBox_filename_include_regex->setEnabled(targetHasDir && targetValid);
    this->ui->checkBox_filename_include_regex->setChecked(false);
    if(this->ui->lineEdit_filename_include->isEnabled() && this->ui->lineEdit_filename_include->text().isEmpty())
    {
        this->ui->lineEdit_filename_include->setPlaceholderText("文件名");
    }

    this->ui->lineEdit_content_include->setEnabled(targetNotEmpty && targetValid);
    this->ui->lineEdit_content_include->setText("");
    this->ui->checkBox_content_include_casesensitive->setEnabled(targetNotEmpty && targetValid);
    this->ui->checkBox_content_include_casesensitive->setChecked(false);
    this->ui->checkBox_content_include_whole_word->setEnabled(targetNotEmpty && targetValid);
    this->ui->checkBox_content_include_whole_word->setChecked(false);
    this->ui->checkBox_content_include_regex->setEnabled(targetNotEmpty && targetValid);
    this->ui->checkBox_content_include_regex->setChecked(false);
    if(this->ui->lineEdit_content_include->isEnabled() && this->ui->lineEdit_content_include->text().isEmpty())
    {
        this->ui->lineEdit_content_include->setPlaceholderText("字符串");
    }

    this->ui->pushButton_delete->setEnabled(false);
    this->ui->pushButton_export->setEnabled(false);


    if(this->ui->lineEdit_search_target->isEnabled() && this->ui->lineEdit_search_target->text().isEmpty())
    {
        this->ui->lineEdit_search_target->setPlaceholderText("文件或文件夹（多个则分号分割，但不能同时包含）");
    }
}

bool SearchWindow::checkTargets(const QString& targets,bool bShowMsg)
{
    if(targets.isEmpty()){
        if(bShowMsg)
        {
            MSG_BOX_warn("搜索目标-空值");
        }
        return false;
    }
    QStringList targetlist = targets.split(SearchConst::multi_split);
    for (const auto& target: targetlist)
    {
        if(QFileInfo::exists(target)) continue;
        if(bShowMsg)
        {
            MSG_BOX_warn("搜索目标-路径非法：\n"+target);
        }
        return false;
    }
    return true;
}

bool SearchWindow::checkCondtions()
{
    auto checkPath = [&](const QString& object, const QLineEdit* lineEdit){
        QString value = lineEdit->text();
        if(lineEdit->isEnabled() && (value.contains('/') || value.contains('\\')))
        {
            MSG_BOX_warn(object+"不支持路径\n"+value);
            return false;
        }
        return true;
    };
    return checkPath("\"包含目录\" - ",this->ui->lineEdit_search_dir_include) &&
            checkPath("\"排除目录\" - ",this->ui->lineEdit_search_dir_exclude) &&
            checkPath("\"包含文件\" - ",this->ui->lineEdit_filename_include) &&
            checkPath("\"排除文件\" - ",this->ui->lineEdit_filename_exclude);
}

SearchImpl::CCondition SearchWindow::getCondition()
{
    SearchImpl::CCondition condition;

    condition.dir_include.upsert(this->ui->lineEdit_search_dir_include->text());
    condition.dir_exclude.upsert(this->ui->lineEdit_search_dir_exclude->text());

    condition.filename_include.upsert(this->ui->lineEdit_filename_include->text());
    condition.filename_include.casesensitive = condition.filename_include.enabled()&&
            this->ui->checkBox_filename_include_casesensitive->isEnabled()&&
            this->ui->checkBox_filename_include_casesensitive->isChecked();
    condition.filename_include.whole_word =condition.filename_include.enabled()&&
            this->ui->checkBox_filename_include_whole_word->isEnabled()&&
            this->ui->checkBox_filename_include_whole_word->isChecked();
    condition.filename_include.regex_match =condition.filename_include.enabled()&&
            this->ui->checkBox_filename_include_regex->isEnabled()&&
            this->ui->checkBox_filename_include_regex->isChecked();

    condition.filename_exclude.upsert(this->ui->lineEdit_filename_exclude->text());
    condition.filename_exclude.casesensitive = condition.filename_exclude.enabled() &&
            this->ui->checkBox_filename_exclude_casesensitive->isEnabled()&&
            this->ui->checkBox_filename_exclude_casesensitive->isChecked();
    condition.filename_exclude.whole_word = condition.filename_exclude.enabled() &&
            this->ui->checkBox_filename_exclude_whole_word->isEnabled()&&
            this->ui->checkBox_filename_exclude_whole_word->isChecked();
    condition.filename_exclude.regex_match = condition.filename_exclude.enabled() &&
            this->ui->checkBox_filename_exclude_regex->isEnabled()&&
            this->ui->checkBox_filename_exclude_regex->isChecked();

    condition.content_include.upsert(this->ui->lineEdit_content_include->text());
    condition.content_include.casesensitive = condition.content_include.enabled()&&
            this->ui->checkBox_content_include_casesensitive->isEnabled()&&
            this->ui->checkBox_content_include_casesensitive->isChecked();
    condition.content_include.whole_word =condition.content_include.enabled()&&
            this->ui->checkBox_content_include_whole_word->isEnabled()&&
            this->ui->checkBox_content_include_whole_word->isChecked();
    condition.content_include.regex_match =condition.content_include.enabled()&&
            this->ui->checkBox_content_include_regex->isEnabled()&&
            this->ui->checkBox_content_include_regex->isChecked();

    return condition;
}

void SearchWindow::searchAndShow(const SearchImpl::CTarget& targetObj, const SearchImpl::CCondition& condition)
{
    SearchImpl::CResultVec resultVec;
    SearchImpl::CTarget::List searchList;
    const SearchImpl::CTarget::List& targetList = targetObj.list();
    for(const auto& targetFileInfo : targetList)
    {
        searchList.clear();
        if(targetFileInfo.isDir())
        {
            //获取目录下层文件，并放入搜索列表
            SearchImpl::CTarget::GetFileList(targetFileInfo.absoluteFilePath(),searchList);
        } else
        {
            //目标是文件，放入搜索列表
            searchList.push_back(targetFileInfo);
        }
        //开始整数搜索处理（不使用递归，文件目录过大时，递归异常）
        searchByQueue(condition,searchList,resultVec);
        //searchByRecursion(condition,targetFileInfo,resultVec);
    }

    this->ui->pushButton_delete->setEnabled(false);
    this->ui->pushButton_export->setEnabled(false);

    //输出到表中
    showResult(resultVec,condition.content_include.enabled());
    emit sendWaitClose();
    return;
}

void SearchWindow::searchByQueue(const SearchImpl::CCondition& condition, SearchImpl::CTarget::List& searchList, SearchImpl::CResultVec &resultVec) const
{
    QString targetPath;
    QFileInfo target;
    while(!searchList.isEmpty()){
        target = searchList.first();
        searchList.pop_front();

        targetPath = target.absoluteFilePath();
        qDebug()<<"handle: "<<targetPath;
        emit sendWaitMessage("正在处理："+targetPath);

        SearchImpl::CResult searchResult(target);
        if(condition.matched(target,searchResult.lineInfo))
        {
            if(target.isDir())
            {
                //继续获取下层文件列表
                SearchImpl::CTarget::GetFileList(targetPath,searchList);
                continue;
            }
            qDebug()<< "result: "<<target.absoluteFilePath();
            resultVec.push_back(std::move(searchResult));
        }
    }
    return;
}

void SearchWindow::searchByRecursion(const SearchImpl::CCondition& condition, const QFileInfo &target, SearchImpl::CResultVec &resultVec) const
{
    //使用递归搜索
    if(target.fileName()=="." || target.fileName()=="..") return;
    if(target.isDir())
    {
        //继续获取下层文件列表
        QDir dir(target.absoluteFilePath());
        QFileInfoList fileInfoList = dir.entryInfoList();
        for(const auto& finfo:fileInfoList){
            if(finfo.fileName()=="." || finfo.fileName()=="..") continue;
            emit sendWaitMessage("正在处理："+finfo.fileName(),true);
            searchByRecursion(condition,finfo,resultVec);
        }
        return;
    }

    SearchImpl::CResult searchResult(target);
    if(condition.matched(target,searchResult.lineInfo))
    {
        resultVec.push_back(std::move(searchResult));
        qDebug()<< "result: "<<target.absoluteFilePath();
        return ;
    }
    return;
}

void SearchWindow::showProperty(bool searchContent)
{
    //删除所有行
    this->ui->tableWidget->clear();
    this->ui->tableWidget->setRowCount(0);
    this->ui->tableWidget->clearContents();
    //设置表头
    QStringList headers;
    headers << SearchConst::Col::Head::INDEX << SearchConst::Col::Head::TYPE << (searchContent ? SearchConst::Col::Head::LINE : SearchConst::Col::Head::SIZE) << SearchConst::Col::Head::PATH;
    this->ui->tableWidget->setColumnCount(headers.size());
    this->ui->tableWidget->setHorizontalHeaderLabels(headers);
    //最后一列不留空
    this->ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    //默认均分列宽
    //this->ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //表头加粗
    QFont font = this->ui->tableWidget->horizontalHeader()->font();
    font.setBold(true);
    this->ui->tableWidget->horizontalHeader()->setFont(font);
    //设置垂直头不可见
    this->ui->tableWidget->verticalHeader()->setVisible(false);
    //点击表时不对表头行光亮（获取焦点）
    this->ui->tableWidget->horizontalHeader()->setHighlightSections(false);
    //禁止编辑表格
    this->ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //选择为整行
    this->ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    //设置可以选中多个目标(Ctrl多选)
    this->ui->tableWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    return;
}

void SearchWindow::showResult(const SearchImpl::CResultVec &resultVec, bool searchContent)
{
    if(resultVec.empty())
    {
        this->ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        return;
    }
    this->ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    QString temp;
    QTableWidgetItem *strItem = nullptr;
    int rows = resultVec.size();
    for(int row =0; row< rows;++row)
    {
        int ratio = row*1.0/rows*100;
        if(ratio%5 ==0)
        {
            emit sendWaitMessage(QString::asprintf("正在展示 %%%02d ...",ratio));
        }
        const SearchImpl::CResult& searchResult = resultVec.at(row);
        this->ui->tableWidget->insertRow(row);
        //序号
        temp = QString::number(row+1);
        strItem = new QTableWidgetItem(temp);
        strItem->setToolTip(temp);
        this->ui->tableWidget->setItem(row,SearchConst::Col::Idx::INDEX,strItem);
        //类型
        temp = SearchImpl::CResult::GetFileType(temp);
        strItem = new QTableWidgetItem(temp);
        strItem->setToolTip(temp);
        this->ui->tableWidget->setItem(row,SearchConst::Col::Idx::TYPE,strItem);

        if(searchContent)
        {
            //所在行
            temp = SearchImpl::CResult::ToString(searchResult.lineInfo,SearchConst::multi_split);
            strItem = new QTableWidgetItem(temp);
            strItem->setToolTip(temp);
            this->ui->tableWidget->setItem(row,SearchConst::Col::Idx::LINE,strItem);
        } else {
            //大小
            temp = SearchWindow::FileSize(searchResult.fileInfo.size());
            strItem = new QTableWidgetItem(temp);
            strItem->setToolTip(QString::number(searchResult.fileInfo.size())+"字节");
            this->ui->tableWidget->setItem(row,SearchConst::Col::Idx::SIZE,strItem);
        }
        //路径
        temp = searchResult.fileInfo.filePath();
        strItem = new QTableWidgetItem(temp);
        strItem->setToolTip(temp);
        //strItem->setTextAlignment(Qt::AlignCenter);
        this->ui->tableWidget->setItem(row,SearchConst::Col::Idx::PATH,strItem);
    }
    return;
}

void SearchWindow::recvBtnSearchReset()
{
    searchReset("",this->ui->tableWidget->rowCount());
    return;
}

void SearchWindow::recvBtnSearchBegin()
{
    QString target = this->ui->lineEdit_search_target->text();
    if(!checkTargets(this->ui->lineEdit_search_target->text(),true) || !checkCondtions()) return;
    SearchImpl::CCondition condition = getCondition();
    showProperty(condition.content_include.enabled());

    SearchImpl::CTarget targetObj(target);
    pDialogWait->setSteps(targetObj.allCount());
    pDialogWait->exec(&SearchWindow::searchAndShow,this,targetObj,condition);

    if(this->ui->tableWidget->rowCount()==0)
    {
        emit sendWaitClose();
        MSG_BOX_warn("搜索结果为空");
        return;
    }
    return;
}

void SearchWindow::recvBtnSearchExport()
{
    if(this->ui->tableWidget->rowCount()==0)
    {
        MSG_BOX_warn("表数据为空");
        return;
    }
    QString search_browse_dir = QFileDialog::getExistingDirectory(this,"请选择保存目录",
                                                                  QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
    if(search_browse_dir.isEmpty())
    {
        MSG_BOX_warn("选择保存目录为空");
        return;
    }

    QString exportPath = search_browse_dir + "/export.csv";
    QFile exportFile(exportPath);
    if( !exportFile.open(QFile::ReadWrite|QFile::Text|QFile::Truncate))
    {
        MSG_BOX_warn(QString("写导出文件失败："+exportPath));
        return;
    }
    QString temp = this->ui->tableWidget->horizontalHeaderItem(SearchConst::Col::Idx::SIZE)->text();
    bool searchContent = (SearchConst::Col::Head::LINE==temp);
    QTextStream textStream(&exportFile);
    textStream.setCodec("GB2312");
    textStream << "\"" << SearchConst::Col::Head::INDEX << "\",";
    textStream << "\"" << SearchConst::Col::Head::TYPE << "\",";
    textStream << "\"" << (searchContent ? SearchConst::Col::Head::LINE : SearchConst::Col::Head::SIZE) << "\",";
    textStream << "\"" << SearchConst::Col::Head::PATH << "\"\n";

    for(int row=0;row <this->ui->tableWidget->rowCount();++row)
    {
        textStream << "\"" << this->ui->tableWidget->item(row,SearchConst::Col::Idx::INDEX)->text() << "\",";
        textStream << "\"" << this->ui->tableWidget->item(row,SearchConst::Col::Idx::TYPE)->text() << "\",";
        temp = this->ui->tableWidget->item(row,SearchConst::Col::Idx::PATH)->text();
        temp = searchContent ? this->ui->tableWidget->item(row,SearchConst::Col::Idx::LINE)->text()
                             : this->ui->tableWidget->item(row,SearchConst::Col::Idx::SIZE)->text() ;
        textStream << "\"" << temp << "\",";
        textStream << "\"" << this->ui->tableWidget->item(row,SearchConst::Col::Idx::PATH)->text() << "\"\n";
    }
    exportFile.close();
    MSG_BOX_info(QString("保存至文件：\n"+exportPath));
    return;
}

void SearchWindow::recvBtnSearchDelete()
{
    if(this->ui->tableWidget->rowCount()==0)
    {
        MSG_BOX_warn("表数据为空");
        return;
    }
    if( this->ui->tableWidget->selectedRanges().empty())
    {
        MSG_BOX_warn("所选目标为空");
        return;
    }

    //不使用selectedRanges，因为Ctrl+A，和Shift按钮选中的，不没有选中
    QList<QTableWidgetItem*> selectItems = this->ui->tableWidget->selectedItems();
    QSet<int> selectRows;
    for(const auto& item: selectItems)
    {
        //先把选中的所在行记录下来
        selectRows.insert(item->row());
    }
    QList<int> selectList = selectRows.toList();
    qSort(selectList);

    //删除选项
    DialogDelete dialogDelete(selectList,this->ui->tableWidget,this);
    dialogDelete.exec();

    DialogDelete::Choise choise = dialogDelete.getChoise();
    if(DialogDelete::Choise::canceled == choise )
    {
        return;
    }

    //从后往前删除
    QString filepath;
    for(auto it = selectRows.rbegin();it!=selectRows.rend();++it)
    {
        int row = *it;
        filepath = this->ui->tableWidget->item(row,SearchConst::Col::Idx::PATH)->text();
        if(DialogDelete::Choise::absolutely == choise )
        {
            QFile::remove(filepath);
        } else if(DialogDelete::Choise::recyclebin == choise )
        {
            SearchWindow::RrecycleFile(filepath);
        }
        this->ui->tableWidget->removeRow(row);
    }
    return;
}

QString SearchWindow::FileSize(qint64 size)
{
    static const double unit = 1024.0;
    double dSize = size*1.0;
    if(dSize<=unit) return QString::number(size)+" 字节";
    dSize = dSize/unit;
    if(dSize<=unit) return QString::number(dSize,'g',3)+" KB";
    dSize = dSize/unit;
    if(dSize<=unit) return QString::number(dSize,'g',3)+" MB";
    dSize = dSize/unit;
    if(dSize<=unit) return QString::number(dSize,'g',3)+" GB";
    dSize = dSize/unit;
    if(dSize<=unit) return QString::number(dSize,'g',3)+" TB";
    dSize = dSize/unit;
    return QString::number(dSize,'g',3)+" PB";
}

bool SearchWindow::RrecycleFile(const QString&filePath)
{
    //防止中文乱码
    std::string path(filePath.toLocal8Bit());
    int num = MultiByteToWideChar(0,0,path.c_str(),-1,NULL,0);
    TCHAR buf[_MAX_PATH + 1] ={0};
    MultiByteToWideChar(0,0,path.c_str(),-1,buf,num);

    SHFILEOPSTRUCT  shDelFile;
    memset(&shDelFile,0,sizeof(SHFILEOPSTRUCT));
    shDelFile.hwnd = NULL;
    shDelFile.wFunc = FO_DELETE;           //执行的操作
    shDelFile.pFrom = buf;                 //操作的对象
    shDelFile.pTo = L"\0\0";               //必须设置为NULL
    shDelFile.fFlags |= FOF_SILENT;        //不显示进度
    shDelFile.fFlags |= FOF_NOERRORUI;     //不报告错误信息
    shDelFile.fFlags |= FOF_NOCONFIRMATION;//直接删除，不进行确认
    shDelFile.fFlags |= FOF_ALLOWUNDO;     //删除到回收站
    if( 0!=SHFileOperation(&shDelFile)) return false; //执行删除
    return !shDelFile.fAnyOperationsAborted;
}
