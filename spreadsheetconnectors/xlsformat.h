#ifndef XLSFORMAT_H
#define XLSFORMAT_H

namespace Ilwis {
namespace SpreadSheetConnectors {


class XLSFormat : public SpreadSheet
{
public:
    XLSFormat();
    ~XLSFormat();

   bool openSheet(const QString& spreadsheetPath, bool loading=true);
   QVariant cellValue(quint32 col, quint32 row) const;
   quint32 rowCount() const;
   quint32 columnCount();
   IlwisTypes cellValueType(quint32 col, quint32 row) const;
   quint32 sheetCount() const;
   QString sheetName(quint32 index) const;

   bool isValid() const;
   bool isRowValid(quint32 rowIndex) const;
   bool isCellValid(quint32 columnIndex,quint32 rowIndex) const;
   bool isReadOnly() const;


   QString format() const;
private:
    const void *_handle = 0;
    int _currentSheet = iUNDEF;
};
}
}

#endif // XLSFORMAT_H
