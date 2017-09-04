#include "kernel.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "errorobject.h"
#include "ilwisdata.h"
#include "domain.h"
#include "ods/ods.h"
#include "SpreadSheet.h"
#include "odsformat.h"

using namespace Ilwis;
using namespace SpreadSheetConnectors;

ODSFormat::ODSFormat()
{
}


bool Ilwis::SpreadSheetConnectors::ODSFormat::openSheet(const QString &spreadsheetPath, bool isLoading)
{
    QFileInfo odsinfo(spreadsheetPath);
    QString sheetName;
    if ( odsinfo.suffix() != "ods"){ // the path was a path inside a catalog of an ods file, so strip the sheetname to get the filename, retain the sheetname
        int index  = odsinfo.absoluteFilePath().lastIndexOf("/");
        sheetName = odsinfo.absoluteFilePath().mid(index + 1);
        odsinfo = QFileInfo(odsinfo.absolutePath());
    }
    _book.reset(new ods::Book(odsinfo.absoluteFilePath()));
    if ( sheetName == ""){ // no sheetname given so we have to figure out one for our selves
        if ( isLoading) {
            if (!_book->sheet(0)) // vby default, when loading we choose the first sheet (if it exists)
                return false;
            sheetName = _book->sheet(0)->name();
        }else{
            sheetName = "Sheet1"; // if no name is given we create a default name
        }
    }
    _currentSheet = _book->sheet(sheetName);
    if ( !isLoading  && _currentSheet == 0) // are we saving to an existing sheet or do we need to create a new one?
        _currentSheet =  _book->CreateSheet(sheetName);

    if (!_currentSheet)
        return ERROR2(ERR_NO_OBJECT_TYPE_FOR_2,"worksheet", sheetName);

    _rows = _currentSheet->rows();
    return true;

}

ods::Cell *ODSFormat::getCellInternal(quint32 col, quint32 row) const{
    ods::Row *sheetRow =  _rows->at(row);
    if (!sheetRow)
        return 0;

    ods::Cell *cell = sheetRow->cell(col);
    if ( !cell)
        return 0;
    ods::Value &value = cell->value();
    if ( !value.Ok())
        return 0;
    return cell;
}

QVariant ODSFormat::cellValue(quint32 col, quint32 row) const
{
    if ( row < _rows->size()){

        ods::Cell *cell = 0;
        if ((cell = getCellInternal( col, row)) == 0)
            return QVariant();
        ods::Value &value = cell->value();
        if ( value.IsBool()){
            return QVariant(*value.AsBool());
        }else if ( value.IsDouble()){
            return QVariant(*value.AsDouble());
        } else if ( value.IsDate()){
            return QVariant(*value.AsDate());
        } else if ( value.IsTime()){
            return QVariant(*value.AsDate());
        } else if ( value.IsString()){
            return QVariant(*value.AsString());
        }
    }
    return QVariant();
}

void ODSFormat::cellValue(quint32 col, quint32 row, const QVariant &value, bool newRow)
{
    if ( newRow && row >= _rows->size()){
        _currentSheet->CreateRow(row);
    }
    if ( row < _rows->size()){
        ods::Row *sheetrow =  _rows->at(row);
        if ( !sheetrow)
            return;

        ods::Cell *cell = sheetrow->cell(col);
        if ( ! cell){
            cell = sheetrow->CreateCell(col);
        }
        _columnCount = Ilwis::max((long)_columnCount, (long)col + 1);
        IlwisTypes tp = Domain::ilwType(value);
        if ( hasType(tp, itNUMBER))
            cell->SetValue(value.toDouble());
        else
            cell->SetValue(value.toString());
    }
}

quint32 ODSFormat::rowCountRaw() const
{
    if ( _rows->size() == 0)
        return _currentSheet->CalcRowCount();

    return _rows->size();
}

quint32 ODSFormat::rowCount() const
{
    int rowCount = -1;
    quint32 raw = rowCountRaw();
    for(int row = 0; row < raw; ++row){
        if (! isCellValid(0, row)){
            rowCount = row;
            break;
        }
    }

    return rowCount != -1 ? rowCount : raw;
}

quint32 ODSFormat::columnCount()
{
    if ( _columnCount == iUNDEF)    {
        _columnCount = 0;
        for(int row = 0; row < _rows->size(); ++row){
            quint32 colcount = _rows->at(row)->column_count();
            _columnCount = std::max(_columnCount,colcount);
        }
    }
    return _columnCount;
}

IlwisTypes ODSFormat::cellValueType(quint32 col, quint32 row) const
{
    return Domain::ilwType(cellValue(col, row));
}

quint32 ODSFormat::sheetCount() const
{
    if ( _book)
        return _book->sheet_count();

    return iUNDEF;
}

QString ODSFormat::sheetName(quint32 index) const
{
    if (!_book || index >= _book->sheet_count())
        return sUNDEF;
    return _book->sheet(index)->name();
}

bool ODSFormat::isValid() const
{
    return _book && _currentSheet != 0;
}

bool ODSFormat::isRowValid(quint32 rowIndex) const
{
    return rowIndex < _rows->size() && _rows->at(rowIndex);
}

bool ODSFormat::isCellValid(quint32 columnIndex, quint32 rowIndex) const
{
    return getCellInternal(columnIndex, rowIndex) != 0;
}

bool ODSFormat::isReadOnly() const
{
    return false;
}

QString ODSFormat::format() const
{
    return "ODS Spreadsheet";
}

QString ODSFormat::storeSheet(const QString &spreadsheetPath) const
{
    QFile file(spreadsheetPath);
    if (file.exists() && !file.remove())
        return QString("file %1 is read only").arg(spreadsheetPath);
    if ( !isValid())
        return "not a valid spread sheet";

    return _book->Save(file);
}
