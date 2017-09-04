#ifndef PYTHONAPI_COLLECTION_H
#define PYTHONAPI_COLLECTION_H

namespace pythonapi {
    class Collection : public Object {
    friend class Engine;
    public:
        virtual bool __bool__() const;
        virtual std::string __str__();
        virtual IlwisTypes ilwisType();
        int __len__();
        Object* _getitem(quint32 colIndex);
        static Collection* toCollection(Object *obj);
    protected:
        Collection(std::vector<Object*> items);
    private:
        std::vector<Object*> _items;
    };
}
#endif // PYTHONAPI_COLLECTION_H
