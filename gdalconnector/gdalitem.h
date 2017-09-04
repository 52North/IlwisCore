#ifndef GDALITEM_H
#define GDALITEM_H

namespace Ilwis {
namespace Gdal {
class GDALItems : public std::set<Resource>
{
public:
    GDALItems(const QUrl& path, const QFileInfo &localFile, IlwisTypes tp, IlwisTypes extTypes);
    GDALItems(const QFileInfo &localContainerFile, IlwisTypes tp, IlwisTypes extTypes);

    quint64 extractNameAndDomain(const QString &parts, QString &shortname);

private:
    quint64 addItem(GdalHandle* handle, const QUrl &url, quint64 csyid, quint64 grfId, IlwisTypes tp =itRASTER, IlwisTypes extTypes=itUNKNOWN, quint64 sz=i64UNDEF, int layerindex=iUNDEF, bool isExtendedType=true);
    quint64 addCsy(GdalHandle* handle, const QString &path, const QUrl &url, bool message=true);
    QString dimensions(GdalHandle* handle, bool &is3d, int layerindex=iUNDEF) const;
    std::map<QString, QString> kvp2Map(char **kvplist);
    int handleComplexDataSet(void *handle);
    quint64 numbertype2domainid(const QString &numbertype) const;
    quint64 findSize(const QFileInfo &inf);
    int layerCount(GdalHandle* handle);
    quint64 caseWithurl(const QStringList &parts, Size<> &sz, QString &shortname);
    quint64 caseWithSquareBrackets(const QStringList &parts, Size<> &sz, QString &shortnames);
    Size<> getSize(const QStringList& szMembers);
    void addOffsetScale(void *handle, const int count, Ilwis::Resource &gdalitem);
};
}
}

#endif // GDALITEM_H
