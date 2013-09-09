#ifndef NUMERICITEM_H
#define NUMERICITEM_H

namespace Ilwis {

class  NumericItemRange;

class KERNELSHARED_EXPORT NumericItem : public DomainItem
{
public:
    NumericItem();
    NumericItem(const NumericRange &vr);

    virtual ~NumericItem() {}

    bool isValid() const;
    QString name(quint32 index= 0) const ;
    DomainItem *clone() const;
    IlwisTypes valueType() const;

    NumericRange range() const;
    void range(const NumericRange &valuerange);
    static Ilwis::NumericItemRange *createRange();
    static IlwisTypes valueTypeS();

private:
    NumericRange _valuerange;
};

struct CompareNumericItem {
    bool operator()(const NumericItem& it1, const NumericItem& it2)     {
        return it1.range() < it2.range();
    }
};

typedef QSharedPointer<NumericItem> SPNumericItem;

}

#endif // NUMERICITEM_H
