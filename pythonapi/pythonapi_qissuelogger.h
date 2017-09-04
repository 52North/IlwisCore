#ifndef PYTHONAPI_ISSUELOGGER_H
#define PYTHONAPI_ISSUELOGGER_H

#include "../../core/issuelogger.h"
#include <QObject>

namespace pythonapi{

    class QIssueLogger: public QObject{
        Q_OBJECT

    public:
        QIssueLogger();

    public slots:
        void ilwiserrormessage(const Ilwis::IssueObject& issueObject);
    };

}

#endif // PYTHONAPI_ISSUELOGGER_H
