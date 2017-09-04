#include <QSqlQuery>
#include <QSqlError>
#include <QDir>

#include "kernel.h"
#include "angle.h"
#include "geos/geom/Coordinate.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "errorobject.h"
#include "ilwisdata.h"
#include "domainitem.h"
#include "domain.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "basetable.h"
#include "flattable.h"
#include "ilwiscontext.h"
#include "exceltableconnector.h"
#include "freexl.h"

using namespace Ilwis;
using namespace SpreadSheetConnectors;

ExcelTableConnector::ExcelTableConnector(const Ilwis::Resource &resource, bool load, const IOOptions &options) : IlwisObjectConnector(resource, load, options)
{

}

bool ExcelTableConnector::loadMetaData(IlwisObject *data, const IOOptions &options)
{

    unsigned int worksheet_index;
    QString tableName;
    const void *handle;
    int ret;
    unsigned int info;
    unsigned int max_worksheet;
    unsigned int rows;
    unsigned short columns;
    unsigned int row;
    unsigned short col;

    QString excelfile = _resource.toLocalFile();
    int index  = excelfile.lastIndexOf("/");
    QString sheetName = excelfile.mid(index + 1);
    excelfile = excelfile.left(index);
    ret = freexl_open (excelfile.toLocal8Bit(), &handle);
    if (ret != FREEXL_OK)
    {
        return ERROR1(ERR_COULD_NOT_OPEN_READING_1, excelfile);
    }
    ret = freexl_get_info (handle, FREEXL_BIFF_PASSWORD, &info);
    if (ret != FREEXL_OK)
    {
        return ERROR2(ERR_COULD_NOT_OPEN_READING_2, excelfile, "password/username incorrect");
    }
    switch (info)
    {
    case FREEXL_BIFF_PLAIN:
        break;
    case FREEXL_BIFF_OBFUSCATED:
    default:
        return ERROR2(ERR_COULD_NOT_OPEN_READING_2, excelfile, "file is protected");
    };
    ret = freexl_get_info (handle, FREEXL_BIFF_SHEET_COUNT, &max_worksheet);
    if (ret != FREEXL_OK)
    {
        return ERROR2(ERR_NOT_FOUND2, TR("workseehts"),excelfile);
    }
    Table * tbl = static_cast<Table *>(data);
    QString wsName;
    for (worksheet_index = 0; worksheet_index < max_worksheet;
         worksheet_index++)
    {
        const char *utf8_worksheet_name;
        freexl_get_worksheet_name (handle,worksheet_index, &utf8_worksheet_name);
        wsName = QString(utf8_worksheet_name);
        if ( wsName.toLower() == sheetName.toLower())
            break;
    }
    if ( worksheet_index == max_worksheet){
        return ERROR3(ERR_INVALID_PROPERTY_IN_4,"worksheet name", excelfile, wsName);
    }
    ret = freexl_select_active_worksheet (handle, worksheet_index);
    if (ret != FREEXL_OK)
    {
        return false;
    }
    /* dimensions */
    freexl_worksheet_dimensions (handle,&rows, &columns);
    if (rows ==0 || columns == 0)
    {
        return ERROR2(ERR_INVALID_PROPERTY_FOR_2, "worksheet dimensions", excelfile);
    }

    auto iter =  options._values.find("headerline");
    int rowShift = iter == options._values.end() ? 0 : 1;

    std::vector<IlwisTypes> columnType(columns, itUNKNOWN);
    std::vector<std::vector<QVariant>> table(rows - rowShift);
    std::vector<QString> columnNames(columns);

    for(auto& row : table)
        row.resize(columns);
    for(int i = 0; i < columns; ++i){
        columnNames[i] = "column_" + QString::number(i) ;
    }



    for (row = 0; row < rows; row++)
    {
        FreeXL_CellValue cell;
        for (col = 0; col < columns; col++)
        {
            freexl_get_cell_value (handle,row, col, &cell);
            if ( rowShift == 1 && row == iter->second.toUInt()){
                columnNames[col] = cell.value.text_value;;
            } else {
                switch (cell.type)
                {
                case FREEXL_CELL_INT:
                    table[row - rowShift][col] = cell.value.int_value;
                    if ( columnType[col] == itUNKNOWN)
                        columnType[col] = itINTEGER;
                    break;
                case FREEXL_CELL_DOUBLE:
                    table[row- rowShift][col] = cell.value.double_value;
                    if ( hasType(columnType[col], itUNKNOWN | itINTEGER))
                        columnType[col] = itDOUBLE;
                    break;
                case FREEXL_CELL_TEXT:
                case FREEXL_CELL_SST_TEXT:
                    table[row- rowShift][col] = cell.value.text_value;
                    columnType[col] = itSTRING;
                    break;
                case FREEXL_CELL_DATE:
                case FREEXL_CELL_DATETIME:
                case FREEXL_CELL_TIME:
                    table[row- rowShift][col] = cell.value.text_value;
                    columnType[col] = itDATETIME;
                    break;
                case FREEXL_CELL_NULL:
                default:
                    break;
                };
            }
        }
    }

    int column = 0;
    for(auto type : columnType){
        ColumnDefinition coldef;
        if ( type == itINTEGER){
            coldef = ColumnDefinition(columnNames[column],DataDefinition({"integer"}), column);
        } else if ( type == itDOUBLE) {
            coldef = ColumnDefinition(columnNames[column],DataDefinition({"value"}), column);
        } else if ( type == itDATETIME){
            coldef = ColumnDefinition(columnNames[column],DataDefinition({"time"}), column);
        } else {
            coldef = ColumnDefinition(columnNames[column],DataDefinition({"text"}), column);
        }
        ++column;
        tbl->addColumn(coldef);
    }
    for(int rec = 0; rec < rows - rowShift; ++rec){
        tbl->record(rec, table[rec]);
    }

    freexl_close(handle);
    return true;
}

bool ExcelTableConnector::storeMetaData(Ilwis::IlwisObject *obj)
{
return false;
}

QString ExcelTableConnector::type() const
{
    return "Table";
}

IlwisObject *ExcelTableConnector::create() const
{
    return new FlatTable(_resource);
}

bool ExcelTableConnector::loadData(IlwisObject *, const LoadOptions &options)
{
    return true;
}

bool ExcelTableConnector::storeBinaryData(IlwisObject *obj)
{
    return false;
}

QString ExcelTableConnector::provider() const
{
    return "spreadsheet";
}

ConnectorInterface *ExcelTableConnector::create(const Ilwis::Resource &resource, bool load, const PrepareOptions &options)
{
    return new ExcelTableConnector(resource, load, options);
}
