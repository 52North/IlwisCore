#include "tranquilizerhandler.h"


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
        return  QQmlListProperty<TranquilizerModel>(this, _tranquilizers);
}

void TranquilizerHandler::updateTranquilizer(quint64 id, double amount)
{
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
    TranquilizerModel *trq = new TranquilizerModel(id, title, description,start, end,this);

    _tranquilizers.push_back(trq);

    //emit createTranquilizerUI(id, title, description, start, end);
    emit tranquilizersChanged();
}

void TranquilizerHandler::removeTranquilizer(quint64 id)
{
    for(auto trq  = _tranquilizers.begin(); trq != _tranquilizers.end(); ++trq){
        if ( (*trq)->id() == id){
            _tranquilizers.erase(trq);
            emit tranquilizersChanged();
            emit aggregateValueChanged();
            (*trq)->deleteLater();
            return;
        }
    }
}

int TranquilizerHandler::aggregateValue() const
{
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
    return _title;
}

QString TranquilizerModel::description() const
{
    return _description;
}

double TranquilizerModel::endValue() const
{
    return _endValue;
}

double TranquilizerModel::startValue() const{
    return _beginValue;
}

double TranquilizerModel::currentValue() const
{
    return _currentValue;
}

void TranquilizerModel::currentValue(double v)
{
    _currentValue = v;
    emit currentValueChanged();
}

double TranquilizerModel::currentValueP() const
{
    if ( (_endValue - _beginValue) == 0)
        return 0;

    return (_currentValue - _beginValue) / (_endValue - _beginValue);
}

quint64 TranquilizerModel::id() const
{
    return _id;
}
