#ifndef TRANQUILIZER_H
#define TRANQUILIZER_H

#include <QObject>
#include <memory>
#include "locker.h"
#include "iooptions.h"

namespace Ilwis {

class KERNELSHARED_EXPORT TranquilizerWorker : public QObject {
    Q_OBJECT

public:
    TranquilizerWorker();
public slots:

    void updateTranquilizer(quint64 id, double amount);
    void createTranquilizer(quint64 id, const QString &title, const QString &description, double start, double end);
    void removeTranquilizer(quint64 id);
signals:
    void sendUpdateTranquilizer(quint64 id, double amount);
    void sendCreateTranquilizer(quint64 id,const QString &title, const QString &description, double start, double end);
    void sendRemoveTranquilizer(quint64 id);
    //void sendtranquilizersChanged();
};

class KERNELSHARED_EXPORT Tranquilizer : public QObject{
public:
    explicit Tranquilizer(QObject *parent = 0);
    ~Tranquilizer();
    virtual double current() const = 0;
    virtual void current(double cur) =0;
    virtual void end(double number) = 0;
    virtual double end() const = 0;
    virtual void prepare(const QString &title, const QString &description, double end, double start=0) = 0;

    virtual bool update(double step) = 0;
    virtual bool inform(QString msg) { return true; }
    virtual void stop() = 0;

    static Tranquilizer *create(int mode, const Ilwis::IOOptions &options = IOOptions());
};

class KERNELSHARED_EXPORT BaseTranquilizer : public Tranquilizer
{
    Q_OBJECT
public:
    explicit BaseTranquilizer(const IOOptions &, QObject *parent = 0);
    ~BaseTranquilizer();

    double current() const;
    void current(double cur);
    void end(double number);
    double end() const;

protected:
    void prepare(const QString &title, const QString &description, double end, double start=0);

    static quint64 _trqId;
    quint64 _id;
    QString _title;
    QString _desc;
    double _start;
    double  _end;
    double _current;
    std::mutex _mutex;
    std::unique_ptr<Tranquilizer> _tranquilizer;
signals:
    void updateTranquilizer(quint64 id, double current);
    void removeTranquilizer(quint64 id);
    void tranquilizerCreated(quint64 id, const QString &title, const QString &description, quint64 end);
    
public slots:
    void stopTranquilizer();
    
};

class EmptyTranquilizer : public BaseTranquilizer{
    Q_OBJECT
public:
    EmptyTranquilizer(const IOOptions &opt, QObject* parent);
    void prepare(const QString &title, const QString &description, double end, double start=0);

    bool update(double step);
    void stop();

    static Tranquilizer *create(const IOOptions& opt);
};

typedef std::unique_ptr<Tranquilizer> UPTranquilizer;
}

#endif // TRANQUILIZER_H
