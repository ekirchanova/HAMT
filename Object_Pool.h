#pragma once
#include <new>
#include <vector>
template< class T>
class Object_Pool final {
	std::vector<bool> used;
	T* pool;
	size_t pool_size;
	size_t current_count_elements;
public:
	Object_Pool()=delete;
	Object_Pool(size_t maximum_amount);
	Object_Pool(const Object_Pool& cur);
	Object_Pool(Object_Pool&& cur);;
	~Object_Pool();

	Object_Pool& operator=(Object_Pool&& cur)noexcept;
	Object_Pool& operator=(const Object_Pool& cur) noexcept;
	
	void clear();
	T& operator[] (size_t number);
	const T& operator[] (size_t number) const;
	template< class... Arguments>
	decltype(auto)  alloc(size_t i,Arguments&& ... args);

	void free(T* deleted_object);
	void free(size_t number);

	size_t cur_count_elements() const noexcept;
	bool check_using_cell(size_t number) const noexcept;
	bool check_no_exist_children() const noexcept;
};

template< class T>
Object_Pool<T>::Object_Pool(size_t maximum_amount) {
	pool = static_cast<T*>(operator new[](maximum_amount * sizeof(T)));
	pool_size = maximum_amount;
	current_count_elements = 0;
	used.resize(maximum_amount, false);

}

template< class T>
Object_Pool<T>::Object_Pool(const Object_Pool& cur) {
	pool_size = cur.pool_size;
	pool = static_cast<T*>(operator new[](pool_size * sizeof(T)));
	current_count_elements = 0;
	used.resize(pool_size, false);
	if (cur.check_no_exist_children()) return;
	for (size_t i = 0; i < pool_size; ++i) {
		if (cur.used[i]) {
			used[i] = true;
			current_count_elements++;
			T* new_obj = new(pool + i)T(*(cur.pool + i));
		}
	}
}

template< class T>
Object_Pool<T>::Object_Pool(Object_Pool&& cur) :pool{ cur.pool }, pool_size{ cur.pool_size }, used(std::move(cur.used)), current_count_elements{ cur.current_count_elements }
{
	cur.pool = nullptr;
	cur.current_count_elements = 0;
	cur.pool_size = 0;
}

template< class T>
Object_Pool<T>::~Object_Pool() {
	clear();
	operator delete (pool);
}
template< class T>
Object_Pool<T>& Object_Pool<T>::operator=(Object_Pool&& cur) noexcept {
	if (pool) {
		clear();
		operator delete (pool);
	}
	std::swap(used, cur.used);
	pool = cur.pool;
	pool_size = cur.pool_size;
	current_count_elements = cur.current_count_elements;
	cur.pool = nullptr;
	cur.current_count_elements = 0;
	cur.pool_size = 0;
	return *this;
}

template< class T>
Object_Pool<T>& Object_Pool<T>::operator=(const Object_Pool& cur) noexcept {
	if (pool) {
		clear();
		operator delete (pool);
	}
	pool_size = cur.pool_size;
	pool = static_cast<T*>(operator new[](pool_size * sizeof(T)));
	for (size_t i = 0; i < pool_size; ++i) {
		if (cur.check_no_exist_children()) break;
		if (cur.used[i]) {
			used[i] = true;
			current_count_elements++;
			T* new_obj = new(pool + i)T(*(cur.pool + i));
		}
	}
	return *this;
}

template< class T>
void Object_Pool<T>::clear() {
	if (check_no_exist_children()) return;
	for (size_t i = 0; i < pool_size; i++) {
		free(pool + i);
	}
}

template< class T>
T& Object_Pool<T>::operator[] (size_t number) {
	return *(pool + number);
}

template< class T>
const T& Object_Pool<T>::operator[] (size_t number) const {
	return *(pool + number);
}

template< class T>
template< class... Arguments>
decltype(auto)  Object_Pool<T>::alloc(size_t i, Arguments && ... args) {
	if (i > pool_size) throw std::length_error("can't add new element");
	if (!used[i]) {
		used[i] = true;
		current_count_elements++;
		T* new_obj = new(pool + i)T(std::forward<Arguments>(args)...);
		return *new_obj;
	}
}

template< class T>
void Object_Pool<T>::free(T* deleted_object) {
	size_t number = deleted_object - pool;
	if (number < pool_size && used[number])
	{
		used[number] = false;
		current_count_elements--;
		(pool + number)->~T();
	}
}

template< class T>
void Object_Pool<T>::free(size_t number) {
	if (number < pool_size && used[number])
	{
		used[number] = false;
		current_count_elements--;
		(pool + number)->~T();
	}
}
template< class T>
size_t Object_Pool<T>::cur_count_elements() const noexcept {
	return current_count_elements;
}
template< class T>
bool Object_Pool<T>::check_using_cell(size_t number) const noexcept {

	return number < pool_size ? used[number] : false;
}
template< class T>
bool Object_Pool<T>::check_no_exist_children() const noexcept {
	std::vector<bool> exist_children(pool_size, false);
	return used == exist_children;
}