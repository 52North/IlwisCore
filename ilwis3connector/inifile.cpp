#include  <stdio.h>
#include  <stdlib.h>
#include <QString>
#include <QTextStream>
#include <QRegExp>
#include "catalog.h"
#include "kernel.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "ilwiscontext.h"
#include "inifile.h"

using namespace Ilwis;

IniFile::IniFile()
{
}

IniFile::IniFile(const QFileInfo &file, bool loadfile)
{
    setIniFile(file, loadfile);
}

IniFile::~IniFile()
{
}

bool IniFile::setIniFile(const QFileInfo& file, bool loadfile) {

     _filename = file;
    if ( !loadfile ) // not interested in loading an inifile; we are creating a new one
        return true;

    if(!load())
        return ERROR1(ERR_MISSING_DATA_FILE_1,file.fileName());

    return true;

}

void IniFile::setValue(const QString& section, const QString& key, const QString& value){
    Sections::iterator iterSect = _sections.find(section);
    if (iterSect == _sections.end())
    {
        // Add a new section, with one new key/value entry
        SectionEntries entries;
        //entries[key.toLower()] = value;
        entries[key] = value;
        _sections[section] = entries;
    }
    else
    {
        // Add one new key/value entry in an existing section
        SectionEntries& entries = iterSect->second;
        //entries[key.toLower()] = value;
        entries[key] = value;
    }
}

void IniFile::setKeyValue(const QString& section, const QString& key, const QString& value)
{
    setValue(section, key, value);
}

void IniFile::setKeyValue(const QString& section, const QString& key, double value)
{
    setValue(section, key, FormatElement(value)) ;
}

void IniFile::setKeyValue(const QString& section, const QString& key, int value)
{
    setValue(section, key, FormatElement(value)) ;
}

QString IniFile::value(const QString& section, const QString& key) const
{

    Sections::const_iterator iterSect = _sections.find(section);
    if (iterSect != _sections.end())
    {
        const SectionEntries& entries = iterSect->second;

        for(const auto& entry : entries){
            if ( entry.first.toLower() == key.toLower())
                return entry.second;
        }

    }

    return sUNDEF;
}

void IniFile::removeKeyValue(const QString& section, const QString& key)
{
    Sections::iterator iterSect = _sections.find(section);
    if (iterSect != _sections.end())
    {
        // The section exists, now erase entry "key"
        SectionEntries& entries = iterSect->second;
        entries.erase(key);
    }
}

void IniFile::removeSection(const QString& section)
{
    Sections::iterator iterSect = _sections.find(section);
    if (iterSect != _sections.end())
    {
        // The section exists, so remove it and all its entries.
        SectionEntries& entries = iterSect->second;
        entries.clear();
        _sections.erase(iterSect);
    }
}

QString IniFile::url() const
{
    return QUrl::fromLocalFile(_filename.absoluteFilePath()).toString();
}

QString IniFile::filepath() const{
    return _filename.absoluteFilePath();
}

QString IniFile::path() const
{
    return _filename.absolutePath();
}

Time IniFile::modified() const
{
    return _filename.lastModified();
}

QStringList IniFile::childKeys(const QString &section) const
{
    QStringList keys;
    for (Sections::const_iterator iter = _sections.begin(); iter != _sections.end(); ++iter)
    {
        if ( iter->first == section) {
            const SectionEntries& entries = iter->second;
            for(SectionEntries::const_iterator cur = entries.begin(); cur != entries.end(); ++cur)
                keys.push_back(cur->second) ;
        }
    }
    return keys;
}

bool IniFile::load()
{
    enum ParseState { FindSection, FindKey, ReadFindKey, StoreKey, None } state;
    if (!_filename.exists())
        return false;

    QFile txtfile(_filename.absoluteFilePath());
    if (!txtfile.open(QIODevice::ReadOnly | QIODevice::Text)){
        return ERROR1(ERR_COULD_NOT_OPEN_READING_1, _filename.fileName());
    }
    QTextStream textfile(&txtfile);
    QString text = textfile.readAll();
    if (text == ""){
        return false;
    }
     QStringList lines = text.split("\n");

     QString section, key, value;
     state = FindSection;
     QString line;
     int linecount =0;
     while(linecount < lines.size())
     {
         switch (state)
         {
         case FindSection:
             line = lines[linecount++];
             if (line.size() == 0)
                 continue;

             if (line[0] == '[')
             {
                 int iLast = line.indexOf(']');
                 if (iLast != -1)
                 {
                     section = line.mid(1, iLast - 1);
                     state = ReadFindKey;
                 }
             }
             else
                 state = FindKey;
             break;
         case ReadFindKey:
             line = lines[linecount++];
         case FindKey:
         {
             int iEqu = line.indexOf('=');
             if (iEqu != -1)
             {
                 key = line.mid(0, iEqu);
                 value = line.mid(iEqu + 1);
                 state = StoreKey;
             }
             else
                 state = ReadFindKey;
         }
             break;
         case StoreKey:
             if (value[0] == '\''){
                 int index = value.lastIndexOf("'");
                 value = value.mid(1,index-1) + value.mid(index + 1);
             }
             setKeyValue(section, key, value);
             state = FindSection;
             break;

         case None:
             // Do we need to do anything?  Perhaps this never occurs.
             break;
         }
     }
     return true;
}

void IniFile::store(const QString& ext, const QFileInfo& file )
{
    QString path = file.absoluteFilePath();

    if ( ext != "" && file.suffix() != ext ) {
        QString exts = "mprmpamppmpsdomtbtgrfcsympl";
        if ( exts.indexOf(file.suffix()) != -1)
            path = file.absolutePath() + "/" + file.baseName() + "." + ext;
        else
            path = file.absolutePath() + "/" + file.fileName() + "." + ext;
    }
    QFile fileIni(path);

    if (!fileIni.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate))
        return;

    QTextStream text(&fileIni);
    Sections::iterator iterSect;
    bool first = true;
    for (iterSect = _sections.begin(); iterSect != _sections.end(); ++iterSect)
    {
        if ( !first)
            text << "\n";
        text << "[" << iterSect->first << "]"  << "\n";
        SectionEntries& entries = iterSect->second;
        SectionEntries::iterator iterEntry;
        for (iterEntry = entries.begin(); iterEntry != entries.end(); ++iterEntry)
        {
            QString key = iterEntry->first;
            if ( key.size() > 1) // else the a from ellipsoid (major axis) goes to upper
                key[0] = key[0].toUpper();
            text <<  key.trimmed() << "=" << iterEntry->second << "\n";
        }
        first = false;
    }
    text.flush();
    fileIni.close();

}

const QFileInfo &IniFile::fileInfo() const
{
    return _filename;
}

QString IniFile::FormatElement(double value)
{
    if (value == rUNDEF)
        return "?";
    else if (abs(value) < 1e12)
        return QString::number(value, 'f', 12);
    else
        return QString::number(value, 'e', 6); // see Ilwis3 WriteElement() and printf spec
}

QString IniFile::FormatElement(long value)
{
    if (value == iUNDEF)
        return "?";
    else
        return QString::number(value);
}

QString IniFile::FormatElement(int value)
{
    return FormatElement((long)value);
}

QString IniFile::FormatElement(quint32 value)
{
    return FormatElement((long)value);
}

QString IniFile::FormatElement(bool value)
{
    if (value)
        return "Yes";
    else
        return "No";
}
