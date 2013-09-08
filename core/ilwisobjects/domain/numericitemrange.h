#ifndef NUMERICITEMRANGE_H
#define NUMERICITEMRANGE_H

namespace Ilwis {

class NumericItemRange : public ItemRange
{
public:
    NumericItemRange();

    QString value(quint32 index) const ;
    quint32 count() const ;
    SPDomainItem item(quint32 index) const ;
    SPDomainItem item(const QString &def) const ;
    virtual double index(double v) const;
    bool contains(const QString& name) const ;
    bool isValid() const ;
    void add(DomainItem *item) ;
    void remove(const QString& nm) ;
    QString toString() const;
    ItemRange *clone() const;
    NumericItemRange &operator <<(const QString &itemdef);
    bool isContinous() const;
    void setInterpolation(const QString& ip);
    double index(double v);
private:
    std::vector<SPNumericItem> _items;
    QString _interpolation;
};
}

#endif // NUMERICITEMRANGE_H
