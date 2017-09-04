/**
  @file
  @author Stefan Frings
*/

#include "kernel.h"
#include "ilwiscontext.h"
#include "templateloader.h"
#include <QFile>
#include <QFileInfo>
#include <QStringList>
#include <QDir>
#include <QSet>

using namespace Ilwis;

TemplateLoader::TemplateLoader(QObject* parent)
    : QObject(parent)
{
    templatePath = ilwisconfig("server-settings/template-path",QString("app-base"));
    // Convert relative path to absolute, based on the directory of the config file.
    if (QDir::isRelativePath(templatePath))
    {
        templatePath = context()->resourceRoot().absoluteFilePath() + "servertemplates";
    }
    fileNameSuffix=ilwisconfig("server-settings/template-suffix",QString(".tpl"));
    QString encoding=ilwisconfig("server-settings/encoding",QString("UTF-8"));
    if (encoding.isEmpty()) {
        textCodec=QTextCodec::codecForLocale();
    }
    else {
       textCodec=QTextCodec::codecForName(encoding.toLocal8Bit());
   }
   qDebug("TemplateLoader: path=%s, codec=%s",qPrintable(templatePath),textCodec->name().data());
}

TemplateLoader::~TemplateLoader() {}

QString TemplateLoader::tryFile(QString localizedName) {
    QString fileName=templatePath+"/"+localizedName+fileNameSuffix;
    qDebug("TemplateCache: trying file %s",qPrintable(fileName));
    QFile file(fileName);
    if (file.exists()) {
        file.open(QIODevice::ReadOnly);
        QString document=textCodec->toUnicode(file.readAll());
        file.close();
        if (file.error()) {
            qCritical("TemplateLoader: cannot load file %s, %s",qPrintable(fileName),qPrintable(file.errorString()));
            return "";
        }
        else {
            return document;
        }
    }
    return "";
}

Template TemplateLoader::getTemplate(QString templateName, QString locales) {
    mutex.lock();
    QSet<QString> tried; // used to suppress duplicate attempts
    QStringList locs=locales.split(',',QString::SkipEmptyParts);

    // Search for exact match
    foreach (QString loc,locs) {
        loc.replace(QRegExp(";.*"),"");
        loc.replace('-','_');
        QString localizedName=templateName+"-"+loc.trimmed();
        if (!tried.contains(localizedName)) {
            QString document=tryFile(localizedName);
            if (!document.isEmpty()) {
                mutex.unlock();
                return Template(document,localizedName);
            }
            tried.insert(localizedName);
        }
    }

    // Search for correct language but any country
    foreach (QString loc,locs) {
        loc.replace(QRegExp("[;_-].*"),"");
        QString localizedName=templateName+"-"+loc.trimmed();
        if (!tried.contains(localizedName)) {
            QString document=tryFile(localizedName);
            if (!document.isEmpty()) {
                mutex.unlock();
                return Template(document,localizedName);
            }
            tried.insert(localizedName);
        }
    }

    // Search for default file
    QString document=tryFile(templateName);
    if (!document.isEmpty()) {
        mutex.unlock();
        return Template(document,templateName);
    }

    qCritical("TemplateCache: cannot find template %s",qPrintable(templateName));
    mutex.unlock();
    return Template("",templateName);
}
