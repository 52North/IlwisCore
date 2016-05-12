#ifndef RECORD_H
#define RECORD_H

namespace Ilwis {

typedef std::vector<QVariant>::const_iterator CRecordIter;
typedef std::vector<QVariant>::iterator RecordIter;


class KERNELSHARED_EXPORT Record // : private std::vector<QVariant>
{
public:
    friend class FlatTable;
    friend class Feature;

    Record();
    Record(const std::vector<QVariant> &data, quint32 offset = 0);
    Record(const Record& data);

    bool isChanged() const;
    void changed(bool yesno);
    bool isValid() const;

    operator std::vector<QVariant>() const;

    CRecordIter cbegin() const noexcept;
    CRecordIter cend() const noexcept;
    QVariant cell(quint32 column) const;
    void cell(quint32 column, const QVariant& value);
    quint32 columnCount() const;
    operator std::vector<QVariant>() ;


    void storeData(const std::vector<IlwisTypes> &types, QDataStream &stream, const IOOptions &options);
    void loadData(const std::vector<IlwisTypes> &types, QDataStream &stream, const IOOptions &options);
private:
    void addColumn(int n=1){
        if ( _data.size() == 0)
            _data.resize(n);
        else {
            for(int i =0; i < n; ++i)
                _data.push_back(QVariant());
        }
    }

    bool _changed = false;
    quint64 _itemid = i64UNDEF;
    std::vector<QVariant> _data;

};
}

#endif // RECORD_H
