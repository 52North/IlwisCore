#ifndef PYTHONAPI_FEATURE_H
#define PYTHONAPI_FEATURE_H

#include "pythonapi_geometry.h"
#include "pythonapi_columndefinition.h"
#include <memory>

namespace Ilwis{    
    class FeatureInterface;
    class SPFeatureI;
}

class QVariant;

typedef struct _object PyObject;

namespace pythonapi{

    class FeatureCoverage;
    const int COVERAGEATRIB = 100000000;
    class Feature: public Object{
        friend class Geometry;
        friend class FeatureCoverage;
        friend class FeatureIterator;
    public:
        bool __bool__() const;
        std::string __str__();
        VertexIterator __iter__();

        Feature createSubFeature(PyObject* subFeatureIndex, const Geometry& geom) ;

        PyObject* __getitem__(std::string name);
        PyObject* __getitem__(quint32 colIndex);
        PyObject* attribute(std::string name, qint64 defaultValue);
        PyObject* attribute(std::string name, double defaultValue);
        PyObject* attribute(std::string name, std::string defaultValue);
        void __setitem__(std::string name, const PyObject* value);
        void setAttribute(std::string name, const PyObject* value);
        void __setitem__(std::string name, qint64 value);
        void setAttribute(std::string name, qint64 value);
        void __setitem__(std::string name, double value);
        void setAttribute(std::string name, double value);
        void __setitem__(std::string name, std::string value);
        void setAttribute(std::string name, std::string value);
        IlwisTypes ilwisType();

        Geometry* geometry();
        void geometry(const Geometry& geom);
        IlwisTypes geometryType();

        quint64 featureId() const;

        pythonapi::ColumnDefinition attributeDefinition(const std::string& name) const;
        pythonapi::ColumnDefinition attributeDefinition(quint32 index) const;
        quint32 attributeColumnCount() const;

        void removeSubFeature(const std::string& subFeatureIndex);
        void setSubFeature(const std::string &subFeatureIndex, Feature feature);
        void removeSubFeature(double subFeatureIndex);
        void setSubFeature(double subFeatureIndex, Feature feature);
        quint32 subFeatureCount() const;

        void setRecord(PyObject* pyValues, quint32 offset = 0);
        PyObject* record();

        VertexIterator begin();
        VertexIterator end();

    private:
        PyObject* attribute(std::string name, const QVariant& defaultValue);
        QVariant* checkIndex(PyObject* obj);
        Feature(std::shared_ptr<Ilwis::FeatureInterface> ilwisFeature, FeatureCoverage* fc);
        Feature(Ilwis::SPFeatureI* ilwFeat, FeatureCoverage* fc);
        std::shared_ptr<Ilwis::FeatureInterface> ptr() const;
        std::shared_ptr<Ilwis::FeatureInterface> _ilwisSPFeatureI;
        FeatureCoverage* _coverage;
    };

}

#endif // PYTHONAPI_FEATURE_H
