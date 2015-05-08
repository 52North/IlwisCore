#ifndef CONSOLETRANQUILIZER_H
#define CONSOLETRANQUILIZER_H

namespace Ilwis {

// FOR THE MOMENT EMPTY IMPLEMENTATION

class ConsoleTranquilizer : public BaseTranquilizer
{
private:
    double _inc;
    double _next;
    int _count;

public:
    ~ConsoleTranquilizer();

    ConsoleTranquilizer(const IOOptions &opt, QObject* parent);
    void prepare(const QString &title, const QString &description, double end, double start=0);

    bool update(double howfar);
    bool inform(QString msg);
    void stop();

    static Tranquilizer *create(const IOOptions& opt);
};
}

#endif // CONSOLETRANQUILIZER_H
