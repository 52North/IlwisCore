#ifndef FLATTABLE_H
#define FLATTABLE_H

namespace Ilwis {
class KERNELSHARED_EXPORT FlatTable : public BaseTable
{
public:
    FlatTable();
    FlatTable(const Resource& resource);
    ~FlatTable();

    /*!
    \se Ilwis::Table
     */
    bool createTable();
    /*!
    \se Ilwis::Table
     */
    std::vector<QVariant> record(quint32 n) const ;
    /*!
    \se Ilwis::Table
     */
    void record(quint32, const std::vector<QVariant> &vars, quint32 offset=0);
    /*!
    \se Ilwis::Table
     */
    std::vector<QVariant> column(const QString& nme, quint32 start=0, quint32 stop=2e9) const;
    std::vector<QVariant> column(quint32 index, quint32 start=0, quint32 stop=2e9) const;
    /*!
    \se Ilwis::Table
     */
    void column(const QString& nme, const std::vector<QVariant> &vars, quint32 offset=0);
    void column(quint32 index, const std::vector<QVariant>& vars, quint32 offset);
    /*!
    \se Ilwis::Table
     */
    QVariant cell(const QString& col, quint32 rec, bool asRaw=true) const;
    QVariant cell(const quint32 index, quint32 rec, bool asRaw=true) const;
    /*!
    \se Ilwis::Table
     */
    void cell(const QString& col, quint32 rec, const QVariant& var);
    void cell(quint32, quint32 rec, const QVariant& var);
    /*!
    \se Ilwis::Table
     */
    bool prepare();
    /*!
    \se Ilwis::Table
     */
    bool isValid() const;
    /*!
    \se Ilwis::Table
     */
    bool addColumn(const QString &name, const IDomain& domain);
    /*!
    \se Ilwis::Table
     */
    bool addColumn(const ColumnDefinition& def);
    std::vector<quint32> select(const QString &conditions) const;

    IlwisTypes ilwisType() const;
    IlwisObject *copy() ;

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


};
typedef IlwisData<FlatTable> IFlatTable;
}

#endif // FLATTABLE_H
