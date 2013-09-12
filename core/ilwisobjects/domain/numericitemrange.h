#ifndef NUMERICITEMRANGE_H
#define NUMERICITEMRANGE_H

namespace Ilwis {

class KERNELSHARED_EXPORT NumericItemRange : public ItemRange
{
public:
    NumericItemRange();

    QString value(quint32 index) const ;
    quint32 count() const ;
    SPDomainItem item(quint32 index) const ;
    SPDomainItem item(const QString &def) const ;
    virtual double index(double v) const;
    bool contains(const QString& name, bool inclusive = true) const ;
    bool isValid() const ;
    void add(DomainItem *item) ;
    void remove(const QString& nm) ;
    QString toString() const;
    ItemRange *clone() const;
    NumericItemRange &operator <<(const QString &itemdef);
    std::vector<SPDomainItem> items() const;

protected:
    void addRange(const ItemRange &range);
private:
    std::vector<SPNumericItem> _items;
};

typedef QSharedPointer<NumericItemRange> SPNumericItemRange;
}

#endif // NUMERICITEMRANGE_H
