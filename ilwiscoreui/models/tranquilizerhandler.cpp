#include "tranquilizerhandler.h"


TranquilizerModel::TranquilizerModel()
{

}

TranquilizerModel::TranquilizerModel(quint64 id, const QString &title, const QString &description, double endval, QObject *obj) :
    QObject(obj),
    _id(id),
    _title(title),
    _description(description),
    _endValue(endval)
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
            //emit updateTranquilizerUI(id, trq->currentValue());
            emit tranquilizersChanged();
            return;
        }
    }
}

void TranquilizerHandler::createTranquilizer(quint64 id, const QString &title, const QString &description, double end)
{
    TranquilizerModel *trq = new TranquilizerModel(id, title, description,end,this);

    _tranquilizers.push_back(trq);

    emit createTranquilizerUI(id, title, description, end);
}

void TranquilizerHandler::removeTranquilizer(quint64 id)
{
    for(auto trq  = _tranquilizers.begin(); trq != _tranquilizers.end(); ++trq){
        if ( (*trq)->id() == id){
            emit removeTranquilizerUI(id);
            (*trq)->deleteLater();
            return;
        }
    }
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

double TranquilizerModel::currentValue() const
{
    return _currentValue;
}

void TranquilizerModel::currentValue(double v)
{
    _currentValue = v;
}

double TranquilizerModel::currentValueP() const
{
    if ( _currentValue == 0)
        return 0;

    return (_endValue - _beginValue) / _currentValue;
}

quint64 TranquilizerModel::id() const
{
    return _id;
}
