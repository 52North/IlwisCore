#ifndef PYTHONAPI_DOMAINITEM_H
#define PYTHONAPI_DOMAINITEM_H

#include "pythonapi_object.h"
#include "pythonapi_pyobject.h"
#include <memory>

namespace Ilwis{
class DomainItem;
}

namespace pythonapi{

    class NumericRange;

    class DomainItem : public Object{

        template<typename OutputType, typename RangeType, typename IlwOutput, typename IlwRange>
        friend class RangeIterator;

    public:
        DomainItem();
        DomainItem(Ilwis::DomainItem* ilwItem);

        bool __bool__() const;
        std::string __str__();
        IlwisTypes ilwisType();

        DomainItem* clone();
        void raw(quint32 rw);
        quint32 raw();

    protected:
        std::shared_ptr<Ilwis::DomainItem> ptr() const;
        std::shared_ptr<Ilwis::DomainItem> _ilwisItem;

        //just a dummy constructor for template methods in rangeiterator
        DomainItem(double d);
    };

//==============================================IndexedIdentifier============================================================

        class IndexedIdentifier : public DomainItem{
        public:
                IndexedIdentifier();
                IndexedIdentifier(const std::string& label, quint32 ind=0, qint32 cnt =iUNDEF);
                IndexedIdentifier(Ilwis::DomainItem* ilwItem);

                std::string prefix();
                void setPrefix(const std::string& name);
                bool operator==(IndexedIdentifier& item) const;
        };



//==============================================NamedIdentifier============================================================

    class NamedIdentifier : public DomainItem{
    public:
            NamedIdentifier();
            NamedIdentifier(const std::string& name, quint32 rawvalue=iUNDEF);
            NamedIdentifier(Ilwis::DomainItem* ilwItem);

            void setName(const std::string& name);
            bool operator==(NamedIdentifier& item) const;
    };

//==============================================Thematic Item============================================================

    class ThematicItem : public NamedIdentifier{
    public:
        ThematicItem();
        ThematicItem(Ilwis::DomainItem* ilwItem);
        ThematicItem(PyObject* tup, quint32 rawValue=iUNDEF);

        std::string description();
        void description(std::string& descr);
        std::string code();
        void code(std::string& code);
    };

//==============================================Interval==============================================================

   class Interval : public DomainItem{
   public:
       Interval();
       Interval(const std::string& label, NumericRange& nr);
       bool isOrdered() const;
       NumericRange range();
       void range(const NumericRange& nr);
   };


}

#endif // PYTHONAPI_DOMAINITEM_H
