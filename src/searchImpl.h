#pragma once

// Coding: UTF-8(BOM)
#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif

#include <QString>
#include <QStringList>
#include <QFileInfo>
#include <QVector>
#include <QDir>
#include <QList>

class SearchImpl
{
public:
    class CTarget
    {
    public:
        typedef QList<QFileInfo> List;

        CTarget(){}
        CTarget(const QString& targets){ upsert(targets);}
        bool operator ==(const CTarget& other) const;
        void clear() {targetList.clear();}
        const List& list() const { return targetList;}
        unsigned int upsert(const QString& targets);
        unsigned int allCount()const { return targetList.size(); }
        unsigned int dirCount()const ;
        unsigned int fileCount()const ;
        bool isEmpty()const { return targetList.isEmpty();}
        static void GetFileList(const QString& dirPath, List& fileList);
    protected:
        List targetList;
    };

public:
    class CCondition
    {
    private:
        class CCondBase
        {
        public:
            bool operator ==(const CCondBase& other) const;
            bool enabled() const { return false == valueList.isEmpty();}
            void clear() {valueList.clear();}
            const QStringList& list() const { return valueList;}
            unsigned int upsert(const QString& values);
        protected:
            QStringList valueList;
            virtual bool matched(const QString& subject,const QString& pattern) const=0;
        };

    public:
        class DirInclude:public  CCondBase
        {
            virtual bool matched(const QString& subject,const QString& pattern) const;
        public:
            bool matched(const QString& dirName) const;
        };

        class DirExclude:public  CCondBase
        {
            virtual bool matched(const QString& subject,const QString& pattern) const;
        public:
            bool matched(const QString& dirName) const;
        };

        class FileInclude:public  CCondBase
        {
            virtual bool matched(const QString& subject,const QString& pattern) const;
        public:
            bool casesensitive = true;//匹配大小写
            bool whole_word = true;   //全字匹配
            bool regex_match = false;       //正则表达式
            bool operator ==(const FileInclude& other) const;
            bool matched(const QString& fileName) const;
        };

        class FileExclude:public  CCondBase
        {
            virtual bool matched(const QString& subject,const QString& pattern) const;
        public:
            bool casesensitive = true;//匹配大小写
            bool whole_word = true;   //全字匹配
            bool regex_match = false; //正则表达式
            bool operator ==(const FileExclude& other) const;
            bool matched(const QString& fileName) const;
        };

        class ContentInclude:public  CCondBase
        {
            virtual bool matched(const QString& subject,const QString& pattern) const;
        public:
            bool casesensitive = true;//匹配大小写
            bool whole_word = true;   //全字匹配
            bool regex_match = false;       //正则表达式
            bool operator ==(const ContentInclude& other) const;
            bool matched(const QString& filePath, QList<int>& lineList) const;
        };

    public:
        DirInclude dir_include;
        DirExclude dir_exclude;

        FileInclude filename_include;
        FileExclude filename_exclude;

        ContentInclude content_include;

        bool operator ==(const CCondition &other) const;
        void clear();
        bool matched(const QFileInfo& targetFileInfo, QList<int>& lineList) const;
    };

public:
    class CResult
    {
    public:
        typedef QList<int> LineInfo;
        enum class FileType {UNKNOW=0,DIR,LINK,TEXT,XML,WORD,IMAGE,AUDIO,VIDEO,BINARY};

        LineInfo lineInfo;
        QFileInfo fileInfo;
        CResult(){}
        CResult(const QFileInfo& fileInfo):fileInfo(fileInfo){}
        static QString ToString(const LineInfo &lineInfo, const QString &split);
        static QString GetFileType(FileType fileType);
        static FileType GetFileType(const QFileInfo& fileinfo);
        static QString GetFileType(const QString& filepath);
    };
    typedef QVector<CResult> CResultVec;
};
