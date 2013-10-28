#ifndef ITEMITERATOR_H
#define ITEMITERATOR_H

namespace Ilwis {
template<class C> class ItemDomain;

template<typename T> class ItemIterator : public std::iterator<std::random_access_iterator_tag, T>
{
public:

    /**
     * Constructor for an empty ItemIterator
     */
    ItemIterator() : _current(0){

    }

    /**
     * Constructs an ItemIterator from a ItemDomain and a current position
     *
     * @param dom the ItemDomain in IlwisData form
     * @param cur the current position in this domain, default = 0
     */
    ItemIterator( const IlwisData<ItemDomain<T>>& dom, quint32 cur=0) : _current(cur), _range(dom->_range){

    }

    /**
     * Constructs an ItemIterator from an ItemDomain and a current possition
     *
     * @param dom The ItemDomain
     * @param cur The current position in this domain, default = 0
     */
    ItemIterator( const ItemDomain<T>& dom, quint32 cur=0) : _current(cur), _range(dom._range){

    }

    /**
     * @brief Constructs an ItemIterator from a SPItemRange and a currentposition
     *
     * @param rng The SPItemrange
     * @param cur The current position in this range, default = 0
     */
    ItemIterator( const SPItemRange rng, quint32 cur=0) : _current(cur), _range(rng){

    }

    /**
     * Copy Constructor
     *
     * @param iter ItemIterator that should be copied
     */
    ItemIterator(const ItemIterator& iter) {
        _current = iter._current;
        _range = iter._range;
    }

    /**
     * override of operator=
     *
     * Copies the contents of the second ItemIterator to this one
     *
     * @param The other ItemIterator
     * @return this iterator with the current and range of the other iterator
     */
    ItemIterator& operator=(const ItemIterator& iter) {
        _current = iter._current;
        _range = iter._range;
        return *this;
    }

    /**
     * override of operator++
     * moves this ItemIterator by 1
     *
     * @return this moved 1
     */
    ItemIterator& operator++() {
        move(1);
        return *this;
    }

    /**
     * override of operator--
     * moves this ItemIterator by -1
     *
     * @return this moved -1
     */
    ItemIterator& operator--() {
        move(-1);
        return *this;
    }

    /**
     * override of operator++
     * moves this ItemIterator by n
     *
     * @param n the amount to move
     * @return this moved n
     */
    ItemIterator& operator+=(int n) {
        move(n);
        return *this;
    }

    /**
     * override of operator++
     * moves this ItemIterator by 1
     *
     * @param n
     * @return
     */
    ItemIterator& operator-=(int n) {
        move(-n);
        return *this;
    }

    /**
     * @brief operator <
     * @param iter
     * @return
     */
    ItemIterator& operator<(const ItemIterator& iter) {
        return _range == iter._range && _current < iter._current;
    }

    /**
     * @brief operator >
     * @param iter
     * @return
     */
    ItemIterator& operator>(const ItemIterator& iter) {
        return _range == iter._range && _current > iter._current;
    }

    /**
     * @brief operator <=
     * @param iter
     * @return
     */
    ItemIterator& operator<=(const ItemIterator& iter) {
        return _range == iter._range && _current < iter._current;
    }

    /**
     * @brief operator >=
     * @param iter
     * @return
     */
    ItemIterator& operator>=(const ItemIterator& iter) {
        return _range == iter._range && _current >= iter._current;
    }

    /**
     * @brief operator *
     * @return
     */
    QSharedPointer<T> operator*() {
        if ( _current != iUNDEF) {
            QSharedPointer<T> obj =_range->itemByOrder(_current).staticCast<T>();
            return obj;
        }
        return QSharedPointer<T>();
    }

    /**
     * @brief operator ++
     * @return
     */
    ItemIterator operator++(int){
        ItemIterator temp = *this;
        move(1);
        return temp;
    }

    /**
     * @brief operator --
     * @return
     */
    ItemIterator operator--(int){
        ItemIterator temp = *this;
        move(-1);
        return temp;
    }

    /**
     * @brief operator []
     * @param index
     * @return
     */
    ItemIterator& operator[](quint32 index) {
        _range->gotoIndex(0, index);
    }

    /**
     * @brief operator ==
     * @param iter
     * @return
     */
    bool operator==(const ItemIterator& iter) const{
        return _current != iUNDEF && iter._current!= iUNDEF && _current == iter._current && _range ==iter._range;
    }

    /**
     * @brief operator !=
     * @param iter
     * @return
     */
    bool operator!=(const ItemIterator& iter) const{
        return !operator==(iter);
    }

    /**
     * @brief isValid
     * @return
     */
    bool isValid() {
        return _range->isValid() && _current <_range->count();
    }

    /**
     * @brief begin
     * @return
     */
    ItemIterator begin() const {
        return ItemIterator(_range,0);
    }

    /**
     * @brief end
     * @return
     */
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
