#include <iostream>
#include <list>
#include <functional>
#include <string>

const double max_fact_fill = 1.33;
typedef int I;
typedef double D;
typedef std::string S;

template<typename Key_T, typename Val_T>
class HashMap {
	int size_arr;
	int num_elems;
	std::list<std::pair<Key_T, Val_T>>* hash_arr;
	void init(const int sz);
	void del();
	typename std::list<std::pair<Key_T, Val_T>>::iterator find(const int ind, const Key_T key) const;
	void HashMap_copy(const HashMap& that);
public:
	HashMap();
	HashMap(const int sz);
	HashMap(const HashMap& that);
	HashMap& operator=(const HashMap& that);
	~HashMap();
	void add(const Key_T key, const Val_T val);
	void rebuild();
	Val_T get_value(const Key_T key) const;
	void remove(const Key_T key);
	int fill() const;
	class iterator {
		HashMap* map;
		int index;
		typename std::list<std::pair<Key_T, Val_T>>::iterator it;
	public:
		iterator(HashMap* mp, int ind, typename std::list<std::pair<Key_T, Val_T>>::iterator i);
		iterator() = default;
		iterator(const iterator& that) = default;
		iterator& operator=(const iterator& that);
		bool operator==(const iterator& that) const;
		bool operator!=(const iterator& that) const;
		std::pair<Key_T, Val_T>& operator*() const;
		std::pair<Key_T, Val_T>* operator->() const;
		iterator& operator++();
		iterator operator++(int);
	};
	iterator begin();
	iterator end();
};

template<typename Key_T, typename Val_T>
void HashMap<Key_T, Val_T>::init(const int sz) {
	this->size_arr = sz;
	this->num_elems = 0;
	this->hash_arr = new std::list<std::pair<Key_T, Val_T>>[sz];
}

template<typename Key_T, typename Val_T>
void HashMap<Key_T, Val_T>::del() {
	delete[] this->hash_arr;
}

template<typename Key_T, typename Val_T>
void HashMap<Key_T, Val_T>::HashMap_copy(const HashMap& that) {
	this->init(that.size_arr);
	for (int i = 0; i < that.size_arr; i++) {
		this->hash_arr[i] = that.hash_arr[i];
	}
}

template<typename Key_T, typename Val_T>
HashMap<Key_T, Val_T>::HashMap() {
	this->init(100);
}

template<typename Key_T, typename Val_T>
HashMap<Key_T, Val_T>::HashMap(const int sz) {
	this->init(sz);
}

template<typename Key_T, typename Val_T>
HashMap<Key_T, Val_T>::HashMap(const HashMap& that) {
	this->HashMap_copy(that);
}

template<typename Key_T, typename Val_T>
HashMap<Key_T, Val_T>& HashMap<Key_T, Val_T>::operator=(const HashMap& that) {
	if (this != &that) {
		this->del();
		this->HashMap_copy(that);
	}
	return *this;
}

template<typename Key_T, typename Val_T>
HashMap<Key_T, Val_T>::~HashMap() {
	this->del();
}

template<typename Key_T, typename Val_T>
typename std::list<std::pair<Key_T, Val_T>>::iterator HashMap<Key_T, Val_T>::find(const int ind, const Key_T key) const {
	typename std::list<std::pair<Key_T, Val_T>>::iterator it;
	for (it = this->hash_arr[ind].begin(); it != this->hash_arr[ind].end(); it++) {
		if (it->first == key) {
			break;
		}
	}
	return it;
}

template<typename Key_T, typename Val_T>
void HashMap<Key_T, Val_T>::add(const Key_T key, const Val_T val) {
	std::hash<Key_T> key_hash;
	int ind = key_hash(key) % this->size_arr;
	typename std::list<std::pair<Key_T, Val_T>>::iterator it = this->find(ind, key);
	if (it != this->hash_arr[ind].end()) {
		it->second = val;
	}
	else {
		this->hash_arr[ind].push_back(std::pair<Key_T, Val_T>(key, val));
		++this->num_elems;
	}
	if ((double)this->num_elems / this->size_arr > max_fact_fill) {
		this->rebuild();
	}
}

template<typename Key_T, typename Val_T>
Val_T HashMap<Key_T, Val_T>::get_value(const Key_T key) const {
	std::hash<Key_T> key_hash;
	int ind = key_hash(key) % this->size_arr;
	typename std::list<std::pair<Key_T, Val_T>>::iterator it = this->find(ind, key);
	return it->second;
}

template<typename Key_T, typename Val_T>
void HashMap<Key_T, Val_T>::remove(const Key_T key) {
	std::hash<Key_T> key_hash;
	int ind = key_hash(key) % this->size_arr;
	typename std::list<std::pair<Key_T, Val_T>>::iterator it = this->find(ind, key);
	if (it != this->hash_arr[ind].end()) {
		this->hash_arr[ind].erase(it);
		--this->num_elems;
	}
}

template<typename Key_T, typename Val_T>
void HashMap<Key_T, Val_T>::rebuild() {
	HashMap temp(this->size_arr * 2);
	HashMap<Key_T, Val_T>::iterator it;
	for (it = this->begin(); it != this->end(); it++) {
		temp.add(it->first, it->second);
	}
	*this = temp;
}

template<typename Key_T, typename Val_T>
int HashMap<Key_T, Val_T>::fill() const {
	return this->num_elems;
}

template<typename Key_T, typename Val_T>
typename HashMap<Key_T, Val_T>::iterator HashMap<Key_T, Val_T>::begin() {
	typename std::list<std::pair<Key_T, Val_T>>::iterator it;
	int flag = 0;
	int temp_ind;
	for (int i = 0; i < this->size_arr; i++) {
		temp_ind = i;
		if (this->hash_arr[i].begin() != this->hash_arr[i].end()) {
			flag = 1;
			it = this->hash_arr[i].begin();
		}
	}
	if (!flag) {
		it= this->hash_arr[this->size_arr-1].end();
	}
	iterator iter(this, temp_ind, it);
	return iter;
}

template<typename Key_T, typename Val_T>
typename HashMap<Key_T, Val_T>::iterator HashMap<Key_T, Val_T>::end() {
	iterator iter(this, this->size_arr-1, this->hash_arr[this->size_arr - 1].end());
	return iter;
}

template<typename Key_T, typename Val_T>
HashMap<Key_T, Val_T>::iterator::iterator(HashMap* mp, int ind, typename std::list<std::pair<Key_T, Val_T>>::iterator i) :map(mp), index(ind), it(i) {}

template<typename Key_T, typename Val_T>
typename HashMap<Key_T, Val_T>::iterator& HashMap<Key_T, Val_T>::iterator::operator=(const iterator& that) {
	this->map = that.map;
	this->index = that.index;
	this->it = that.it;
	return *this;
}

template<typename Key_T, typename Val_T>
bool HashMap<Key_T, Val_T>::iterator::operator== (const iterator& that) const {
	return (this->map == that.map) && (this->index==that.index) && (this->it==that.it);
}

template<typename Key_T, typename Val_T>
bool HashMap<Key_T, Val_T>::iterator::operator!= (const iterator& that) const {
	std::cout << (this->map != that.map) << std::endl;
	std::cout << (this->index != that.index) << std::endl;
	std::cout << (this->it != that.it) << std::endl;
	return (this->map != that.map) || (this->index != that.index) || (this->it != that.it);
}

template<typename Key_T, typename Val_T>
std::pair<Key_T, Val_T>& HashMap<Key_T, Val_T>::iterator::operator*() const {
	return *(this->it);
}

template<typename Key_T, typename Val_T>
std::pair<Key_T, Val_T>* HashMap<Key_T, Val_T>::iterator::operator->() const {
	return &(*(this->it));
}

template<typename Key_T, typename Val_T>
typename HashMap<Key_T, Val_T>::iterator& HashMap<Key_T, Val_T>::iterator::operator++() {
	if (this->index != map->size_arr - 1 || this->it != this->map->hash_arr[map->size_arr - 1].end()) {
		++this->it;
		if (this->it != this->map->hash_arr[this->index].end()) {
			return *this;
		}
		else {
			while (this->it == this->map->hash_arr[this->index].end() && this->map->size_arr-1 > this->index) {
				++this->index;
				this->it = this->map->hash_arr[this->index].begin();
			}
			return *this;
		}
	}
}

template<typename Key_T, typename Val_T>
typename HashMap<Key_T, Val_T>::iterator HashMap<Key_T, Val_T>::iterator::operator++(int) {
	iterator saved = *this;
	//std::cout << map->size_arr - 1 << std::endl;
	//typename std::list<std::pair<Key_T, Val_T>>::iterator it1;
	//it1=this->map->hash_arr[map->size_arr - 1].end();
	//std::cout << this->it== << std::endl;
	if (this->index != map->size_arr - 1 || this->it != this->map->hash_arr[map->size_arr - 1].end()) {
		++this->it;
		if (this->it != this->map->hash_arr[this->index].end()) {
			std::cout << "111" << std::endl;
			return *this;
		}
		else {
			std::cout << "222" << std::endl;
			while (this->it == this->map->hash_arr[this->index].end() && this->map->size_arr - 1 > this->index) {
				++this->index;
				this->it = this->map->hash_arr[this->index].begin();
			}
			return *this;
		}
	}
	return saved;
}


template<typename Key_T, typename Val_T>
void start(int N) {
	HashMap<Key_T, Val_T> map;
	Key_T key;
	Val_T val;
	char act;
	while (N) {
		--N;
		std::cin >> act;
		switch (act) {
		case 'A': {
			std::cin >> key;
			std::cin >> val;
			map.add(key, val);
			break;
		}
		case 'R': {
			std::cin >> key;
			map.remove(key);
			break;
		}
		}
	}
	std::cout << map.fill() << "\n";
	typename HashMap<Key_T, Val_T>::iterator it;
	for (it = map.begin(); it != map.end(); it++) {
		std::cout << it->first << " " << it->second<< std::endl;
	}
}


int main()
{
	start<int, std::string>(5);
	/*char Frst, Scnd;
	std::cin >> Frst >> Scnd;
	int N;
	std::cin >> N;
	switch (Frst) {
	case 'I': {
		switch (Scnd) {
		case 'I': {
			start<I, I>(N);
			break;
		}
		case 'D': {
			start<I, D>(N);
			break;
		}
		case 'S': {
			start<I, S>(N);
			break;
		}
		}
		break;
	}
	case 'D': {
		switch (Scnd) {
		case 'I': {
			start<D, I>(N);
			break;
		}
		case 'D': {
			start<D, D>(N);
			break;
		}
		case 'S': {
			start<D, S>(N);
			break;
		}
		}
		break;
	}
	case 'S': {
		switch (Scnd) {
		case 'I': {
			start<S, I>(N);
			break;
		}
		case 'D': {
			start<S, D>(N);
			break;
		}
		case 'S': {
			start<S, S>(N);
			break;
		}
		}
		break;
	}
	}*/
}
