#ifndef FLATTABLE_H
#define FLATTABLE_H

namespace Ilwis {
class KERNELSHARED_EXPORT FlatTable : public BaseTable
{
public:
    FlatTable();
    FlatTable(const Resource& res);
    ~FlatTable();

    /*!
    \se Ilwis::Table
     */
    bool createTable();
    /*!
    \se Ilwis::Table
     */
    QVariantList record(quint32 n) const ;
    /*!
    \se Ilwis::Table
     */
    void record(quint32, const QVariantList& vars, quint32 offset=0);
    /*!
    \se Ilwis::Table
     */
    QVariantList column(const QString& nme) const;
    /*!
    \se Ilwis::Table
     */
    void column(const QString& nme, const QVariantList& vars, quint32 offset=0);
    /*!
    \se Ilwis::Table
     */
    QVariant cell(const QString& col, quint32 rec) const;
    /*!
    \se Ilwis::Table
     */
    void cell(const QString& col, quint32 rec, const QVariant& var);
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

    IlwisTypes ilwisType() const;
protected:
    bool isColumnIndexValid(quint32 index) const{
        bool ok =  index != iUNDEF && _datagrid.size() != 0 && index < _datagrid[0].size();
        if ( ok)
            return true;
        else
            return false;
    }
    std::vector< std::vector<QVariant> > _datagrid;


};
typedef IlwisData<FlatTable> IFlatTable;
}

#endif // FLATTABLE_H
