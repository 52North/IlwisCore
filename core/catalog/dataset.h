#ifndef DATASET_H
#define DATASET_H

namespace Ilwis {
class KERNELSHARED_EXPORT DataSet : public Catalog
{
public:
    DataSet();
    DataSet(const Resource& resource);
    ~DataSet();

    bool isValid() const;

protected:
    IIlwisObject createObject(const QVariant &indexValue) const;
private:
    ITable _attributeTable;
    bool prepare();
};
}

#endif // DATASET_H
