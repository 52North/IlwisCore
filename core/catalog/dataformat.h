#ifndef DATAFORMAT_H
#define DATAFORMAT_H

namespace Ilwis {
class KERNELSHARED_EXPORT DataFormat
{
public:
    enum FormatProperties{fpCODE, fpNAME, fpDESCRIPTION, fpEXTENSION, fpCONTAINER, fpDATATYPE, fpCONNECTOR, fpREADWRITE, fpEXTENDEDTYPE};
    DataFormat();
    DataFormat(const QString& code, const QString connector = sUNDEF);
    QVariant property(FormatProperties prop) const;
    bool isValid() const;

    static QVariantList getFormatProperties(FormatProperties, IlwisTypes types, QString connector=sUNDEF, QString code=sUNDEF);
    static bool supports(FormatProperties fp, IlwisTypes tp, const QVariant& prop, const QString& connector=sUNDEF);
    static bool setFormatInfo(const QString &path, const QString connector);
    static std::multimap<QString, DataFormat> getSelectedBy(FormatProperties prop, const QString &selection);

private:
    QVariant set(const QVariant& original) const;
    std::map<FormatProperties, QVariant> _properties;
    bool _isValid = false;

    void setProps(QSqlQuery &db, const QString &code);
};
}

#endif // DATAFORMAT_H
