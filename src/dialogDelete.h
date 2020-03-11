#pragma once

// Coding: UTF-8(BOM)
#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif

#include <QDialog>
#include <QString>
#include <QList>
#include <QTableWidget>

namespace Ui {
class DialogDelete;
}

class DialogDelete : public QDialog
{
    Q_OBJECT

public:
    explicit DialogDelete(const QList<int>& selectRows,
                          const QTableWidget* tableWidget, QWidget *parent = 0);
    ~DialogDelete();
public slots:
    void recvSortTableInfo(int colIndex);

private:
    Ui::DialogDelete *ui;

public:
    enum class Choise{ canceled,absolutely,recyclebin,onlytable};
    Choise getChoise(){ return choise;}

private:
    enum class Col{INDEX=0,PATH=1};
    Choise choise = Choise::canceled;
    void initTable(const QList<int> &selectRows, const QTableWidget* tableWidget);
};
