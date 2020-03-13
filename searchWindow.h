#pragma once

// Coding: UTF-8(BOM)
#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif

#include <QMainWindow>
#include <QCheckBox>
#include <QMenu>
#include <QAction>
#include "searchImpl.h"
#include "dialogWait.h"
#include "dialogDelete.h"

namespace Ui {
class SearchWindow;
}

class SearchWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit SearchWindow(QWidget *parent = 0);
    ~SearchWindow();


private:
    Ui::SearchWindow *ui;
signals:
    Q_SIGNAL void sendWaitMessage(const QString & msgInfo, bool onlyInfo = false) const;
    Q_SIGNAL bool sendWaitClose();

protected:
    virtual bool eventFilter(QObject *watched, QEvent *event);

public slots:
    void recvFocusChanged(QWidget *old, QWidget *now);
    void recvBtnBrowseDir();
    void recvBtnBrowseFile();
    void recvBtnSearchReset();
    void recvBtnSearchBegin();
    void recvBtnSearchExport();
    void recvBtnSearchDelete();
    void recvSortTableInfo(int colIndex);

    void recvTabRightClickedMenu(const QPoint &){tab_menu->exec(QCursor::pos());}
    void recvTabRightClickedMenu_copy_path();
    void recvTabRightClickedMenu_file_info();
    void recvTabRightClickedMenu_open_dir();
    void recvTabRightClickedMenu_delete_onlytable();
    void recvTabRightClickedMenu_delete_recyclebin();
    void recvTabRightClickedMenu_delete_absolutely();

private:
    void initSignal();
    void initTabMenu();
    bool checkTargets(const QString &targets, bool bShowMsg);
    bool checkCondtions();
    SearchImpl::CCondition getCondition();
    void searchReset(const QString &target, bool bAsk);
    void searchAndShow(const SearchImpl::CTarget& targetObj, const SearchImpl::CCondition &condition);
    void searchByQueue(const SearchImpl::CCondition &condition, SearchImpl::CTarget::List &searchList, SearchImpl::CResultVec &resultVec) const;
    void searchByRecursion(const SearchImpl::CCondition& condition,const QFileInfo &target, SearchImpl::CResultVec& resultVec) const;
    void showProperty(bool searchContent);
    void showResult(const SearchImpl::CResultVec& resultVec, bool searchContent);
    void tabFileDelete(int row, DialogDelete::Choise choise);
    static  QString FileSize(qint64 size);
    static bool RrecycleFile(const QString& filePath);

private:
    DialogWait* pDialogWait;
    QMenu   *tab_menu;
    QAction *tab_menu_copy_path=nullptr;
    QAction *tab_menu_file_info=nullptr;
    QAction *tab_menu_open_dir=nullptr;
    QAction *tab_menu_delete_onlytable=nullptr;
    QAction *tab_menu_delete_recyclebin=nullptr;
    QAction *tab_menu_delete_absolutely=nullptr;

};

