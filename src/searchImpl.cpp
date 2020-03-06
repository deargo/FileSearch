#include "searchImpl.h"
#include "searchConst.hpp"
#include <QMimeDatabase>
#include <QMimeType>
#include <QFileInfo>
#include <QMap>
#include <QFileInfo>
#include <QDebug>
#include <QRegularExpression>
#include <algorithm>
#include <QTextStream>

bool SearchImpl::CTarget::operator ==(const CTarget& other) const
{
    return targetList.size() == other.targetList.size() &&
            std::equal(targetList.begin(),targetList.end(),other.targetList.begin());
}
unsigned int SearchImpl::CTarget::upsert(const QString& targets)
{
    targetList.clear();
    if(targets.isEmpty())
    {
        return targetList.size();
    }
    QStringList newTargets = targets.split(SearchConst::multi_split);
    QFileInfo fileInfo;
    for (QString target: newTargets)
    {
        fileInfo.setFile(target);
        if(target.isEmpty() || !fileInfo.exists())
        {
            //不存在则不保存了
            continue;
        }
        //如果输入 . 或者 ./ 或者 ../ 或者 \ 或者 / 也是可以的，所以一定要取绝对路径
        target = fileInfo.absoluteFilePath();
        //末尾为路径分隔符时，必须去掉分隔符
        while (!(target.size() == 3 && target.endsWith(":/")) && //非盘符的根路径
               (target.endsWith("/") || target.endsWith("\\")))  //且以分隔符结尾
        {
            target.chop(1);
        }
        fileInfo.setFile(target);
        //将绝对路径存储
        targetList.push_front(std::move(QFileInfo(fileInfo.absoluteFilePath())));
    }
    //去重
    for(auto it =targetList.begin();it!=targetList.end();++it)
    {
        auto it2 = it+1;
        while(it2!=targetList.end())
        {
            if(*it == *it2) it2 = targetList.erase(it2);
            else ++it2;
        }
    }

    return targetList.size();
}
unsigned int SearchImpl::CTarget::dirCount()const
{
    unsigned int count = targetList.size();
    for (const auto& value: targetList)
    {
        QFileInfo targetInfo(value);
        if(targetInfo.isDir()) continue;
        count--;
    }
    return count;
}
unsigned int SearchImpl::CTarget::fileCount()const
{
    unsigned int count = targetList.size();
    for (const auto& value: targetList)
    {
        QFileInfo targetInfo(value);
        if(targetInfo.isFile()) continue;
        count--;
    }
    return count;
}
void SearchImpl::CTarget::GetFileList(const QString& dirPath, SearchImpl::CTarget::List& fileList)
{
    QDir dir(dirPath);
    QFileInfoList fileInfoList = dir.entryInfoList();
    for(const auto& fileInfo:fileInfoList){
        if(fileInfo.fileName()=="." || fileInfo.fileName()=="..") continue;
        fileList.push_back(fileInfo);
    }
    return;
}


bool SearchImpl::CCondition::CCondBase::operator ==(const SearchImpl::CCondition::CCondBase &other) const
{
    return  (other.valueList.size() == this->valueList.size()) &&                 //取值个数一样
            std::equal(valueList.begin(),valueList.end(),other.valueList.begin());//具体取值相等
}
unsigned int SearchImpl::CCondition::CCondBase::upsert(const QString& values)
{
    valueList.clear();
    if(values.isEmpty())
    {
        //为空则直接返回
        return valueList.size();
    }
    QStringList valList = values.split(SearchConst::multi_split);
    for (const auto& value: valList)
    {
        if(value.isEmpty())
        {
            continue;
        }
        valueList.push_front(value);
    }
    //去重
    valueList.removeDuplicates();
    return valueList.size();
}



bool SearchImpl::CCondition::DirInclude::matched(const QString &dirName) const
{
    //条件不可用，直接返回true
    for (const auto& condition: valueList)
    {
        //条件不为空，逐个校验，满足就返回
        if(matched(dirName,condition))
        {
            return true;
        }
    }
    //条件为空，也返回true
    return valueList.empty();
}
bool SearchImpl::CCondition::DirInclude::matched(const QString &subject,const QString& pattern) const
{
    return subject.contains(pattern);
}



bool SearchImpl::CCondition::DirExclude::matched(const QString& dirName) const
{
    //条件不可用，直接返回true
    for (const auto& condition: valueList)
    {
        //条件不为空，逐个校验，满足就返回
        if(matched(dirName,condition))
        {
            return true;
        }
    }
    //条件为空，也返回true
    return valueList.empty();
}
bool SearchImpl::CCondition::DirExclude::matched(const QString &subject,const QString& pattern) const
{
    return false == subject.contains(pattern);
}



bool SearchImpl::CCondition::FileInclude::matched(const QString &fileName) const
{
    //条件不可用，直接返回true
    for (const auto& condition: valueList)
    {
        //条件不为空，逐个校验，满足就返回
        if(matched(fileName,condition))
        {
            return true;
        }
    }
    //条件为空，也返回true
    return valueList.empty();
}
bool SearchImpl::CCondition::FileInclude::matched(const QString &subject,const QString& pattern) const
{
    if(whole_word && subject.size()!=pattern.size())
    {
        return false;
    }
    if(regex_match)
    {
        QRegularExpression regularExpression(pattern,casesensitive
                                             ? QRegularExpression::NoPatternOption
                                             : QRegularExpression::CaseInsensitiveOption);
        return  regularExpression.match(subject,0,
                                        QRegularExpression::PartialPreferFirstMatch).hasMatch();
    }
    return subject.contains(pattern,casesensitive? Qt::CaseSensitive : Qt::CaseInsensitive);
}
bool SearchImpl::CCondition::FileInclude::operator ==(const SearchImpl::CCondition::FileInclude& other) const
{
    return this->casesensitive == other.casesensitive &&
            this->whole_word == other.whole_word &&
            this->regex_match == other.regex_match &&
            (other.valueList.size() == this->valueList.size()) &&
            std::equal(valueList.begin(),valueList.end(),other.valueList.begin());
}



bool SearchImpl::CCondition::FileExclude::matched(const QString &fileName) const
{
    //条件不可用，直接返回true
    for (const auto& condition: valueList)
    {
        //条件不为空，逐个校验，满足就返回
        if(matched(fileName,condition))
        {
            return true;
        }
    }
    //条件为空，也返回true
    return valueList.empty();
}
bool SearchImpl::CCondition::FileExclude::matched(const QString &subject,const QString& pattern) const
{
    if(whole_word && subject.size()!=pattern.size())
    {
        return true;
    }
    if(regex_match)
    {
        QRegularExpression regularExpression(pattern,casesensitive
                                             ? QRegularExpression::NoPatternOption
                                             : QRegularExpression::CaseInsensitiveOption);
        return false == regularExpression.match(subject,0,
                                                QRegularExpression::PartialPreferFirstMatch).hasMatch();
    }
    return false == subject.contains(pattern,casesensitive? Qt::CaseSensitive : Qt::CaseInsensitive);
}
bool SearchImpl::CCondition::FileExclude::operator ==(const SearchImpl::CCondition::FileExclude& other) const
{
    return this->casesensitive == other.casesensitive &&
            this->whole_word == other.whole_word &&
            this->regex_match == other.regex_match &&
            (other.valueList.size() == this->valueList.size()) &&
            std::equal(valueList.begin(),valueList.end(),other.valueList.begin());
}



bool SearchImpl::CCondition::ContentInclude::matched(const QString &filePath, QList<int> &lineList) const
{
    if(valueList.empty())
    {
        //条件为空，也返回true
        return true;
    }
    QFile targetFile(filePath);
    if (!targetFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return false;
    }
    QTextStream textStream(&targetFile);
    textStream.setCodec("GB2312");

    bool bFind = false;
    int line=0;
    QString temp;
    while (!textStream.atEnd()) {
        line++;
        temp = textStream.readLine();
        //条件不可用，直接返回true
        for (const auto& condition: valueList)
        {
            //条件不为空，逐个校验，满足就返回
            if(matched(temp,condition))
            {
                lineList.push_back(line);
                bFind = true;
                break;
            }
        }
    }
    targetFile.close();
    return bFind;
}
bool SearchImpl::CCondition::ContentInclude::matched(const QString &subject,const QString& pattern) const
{
    if(whole_word && subject.size()!=pattern.size())
    {
        return false;
    }
    if(regex_match)
    {
        QRegularExpression regularExpression(pattern,casesensitive
                                             ? QRegularExpression::NoPatternOption
                                             : QRegularExpression::CaseInsensitiveOption);
        return  regularExpression.match(subject,0,
                                        QRegularExpression::PartialPreferFirstMatch).hasMatch();
    }
    return subject.contains(pattern,casesensitive? Qt::CaseSensitive : Qt::CaseInsensitive);
}
bool SearchImpl::CCondition::ContentInclude::operator ==(const SearchImpl::CCondition::ContentInclude& other) const
{
    return this->casesensitive == other.casesensitive &&
            this->whole_word == other.whole_word &&
            this->regex_match == other.regex_match &&
            (other.valueList.size() == this->valueList.size()) &&
            std::equal(valueList.begin(),valueList.end(),other.valueList.begin());
}



bool SearchImpl::CCondition::operator ==(const SearchImpl::CCondition& other) const
{
    return  this->dir_include == other.dir_include &&
            this->dir_exclude == other.dir_exclude &&
            this->filename_include == other.filename_include &&
            this->filename_exclude == other.filename_exclude &&
            this->content_include == other.content_include ;
}
void SearchImpl::CCondition::clear()
{
    dir_include.clear();
    dir_exclude.clear();
    filename_include.clear();
    filename_exclude.clear();
    content_include.clear();
}

bool SearchImpl::CCondition::matched(const QFileInfo &targetFileInfo, QList<int>& lineList) const
{
    QString targetDir;
    if(targetFileInfo.isDir())
    {
        targetDir = targetFileInfo.fileName();
        return  (false==dir_include.enabled() || dir_include.matched(targetDir) ) &&
                (false==dir_exclude.enabled() || dir_exclude.matched(targetDir) );
    }
    targetDir = QFileInfo(targetFileInfo.absolutePath()).fileName();
    QString targetName = targetFileInfo.fileName();
    return  (false==dir_include.enabled() || dir_include.matched(targetDir) ) &&
            (false==dir_exclude.enabled() || dir_exclude.matched(targetDir) ) &&
            (false==filename_include.enabled() || filename_include.matched(targetName) ) &&
            (false==filename_exclude.enabled() || filename_exclude.matched(targetName) ) &&
            (false==content_include.enabled() || content_include.matched(targetFileInfo.absoluteFilePath(),lineList) );
}


QString SearchImpl::CResult::ToString(const LineInfo& lineInfo, const QString& split)
{
    QString value;
    for(auto line : lineInfo)
    {
        value+=QString::number(line);
        value+=split;
    }
    if(value.endsWith(split))
    {
        value.chop(split.size());
    }
    return value;
}

QString SearchImpl::CResult::GetFileType(SearchImpl::CResult::FileType fileType)
{
    //UNKNOW,DIR,LINK,TEXT,XML,WORD,IMAGE,AUDIO,VIDEO,BINARY
    static QString fileTypes[]={"未知","目录","链接","文本","XML","文档","图片","音频","视频","二进制"};
    return fileTypes[static_cast<int>(fileType)];
}

SearchImpl::CResult::FileType SearchImpl::CResult::GetFileType(const QFileInfo &fileinfo)
{
    static QMimeDatabase db;
    if(fileinfo.isDir()) return SearchImpl::CResult::FileType::DIR;
    if(fileinfo.isSymLink()) return SearchImpl::CResult::FileType::LINK;
    QMimeType mime = db.mimeTypeForFile(fileinfo);
    if(mime.name().startsWith("text/plain")) return SearchImpl::CResult::FileType::TEXT;
    if(mime.name().startsWith("text/htm")) return SearchImpl::CResult::FileType::XML;
    if(mime.name().startsWith("text/html")) return SearchImpl::CResult::FileType::XML;
    if(mime.name().startsWith("text/xhtml")) return SearchImpl::CResult::FileType::XML;
    if(mime.name().startsWith("text/xml")) return SearchImpl::CResult::FileType::XML;
    if(mime.name().startsWith("text/")) return SearchImpl::CResult::FileType::TEXT;
    if(mime.name().startsWith("image/")) return SearchImpl::CResult::FileType::IMAGE;
    if(mime.name().startsWith("audio/")) return SearchImpl::CResult::FileType::AUDIO;
    if(mime.name().startsWith("video/")) return SearchImpl::CResult::FileType::VIDEO;
    if(mime.name().startsWith("application/octet-stream")) return SearchImpl::CResult::FileType::BINARY;
    if(mime.name().startsWith("application/xml")) return SearchImpl::CResult::FileType::XML;
    if(mime.name().startsWith("application/xhtml")) return SearchImpl::CResult::FileType::XML;
    if(mime.name().startsWith("application/xhtml+xml")) return SearchImpl::CResult::FileType::XML;
    if(mime.name().startsWith("application/atom+xml")) return SearchImpl::CResult::FileType::XML;
    if(mime.name().startsWith("application/json")) return SearchImpl::CResult::FileType::TEXT;
    if(mime.name().startsWith("application/msword")) return SearchImpl::CResult::FileType::WORD;
    if(mime.name().startsWith("application/pdf")) return SearchImpl::CResult::FileType::WORD;
    if(mime.name().startsWith("application/x-jpg")) return SearchImpl::CResult::FileType::IMAGE;
    if(mime.name().startsWith("application/x-jpeg")) return SearchImpl::CResult::FileType::IMAGE;
    if(mime.name().startsWith("application/x-img")) return SearchImpl::CResult::FileType::IMAGE;
    if(mime.name().startsWith("application/x-png")) return SearchImpl::CResult::FileType::IMAGE;
    if(mime.name().startsWith("application/x-ppt")) return SearchImpl::CResult::FileType::WORD;
    if(mime.name().startsWith("application/x-vnd.msg-powerpoint")) return SearchImpl::CResult::FileType::WORD;
    if(mime.name().startsWith("application/x-vnd.msg-excel")) return SearchImpl::CResult::FileType::WORD;
    if(mime.name().startsWith("application/x-xls")) return SearchImpl::CResult::FileType::WORD;
    if(mime.name().startsWith("application/x-vnd.visio")) return SearchImpl::CResult::FileType::WORD;
    if(mime.name().startsWith("application/x-vsd")) return SearchImpl::CResult::FileType::WORD;
    if(mime.name().startsWith("application/x-visio")) return SearchImpl::CResult::FileType::WORD;
    if(fileinfo.isExecutable()) return SearchImpl::CResult::FileType::BINARY;
    return SearchImpl::CResult::FileType::UNKNOW;
}

QString SearchImpl::CResult::GetFileType(const QString& filepath)
{
    QFileInfo fileInfo(filepath);
    SearchImpl::CResult::FileType fileType = SearchImpl::CResult::GetFileType(fileInfo);
    return SearchImpl::CResult::GetFileType(fileType);
}


