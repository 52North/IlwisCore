#ifndef PROJ4PARAMETERS_H
#define PROJ4PARAMETERS_H

class KERNELSHARED_EXPORT Proj4Parameters
{
public:
    Proj4Parameters(const QString& parms);
    QString operator [](const QString& key) const;
    bool hasDatum() const;


private:
    void parseShifts(const QString &shifts);
    QString datum() const;
    QHash<QString, QString> _keyvalues;
    bool _hasDatum;
};

#endif // PROJ4PARAMETERS_H
