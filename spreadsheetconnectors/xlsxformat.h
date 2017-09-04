#ifndef XSLXFORMAT_H
#define XSLXFORMAT_H

namespace QXlsx {
class Document;

}

namespace Ilwis {
namespace SpreadSheetConnectors {


class XLSXFormat : public SpreadSheet
{
public:
    XLSXFormat();

    virtual bool openSheet(const QString& spreadsheetPath, bool isLoading=true);
    virtual QVariant cellValue(quint32 col, quint32 row) const;
    virtual void cellValue(quint32 col, quint32 row, const QVariant& value, bool newRow = false);
    virtual quint32 rowCountRaw() const;
    virtual quint32 rowCount() const;
    virtual quint32 columnCount();
    virtual IlwisTypes cellValueType(quint32 col, quint32 row) const;

    virtual bool isValid() const;
    virtual bool isRowValid(quint32 rowIndex) const;
    virtual bool isCellValid(quint32 columnIndex,quint32 rowIndex) const;

    bool isReadOnly() const;

    virtual QString storeSheet(const QString& spreadsheetPath) const ;

    quint32 sheetCount() const;
    QString sheetName(quint32 index) const;
    QString format() const;
private:
    std::unique_ptr<QXlsx::Document> _xlsxdocument;
};
}
}

#endif // XSLXFORMAT_H
