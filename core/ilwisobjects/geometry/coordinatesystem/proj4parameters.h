#ifndef PROJ4PARAMETERS_H
#define PROJ4PARAMETERS_H

#include <map>

struct Proj4Def{
    Proj4Def(const QString& name=sUNDEF, const QString& proj4def=sUNDEF, const QString epsg=sUNDEF) : _proj4def(proj4def), _name(name), _epsg(epsg){}
    QString _proj4def;
    QString _name;
    QString _epsg;
};

class KERNELSHARED_EXPORT Proj4Parameters
{
public:
    Proj4Parameters(const QString& parms);
    QString operator [](const QString& key) const;
    bool hasDatum() const;
    bool operator==(const Proj4Parameters& parms) const;


    static void add2lookup(const QString &name, const QString &proj4def, const QString epsg);
    static Proj4Def lookupDefintion(const QString &proj4Def);
    static Proj4Def lookupByName(const QString &name);
private:
    static quint32 hash(QString code);
    void parseShifts(const QString &shifts);
    QString datum() const;
    std::map<QString, QString> _keyvalues;
    bool _hasDatum;

    static std::multimap<quint32, Proj4Def> _lookup;
    static std::map<QString, QString> _lookupByName; // to proj4defr
    static QString simplifyName(QString name);
};

#endif // PROJ4PARAMETERS_H
