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

private:
    Ui::DialogDelete *ui;

public:
    enum class Choise{ canceled,absolutely,recyclebin,onlytable};
    Choise getChoise(){ return choise;}

private:
    Choise choise = Choise::canceled;
    void initTable(const QList<int> &selectRows, const QTableWidget* tableWidget);
};
