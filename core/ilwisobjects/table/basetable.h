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

    /*!
     * The constructor for a BaseTable with a a Resource that is registered in the mastercatalog.<br>
     * This resource must have a name and an Id.<br>
     * Code and description are optional.
     *
     * \sa IlwisObject
     * \sa Resource
     * \param resource The resource to use
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


    //@override
    bool prepare();

    /*!
     * a BaseTable is valid if it has rows and columns
     * \return true when valid.
     */
    bool isValid() const;

    /**
     * Merges this BaseTable with another BaseTable.
     * Requires the given object to be a BaseTable, and the object cannot be null or undefined ofcourse
     *
     * \sa TableMerger
     * @param obj the object this shouls merge with
     * @param options options for the merging, see TableMerger
     * @return true if the merging succeeded
     */
    bool merge(const IlwisObject *obj, int options);
    void dataLoaded(bool yesno);
    bool isDataLoaded() const;
protected:
    QHash<QString, ColumnDefinition> _columnDefinitionsByName;
    QHash<quint32, ColumnDefinition> _columnDefinitionsByIndex;

    virtual bool initLoad();
    virtual void adjustRange(int index);
    void copyTo(IlwisObject *obj);
    quint32 columnIndex(const QString& nme) const;
    void columnCount(int cnt);
    QVariant checkInput(const QVariant &inputVar, quint32 columnIndex);
    void initValuesColumn(const ColumnDefinition &def);
    void initRecord(std::vector<QVariant>& values) const;
private:
    quint32 _rows;
    quint32 _columns;
    bool _dataloaded;
};
}


#endif // BASETABLE_H
