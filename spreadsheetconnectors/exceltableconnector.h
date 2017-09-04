#ifndef EXCELTABLECONNECTOR_H
#define EXCELTABLECONNECTOR_H

namespace Ilwis {
namespace SpreadSheetConnectors {


class ExcelTableConnector : public IlwisObjectConnector
{
public:
    ExcelTableConnector(const Ilwis::Resource &resource, bool load,const IOOptions& options=IOOptions());
    bool loadMetaData(IlwisObject* data, const IOOptions&options);
    bool storeMetaData(Ilwis::IlwisObject *obj);
    QString type() const;
    virtual IlwisObject *create() const;
    bool loadData(IlwisObject *, const LoadOptions& options = LoadOptions());
    bool storeBinaryData(IlwisObject* obj);
    QString provider() const;

    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load,const IOOptions& options=IOOptions());
};
}
}

#endif // EXCELTABLECONNECTOR_H
