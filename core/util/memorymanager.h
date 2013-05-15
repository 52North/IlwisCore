template<class T> class MemoryManager {
	struct MemoryPool {
		T * _pool;
		vector<unsigned long> _freeList;

	};
public:
	MemoryManager(unsigned long initSize=10000) : size(initSize){
		createNewPool();
	}

	~MemoryManager() {
		for(int i = 0; i < _pools.size(); ++i) {
			delete [] _pools[i]._pool;
		}
	}

	T* allocate() {
		if ( _pools[current]._freeList.size() == 0) {
			int i=0;
			// try to find a pool that has space
			for( ; i < _pools.size(); ++i) {
				if( _pools[i]._freeList.size() > 0) {
					current = i;
					break;
				}
			}
			if ( i == _pools.size()) // no pools to be found
				createNewPool();
		}
		unsigned long index = _pools[current]._freeList.back();
		_pools[current]._freeList.pop_back();
		return &_pools[current]._pool[index];			
	}

	// frees the index of the used pointer,
	void deallocate(T* object) {
		unsigned long ptr = (unsigned long)object;		
		for(int i=0 ; i < _pools.size(); ++i) {
			unsigned long begin = (unsigned long)_pools[i]._pool;
			unsigned long end = begin + sizeof(T)*size;
			if ( ptr >= begin && ptr <= end) {
				unsigned long index = (end - ptr) / sizeof(T);
				current = i;
				if ( index > 0){
					_pools[current]._freeList.push_back(size - index);
					break;
				}
			}
		}
	}

private:
	void createNewPool() {
		list<unsigned long> newList;
		MemoryPool p;
		p._pool = new T[size];
		_pools.push_back(p);
		current = _pools.size() - 1;
		_pools[current]._freeList.resize(size);
		for(unsigned long index = 0; index< size; ++index)
			_pools[current]._freeList[index] = index;
		


	}
	vector<MemoryPool> _pools;
	int current;
	unsigned long size;

};