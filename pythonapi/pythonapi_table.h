#ifndef PYTHONAPI_TABLE_H
#define PYTHONAPI_TABLE_H

#include "pythonapi_ilwisobject.h"
#include "pythonapi_columndefinition.h"

namespace Ilwis {
    class Table;
    typedef IlwisData<Table> ITable;
}

typedef struct _object PyObject;

namespace pythonapi {
    class ColumnDefinition;
    class Table : public IlwisObject{
        friend class Engine;
        friend class Coverage;
        friend class Catalog;
        friend class FeatureCoverage;
        public:
            Table();
            Table(std::string resource, const IOOptions& opt = IOOptions());

            quint32 columnCount() const;
            PyObject* columns() const;
            void addColumn(const std::string& name, const std::string &domain);
            void addColumn(ColumnDefinition& coldef);
            qint32 columnIndex(const std::string& name) const;
            PyObject* column(const std::string& name) const;
            PyObject* column(quint32 columnIndex) const;

            quint32 recordCount() const;
            PyObject* select(const std::string& conditions) const;
            PyObject* record(quint32 rec) const;

            PyObject* cell(const std::string& name, quint32 rec);
            PyObject* cell(quint32 colIndex, quint32 rec);
            void setCell(const std::string &name, quint32 rec, const PyObject* value);
            void setCell(quint32 colIndex, quint32 rec, const PyObject* value);
            void setCell(const std::string &name, quint32 rec, qint64 value);
            void setCell(quint32 colIndex, quint32 rec, qint64 value);
            void setCell(const std::string &name, quint32 rec, std::string value);
            void setCell(quint32 colIndex, quint32 rec, std::string value);
            void setCell(const std::string &name, quint32 rec, double value);
            void setCell(quint32 colIndex, quint32 rec, double value);

            static Table* toTable(Object* obj);

            ColumnDefinition columnDefinition(const std::string& name) const;
            ColumnDefinition columnDefinition(quint32 index) const;
            void setColumnDefinition(ColumnDefinition& coldef);
            void setColumnDefinition(const std::string& name, ColumnDefinition* coldef);
            void setColumnDefinition(quint32 index, ColumnDefinition* coldef);

        protected:
            virtual const QString getStoreFormat() const;

        private:
            Table(const Ilwis::ITable& table);
    };

} // namespace pythonapi

#endif //PYTHONAPI_TABLE_H
