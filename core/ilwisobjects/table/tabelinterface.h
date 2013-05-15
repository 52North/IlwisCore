#ifndef TABELINTERFACE_H
#define TABELINTERFACE_H

class TableInterface {
public:
    virtual quint32 rows() const = 0;
    virtual quint32 columns() const = 0;
    virtual bool createTable( const QVector<Column> &cols) = 0;
    virtual Column column(const QString& nme) const = 0;
    virtual Column column(quint32 index) const = 0;
    virtual Record record(quint32 row) = 0;
    virtual Record record(quint32 row, const Record& rec) = 0;
    virtual QVariant value(quint32 row, quint32 col) const = 0;



};

#endif // TABELINTERFACE_H
