#ifndef DATAFORMAT_H
#define DATAFORMAT_H

namespace Ilwis {
class KERNELSHARED_EXPORT DataFormat
{
public:
    enum FormatProperties{fpCODE, fpNAME, fpDESCRIPTION, fpEXTENSION, fpCONTAINER, fpDATATYPE, fpCONNECTOR, fpREADWRITE};
    DataFormat();
    DataFormat(const QString& code, const QString connector = sUNDEF);
    static QStringList getFormatProperties(FormatProperties, IlwisTypes types, QString connector=sUNDEF, QString code=sUNDEF);
    static bool setFormatInfo(const QString &path, const QString connector);
    QVariant property(FormatProperties prop) const;
    bool isValid() const;
private:
    QVariant set(const QVariant& original) const;
    std::map<FormatProperties, QVariant> _properties;
    bool _isValid = false;

};
}

#endif // DATAFORMAT_H
