#ifndef ATTRIBUTEDEFINTION_H
#define ATTRIBUTEDEFINTION_H

#include "boost/container/flat_map.hpp"

namespace Ilwis {
class BaseTable;
class IlwisObject;
class FeatureAttributeDefinition;

class KERNELSHARED_EXPORT AttributeDefinition
{
public:

    AttributeDefinition();

    bool addColumn(const ColumnDefinition &def);
    bool addColumn(const QString &name, const QString &domainname);
    ColumnDefinition columndefinition(const QString &nme) const;
    ColumnDefinition columndefinition(quint32 index) const;
    ColumnDefinition &columndefinitionRef(quint32 index);
    const ColumnDefinition &columndefinitionRef(quint32 index) const;
    ColumnDefinition &columndefinitionRef(const QString &columnname);
    void columndefinition(const ColumnDefinition &coldef);
    quint32 columnIndex(const QString &nme) const;
    ColumnDefinition operator[](quint32 index);

    QVariant checkInput(const QVariant &inputVar, quint32 columnIndex) const;
    quint32 definitionCount() const;

private:
    boost::container::flat_map<QString, quint32> _columnDefinitionsByName;
    std::vector<ColumnDefinition> _columnDefinitionsByIndex;
};

class KERNELSHARED_EXPORT SubFeatureDefinition  {
public:
    void setSubDefinition(const IDomain& dom, const std::vector<QString> &items);
    void setSubDefinition(const IDomain &dom, const std::vector<double> &items);
    quint32 index(const QString& variantId) const;
    quint32 index(double domainItem) const;
    QString index(quint32 idx) const;
    std::vector<QString> indexes() const;
    quint32 count() const;
    IDomain domain() const;
    void clear();
    void store(QDataStream& stream);
protected:
    IDomain _subFeatureDomain;
    std::vector<QString> _index2subFeature;
    boost::container::flat_map<QString, quint32> _subFeature2Index;
};

typedef std::unique_ptr<FeatureAttributeDefinition > UPFeatureAttributeDefinition;

class KERNELSHARED_EXPORT FeatureAttributeDefinition : public SubFeatureDefinition, public AttributeDefinition {
public:
    void featureAttributeDefinition(FeatureAttributeDefinition *fad);
    FeatureAttributeDefinition& featureAttributeDefinitionRef(int level=0);
    const FeatureAttributeDefinition& featureAttributeDefinition(int level=0) const;
private:
    UPFeatureAttributeDefinition _subFeatureDefinition;



};
}

#endif // ATTRIBUTEDEFINTION_H
