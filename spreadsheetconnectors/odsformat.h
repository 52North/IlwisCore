#ifndef ODSFORMAT_H
#define ODSFORMAT_H

namespace ods{
class Book;
class Row;
class Sheet;
class Cell;
}
namespace Ilwis {
namespace SpreadSheetConnectors {


class ODSFormat : public SpreadSheet
{
public:
    ODSFormat();

    bool openSheet(const QString& spreadsheetPath, bool isLoading=true);
    QVariant cellValue(quint32 col, quint32 row) const;
    void cellValue(quint32 col, quint32 row, const QVariant& value, bool newRow = false);
    quint32 rowCountRaw() const;
    quint32 rowCount() const;
    quint32 columnCount();
    IlwisTypes cellValueType(quint32 col, quint32 row) const;
    quint32 sheetCount() const;
    QString sheetName(quint32 index) const;

    bool isValid() const;
    bool isRowValid(quint32 rowIndex) const;
    bool isCellValid(quint32 columnIndex,quint32 rowIndex) const;

    bool isReadOnly() const;

    QString storeSheet(const QString& spreadsheetPath) const ;

    QString format() const;
private:
    std::unique_ptr<ods::Book> _book;
    ods::Sheet *_currentSheet;
    QVector<ods::Row*>* _rows;
    quint32 _columnCount = iUNDEF;

    ods::Cell *getCellInternal(quint32 col, quint32 row) const;
};
}
}

#endif // ODSFORMAT_H
