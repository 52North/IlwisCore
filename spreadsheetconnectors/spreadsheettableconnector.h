#ifndef SPREADSHEETCONNECTOR_H
#define SPREADSHEETCONNECTOR_H

namespace Ilwis {
class Table;

namespace SpreadSheetConnectors {

class SpreadSheet;

class SpreadSheetTableConnector : public IlwisObjectConnector
{
public:
    SpreadSheetTableConnector(const Ilwis::Resource &resource, bool load, const Ilwis::IOOptions &options=IOOptions());
    SpreadSheetTableConnector(const QString& file);

    bool loadMetaData(IlwisObject* data, const Ilwis::IOOptions &options);
    QString type() const;
    virtual IlwisObject *create() const;
    bool loadData(IlwisObject *, const Ilwis::IOOptions &options = IOOptions());
    bool store(IlwisObject *object, const IOOptions& options = IOOptions());
    QString provider() const;
    const std::unique_ptr<SpreadSheet>& spreadsheet() const;
    bool isValid() const;
    bool isReadOnly() const;

    static bool knownSuffix(const QString& suffix);

    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load, const Ilwis::IOOptions &options=IOOptions());

    QString format() const;
private:
    struct ColInfo{
        IlwisTypes _type = itUNKNOWN;
        QString _name = sUNDEF;
    };

    std::unique_ptr<SpreadSheet> _spreadsheet;
    std::vector<bool> _validColumns;
    quint32 _validColumnCount = 0;
    quint32 _headerline = iUNDEF;
    static const std::vector<QString> _suffixes;

    void setColumnDefinitions(Table *tbl, const std::vector<ColInfo> &columnType);
};
}
}

#endif // SPREADSHEETCONNECTOR_H
