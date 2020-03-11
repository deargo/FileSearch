#pragma once

// Coding: UTF-8(BOM)
#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif

#include <QDialog>

namespace Ui {
class DialogHelp;
}

class DialogHelp : public QDialog
{
    Q_OBJECT

public:
    explicit DialogHelp(bool bAbout, QWidget *parent = nullptr);
    ~DialogHelp();

private:
    Ui::DialogHelp *ui;
    void showAbout();
    void showUsage();
    void addText(const QString& flage, const QString& text);
    void addUrl(const QString& flage, const QString& url);
    void addBlank();
    void addBrief();
    void addAuthor();
};
