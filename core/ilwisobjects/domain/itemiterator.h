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
     * Constructs an ItemIterator from an ItemDomain and a current position<br>
     * The itemdomain cannot be null and the position must be valid
     *
     * @param dom the ItemDomain in IlwisData form
     * @param cur the current position in this domain, default = 0
     */
    ItemIterator( const IlwisData<ItemDomain<T>>& dom, quint32 cur=0) : _current(cur), _range(dynamic_cast<ItemRange *>(dom->_range.data())){

    }

    /**
     * Constructs an ItemIterator from an ItemDomain and a current possition<br>
     * The itemdomain cannot be null and the position must be valid
     *
     * @param dom The ItemDomain
     * @param cur The current position in this domain, default = 0
     */
    ItemIterator( const ItemDomain<T>& dom, quint32 cur=0) : _current(cur), _range(dynamic_cast<ItemRange *>(dom._range.data())){

    }

    /**
     * @brief Constructs an ItemIterator from a SPItemRange and a currentposition
     *
     * @param rng The SPItemrange
     * @param cur The current position in this range, default = 0
     */
    ItemIterator( ItemRange *rng, quint32 cur=0) : _current(cur), _range(rng){

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
     * override of operator+=
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
     * override of operator-=
     * moves this ItemIterator by 1
     *
     * @param n the amount to move
     * @return this moved -n
     */
    ItemIterator& operator-=(int n) {
        move(-n);
        return *this;
    }

    /**
     * override of operator<
     * compares this ItemIterator with another based on the currentposition
     * only works if both have the same range
     *
     * @param iter the ItemIterator to compare with
     * @return true when this is smaller (also false when the ranges are not equal)
     */
    ItemIterator& operator<(const ItemIterator& iter) {
        return _range == iter._range && _current < iter._current;
    }

    /**
     * override of operator>
     * compares this ItemIterator with another based on the currentposition
     * only works if both have the same range
     *
     * @param iter the ItemIterator to compare with
     * @return true when this is bigger (also false when the ranges are not equal)
     */
    ItemIterator& operator>(const ItemIterator& iter) {
        return _range == iter._range && _current > iter._current;
    }

    /**
     * override of operator<=
     * compares this ItemIterator with another based on the currentposition
     * only works if both have the same range
     *
     * @param iter the ItemIterator to compare with
     * @return true when this is smaller or equal (also false when the ranges are not equal)
     */
    ItemIterator& operator<=(const ItemIterator& iter) {
        return _range == iter._range && _current <= iter._current;
    }

    /**
     * override of operator<=
     * compares this ItemIterator with another based on the currentposition
     * only works if both have the same range
     *
     * @param iter the ItemIterator to compare with
     * @return true when this is bigger or equal (also false when the ranges are not equal)
     */
    ItemIterator& operator>=(const ItemIterator& iter) {
        return _range == iter._range && _current >= iter._current;
    }

    /**
     * override of operator *
     * @return
     */
    T *operator*() {
        if ( _current != iUNDEF) {
            T *obj = static_cast<T *>(_range->itemByOrder(_current).data());
            return obj;
        }
        return 0;
    }

    /**
     * override of operator++
     * moves this ItemIterator by 1
     *
     * @return this moved 1
     */
    ItemIterator operator++(int){
        ItemIterator temp = *this;
        move(1);
        return temp;
    }

    /**
     * override of operator--
     * moves this ItemIterator by -1
     *
     * @return this moved -1
     */
    ItemIterator operator--(int){
        ItemIterator temp = *this;
        move(-1);
        return temp;
    }

    /**
     * override of operator[]
     * gives this iterator at the index between brackets
     *
     * @param index the index
     * @return this at the given index
     */
    ItemIterator& operator[](quint32 index) {
        _range->gotoIndex(0, index);
    }

    /**
     * override of operator==
     * checks if this ItemIterator is equal to another
     * only works if neither position is iUNDEF and the ranges are equal
     *
     * @param iter the ItemIterator to compare to
     * @return true when the current position between the 2 is equal (also false if the conditions are not met)
     */
    bool operator==(const ItemIterator& iter) const{
        return _current != iUNDEF && iter._current!= iUNDEF && _current == iter._current && _range ==iter._range;
    }

    /**
     * override of operator!=
     * checks if this ItemIterator is equal to another
     * only works if neither position is iUNDEF and the ranges are equal
     *
     * @param iter the ItemIterator to compare to
     * @return true when the current position between the 2 is  not equal (also false if the conditions are not met)
     */
    bool operator!=(const ItemIterator& iter) const{
        return !operator==(iter);
    }

    /**
     * Checks if this ItemIterator is valid
     *
     * @return true when valid
     */
    bool isValid() {
        return _range->isValid() && _current <_range->count();
    }

    /**
     * Generates this ItemIterator at position 0
     *
     * @return this at position 0
     */
    ItemIterator begin() const {
        return ItemIterator(_range,0);
    }

    /**
     * Generates this ItemIterator at its last position
     *
     * @return this at the last position
     */
    ItemIterator end() const {
        return ItemIterator(_range,_range->count());
    }

private:
    void move(qint32 n) {
        _current = _range->gotoIndex(_current, n);
    }

    quint32 _current;
    ItemRange *_range;
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
