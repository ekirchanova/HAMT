#pragma once
#include"Object_Pool.h"
#include <algorithm>
#include <iterator>
#include <functional>
#include <type_traits>
#include <string>
#include <bitset>
#include <vector>
#include <array>
template<class T>
class Trie;
template<class T>
class SubTrie;
namespace {
	const size_t SIZE_HASH = 32;
	const size_t COUNT_CHILDREN = 16;
	const size_t SIZE_BLOCK = 4;
	enum class status_erase
	{
		needed_erase ,
		already_erase,
		fail_erase

	};
	std::bitset<SIZE_HASH> binary_conversion(size_t hash) {
		std::array<bool, SIZE_HASH> mas;
		mas.fill(0);
		size_t l = 0;
		do {
			mas[l++] = hash % 2;
			hash /= 2;
		} while (hash != 0);
		std::bitset<SIZE_HASH> cur_hash;
		cur_hash.set(false);
		for (size_t i = 0, m = SIZE_HASH - 1; i<SIZE_HASH; --m, ++i) {
			if (cur_hash[i] != mas[m]) {
				cur_hash[i].flip();
			}
		}
		return cur_hash;
	}
	std::bitset<SIZE_HASH> string_hash(const std::string& cur_key) {
		std::hash<std::string> hash_fn;
		size_t str_hash = hash_fn(cur_key);
		return binary_conversion(str_hash);
	}
	std::bitset<SIZE_BLOCK> take_block(size_t level, const std::bitset<SIZE_HASH> &hash) {
		std::bitset<SIZE_BLOCK> cur_block;
		for (size_t i = (level) * SIZE_BLOCK , l = 0; l < SIZE_BLOCK; ++i, ++l) {
			cur_block[l] = hash[i];
		}
		return cur_block;
	}
	size_t power(size_t _power) {
		if (!_power)return 1;
		return 2 * power(_power - 1);
	}
	size_t convert_from_binary_system(const std::bitset<SIZE_BLOCK>& block) {
		size_t number=0;
		for (size_t i = 0, l = SIZE_BLOCK - 1; i < SIZE_BLOCK; ++i,--l) {
			number = number+block[i] * power(l);
		}
		return number;

	}
	size_t calculate_number_child(size_t level, const std::bitset<SIZE_HASH>& hash) {
		std::bitset<SIZE_BLOCK> block = take_block(level, hash);
		return convert_from_binary_system(block);
	}
	template<class T>
	size_t find_first_child(const Object_Pool<SubTrie<T>>& children) {
		for (size_t i = 0; i < COUNT_CHILDREN; i++) {
			if (children.check_using_cell(i))
				return i;
		}
		return -1;
	}
}; 

template <class T>
class SubTrie final {
public:
	typedef T value_type;
	typedef std::string key_type;
private:
	SubTrie& find_parent(SubTrie<T>& parent,size_t cur_level,size_t level, const std::bitset<SIZE_HASH>& hash)noexcept;
	std::pair<const key_type, value_type>* find_near_node(size_t number)  noexcept;
	std::pair<const key_type, value_type>* find_near_in_trie( Trie<T>& parent, size_t number)noexcept;

	const SubTrie& find_parent(const SubTrie<T>& parent, size_t cur_level, size_t level, const std::bitset<SIZE_HASH>& hash) const noexcept;
	const std::pair<const key_type, value_type>* find_near_node(size_t number) const  noexcept;
	const std::pair<const key_type, value_type>* find_near_in_trie(const Trie<T>& parent, size_t number)const noexcept;

	void replacment_child_with_grandchild(size_t cur_child);
private:
	Object_Pool<SubTrie<T>> children;
	size_t level;
	std::pair<const key_type, value_type> current;
	std::bitset<SIZE_HASH> hash;
	
public:
	SubTrie();
	SubTrie(size_t cur_level, const std::pair< const key_type, value_type>&cur, const std::bitset<SIZE_HASH>& cur_hash) ;
	
	std::pair <std::pair<const key_type, value_type>*, bool> insert(size_t level,const std::pair< const key_type, value_type> &cur, const std::bitset<SIZE_HASH> &hash);
	
	status_erase erase(const key_type& k, const std::bitset<SIZE_HASH> &hash);

	std::pair<const key_type, value_type>& value() noexcept;
	std::pair< const key_type, value_type>* pointer_value();

	const std::pair<const key_type, value_type>& value() const noexcept;
	const std::pair< const key_type, value_type>* pointer_value() const ;

	std::pair< const key_type, value_type>* begin() ;
	const std::pair< const key_type, value_type>* const_begin() const;
	std::pair< const key_type, value_type>* next(Trie<T>& parent)noexcept;
	const std::pair< const key_type, value_type>* next(const Trie<T>& parent)const noexcept;
	std::pair< const key_type, value_type>* find(const key_type& k, const std::bitset<SIZE_HASH>& hash);
	const std::pair< const key_type, value_type>* const_find(const key_type& k, const std::bitset<SIZE_HASH>& hash)const;
	value_type& find_or_insert(const key_type& k, const std::bitset<SIZE_HASH>& hash);

	SubTrie<T> GetSubTrie(const key_type& subKey,const  std::bitset<SIZE_HASH> &hash)const ;
	

};
template <class T> 
class Trie final
{
	size_t size_trie;
	public:
	Object_Pool<SubTrie<T>> children;
	public:
		template <bool Const=false>
		class TrieIterator final
		{
			std::conditional_t < Const, const Trie<T>&, Trie<T>&> parent;
			std::conditional_t < Const, const std::pair < const std::string, T>*, std::pair < const std::string, T>*> current;
		public:
			using iterator_category = std::forward_iterator_tag;
			using value_type = std::pair<const std::string, T>;
			using pointer = std::conditional_t < Const,const std::pair <const std::string, T> *, std::pair <const std::string, T>*>;
			using reference = std::conditional_t < Const, const std::remove_reference_t<std::pair <const std::string, T>>&, std::remove_reference_t<std::pair <const std::string, T>>&>;
			using const_reference = const std::remove_reference_t<std::pair <const std::string, T>>&;
		public:
			TrieIterator(std::conditional_t < Const,const std::pair < const std::string, T>*, std::pair < const std::string, T>*>cur, std::conditional_t < Const, const Trie<T>&, Trie<T>&> trie) :current(cur), parent(trie) {};
			TrieIterator(const TrieIterator& rhs) = default;
			TrieIterator(TrieIterator&& rhs) = default;
			~TrieIterator() = default;

			TrieIterator& operator=(const TrieIterator& rhs);

			TrieIterator& operator++();
			TrieIterator operator++(int);

			bool operator==(const TrieIterator& rhs)const;
			bool operator!=(const TrieIterator& rhs)const;
			
			const_reference operator*() const;
			reference operator*();
			pointer operator->();
			pointer operator->()const ;
		};
		using  iterator=TrieIterator<false>;
		using  const_iterator = TrieIterator<true>;
public:
		typedef T value_type;
		typedef std::string key_type;


		Trie() :size_trie(0), children(Object_Pool<SubTrie<T>>(COUNT_CHILDREN)) {};
		template <class InputIterator> Trie(InputIterator first, InputIterator last);
		
		iterator begin();
		const_iterator begin() const;

		iterator end();
		const_iterator end() const;
		
		const_iterator cbegin() const;
		const_iterator cend() const;

		bool empty() const;
		size_t size() const;

		void swap(Trie<T>& trie);
		
		std::pair<iterator, bool> insert(const key_type& k, const value_type& val);
		template <class InputIterator> void insert(InputIterator first, InputIterator last);
		

		value_type& operator[] (const key_type& k)noexcept;
		const value_type& operator[] (const key_type& k)const ;
		iterator find(const key_type& k);
		const_iterator find(const key_type& k) const;

		void erase(iterator position);
		void erase(const key_type& k);
		void erase(iterator first, iterator last);
		
		void clear();

		SubTrie<T> GetSubTrie(const key_type& subKey)const; 
	
};
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class T>
bool Trie<T>::empty() const {
	return size_trie == 0;
}
template <class T>
size_t Trie<T>::size() const {
	return size_trie;
}

template <class T>
std::pair<typename Trie<T>::iterator, bool>  Trie<T>::insert(const key_type& k, const value_type& val) {
	const std::bitset<SIZE_HASH> cur_hash = string_hash(k);
	const size_t cur_child = calculate_number_child(0, cur_hash);
	const std::pair<const key_type, value_type> cur = std::make_pair(k,val);
	if (!children.check_using_cell(cur_child)) {
		SubTrie<T>& new_sub_trie=children.alloc(cur_child, 0, std::move(cur), cur_hash);
		++size_trie;
		iterator my_itr(new_sub_trie.pointer_value(), *this);
		return std::make_pair(my_itr, true );
	}
	auto& [pointer,inserter]=children[cur_child].insert(0,std::move(cur), cur_hash);
	if (inserter) {
		++size_trie;
	}
	return std::make_pair( iterator(pointer, *this), true);
}

template <class T>
template <class InputIterator>
void Trie<T>::insert(InputIterator first, InputIterator last) {
	for (auto i = first; i != last; ++i) {
		insert((*i).first, (*i).second);
	}
}
template <class T>
template <class InputIterator>
Trie<T>::Trie(InputIterator first, InputIterator last):size_trie(0), children(Object_Pool<SubTrie<T>>(COUNT_CHILDREN)) {
	insert(first, last);
}

template <class T>
void Trie<T>::swap(Trie<T>& trie) {
	std::swap(*this, trie);
}
template <class T>
typename Trie<T>::value_type& Trie<T>:: operator[] (const key_type& k) noexcept{
	std::bitset<SIZE_HASH> cur_hash = string_hash(k);
	size_t cur_child = calculate_number_child(0, cur_hash);
	if (!children.check_using_cell(cur_child)) {
		std::pair<const key_type, value_type> cur = std::make_pair(k, value_type());
		SubTrie<T> subtrie=children.alloc(cur_child,0,cur, cur_hash);
		++size_trie;
		return subtrie.value().second ;
	}
	return children[cur_child].find_or_insert(k, cur_hash);
}
template <class T>
const typename Trie<T>::value_type& Trie<T>:: operator[] (const key_type& k)const {
	std::bitset<SIZE_HASH> cur_hash = string_hash(k);
	size_t cur_child = calculate_number_child(0, cur_hash);
	if (!children.check_using_cell(cur_child)) {
		throw std::invalid_argument("can't find element");
	}
	auto it=children[cur_child].const_find(k, cur_hash);
	if (!it) {
		throw std::invalid_argument("can't find element");
	}
	else {
		return (*it).second;
	}

}
template <class T>
typename Trie<T>::iterator Trie<T>::find(const key_type& k) {
	const std::bitset<SIZE_HASH> cur_hash = string_hash(k);
	const size_t cur_child = calculate_number_child(0, cur_hash);
	if (!children.check_using_cell(cur_child)) {
		return end();
	}
	auto it= children[cur_child].find(k, cur_hash);
	return iterator(it, *this);
}
template <class T>
typename Trie<T>::const_iterator Trie<T>::find(const key_type& k) const {
	const std::bitset<SIZE_HASH> cur_hash = string_hash(k);
	const size_t cur_child = calculate_number_child(0, cur_hash);
	if (!children.check_using_cell(cur_child)) {
		return cend();
	}
	return const_iterator(children[cur_child].const_find(k, cur_hash),*this);
}

template <class T>
void Trie<T>::erase(iterator position) {
	if (position==end()) return;
	const key_type k = (*position).first;
	erase(k);
}
template <class T>
void Trie<T>::erase(const key_type& k) {
	if (!size_trie) return ;
	const std::bitset<SIZE_HASH> cur_hash = string_hash(k);
	const size_t cur_child = calculate_number_child(0, cur_hash);
	if (!children.check_using_cell(cur_child)){
		return ;
	}
	switch (children[cur_child].erase(k, cur_hash)) {
	case status_erase::needed_erase :
			children.free(cur_child);
			break;
	case status_erase::fail_erase:
			return;
	}
	--size_trie;
}
template <class T>
void Trie<T>::erase(iterator first, iterator last) {
	if (first == last)return; 
	iterator temp = first++;
	for (auto i =first; i!= last; i=temp++) {
		erase(i);
	}
}
template <class T>
void Trie<T>::clear() {
	children.clear();
	size_trie = 0;
}
template <class T>
SubTrie<T> Trie<T>::GetSubTrie(const key_type& subKey) const {
	const std::bitset<SIZE_HASH> cur_hash = string_hash(subKey);
	const size_t cur_child = calculate_number_child(0, cur_hash);
	if (!children.check_using_cell(cur_child)) {
		return SubTrie<T>();
	}
	return children[cur_child].GetSubTrie(subKey, cur_hash);
}

template <class T>
typename Trie<T>::iterator Trie<T>::begin() {
	const size_t number_first_child = find_first_child(children);
	if (number_first_child == -1) return iterator(nullptr, *this);
	return iterator(children[number_first_child].begin(), *this);
}
template <class T>
typename Trie<T>::const_iterator Trie<T>::begin() const {
	const size_t number_first_child = find_first_child(children);
	if (number_first_child == -1) return const_iterator(nullptr, *this);
	return const_iterator(children[number_first_child].const_begin(), *this);
}
template <class T>
typename Trie<T>::iterator Trie<T>::end() {
	return iterator(nullptr,*this);
}
template <class T>
typename Trie<T>::const_iterator Trie<T>::end() const {
	return const_iterator(nullptr, *this);
}
template <class T>
typename Trie<T>::const_iterator Trie<T>::cbegin() const {
	return begin();
}
template <class T>
typename Trie<T>::const_iterator Trie<T>::cend() const {
	return end();
}
////----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

template<class T>
template <bool Const>
typename Trie<T>::TrieIterator<Const>& Trie<T>::TrieIterator<Const>::operator++() {
	if (current == nullptr) return *this;
	auto& [key, value] = *current;
	auto&  cur = parent.GetSubTrie(key);
	current = cur.next(parent);
	return *this;
}
template<class T>
template <bool Const>
typename Trie<T>::TrieIterator<Const>& Trie<T>::TrieIterator<Const>::operator=(const TrieIterator& rhs) {
	current = rhs.current;
	return *this;
}
template <class T>
template <bool Const>
typename Trie<T>::TrieIterator<Const> Trie<T>::TrieIterator<Const>:: operator++(int) {
	TrieIterator tmp = *this;
	++(*this);
	return tmp;
}
template <class T>
template <bool Const>
bool Trie<T>::TrieIterator<Const>:: operator==(const TrieIterator& rhs) const
{
	if (!current || !rhs.current) return current == rhs.current;
	return *current == *rhs.current;
}
template <class T>
template <bool Const>
bool Trie<T>::TrieIterator<Const>::operator!=(const TrieIterator& rhs) const
{
	return !((*this) == rhs);
}
template <class T>
template <bool Const>
typename Trie<T>::TrieIterator<Const>::reference Trie<T>::TrieIterator<Const>::operator*() {
	return *current;
}
template <class T>
template <bool Const>
typename Trie<T>::TrieIterator<Const>::pointer Trie<T>::TrieIterator<Const>::operator->() {
	return current;
}
template <class T>
template <bool Const>
typename Trie<T>::TrieIterator<Const>::pointer Trie<T>::TrieIterator<Const>::operator->()const  {
	return current;
}
template <class T>
template <bool Const>
typename Trie<T>::TrieIterator<Const>::const_reference Trie<T>::TrieIterator<Const>::operator*() const  {
	return *current;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class T>
SubTrie<T>::SubTrie(size_t cur_level, const std::pair< const key_type, value_type>& cur, const std::bitset<SIZE_HASH>& cur_hash) :
	level(cur_level), current(cur), hash(cur_hash), children(Object_Pool<SubTrie<T>>(COUNT_CHILDREN)) {};

template <class T>
SubTrie<T>::SubTrie() :current(std::pair<const key_type, value_type>()), hash(hash.reset()), children(Object_Pool<SubTrie<T>>(COUNT_CHILDREN)), level(0) {};
template <class T>
std::pair< std::pair<const typename  SubTrie<T>::key_type, typename  SubTrie<T>::value_type>*,bool> SubTrie<T>::insert(size_t cur_level,const std::pair<const key_type,value_type> &cur, const std::bitset<SIZE_HASH> &cur_hash ){
	auto& [key, value] = current;
	auto&[cur_key, cur_value] = cur;
	if (children.check_no_exist_children() ) {
		if (key == cur_key)
		{
			value = cur_value;
			return std::make_pair(pointer_value(),false);
		}
		const size_t new_number = calculate_number_child(++cur_level, hash); 
		children.alloc(new_number, cur_level, current, hash);
		--cur_level;
		value = value_type();
		hash = hash.reset();
	}
	size_t cur_child = calculate_number_child(++cur_level, cur_hash);
	if (!children.check_using_cell(cur_child)) {
		auto& new_sub_trie = children.alloc(cur_child, cur_level,cur, cur_hash);
		return  std::make_pair(new_sub_trie.pointer_value(),true);
	}
	return children[cur_child].insert(cur_level, cur, cur_hash);
}

template <class T>
typename  SubTrie<T>::value_type&  SubTrie<T>::find_or_insert(const key_type& k, const std::bitset<SIZE_HASH>& hash) {
	auto &[key, value] = current;
	if (children.check_no_exist_children())  {
		if (key == k) {
			return value;
		}
	}
	const size_t cur_level = level + 1;
	const size_t cur_child = calculate_number_child(cur_level, hash);
	if (!children.check_using_cell(cur_child)) {
		std::pair<const key_type, value_type> cur = std::make_pair(k, value_type());
		auto [pointer, inserter] = insert(level, cur, hash);
		return (*pointer).second;
	}
	return children[cur_child].find_or_insert(k, hash);
}
template <class T>
std::pair<const typename  SubTrie<T>::key_type, typename  SubTrie<T>::value_type>* SubTrie<T>::find(const key_type& k, const std::bitset<SIZE_HASH>& hash) {
	auto& [key, value] = current;
	if (children.check_no_exist_children()) {
		if (key == k) {
			return &current;
		}
		else {
			return nullptr;
		}
	}
	const size_t cur_level = level + 1;
	const size_t cur_child = calculate_number_child(cur_level, hash);
	if (!children.check_using_cell(cur_child)) {
		return nullptr;
	}
	return children[cur_child].find(k, hash);
}
template <class T>
const std::pair<const typename  SubTrie<T>::key_type, typename  SubTrie<T>::value_type>* SubTrie<T>::const_find(const key_type& k, const std::bitset<SIZE_HASH>& hash) const {
	auto& [key, value] = current;
	if (children.check_no_exist_children()) {
		if (key == k) {
			return &current;
		}
		else {
			return nullptr;
		}
	}
	const size_t cur_level = level + 1;
	const size_t cur_child = calculate_number_child(cur_level, hash);
	if (!children.check_using_cell(cur_child)) {
		return nullptr;
	}
	return children[cur_child].const_find(k, hash);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class T>
void SubTrie<T>::replacment_child_with_grandchild(size_t cur_child) {
	const size_t cur_grandchild = find_first_child(children[cur_child].children);
	const SubTrie grandchild = children[cur_child].children[cur_grandchild];
	children.free(cur_child);
	children.alloc(cur_child, grandchild);
}
template <class T>
status_erase SubTrie<T>::erase(const key_type& k,const std::bitset<SIZE_HASH>&cur_hash) {
	auto& [key, value] = current;
	if(children.check_no_exist_children()){
		if (key == k) {
			value = value_type();
			hash = hash.reset();
			return status_erase::needed_erase;
		}
		else {
			return status_erase::fail_erase;
		}
	}
	const size_t cur_level = level+1;
	const size_t cur_child = calculate_number_child(cur_level, cur_hash);
	if (!children.check_using_cell(cur_child)) {
		return status_erase::fail_erase;
	}
	if (status_erase::needed_erase==children[cur_child].erase(k, cur_hash)) {
		children.free(cur_child);
	}else if (children[cur_child].children.cur_count_elements() == 1) {
		replacment_child_with_grandchild(cur_child);
	}
	return children.check_no_exist_children()?status_erase::needed_erase:status_erase::already_erase;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class T>
std::pair<const typename  SubTrie<T>::key_type, typename  SubTrie<T>::value_type>* SubTrie<T>::begin() {
	if (children.check_no_exist_children()) return &current;
	const size_t number_first_child = find_first_child(children);
	if (number_first_child == -1) {
		return nullptr;
	}
	return children[number_first_child].begin();
}
template <class T>
const std::pair<const typename  SubTrie<T>::key_type, typename  SubTrie<T>::value_type>* SubTrie<T>::const_begin()const  {
	if (children.check_no_exist_children()) return &current;
	const size_t number_first_child = find_first_child(children);
	if (number_first_child == -1) {
		return nullptr;
	}
	return children[number_first_child].const_begin();
}
template <class T>
SubTrie<T> SubTrie<T>::GetSubTrie(const key_type& subKey, const std::bitset<SIZE_HASH> &hash) const {
	auto &[key, value] = current;
	if (children.check_no_exist_children()) {
		if (key == subKey) {
			return *this;
		}
		else
			return SubTrie();
	}
	const size_t cur_level = level + 1;
	const size_t cur_child = calculate_number_child(cur_level, hash);
	return children[cur_child].GetSubTrie(subKey, hash);
}
template <class T>
std::pair< const typename  SubTrie<T>::key_type, typename  SubTrie<T>::value_type>& SubTrie<T>::value() noexcept {
	return current;
}
template <class T>
std::pair<const typename SubTrie<T>::key_type, typename SubTrie<T>::value_type>* SubTrie<T>::pointer_value()
{
	return &current;
}
template <class T>
const std::pair< const typename  SubTrie<T>::key_type, typename  SubTrie<T>::value_type>& SubTrie<T>::value() const noexcept{
	return current;
}
template <class T>
const std::pair<const typename SubTrie<T>::key_type, typename SubTrie<T>::value_type>* SubTrie<T>::pointer_value() const
{
	return &current;
}

////--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class T>
std::pair<const typename SubTrie<T>::key_type, typename SubTrie<T>::value_type>* SubTrie<T>::find_near_node(size_t number) noexcept{
	if (children.check_no_exist_children()) return pointer_value();
	for (size_t i = ++number; i < COUNT_CHILDREN; ++i)
	{
		if (children.check_using_cell(i)) {
			return children[i].find_near_node(-1);
		}
	}
	return nullptr;
}
template <class T>
SubTrie<T>&  SubTrie<T>::find_parent(SubTrie<T>& parent, size_t cur_level,size_t needed_level,const std::bitset<SIZE_HASH>& cur_hash)noexcept {
	if (needed_level == cur_level) return parent;
	const size_t  cur_child = calculate_number_child(++cur_level, cur_hash);
	return  parent.children[cur_child].find_parent(parent.children[cur_child],cur_level,needed_level,cur_hash);
}

template<class T>
std::pair<const typename SubTrie<T>::key_type, typename SubTrie<T>::value_type>* SubTrie<T>::find_near_in_trie(Trie<T>& parent,size_t number) noexcept{
	if(parent.children.check_no_exist_children()) return nullptr;
	std::pair <const std::string, T>* near = nullptr;
	for (size_t i = ++number; i != COUNT_CHILDREN; ++i) {
		if (parent.children.check_using_cell(i))
		{
			near= parent.children[i].find_near_node(-1);
			if (near) {
				return near;
			}
		}
	}
	return near;
}

template <class T>
std::pair<const typename SubTrie<T>::key_type, typename SubTrie<T>::value_type>* SubTrie<T>::next(Trie<T>&parent)noexcept{
	const std::bitset<SIZE_HASH> cur_hash = string_hash(current.first);
	const size_t first_child = calculate_number_child(0,hash);
	if (!parent.children.check_using_cell(first_child))return  nullptr;
	size_t c_level=level;
	while (c_level != 0) {
		SubTrie& s_parent =find_parent(parent.children[first_child],0,--c_level, cur_hash);
		const size_t cur_child = calculate_number_child(++c_level, cur_hash);
		std::pair <const std::string, T>*  near =s_parent.find_near_node(cur_child);
		if (near) {
			return near;
		}
		--c_level;
	}
	if (c_level == 0) {
		const size_t cur_child= calculate_number_child(0, cur_hash);
		std::pair <const std::string, T>* near = find_near_in_trie(parent,cur_child);
		return near;
	}
	return nullptr;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class T>
const std::pair<const typename SubTrie<T>::key_type, typename SubTrie<T>::value_type>* SubTrie<T>::find_near_node(size_t number) const noexcept {
	if (children.check_no_exist_children()) return pointer_value();
	for (size_t i = ++number; i < COUNT_CHILDREN; ++i)
	{
		if (children.check_using_cell(i)) {
			return children[i].find_near_node(-1);
		}
	}
	return nullptr;
}
template <class T>
const SubTrie<T>& SubTrie<T>::find_parent(const SubTrie<T>& parent, size_t cur_level, size_t needed_level, const std::bitset<SIZE_HASH>& cur_hash)const noexcept {
	if (needed_level == cur_level) return parent;
	const size_t  cur_child = calculate_number_child(++cur_level, cur_hash);
	return  parent.children[cur_child].find_parent(parent.children[cur_child], cur_level, needed_level, cur_hash);
}

template<class T>
const std::pair<const typename SubTrie<T>::key_type, typename SubTrie<T>::value_type>* SubTrie<T>::find_near_in_trie(const Trie<T>& parent, size_t number) const noexcept {
	if (parent.children.check_no_exist_children()) return nullptr;
	for (size_t i = ++number; i != COUNT_CHILDREN; ++i) {
		if (parent.children.check_using_cell(i))
		{
			const std::pair <const std::string, T>* near = parent.children[i].find_near_node(-1);
			if (near) {
				return near;
			}
		}

	}
	return nullptr;
}
template <class T>
const std::pair<const typename SubTrie<T>::key_type, typename SubTrie<T>::value_type>* SubTrie<T>::next(const Trie<T>& parent) const noexcept {
	const std::bitset<SIZE_HASH> cur_hash = string_hash(current.first);
	const size_t first_child = calculate_number_child(0, hash);
	if (!parent.children.check_using_cell(first_child))return  nullptr;
	size_t c_level = level;
	while (c_level != 0) {
		const SubTrie& s_parent = find_parent(parent.children[first_child], 0, --c_level, cur_hash);
		const size_t cur_child = calculate_number_child(++c_level, cur_hash);
		const std::pair <const std::string, T>* near = s_parent.find_near_node(cur_child);
		if (near) {
			return near;
		}
		--c_level;
	}
	if (c_level == 0) {
		const size_t cur_child = calculate_number_child(0, cur_hash);
		const std::pair <const std::string, T>* near = find_near_in_trie(parent, cur_child);
		return near;
	}
	return nullptr;
}

