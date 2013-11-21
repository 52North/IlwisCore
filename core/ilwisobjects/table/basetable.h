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
    /**
     * Constructs an empty BaseTable
     */
    BaseTable();

    /**
     * Constructs a new BaseTable using a resource
     *
     * \sa Resource
     * @param resource the Resource that should be used
     */
    BaseTable(const Resource& resource);

    ~BaseTable();

    //@override
    virtual quint32 recordCount() const;

    //@override
    virtual quint32 columnCount() const;

    //@override
    virtual void recordCount(quint32 r);

    //@override
    virtual bool createTable() ;

    //@override
    virtual bool addColumn(const QString &name, const IDomain &domain);

    //@override
    virtual bool addColumn(const QString &name, const QString& domainname);

    //@override
    virtual bool addColumn(const ColumnDefinition& def);

    //@override
    IlwisTypes ilwisType() const;

    //@override
    ColumnDefinition columndefinition(const QString& nme) const;

    //@override
    ColumnDefinition columndefinition(quint32 index) const;

    //@override
    ColumnDefinition& columndefinition(quint32 index);

    //@override
    void columndefinition(const ColumnDefinition &coldef);

//    //@override
//    virtual std::vector<QVariant> record(quint32 n) const = 0;

//    //@override
//    virtual void record(quint32 rec, const QVariantList& vars, quint32 offset=0) = 0;

//    //@override
//    virtual std::vector<QVariant> column(const QString& nme) const = 0;

//    //@override
//    virtual void column(const QString& nme, const std::vector<QVariant>& vars, quint32 offset=0) = 0;

//    //@override
//    virtual QVariant cell(const QString& col, quint32 rec) const = 0;
//
//    //@override
//    virtual void cell(const QString& col, quint32 rec, const QVariant& var) = 0;

    //@override
    bool prepare();

    /*!
    a table is valid if it has rows and columns
     * \return true when valid.
     */
    bool isValid() const;

    /**
     * Merges this BaseTable with another BaseTable.
     * Requires the given object to be a BaseTable, and the object cannot be null ofcourse
     *
     * \sa TableMerger
     * @param obj the object this shouls merge with
     * @param options options for the merging, see TableMerger
     * @return true if the merging succeeded
     */
    bool merge(const IlwisObject *obj, int options);

protected:
    quint32 _rows;
    quint32 _columns;
    QHash<QString, ColumnDefinition> _columnDefinitionsByName;
    QHash<quint32, ColumnDefinition> _columnDefinitionsByIndex;
    bool _dataloaded;

    virtual bool initLoad();
    virtual void adjustRange(int index);
    void copyTo(IlwisObject *obj);
    quint32 columnIndex(const QString& nme) const;
};
}


#endif // BASETABLE_H
