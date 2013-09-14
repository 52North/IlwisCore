#ifndef ITEMITERATOR_H
#define ITEMITERATOR_H

namespace Ilwis {
template<class C> class ItemDomain;

template<typename T> class ItemIterator : public std::iterator<std::random_access_iterator_tag, T>
{
public:
    ItemIterator() : _current(0){

    }

    ItemIterator( const IlwisData<ItemDomain<T>>& dom, quint32 cur=0) : _current(cur), _range(dom->_range){

    }

    ItemIterator( const ItemDomain<T>& dom, quint32 cur=0) : _current(cur), _range(dom._range){

    }

    ItemIterator( const SPItemRange rng, quint32 cur=0) : _current(cur), _range(rng){

    }

    ItemIterator(const ItemIterator& iter) {
        _current = iter._current;
        _range = iter._range;
    }

    ItemIterator& operator=(const ItemIterator& iter) {
        _current = iter._current;
        _range = iter._range;
        return *this;
    }

    ItemIterator& operator++() {
        move(1);
        return *this;
    }

    ItemIterator& operator--() {
        move(-1);
        return *this;
    }

    ItemIterator& operator+=(int n) {
        move(n);
        return *this;
    }

    ItemIterator& operator-=(int n) {
        move(-n);
        return *this;
    }

    ItemIterator& operator<(const ItemIterator& iter) {
        return _range == iter._range && _current < iter._current;
    }

    ItemIterator& operator>(const ItemIterator& iter) {
        return _range == iter._range && _current > iter._current;
    }

    ItemIterator& operator<=(const ItemIterator& iter) {
        return _range == iter._range && _current < iter._current;
    }

    ItemIterator& operator>=(const ItemIterator& iter) {
        return _range == iter._range && _current >= iter._current;
    }

    QSharedPointer<T> operator*() {
        if ( _current != iUNDEF) {
            QSharedPointer<T> obj =_range->itemByOrder(_current).staticCast<T>();
            return obj;
        }
        return QSharedPointer<T>();
    }

    ItemIterator operator++(int){
        ItemIterator temp = *this;
        move(1);
        return temp;
    }

    ItemIterator operator--(int){
        ItemIterator temp = *this;
        move(-1);
        return temp;
    }

    ItemIterator& operator[](quint32 index) {
        _range->gotoIndex(0, index);
    }
    bool operator==(const ItemIterator& iter) const{
        return _current != iUNDEF && iter._current!= iUNDEF && _current == iter._current && _range ==iter._range;
    }

    bool operator!=(const ItemIterator& iter) const{
        return !operator==(iter);
    }

    bool isValid() {
        return _range->isValid() && _current <_range->count();
    }


    ItemIterator begin() const {
        return ItemIterator(_range,0);
    }

    ItemIterator end() const {
        return ItemIterator(_range,_range->count());
    }
private:
    void move(qint32 n) {
        _current = _range->gotoIndex(_current, n);
    }

    quint32 _current;
    SPItemRange _range;
};

template<typename T> ItemIterator<T> operator+(const ItemIterator<T>& iter, int n) {
    ItemIterator<T> iterTemp(iter);
    iterTemp += n;
    return iter;
}

template<typename T> ItemIterator<T> operator-(const ItemIterator<T>& iter, int n) {
    ItemIterator<T> iterTemp(iter);
    iterTemp -= n;
    return iter;
}

template<typename T> ItemIterator<T> operator+(int n, const ItemIterator<T>& iter) {
    ItemIterator<T> iterTemp(iter);
    iterTemp += n;
    return iter;
}

template<typename T> ItemIterator<T> operator-(int n,const ItemIterator<T>& iter) {
    ItemIterator<T> iterTemp(iter);
    iterTemp -= n;
    return iter;
}

}
#endif // ITEMITERATOR_H
