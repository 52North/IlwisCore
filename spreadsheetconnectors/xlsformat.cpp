#include "kernel.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "errorobject.h"
#include "ilwisdata.h"
#include "domain.h"
#include "freexl.h"
#include "SpreadSheet.h"
#include "xlsformat.h"

using namespace Ilwis;
using namespace SpreadSheetConnectors;

XLSFormat::XLSFormat()
{
}

XLSFormat::~XLSFormat()
{
    if ( _handle)
        freexl_close(_handle);
}

bool XLSFormat::openSheet(const QString &spreadsheetPath, bool loading)
{
    if (!loading)
        return false;

    QString sheetName=sUNDEF, excelfile;

    QFileInfo xlsfile(spreadsheetPath);
    if ( xlsfile.suffix() != "xls"){
    int index  = spreadsheetPath.lastIndexOf("/");
        sheetName = spreadsheetPath.mid(index + 1);
        excelfile = spreadsheetPath.left(index);
    }else {
        excelfile = spreadsheetPath;
    }
    if ( _handle)
        freexl_close(_handle);

    int ret = freexl_open (excelfile.toLocal8Bit(), &_handle);
    if (ret != FREEXL_OK)
    {
        return ERROR1(ERR_COULD_NOT_OPEN_READING_1, excelfile);
    }
    if ( sheetName != sUNDEF){
        unsigned int max_worksheet;
        ret = freexl_get_info (_handle, FREEXL_BIFF_SHEET_COUNT, &max_worksheet);
        if (ret != FREEXL_OK)
        {
            return ERROR2(ERR_NOT_FOUND2, TR("workseehts"),excelfile);
        }
        QString wsName;
        for (_currentSheet = 0; _currentSheet < max_worksheet;
             _currentSheet++)
        {
            const char *utf8_worksheet_name;
            freexl_get_worksheet_name (_handle,_currentSheet, &utf8_worksheet_name);
            wsName = QString(utf8_worksheet_name);
            if ( wsName.toLower() == sheetName.toLower())
                break;
        }
        if ( _currentSheet == max_worksheet){
            _currentSheet = iUNDEF;
            return ERROR3(ERR_INVALID_PROPERTY_IN_4,"worksheet name", excelfile, wsName);
        }
        freexl_select_active_worksheet (_handle, _currentSheet);
    }else
        _currentSheet = 0;

    return true;
}

QVariant XLSFormat::cellValue(quint32 col, quint32 row) const
{
    FreeXL_CellValue cell;
    freexl_get_cell_value (_handle,row, col, &cell);
    switch (cell.type)
    {
    case FREEXL_CELL_INT:
        return QVariant(cell.value.int_value);
    case FREEXL_CELL_DOUBLE:
        return QVariant(cell.value.double_value);
    case FREEXL_CELL_TEXT:
    case FREEXL_CELL_SST_TEXT:
        return QVariant(cell.value.text_value);
    case FREEXL_CELL_DATE:
    case FREEXL_CELL_DATETIME:
    case FREEXL_CELL_TIME:
        return QVariant(cell.value.text_value);
    case FREEXL_CELL_NULL:
    default:
        return QVariant();
    }

}

quint32 XLSFormat::rowCount() const
{
    unsigned int rows;
    unsigned short columns;
    freexl_worksheet_dimensions (_handle,&rows, &columns);

    return rows;
}

quint32 XLSFormat::columnCount()
{
    unsigned int rows;
    unsigned short columns;
    freexl_worksheet_dimensions (_handle,&rows, &columns);

    return columns;
}

quint32 XLSFormat::sheetCount() const
{
    unsigned int max_worksheet;
    freexl_get_info (_handle, FREEXL_BIFF_SHEET_COUNT, &max_worksheet);

    return max_worksheet;
}

QString XLSFormat::sheetName(quint32 index) const
{
    if ( !_handle)
        return sUNDEF;

    const char *utf8_worksheet_name;
    freexl_get_worksheet_name (_handle,index, &utf8_worksheet_name);
    return QString(utf8_worksheet_name);
}

IlwisTypes XLSFormat::cellValueType(quint32 col, quint32 row) const
{
    return Domain::ilwType(cellValue(col, row));
}

bool XLSFormat::isValid() const
{
    return _handle != 0 && _currentSheet != iUNDEF;
}

bool XLSFormat::isRowValid(quint32 rowIndex) const
{
    return rowIndex < rowCount() ;
}

bool XLSFormat::isCellValid(quint32 columnIndex, quint32 rowIndex) const
{
    return columnIndex < const_cast<XLSFormat *>(this)->columnCount() && rowIndex < rowCount();
}

bool XLSFormat::isReadOnly() const
{
    return true;
}

QString XLSFormat::format() const
{
    return "XLS Spreadsheet";
}


