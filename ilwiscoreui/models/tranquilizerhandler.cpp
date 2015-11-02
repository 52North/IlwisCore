#include "tranquilizerhandler.h"
#include "uicontextmodel.h"


TranquilizerModel::TranquilizerModel()
{

}

TranquilizerModel::TranquilizerModel(quint64 id, const QString &title, const QString &description, double start, double endval, QObject *obj) :
    QObject(obj),
    _id(id),
    _title(title),
    _description(description),
    _beginValue(start),
    _endValue(endval),
    _currentValue(start)
{

}

//---------------------------------------------------------------------------------
TranquilizerHandler::TranquilizerHandler(QObject *parent) :
    QObject(parent)
{
}

TranquilizerList TranquilizerHandler::tranquilizers()
{
    Locker<> lock(_mutex);
    return  QQmlListProperty<TranquilizerModel>(this, _tranquilizers);
}

void TranquilizerHandler::updateTranquilizer(quint64 id, double amount)
{
    Locker<> lock(_mutex);
    for(auto trq : _tranquilizers){
        if ( trq->id() == id){
            trq->currentValue(amount);
            emit aggregateValueChanged();
            return;
        }
    }
}

void TranquilizerHandler::createTranquilizer(quint64 id, const QString &title, const QString &description, double start, double end)
{
    Locker<> lock(_mutex);
    for(TranquilizerModel *trq : _tranquilizers){
        if ( id == trq->id()){
            if ( title != sUNDEF)
                trq->title(title) ;
            if ( description != sUNDEF)
                trq->description(description);
            if ( start != rUNDEF)
                trq->startValue(start);
            if ( end != rUNDEF)
                trq->endValue(end);
            emit tranquilizersChanged();
            return ;
        }
    }
    TranquilizerModel *trq = new TranquilizerModel(id, title, description,start, end,this);

    _tranquilizers.push_back(trq);
    uicontext()->updateThreadCount(1);

    emit tranquilizersChanged();
}

void TranquilizerHandler::removeTranquilizer(quint64 id)
{
    Locker<> lock(_mutex);
    for(auto trq  = _tranquilizers.begin(); trq != _tranquilizers.end(); ++trq){
        if ( (*trq)->id() == id){
            _tranquilizers.erase(trq);
             // TODO this delete cause a crash when quickly moving to folders were one folder has a load of files probably the delete happens before the UI is properly handled and this leads to the use of a deleted pointer
            //(*trq)->deleteLater();
            emit tranquilizersChanged();
            emit aggregateValueChanged();

            uicontext()->updateThreadCount(-1);
            return;
        }
    }
}

int TranquilizerHandler::aggregateValue() const
{
    Locker<> lock(_mutex);
    if ( _tranquilizers.size() == 0)
        return 0;
    int avp = 0;
    for(auto trq  = _tranquilizers.begin(); trq != _tranquilizers.end(); ++trq){
        avp +=  (int)(100 * (*trq)->currentValueP());
    }
    return avp / _tranquilizers.size();
}


QString TranquilizerModel::title() const
{
    Locker<> lock(_mutex);
    return _title;
}

QString TranquilizerModel::description() const
{
    Locker<> lock(_mutex);
    return _description;
}

double TranquilizerModel::endValue() const
{
    Locker<> lock(_mutex);
    return _endValue;
}

double TranquilizerModel::startValue() const{
    Locker<> lock(_mutex);
    return _beginValue;
}

double TranquilizerModel::currentValue() const
{
    return _currentValue;
}

void TranquilizerModel::currentValue(double v)
{
    Locker<> lock(_mutex);
    _currentValue = v;
    emit currentValueChanged();
}

double TranquilizerModel::currentValueP() const
{
    Locker<> lock(_mutex);
    if ( (_endValue - _beginValue) == 0)
        return 0;

    return (_currentValue - _beginValue) / (_endValue - _beginValue);
}

void TranquilizerModel::title(const QString &t)
{
    Locker<> lock(_mutex);
    _title = t;
}

void TranquilizerModel::description(const QString &d)
{
    Locker<> lock(_mutex);
    _description = d;
}

void TranquilizerModel::startValue(double d)
{
    Locker<> lock(_mutex);
    _beginValue = d;
}

void TranquilizerModel::endValue(double d)
{
    Locker<> lock(_mutex);
    _endValue = d;
}

quint64 TranquilizerModel::id() const
{
    Locker<> lock(_mutex);
    return _id;
}
