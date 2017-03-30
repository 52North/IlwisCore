#ifndef RASTERFILTER_H
#define RASTERFILTER_H

namespace Ilwis {

class GridBlock;

class KERNELSHARED_EXPORT RasterFilter
{
public:
    RasterFilter();
    bool isValid() const;
    virtual double applyTo(const Ilwis::GridBlock &block) = 0;
    virtual QSize size() const = 0;

protected:
    bool _valid;

};

class KERNELSHARED_EXPORT LinearGridFilter : public RasterFilter{
public:
    LinearGridFilter(const QString& name);

    double applyTo(const Ilwis::GridBlock &block);
    QSize size() const;

private:
    quint32 _columns;
    quint32 _rows;
    double _gain;
    std::vector<std::vector<double>> _filterdef;

    bool definition(const QString& name);
    bool makeCustomFilter(const QString &definition);
    bool fillDef(int xsize, int ysize, const QStringList &numbers);
};

class KERNELSHARED_EXPORT RankOrderGridFilter : public RasterFilter{
public:
    RankOrderGridFilter(const QString& name);

    double applyTo(const Ilwis::GridBlock &block);
    void colrow(quint32 col, quint32 row);
    void index(quint32 index);
    QSize size() const;

private:
    quint32 _columns;
    quint32 _rows;
    quint32 _index;

    bool definition(const QString& name);


};
}

#endif // RASTERFILTER_H
