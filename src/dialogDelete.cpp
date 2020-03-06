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


    this->ui->tableWidget->setRowCount(selectRows.size());
    int idx=selectRows.size()-1;
    QString temp;
    QTableWidgetItem* strItem = nullptr;
    for(auto it=selectRows.rbegin();it!=selectRows.rend();++it)
    {
        int row = *it;
        //this->ui->tableWidget->insertRow(idx);
        temp = tableWidget->item(row,SearchConst::Col::Idx::INDEX)->text();
        this->ui->tableWidget->setItem(idx,0,new QTableWidgetItem(temp));

        temp = tableWidget->item(row,SearchConst::Col::Idx::PATH)->text();
        strItem = new QTableWidgetItem(temp);
        strItem->setToolTip(temp);
        this->ui->tableWidget->setItem(idx,1,strItem);
        --idx;
    }
    return;
}
