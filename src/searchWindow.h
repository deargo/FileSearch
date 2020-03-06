#pragma once

// Coding: UTF-8(BOM)
#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif

#include <QMainWindow>
#include <QCheckBox>
#include "searchImpl.h"
#include "dialogWait.h"

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

private:
    void initSignal();
    bool checkTargets(const QString &targets, bool bShowMsg);
    bool checkCondtions();
    SearchImpl::CCondition getCondition();
    void searchReset(const QString &target, bool bAsk);
    void searchAndShow(const SearchImpl::CTarget& targetObj, const SearchImpl::CCondition &condition);
    void searchByQueue(const SearchImpl::CCondition &condition, SearchImpl::CTarget::List &searchList, SearchImpl::CResultVec &resultVec) const;
    void searchByRecursion(const SearchImpl::CCondition& condition,const QFileInfo &target, SearchImpl::CResultVec& resultVec) const;
    void showProperty(bool searchContent);
    void showResult(const SearchImpl::CResultVec& resultVec, bool searchContent);
    static  QString FileSize(qint64 size);
    static bool RrecycleFile(const QString& filePath);

private:
    DialogWait* pDialogWait;
};

