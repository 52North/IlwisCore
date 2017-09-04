#ifndef COVERAGECONNECTOR_H
#define COVERAGECONNECTOR_H

namespace Ilwis {
namespace Ilwis3{

class TableConnector;

class CoverageConnector : public Ilwis3Connector
{
public:
    CoverageConnector(const Resource& resource, bool load=true,const IOOptions& options=IOOptions());
protected:
    bool getRawInfo(const QString &range, double &vmin, double &vmax, double &scale, double &offset) const;
    virtual void calcStatistics(const IlwisObject *obj,NumericStatistics::PropertySets set) const = 0;
    bool loadMetaData(Ilwis::IlwisObject *data, const IOOptions &options);
    bool storeMetaData(IlwisObject *obj, IlwisTypes type, const Ilwis::DataDefinition &datadef, const QString &baseName=sUNDEF) ;
   // bool storeBinaryData(IlwisObject *obj, IlwisTypes tp, const QString &baseName);
    TableConnector *createTableStoreConnector(ITable &attTable, Coverage *coverage, IlwisTypes tp, const QString &baseName);

    RawConverter _converter;
    DataDefinition determineDataDefintion(const Ilwis::ODF &odf, const IOOptions &) const;
protected:
    QString _domainName;
    QString _domainInfo;
    QString _csyName;
    int _itemCount = 0;
private:
    ITable prepareAttributeTable(const QString &file, const QString& basemaptype, const IOOptions &) const;

    /**
     * @brief addToMasterCatalog will check if the mastercatalog has a resource under this name, and add it if it doesn't;
     * This is a helper function, its purpose is to workaround there where the ilwisobjects core code isn't properly opening files that aren't registered in the mastercatalog
     * @param filename the file to add as an ODFItem-resource
     * @param it the type under which the item is expected to be found
     */
    void addToMasterCatalog(QString filename, IlwisTypes it) const;

};
}
}

#endif // COVERAGECONNECTOR_H
