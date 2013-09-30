#ifndef BASETABLE_H
#define BASETABLE_H

#include <QSqlDatabase>
#include <unordered_map>

#include "table.h"

#include "Kernel_global.h"

namespace Ilwis {

class KERNELSHARED_EXPORT BaseTable : public Table
{
public:

    BaseTable();
    BaseTable(const Resource& resource);
    ~BaseTable();

    /*!
    \se Ilwis::Table
     */
    virtual quint32 records() const;
    /*!
    \se Ilwis::Table
     */
    virtual quint32 columns() const;
    /*!
    \se Ilwis::Table
     */
    virtual void setRows(quint32 r);
    /*!
    \se Ilwis::Table
     */
    virtual bool createTable() ;
    /*!
    \se Ilwis::Table
    */
    virtual bool addColumn(const QString &name, const IDomain &domain);
    virtual bool addColumn(const QString &name, const QString& domainname);
    /*!
    \se Ilwis::Table
     */
    virtual bool addColumn(const ColumnDefinition& def);
    /*!
    \se Ilwis::IlwisObject::ilwisType
     */
    IlwisTypes ilwisType() const;
    /*!
    \se Ilwis::IlwisObject::ilwisType
     */
    ColumnDefinition columndefinition(const QString& nme) const;
    ColumnDefinition columndefinition(quint32 index) const;
//    /*!
//    \se Ilwis::Table
//     */
//    virtual std::vector<QVariant> record(quint32 n) const = 0;
//    /*!
//    \se Ilwis::Table
//     */
//    virtual void record(quint32 rec, const QVariantList& vars, quint32 offset=0) = 0;
//    /*!
//    \se Ilwis::Table
//     */
//    virtual std::vector<QVariant> column(const QString& nme) const = 0;
//    /*!
//    \se Ilwis::Table
//     */
//    virtual void column(const QString& nme, const std::vector<QVariant>& vars, quint32 offset=0) = 0;
//    /*!
//    \se Ilwis::Table::column
//     */
//    virtual QVariant cell(const QString& col, quint32 rec) const = 0;
//    /*!
//    \se Ilwis::Table::cell
//     */
//    virtual void cell(const QString& col, quint32 rec, const QVariant& var) = 0;


    bool prepare();
    /*!
    a table is valid if it has rows and columns
     * \return true when valid.
     */
    bool isValid() const;
    ColumnDefinition& columndefinition(quint32 index);

protected:
    quint32 _rows;
    quint32 _columns;
    QHash<QString, ColumnDefinition> _columnDefinitionsByName;
    QHash<quint32, ColumnDefinition> _columnDefinitionsByIndex;
    bool _dataloaded;

    virtual bool initLoad();
    quint32 columnIndex(const QString& nme) const;
};
}


#endif // BASETABLE_H
