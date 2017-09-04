#ifndef SPREADSHEET_H
#define SPREADSHEET_H

namespace Ilwis {
namespace SpreadSheetConnectors {

class SpreadSheet {
public:
    virtual ~SpreadSheet() {}
    virtual bool openSheet(const QString& spreadsheetPath, bool loading=true) = 0;
    virtual QVariant cellValue(quint32 col, quint32 row) const = 0;
    virtual void cellValue(quint32 col, quint32 row, const QVariant& value, bool newRow=false) {}
    virtual quint32 rowCount() const = 0;
    virtual quint32 columnCount()  = 0;
    virtual IlwisTypes cellValueType(quint32 col, quint32 row) const = 0;
    virtual quint32 sheetCount() const  =0;
    virtual QString sheetName(quint32 index) const = 0;
    virtual bool isValid() const = 0;
    virtual bool isRowValid(quint32 rowIndex) const = 0;
    virtual bool isCellValid(quint32 columnIndex,quint32 rowIndex) const = 0;
    virtual bool isReadOnly() const = 0;
    virtual QString format() const = 0;

    virtual QString storeSheet(const QString& spreadsheetPath) const { return sUNDEF;}



};
}
}

#endif // SPREADSHEET_H
