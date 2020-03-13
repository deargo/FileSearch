#include "dialogDelete.h"
#include "ui_dialogDelete.h"
#include "searchConst.hpp"
#include <QPushButton>
#include <QStandardItemModel>

DialogDelete::DialogDelete(const QList<int> &selectRows, const QTableWidget *tableWidget, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogDelete)
{
    ui->setupUi(this);
    //设置最小窗口
    this->setMinimumSize(this->width(),this->height());
    //只显示关闭按钮
    this->setWindowFlags(Qt::Dialog|Qt::WindowCloseButtonHint);

    initTable(selectRows,tableWidget);
    //关联相关选项
    QObject::connect(this->ui->pushButton_delete_absolutely,&QPushButton::clicked,this,
                     [&](){this->choise = Choise::absolutely;this->close();});
    QObject::connect(this->ui->pushButton_delete_recyclebin,&QPushButton::clicked,this,
                     [&](){this->choise = Choise::recyclebin;this->close();});
    QObject::connect(this->ui->pushButton_delete_onlytable,&QPushButton::clicked,this,
                     [&](){this->choise = Choise::onlytable;this->close();});
    QObject::connect(this->ui->pushButton_delete_canceled,&QPushButton::clicked,this,
                     [&](){this->choise = Choise::canceled;this->close();});
    //表头排序
    QObject::connect(this->ui->tableWidget->horizontalHeader(),&QHeaderView::sectionClicked,this,&DialogDelete::recvSortTableInfo);
}

DialogDelete::~DialogDelete()
{
    delete ui;
}

void DialogDelete::initTable(const QList<int> &selectRows, const QTableWidget* tableWidget)
{
    //设置表头
    QStringList headers;
    headers << SearchConst::Col::Head::INDEX << SearchConst::Col::Head::PATH;
    this->ui->tableWidget->setColumnCount(headers.size());
    this->ui->tableWidget->setHorizontalHeaderLabels(headers);
    //最后一列不留空
    this->ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    //序列号列为自适应
    this->ui->tableWidget->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
    //表头加粗
    QFont font = this->ui->tableWidget->horizontalHeader()->font();
    font.setBold(true);
    this->ui->tableWidget->horizontalHeader()->setFont(font);
    //设置垂直头不可见
    this->ui->tableWidget->verticalHeader()->setVisible(false);
    //禁止编辑表格
    this->ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //选择为整行
    this->ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    //设置表格行数
    this->ui->tableWidget->setRowCount(selectRows.size());
    int desRow=selectRows.size()-1,srcRow=0;
    QString temp;
    QTableWidgetItem* strItem = nullptr;
    for(auto it=selectRows.rbegin();it!=selectRows.rend();++it)
    {
        srcRow = *it;
        //设置显示为数字，方便排序
        temp = tableWidget->item(srcRow,SearchConst::Col::Idx::INDEX)->text();
        strItem = new QTableWidgetItem();
        strItem->setData(Qt::DisplayRole,temp.toInt());
        strItem->setToolTip(temp);
        this->ui->tableWidget->setItem(desRow,static_cast<int>(Col::INDEX),strItem);

        temp = tableWidget->item(srcRow,SearchConst::Col::Idx::PATH)->text();
        strItem = new QTableWidgetItem();
        strItem->setData(Qt::DisplayRole,temp);
        strItem->setToolTip(temp);
        this->ui->tableWidget->setItem(desRow,static_cast<int>(Col::PATH),strItem);
        --desRow;
    }
    return;
}

void DialogDelete::recvSortTableInfo(int colIndex)
{
    if(static_cast<int>(Col::INDEX) == colIndex)
    {
        //The items are sorted ascending e.g. starts with 'AAA' ends with 'ZZZ' in Latin-1 locales
        static Qt::SortOrder lastSort = Qt::AscendingOrder;
        this->ui->tableWidget->sortByColumn(colIndex,lastSort);
        //升降排序奇偶次数交互
        lastSort = (Qt::AscendingOrder==lastSort) ? Qt::DescendingOrder : Qt::AscendingOrder;
        return;
    }
    if(static_cast<int>(Col::PATH) == colIndex)
    {
        //The items are sorted ascending e.g. starts with 'AAA' ends with 'ZZZ' in Latin-1 locales
        static Qt::SortOrder lastSort = Qt::AscendingOrder;
        this->ui->tableWidget->sortByColumn(colIndex,lastSort);
        //升降排序奇偶次数交互
        lastSort = (Qt::AscendingOrder==lastSort) ? Qt::DescendingOrder : Qt::AscendingOrder;
    }
    return;
}
