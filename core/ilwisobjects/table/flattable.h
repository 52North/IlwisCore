#ifndef FLATTABLE_H
#define FLATTABLE_H

namespace Ilwis {
class KERNELSHARED_EXPORT FlatTable : public BaseTable
{
public:
    /**
     * Constructs an empty FlatTable
     */
    FlatTable();

    /**
     * Constructs a new FlatTable using a resource
     *
     * \sa Resource
     * @param resource the Resource that should be used
     */
    FlatTable(const Resource& resource);

    ~FlatTable();

    //@override
    bool createTable();

    void newRecord();
    //@override
    std::vector<QVariant> record(quint32 n) const ;

    //@override
    void record(quint32, const std::vector<QVariant> &vars, quint32 offset=0);

    //@override
    std::vector<QVariant> column(const QString& nme, quint32 start=0, quint32 stop=2e9) const;

    //@override
    std::vector<QVariant> column(quint32 index, quint32 start=0, quint32 stop=2e9) const;

    //@override
    void column(const QString& nme, const std::vector<QVariant> &vars, quint32 offset=0);

    //@override
    void column(quint32 index, const std::vector<QVariant>& vars, quint32 offset);

    //@override
    QVariant cell(const QString& col, quint32 rec, bool asRaw=true) const;

    //@override
    QVariant cell(const quint32 index, quint32 rec, bool asRaw=true) const;

    //@override
    void setCell(const QString& col, quint32 rec, const QVariant& var);

    //@override
    void setCell(quint32, quint32 rec, const QVariant& var);

    //@override
    bool prepare();

    //@override
    bool isValid() const;

    //@override
    bool addColumn(const QString &name, const IDomain& domain);

    //@override
    bool addColumn(const ColumnDefinition& def);

    //@override
    std::vector<quint32> select(const QString &conditions) const;

    //@override
    IlwisTypes ilwisType() const;

    //@override
    IlwisObject *clone() ;

protected:
    bool isColumnIndexValid(quint32 index) const{
        bool ok =  index != iUNDEF ;
        if ( ok)
            return true;
        else
            return false;
    }
    void copyTo(IlwisObject *obj);
    std::vector< std::vector<QVariant> > _datagrid;

    bool initLoad();
};
typedef IlwisData<FlatTable> IFlatTable;
}

#endif // FLATTABLE_H
