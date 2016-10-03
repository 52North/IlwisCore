#ifndef PYTHONSCRIPTCONNECTOR_H
#define PYTHONSCRIPTCONNECTOR_H

namespace Ilwis{
namespace PythonScript {
class PythonScriptConnector : public Ilwis::IlwisObjectConnector
{
public:
    PythonScriptConnector(const Ilwis::Resource &resource, bool load,const IOOptions& options=IOOptions());

    bool store(IlwisObject *obj, const IOOptions& options = IOOptions() );
    QString type() const;
    Ilwis::IlwisObject *create() const;
    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load=true,const IOOptions& options=IOOptions());
    QString provider() const;
    bool loadMetaData(IlwisObject *obj, const Ilwis::IOOptions &options);
    bool loadData(IlwisObject *, const IOOptions &options = IOOptions());
};
}
}

#endif // PYTHONSCRIPTCONNECTOR_H
