#ifndef COLORITEM_H
#define COLORITEM_H

namespace Ilwis {

#define clrUNDEF QColor()

class ColorItem : public DomainItem
{
public:
    ColorItem(const QColor &clr=clrUNDEF);

    //@override
    bool isValid() const;

    //@override
    QString name() const ;

    //@override
    DomainItem *clone() const;

    //@override
    IlwisTypes valueType() const;

    QColor color() const;
    void color(const QColor clr);

private:
    QColor _color;
};

typedef QSharedPointer<ColorItem> SPColorItem;


}

#endif // COLORITEM_H
