#ifndef CONSOLETRANQUILIZER_H
#define CONSOLETRANQUILIZER_H

namespace Ilwis {

// FOR THE MOMENT EMPTY IMPLEMENTATION

class ConsoleTranquilizer : public BaseTranquilizer
{
public:
    ~ConsoleTranquilizer();

    ConsoleTranquilizer(const IOOptions &opt, QObject* parent);
    void prepare(const QString &title, const QString &description, double end, double start=0);

    void update(double step);
    void stop();

    static Tranquilizer *create(const IOOptions& opt);
};
}

#endif // CONSOLETRANQUILIZER_H
