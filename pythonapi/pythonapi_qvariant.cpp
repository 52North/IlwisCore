#include <QVariant>
#include <QMetaType>
#include "pythonapi_qvariant.h"

#include "../../core/ilwis.h"
#include "../../core/errorobject.h"

#include "pythonapi_pyobject.h"

#include <time.h>
#include "../../core/kernel.h"
#include "../../core/util/range.h"
#include "../../core/ilwisobjects/domain/numericrange.h"
#include "../../core/util/juliantime.h"

#include <math.h>
namespace pythonapi{

    QVariant* PyObject2QVariant(const PyObject* obj){
        if (obj){
            if(PyLongCheckExact(obj)){
                return new QVariant(IVARIANT(PyLongAsLong(obj)));
            }else if(PyFloatCheckExact(obj)){
                return new QVariant(IVARIANT(PyFloatAsDouble(obj)));
            }else if(PyUnicodeCheckExact(obj)){
                return new QVariant(QString::fromStdString(PyBytesAsString(obj)));
            }else if(PyDateCheckExact(obj)){
                Ilwis::Time time(
                            PyDateTimeGET_YEAR(obj),
                            PyDateTimeGET_MONTH(obj),
                            PyDateTimeGET_DAY(obj)
                            );
                return new QVariant(IVARIANT(time));
            }else if(PyTimeCheckExact(obj)){
                Ilwis::Time time(
                            PyDateTimeTIME_GET_HOUR(obj),
                            PyDateTimeTIME_GET_MINUTE(obj),
                            (double)PyDateTimeTIME_GET_SECOND(obj)+(round(((double)PyDateTimeTIME_GET_MICROSECOND(obj)/10000)) / 100)
                            );
                return new QVariant(IVARIANT(time));
            }else if(PyDateTimeCheckExact(obj)){
                Ilwis::Time time(
                            PyDateTimeGET_YEAR(obj),
                            PyDateTimeGET_MONTH(obj),
                            PyDateTimeGET_DAY(obj),
                            PyDateTimeDATE_GET_HOUR(obj),
                            PyDateTimeDATE_GET_MINUTE(obj),
                            (double)PyDateTimeDATE_GET_SECOND(obj)+(round(((double)PyDateTimeDATE_GET_MICROSECOND(obj)/10000)) / 100)
                            );
                return new QVariant(IVARIANT(time));
            } else if(PyDeltaCheckExact(obj)){
                Ilwis::Duration dur(QString::fromStdString(
                            std::to_string(PyDateTimeDELTA_GET_DAYS(obj)) + "D" +
                            std::to_string(PyDateTimeDELTA_GET_SECONDS(obj)) + "s")
                            );
                return new QVariant(IVARIANT(dur));
            }
            throw std::domain_error(QString("Cannot convert PyObject of type %1").arg(typeName(obj)).toStdString());
        }
        throw std::domain_error(QString("Cannot convert NULL pointer").toStdString());
    }//PyObject2QVariant

    PyObject* QVariant2PyObject(const QVariant& var){
        bool ok = false;
        QMetaType::Type t = (QMetaType::Type)var.type();
        if (t == QMetaType::QString){
            QByteArray byteArray = var.toString().toUtf8();
            const char *cString = byteArray.constData();
            return PyUnicodeFromString(cString);
        }else if(t == QMetaType::Bool){
            return PyBoolFromLong(var.toBool());
        }else if(t == QMetaType::Double){
            double ret = var.toDouble(&ok);
            if (!ok)
                throw std::domain_error(QString("Cannot convert '%1' to float").arg(var.toString()).toStdString());
            return PyFloatFromDouble(ret);
        }else if((t == QMetaType::LongLong) || (t == QMetaType::Long) || (t == QMetaType::ULong) || (t == QMetaType::Int) || (t == QMetaType::UInt)){
            qlonglong ret = var.toLongLong(&ok);
            if (!ok)
                throw std::domain_error(QString("Cannot convert '%1' to int(qlonglong)").arg(var.toString()).toStdString());
            return PyLongFromLongLong(ret);
        }else if(t == QMetaType::ULongLong){
            qulonglong ret = var.toULongLong(&ok);
            if (!ok)
                throw std::domain_error(QString("Cannot convert '%1' to int(qulonglong)").arg(var.toString()).toStdString());
            return PyLongFromUnsignedLongLong(ret);
        }else if(QString(var.typeName()).compare("Ilwis::Time") == 0){
            if (var.canConvert<Ilwis::Time>()){
                Ilwis::Time time = var.value<Ilwis::Time>();
                switch(time.valueType()){
                    case itTIME: {
                        double sec = time.get(Ilwis::Time::tpSECOND);
                        int second = (int)sec;
                        int micro = (int)round(((sec-(int)sec)*100));
                        if (micro == 100){
                            second++;
                            micro = 0;
                        }
                        return PyTimeFromTime(
                                time.get(Ilwis::Time::tpHOUR),
                                time.get(Ilwis::Time::tpMINUTE),
                                second,
                                micro*10000
                        );
                    }
                    case itDATE: return PyDateFromDate(
                                time.get(Ilwis::Time::tpYEAR),
                                time.get(Ilwis::Time::tpMONTH),
                                time.get(Ilwis::Time::tpDAYOFMONTH)
                            );
                    case itDATETIME:{
                        double sec = time.get(Ilwis::Time::tpSECOND);
                        int second = (int)sec;
                        int micro = (int)round(((sec-(int)sec)*100));
                        if (micro == 100){
                            second++;
                            micro = 0;
                        }
                        return PyDateTimeFromDateAndTime(
                                    time.get(Ilwis::Time::tpYEAR),
                                    time.get(Ilwis::Time::tpMONTH),
                                    time.get(Ilwis::Time::tpDAYOFMONTH),
                                    time.get(Ilwis::Time::tpHOUR),
                                    time.get(Ilwis::Time::tpMINUTE),
                                    second,
                                    micro*10000
                                );
                    }
                    default: throw std::domain_error(QString("Cannot convert Ilwis::Time object to datetime").toStdString());
                }
            }else{
                throw std::domain_error(QString("Cannot convert Ilwis::Time object to datetime").toStdString());
            }
        }
        throw std::domain_error(QString("Cannot convert: %1 of type %2").arg(var.toString()).arg(QMetaType::typeName(t)).toStdString());
    }


    PyObject* StdVectorOfQVariant2PyTuple(const std::vector<QVariant>& vec){
        PyObject* list = newPyTuple(vec.size());
        int i = 0;
        for(auto it = vec.begin(); it != vec.end(); it++){
            setTupleItem(list, i++, QVariant2PyObject(*it));
        }
        return list;
    }

}//namespace pythonapi
