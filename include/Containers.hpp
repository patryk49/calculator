#pragma once

#include "Utils.hpp"
#include <initializer_list>
#include <type_traits>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"

namespace sp{ // BEGINING OF NAMESPACE //////////////////////////////////////////////////////////////////


template<class T>
struct View{
	constexpr void push_back(const T &arg) noexcept{ *endPtr = arg; ++endPtr; }

	constexpr void push_back(T &&arg) noexcept{ *endPtr = std::move(arg); ++endPtr; }

	template<class... Args>
	constexpr void emplace_back(Args &&...args) noexcept{ endPtr->~T(); new(endPtr) T(std::forward<Args>(args) ...); ++endPtr; }

	constexpr void pop_back() noexcept{ --endPtr; }

	constexpr void expandBy(const size_t count) noexcept{ endPtr += count; }
	constexpr void skrinkBy(size_t count) noexcept{ endPtr -= count; }

	constexpr T &operator [](const size_t index) noexcept{ return beginPtr[index]; }

	[[nodiscard]] constexpr const T &operator [](const size_t index) const noexcept{ return beginPtr[index]; }
	
	constexpr T *begin() noexcept{ return beginPtr; }
	constexpr T *end() noexcept{ return endPtr; }
	[[nodiscard]] constexpr const T *begin() const noexcept{ return beginPtr; }
	[[nodiscard]] constexpr const T *end() const noexcept{ return endPtr; }
	[[nodiscard]] constexpr const T *cbegin() const noexcept{ return beginPtr; }
	[[nodiscard]] constexpr const T *cend() const noexcept{ return endPtr; }

	[[nodiscard]] constexpr size_t size() const noexcept{ return endPtr - beginPtr; }
	constexpr void resize(const size_t size) noexcept{ endPtr = beginPtr + size; }

	constexpr T &front() noexcept{ return *beginPtr; }
	constexpr T &back() noexcept{ return *(endPtr-1); }
	[[nodiscard]] constexpr const T &front() const noexcept{ return *beginPtr; }
	[[nodiscard]] constexpr const T &back() const noexcept{ return *(endPtr-1); }

	typedef T value_type;

	T *beginPtr, *endPtr;
};


template<class T, size_t cap>
struct FiniteArray{
	static_assert(cap, "this makes no sense");

	constexpr FiniteArray() noexcept : endPtr{data} {}
	constexpr FiniteArray(const size_t size) noexcept : endPtr{data + size} { for (T *I=data; I!=endPtr; ++I) new(I) T(); }
	~FiniteArray() noexcept{ for (T *I=data; I!=endPtr; ++I) I->~T(); }

	template<size_t cap1>
	constexpr FiniteArray(const FiniteArray<T, cap1> &rhs) noexcept : endPtr{data + std::size(rhs)} {
		for (T *I=data, *J=rhs.data; I!=endPtr; ++I, ++J) new(I) T(*J);
	}
	template<size_t cap1>
	constexpr FiniteArray(FiniteArray<T, cap1> &&rhs) noexcept : endPtr{data + std::size(rhs)} {
		for (T *I=data, *J=rhs.data; I!=endPtr; ++I, ++J) new(I) T(std::move(*J));
	}

	constexpr FiniteArray(const std::initializer_list<T> &rhs) noexcept : endPtr{data + std::size(rhs)} {
		for (T *I=data, *J=&*std::begin(rhs); I!=endPtr; ++I, ++J) new(I) T(*J);
	}

	template<class It>
	constexpr FiniteArray(It first, const It last) noexcept : endPtr{data + (last-first)} {
		for (T *I=data; I!=endPtr; ++I, first) new(I) T(*first);
	}

	template<size_t cap1>
	constexpr FiniteArray &operator =(const FiniteArray<T, cap1> &rhs) noexcept{
		this->resize(std::size(rhs));
		std::copy(std::begin(rhs), std::end(rhs), data);
		return *this;
	}
	template<size_t cap1>
	constexpr FiniteArray &operator =(FiniteArray<T, cap1> &&rhs) noexcept{
		this->resize(std::size(rhs));
		std::move(std::begin(rhs), std::end(rhs), data);
		return *this;
	}

	constexpr void push_back(const T &arg) noexcept{ new(endPtr) T(arg); ++endPtr; }

	constexpr void push_back(T &&arg) noexcept{ new(endPtr) T(std::move(arg)); ++endPtr; }

	template<class... Args>
	constexpr void emplace_back(Args &&...args) noexcept{ new(endPtr) T(std::forward<Args>(args) ...); ++endPtr; }

	constexpr void pop_back() noexcept{ (--endPtr)->~T(); }

	constexpr void expandBy(const size_t count) noexcept{
		T *I = endPtr; endPtr += count;
		for (; I!=endPtr; ++I) new(I) T();
	}
	constexpr void skrinkBy(size_t count) noexcept{ for (; count; --count) (--endPtr)->~T(); }

	
	constexpr T &operator [](const size_t index) noexcept{ return data[index]; }

	[[nodiscard]] constexpr const T &operator [](const size_t index) const noexcept{ return data[index]; }

	constexpr T *begin() noexcept{ return data; }
	constexpr T *end() noexcept{ return endPtr; }
	[[nodiscard]] constexpr const T *begin() const noexcept{ return data; }
	[[nodiscard]] constexpr const T *end() const noexcept{ return endPtr; }
	[[nodiscard]] constexpr const T *cbegin() const noexcept{ return data; }
	[[nodiscard]] constexpr const T *cend() const noexcept{ return endPtr; }

	constexpr void resize(const size_t size) noexcept{
		T *const newEndPtr = data + size;
		if (newEndPtr > endPtr)
			for (T *I=endPtr; I!=newEndPtr; ++I) new(I) T();
		else
			for (T *I=newEndPtr; I!=endPtr; ++I) I->~T();

		endPtr = newEndPtr;
	}

	[[nodiscard]] constexpr size_t size() const noexcept{ return endPtr - data; }

	[[nodiscard]] constexpr size_t capacity() const noexcept{ return cap; }


	[[nodiscard]] constexpr bool isFull() const noexcept{ return size() >= cap; }

	constexpr T &front() noexcept{ return *data; }
	constexpr T &back() noexcept{ return *(endPtr-1); }
	[[nodiscard]] constexpr const T &front() const noexcept{ return *data; }
	[[nodiscard]] constexpr const T &back() const noexcept{ return *(endPtr-1); }

	typedef T value_type;

private:
	union{
		T data[cap];
	};
	T *endPtr;
};



template<class T>
struct DArray{
	constexpr DArray() noexcept : beginPtr{nullptr}, endPtr{nullptr}, bufferEndPtr{nullptr} {}
	constexpr DArray(const size_t size) noexcept :
		beginPtr{(T *)aligned_alloc(alignof(T), size*sizeof(T))}, endPtr{beginPtr+size}, bufferEndPtr{endPtr}
	{
		for (T *I=beginPtr; I!=endPtr; ++I) new(I) T();
	}
	~DArray() noexcept{
		if constexpr (!std::is_trivially_destructible_v<T>)
			for (T *I=beginPtr; I!=endPtr; ++I) I->~T();
		if (beginPtr) free(beginPtr);
	}

	constexpr DArray(const DArray<T> &rhs) noexcept :
		beginPtr{(T *)aligned_alloc(alignof(T), std::size(rhs)*sizeof(T))}, endPtr{beginPtr+std::size(rhs)}, bufferEndPtr{endPtr}
	{
		for (T *I=beginPtr, *J=rhs.beginPtr; I!=endPtr; ++I, ++J) new(I) T(*J);
	}
	constexpr DArray(DArray<T> &&rhs) noexcept : beginPtr{rhs.beginPtr}, endPtr{rhs.endPtr}, bufferEndPtr{rhs.bufferEndPtr} {
		rhs.beginPtr = rhs.endPtr = rhs.bufferEndPtr = 0;
	}

	constexpr DArray(const std::initializer_list<T> &rhs) noexcept :
		beginPtr{(T *)aligned_alloc(alignof(T), std::size(rhs)*sizeof(T))},
		endPtr{beginPtr+std::size(rhs)}, bufferEndPtr{endPtr}
	{
		const T* I = &*std::begin(rhs); 
		for (T *J=beginPtr; J!=endPtr; ++I, ++J) new(J) T(*I);
	}

	template<class It>
	constexpr DArray(It first, const It last) noexcept :
		beginPtr{(T *)aligned_alloc(alignof(T), (char *)last-(char *)first)},
		endPtr{beginPtr+((char *)last-(char *)first)}, bufferEndPtr{endPtr}
	{
		for (T *I=beginPtr; I!=endPtr; ++I, ++first) new(I) T(*first);
	}

	constexpr DArray &operator =(const DArray<T> &rhs) noexcept{
		this->resize(std::size(rhs));
		std::copy(std::begin(rhs), std::end(rhs), beginPtr);
		return *this;
	}
	constexpr DArray &operator =(DArray<T> &&rhs) noexcept{
		this->~DArray();
		this->beginPtr = rhs.beginPtr;
		this->endPtr = rhs.endPtr;
		this->bufferEndPtr = rhs.bufferEndPtr;
		rhs.beginPtr = rhs.endPtr = rhs.bufferEndPtr = 0;
		return *this;
	}

	constexpr void push_back(const T &arg) noexcept{
		if (endPtr == bufferEndPtr){
			const size_t newSize = 2*std::size(*this) + 4*!std::size(*this);
			T *const newBuffer = (T *)aligned_alloc(alignof(T), newSize*sizeof(T));
			for (T *J=beginPtr, *I=newBuffer; J!=endPtr; ++J, ++I) new(I) T(std::move(*J));
			if (beginPtr) free(beginPtr);
			bufferEndPtr = newBuffer + newSize;
			endPtr += newBuffer - beginPtr;
			beginPtr = newBuffer;
		}
		new(endPtr) T(arg); ++endPtr;
	}
	constexpr void push_back(T &&arg) noexcept{
		if (endPtr == bufferEndPtr){
			const size_t newSize = 2*std::size(*this) + 4*!std::size(*this);
			T *const newBuffer = (T *)aligned_alloc(alignof(T), newSize*sizeof(T));
			for (T *J=beginPtr, *I=newBuffer; J!=endPtr; ++J, ++I) new(I) T(std::move(*J));
			if (beginPtr) free(beginPtr);
			bufferEndPtr = newBuffer + newSize;
			endPtr += newBuffer - beginPtr;
			beginPtr = newBuffer;
		}
		new(endPtr) T(std::move(arg)); ++endPtr;
	}
	template<class... Args>
	constexpr void emplace_back(Args &&...args) noexcept{
		if (endPtr == bufferEndPtr){
			const size_t newSize = 2*std::size(*this) + 4*!std::size(*this);
			T *const newBuffer = (T *)aligned_alloc(alignof(T), newSize*sizeof(T));
			for (T *J=beginPtr, *I=newBuffer; J!=endPtr; ++J, ++I)
				new(I) T(std::move(*J));
			if (beginPtr) free(beginPtr);
			bufferEndPtr = newBuffer + newSize;
			endPtr += newBuffer - beginPtr;
			beginPtr = newBuffer;
		}
		new(endPtr) T(std::forward<Args>(args) ...); ++endPtr;
	}
	constexpr void pop_back() noexcept{ (--endPtr)->~T(); }
	constexpr void expandBy(size_t count) noexcept{
		if (endPtr+count > bufferEndPtr){
			const size_t newSize = std::size(*this) + count;
			T *const newBuffer = (T *)aligned_alloc(alignof(T), newSize*sizeof(T));
			for (T *J=beginPtr, *I=newBuffer; J!=endPtr; ++J, ++I)
				new(I) T(std::move(*J));
			if (beginPtr) free(beginPtr);
			bufferEndPtr = newBuffer + newSize;
			endPtr = bufferEndPtr;
			beginPtr = newBuffer;
		} else endPtr += count;
		for (T *I=endPtr-count-1; I<endPtr; ++I) new(I) T();
	}
	constexpr void shrinkBy(size_t count) noexcept{ for (; count; --count) (--endPtr)->~T(); }
	
	constexpr T &operator [](const size_t index) noexcept{ return beginPtr[index]; }

	[[nodiscard]] constexpr const T &operator [](const size_t index) const noexcept{ return beginPtr[index]; }

	constexpr T *begin() noexcept{ return beginPtr; }
	constexpr T *end() noexcept{ return endPtr; }
	[[nodiscard]] constexpr const T *begin() const noexcept{ return beginPtr; }
	[[nodiscard]] constexpr const T *end() const noexcept{ return endPtr; }
	[[nodiscard]] constexpr const T *cbegin() const noexcept{ return beginPtr; }
	[[nodiscard]] constexpr const T *cend() const noexcept{ return endPtr; }

	constexpr void resize(const size_t size) noexcept{
		if (size > std::size(*this)){
			T *const newBuffer = (T *)aligned_alloc(alignof(T), size*sizeof(T));
			T *I = newBuffer;
			for (T *J=beginPtr; J!=endPtr; ++J, ++I) new(I) T(std::move(*J));
			if (beginPtr) free(beginPtr);
			beginPtr = newBuffer;
			bufferEndPtr = endPtr = newBuffer + size;
			for (; I!=endPtr; ++I) new(I) T();
		} else{
			T *const newEndPtr = beginPtr + size;
			if constexpr (!std::is_trivially_destructible_v<T>)
			for (T *I=newEndPtr; I!=endPtr; ++I) I->~T();
			endPtr = newEndPtr;
		}
	}

	constexpr void reserve(const size_t size) noexcept{
		if (size > (size_t)(bufferEndPtr-beginPtr)){
			T *const newBuffer = (T *)aligned_alloc(alignof(T), size*sizeof(T));
			for (T *J=beginPtr, *I=newBuffer; J!=endPtr; ++J, ++I) new(I) T(std::move(*J));
			if (beginPtr) free(beginPtr);
			bufferEndPtr = newBuffer + size;
			endPtr += newBuffer - beginPtr;
			beginPtr = newBuffer;
		}
	}

	[[nodiscard]] constexpr size_t size() const noexcept{ return endPtr - beginPtr; }

	[[nodiscard]] constexpr size_t capacity() const noexcept{ return bufferEndPtr - beginPtr; }


	[[nodiscard]] constexpr bool isFull() const noexcept{ return endPtr == bufferEndPtr; }

	constexpr T &front() noexcept{ return *beginPtr; }
	constexpr T &back() noexcept{ return *(endPtr-1); }
	[[nodiscard]] constexpr const T &front() const noexcept{ return *beginPtr; }
	[[nodiscard]] constexpr const T &back() const noexcept{ return *(endPtr-1); }

	typedef T value_type;

private:
	T *beginPtr;
	T *endPtr;
	T *bufferEndPtr;
};



template<class T>
struct LookUp16Map{
	LookUp16Map() = default;
	LookUp16Map(const LookUp16Map &) = default;
	LookUp16Map(LookUp16Map &&) = default;
	LookUp16Map &operator =(const LookUp16Map &) = default;
	LookUp16Map &operator =(LookUp16Map &&) = default;
	~LookUp16Map() = default;

	constexpr LookUp16Map(const uint32_t maxId, const uint32_t size) noexcept{
		keyArray.resize(maxId);
		dataArray.resize(size);
	}

	constexpr T &operator [](const uint32_t index) noexcept{
		return dataArray[keyArray[index]];
	}
	[[nodiscard]] constexpr const T &operator [](const uint32_t index) const noexcept{
		return dataArray[keyArray[index]];
	}

	constexpr void push(const T& data, const uint32_t index) noexcept{
		keyArray[index] = std::size(dataArray);
		dataArray.push_back(data);
	}

	constexpr void swap(const uint32_t index1, const uint32_t index2) noexcept{
		swap(keyArray.data()+index1, keyArray.data()+index2);
	}

	typedef T value_type;

	std::vector<ushort> keyArray;
	std::vector<T> dataArray;
};



template<class T>
struct ForwardList{
	ForwardList() noexcept;
	ForwardList(const ForwardList &list) noexcept;
	ForwardList(ForwardList &&list) noexcept;
	~ForwardList();

	ForwardList &operator =(const ForwardList &list) noexcept;
	ForwardList &operator =(ForwardList &&list) noexcept;

	T &operator [](const uint32_t index) noexcept;
	[[nodiscard]] const T &operator [](const uint32_t index) const noexcept;
	[[nodiscard]] uint32_t size() const;
	void resize(const uint32_t size) noexcept;

	void pushFront(const T &newData);
	void popFront();

private:
	struct Node{
		T data;
		Node *next;
	};
	Node *head;

public:
	struct It : public std::iterator<std::forward_iterator_tag ,T>{
		It();
		It(const It &iterator) noexcept;
		It(const ForwardList &list) noexcept;

		It &operator =(const It &iterator) noexcept;
		It &operator =(const ForwardList &list) noexcept;
		
		[[nodiscard]] It operator +(const uint32_t steps) noexcept;
		It &operator +=(const uint32_t steps) noexcept;
		It &operator ++() noexcept;
		T &operator *() noexcept;
		T &operator [](const uint32_t index) noexcept;

		[[nodiscard]] uint32_t operator -(const It iter) const noexcept;

		[[nodiscard]] bool operator ==(const It iterator) const;
		[[nodiscard]] bool operator !=(const It iterator) const;

		void insertBefore(const T &newData) noexcept;
		void removeNext() noexcept;

		Node *ptr;
	};

	[[nodiscard]] constexpr It begin() const noexcept;
	[[nodiscard]] constexpr It end() const noexcept;
};

template<class T>
struct LinkedList{
	LinkedList() noexcept;
	LinkedList(const LinkedList &list) noexcept;
	LinkedList(LinkedList &&list) noexcept;
	~LinkedList();

	LinkedList &operator =(const LinkedList &list) noexcept;
	LinkedList &operator =(LinkedList &&list) noexcept;

	T &operator [](const uint32_t index) noexcept;
	[[nodiscard]] const T &operator [](const uint32_t index) const noexcept;
	[[nodiscard]] uint32_t size() const;
	void resize(const uint32_t size) noexcept; 

	void pushFront(const T &newData);
	void popFront();

private:
	struct Node{
		T data;
		Node *next;
		Node *prev;
	};
	Node *head;

public:
	struct It : public std::iterator<std::random_access_iterator_tag, T>{
		It();
		It(const It &iterator) noexcept;
		It(const LinkedList &list) noexcept;

		It &operator =(const It &iterator) noexcept;
		It &operator =(const LinkedList &list) noexcept;
		
		[[nodiscard]] It operator +(const uint32_t steps) const noexcept;
		[[nodiscard]] It operator -(const uint32_t steps) const noexcept;
		It &operator +=(const uint32_t steps) noexcept;
		It &operator -=(const uint32_t steps) noexcept;
		It &operator ++() noexcept;
		It &operator --() noexcept;
		T &operator *() noexcept;

		[[nodiscard]] uint32_t operator -(const It iter) const noexcept;

		[[nodiscard]] bool operator ==(const It iterator) const;
		[[nodiscard]] bool operator !=(const It iterator) const;

		void insertBefore(const T &newData) noexcept;
		void removeNext() noexcept;
		void insertBehind(const T &newData) noexcept;
		void removePrev() noexcept;

		Node *ptr;
	};

	[[nodiscard]] constexpr It begin() const noexcept;
	[[nodiscard]] constexpr It end() const noexcept;
};



// template<class T, size_t blockSize_ = 64, size_t maxBlockNumber_ = 16>
// struct StableVector{
// 	constexpr StableVector() noexcept : size_{0} {}

// 	constexpr StableVector(const size_t newSize) noexcept : size_{newSize}{
// 		for (size_t i=0; i<newSize; i+=blockSize_)
// 			ptrTable[i] = new T[blockSize];
// 	}

// 	template<size_t blksize, size_t blknum>
// 	constexpr StableVector(const StableVector<T, blksize, blknum> &rhs) noexcept : size_{std::size(rhs)}{
// 		size_t i = size_;
// 		size_t j = 0;
// 		size_t k = 0;
// 		for (; i>=blockSize_; i-=blockSize_, ++j){
// 			ptrTable[j] = (T *) aligned_alloc(alignof(T), blockSize_*sizeof(T));
// 			const T *const end = ptrTable[j] + blockSize_;
// 			const T *J = std::begin(rhs) + j*blockSize_;
// 			for (T *I = ptrTable[j]; I!=end; ++I, ++k)
// 				new(I) T{rhs[k]};
// 		}
// 		if (i){
// 			ptrTable[j] = (T *) aligned_alloc(alignof(T), blockSize_*sizeof(T));
// 			const T *const end = ptrTable[j] + i;
// 			const T *J = std::begin(rhs) + j*blockSize_;
// 			for (T *I = ptrTable[j]; I!=end; ++I, ++k)
// 				new(I) T{rhs[k]};
// 		}
// 	}

// 	template<size_t blknum>
// 	constexpr StableVector(StableVector<T, blockSize_, blknum> &&rhs) noexcept : size_{std::size(rhs)}{
// 		std::copy_n(rhs.ptrTable, rhs.filledBlockNumber(), ptrTable);
// 		rhs.size = 0;
// 	}

// 	constexpr StableVector(const std::initializer_list<T> &rhs) noexcept : size_{std::size(rhs)}{
// 		size_t i = size_;
// 		size_t j = 0;
// 		for (; i>=blockSize_; i-=blockSize_, ++j){
// 			ptrTable[j] = (T *) aligned_alloc(alignof(T), blockSize_*sizeof(T));
// 			const T *const end = ptrTable[j] + blockSize_;
// 			const T *J = std::begin(rhs) + j*blockSize_;
// 			for (T *I = ptrTable[j]; I!=end; ++I, ++J)
// 				new(I) T{*J};
// 		}
// 		if (i){
// 			ptrTable[j] = (T *) aligned_alloc(alignof(T), blockSize_*sizeof(T));
// 			const T *const end = ptrTable[j] + i;
// 			const T *J = std::begin(rhs) + j*blockSize_;
// 			for (T *I = ptrTable[j]; I!=end; ++I, ++J)
// 				new(I) T{*J};
// 		}
// 	}

// 	template<size_t blksize, size_t blknum>
// 	constexpr StableVector &operator =(const StableVector<T, blksize, blknum> &rhs) noexcept{

// 	}

// 	template<size_t blksize, size_t blknum>
// 	constexpr StableVector &operator =(StableVector<T, blksize, blknum> &&rhs) noexcept{
// 		~StableVector();
// 		std::copy_n(rhs.ptrTable, rhs.filledBlockNumber(), ptrTable);
// 		size_ = std::size(rhs);
// 		rhs.size = 0;
// 	}

// 	~StableVector(){
// 		const size_t lastBlockIndex = size_ / blockSize_;
// 		const size_t lastBlockElements = size_ % blockSize_;
// 		for (size_t i = lastBlockElements; i>0; --i)
// 			~T(ptrTable[lastBlockIndex][i]);
// 		if (lastBlockElements) free(ptrTable[lastBlockIndex]);
// 		for (size_t i=size_/blockSize_; i!=0; --i)
// 			delete ptrTable[i];
// 	}

	
// 	constexpr T &operator [](const uint32_t index) noexcept{ return ptrTable[index/blockSize_][index%blockSize_]; }

// 	[[nodiscard]] constexpr const T &operator [](const uint32_t index) const noexcept{ return ptrTable[index/blockSize_][index%blockSize_]; }

// 	constexpr void pushBack(const T &arg) noexcept;
// 	constexpr void pushBack(T &&arg) noexcept;

// 	constexpr void popBack() noexcept;
// 	constexpr void popBack(uint32_t count) noexcept;

// 	constexpr void resize(const size_t newSize) noexcept{
// 		for (size_t i=this->capacity(); i<newSize; i+=blockSize_)
// 			ptrTable[i] = (T *)aligned_alloc(alignof(T), blockSize_*sizeof(T));
// 		size_ = newSize;
// 	}

// 	[[nodiscard]] constexpr size_t size() const noexcept{ return size_; }
// 	[[nodiscard]] constexpr size_t capacity() const noexcept{ return (size_+blockSize_-1)/blockSize_*blockSize_; }
// 	[[nodiscard]] constexpr size_t filledBlockNumber() const noexcept{ return (size_+blockSize_-1)/blockSize_; }

// 	[[nodiscard]] constexpr size_t blockSize() const noexcept{ return blockSize_; }
// 	[[nodiscard]] constexpr size_t maxBlockNumber() const noexcept{ return maxBlockNumber_; }
// 	[[nodiscard]] constexpr size_t totCapacity() const noexcept{ return blockSize_*maxBlockNumber_; }

// 	[[nodiscard]] constexpr bool isFull() const noexcept{ return size_ >= capacity(); }

// 	constexpr T &front() noexcept{ return ptrTable[0][0]; }
// 	constexpr T &back() noexcept{ return ptrTable[(size_-1)/blockSize_][(size_-1)%blockSize_]; }
// 	[[nodiscard]] constexpr const T &front() const noexcept{ return ptrTable[0][0]; }
// 	[[nodiscard]] constexpr const T &back() const noexcept{ ptrTable[(size_-1)/blockSize_][(size_-1)%blockSize_]; }


// 	struct It : public std::iterator<std::random_access_iterator_tag, T>{
// 		[[nodiscard]] constexpr It operator +(const size_t steps) const noexcept{ return It{src, index + steps}; }
// 		[[nodiscard]] constexpr It operator -(const size_t steps) const noexcept{ return It{src, index - steps}; }
// 		constexpr It &operator +=(const size_t steps) noexcept{ index += steps; return *this; }
// 		constexpr It &operator -=(const size_t steps) noexcept{ index -= steps; return *this; }
// 		constexpr It &operator ++() noexcept{ ++index; return *this; }
// 		constexpr It &operator --() noexcept{ --index; return *this; }
// 		constexpr T &operator *() noexcept{ return (*src)[index]; }

// 		[[nodiscard]] constexpr int64_t operator -(const It iter) const noexcept{ return index - iter.index; }

// 		[[nodiscard]] constexpr bool operator ==(const It iter) const{ return src==iter.src && index==iter.index; }
// 		[[nodiscard]] constexpr bool operator !=(const It iter) const{ return src!=iter.src || index!=iter.index; }

// 		StableVector<T, blockSize_, maxBlockNumber_> *src;
// 		size_t index;
// 	};

// 	constexpr T *begin() noexcept{ return It{this, 0}; }
// 	constexpr T *end() noexcept{return It{this, size_}; }

// 	[[nodiscard]] constexpr const T *cbegin() const noexcept{ return It{this, 0}; }
// 	[[nodiscard]] constexpr const T *cend() const noexcept{ return It{this, size_}; }


// 	typedef T value_type;
// private:
// 	size_t size_;
// 	T *ptrTable[maxBlockNumber_];
// };

template<class T>
struct PtrVector{
	PtrVector() noexcept : ptr{nullptr} {}
	PtrVector(const PtrVector &x) noexcept{
		if (x.ptr){
			uint32_t *const memBegin = (uint32_t *)aligned_alloc(std::max(alignof(T), alignof(uint32_t)),
				std::size(x)*sizeof(T) + 2*sizeof(uint32_t)
			);
			memBegin[0] = x.capacity();
			memBegin[1] = std::size(x);
			ptr = (T *)(memBegin + 2);
			if constexpr (!std::is_trivially_constructible_v<T>)
				new(ptr) T[std::size(x)];
		} else ptr = nullptr;
	}
	PtrVector(PtrVector &&x) noexcept{
		ptr = x.ptr;
		x.ptr = nullptr; 
	}
	PtrVector &operator =(const PtrVector &x) noexcept{
		for (auto &I : *this) I.~T();
		if (this->size() < std::size(x)){
			if (ptr) free((uint32_t*)ptr-2);
			uint32_t *const memBegin = (uint32_t *)aligned_alloc(std::max(alignof(T), alignof(uint32_t)),
				std::size(x)*sizeof(T) + 2*sizeof(uint32_t)
			);
			memBegin[0] = x.capacity();
			memBegin[1] = std::size(x);
			ptr = (T *)(memBegin + 2);
		} else ((uint32_t *)ptr)[-1] = std::size(x);
		if constexpr (!std::is_trivially_constructible_v<T>)
			new(ptr) T[std::size(x)];
	}
	PtrVector &operator =(PtrVector &&x) noexcept{
		this->~PtrVector();
		ptr = x.ptr;
		x.ptr = nullptr; 
	}
	~PtrVector(){
		if (ptr){	
			for (auto &I : *this) I.~T();
			free((uint32_t*)ptr-2);
		}
	}

	[[nodiscard]] size_t size() const noexcept{ if (ptr) return ((uint32_t *)ptr)[-1]; else return 0; }
	[[nodiscard]] size_t capacity() const noexcept{ if (ptr) return ((uint32_t *)ptr)[-2]; else return 0; }
	T *begin() noexcept{ return ptr; }
	T *end() noexcept{return ptr + this->size(); }
	[[nodiscard]] const T *cbegin() const noexcept{ return ptr; }
	[[nodiscard]] const T *cend() const noexcept{return ptr + this->size(); }
	T &front() noexcept{ return *ptr; }
	T &back() noexcept{return ptr[this->size()-1]; }

	void resize(const uint32_t newSize){
		if (this->capacity() < newSize){
			uint32_t *const memBegin = (uint32_t *)aligned_alloc(std::max(alignof(T), alignof(uint32_t)),
				newSize*sizeof(T) + 2*sizeof(uint32_t)
			);
			memBegin[0] = newSize;
			memBegin[1] = newSize;
			/* MOVING LOOP */{
				T *I = (T *)(memBegin+2);
				for (auto &J : *this){
					new(I) T{std::move(J)}; ++I;
				}
			}
			if (ptr) free((uint32_t*)ptr-2);
			ptr = (T *)(memBegin + 2);
		} else{
			for (T *I=this->begin()+newSize; I<this->end(); ++I) I->~T();
			((uint32_t*)ptr)[-1] = newSize;
		}
	}
	void reserve(const uint32_t newCapacity){
		if (this->capacity() < newCapacity){
			uint32_t *const memBegin = (uint32_t *)aligned_alloc(std::max(alignof(T), alignof(uint32_t)),
				newCapacity*sizeof(T) + 2*sizeof(uint32_t)
			);
			memBegin[0] = newCapacity;
			memBegin[1] = this->size();
			/* MOVING LOOP */{
				T *I = (T *)(memBegin+2);
				for (auto &J : *this){
					new(I) T{std::move(J)}; ++I;
				}
			}
			if (ptr) free((uint32_t*)ptr-2);
			ptr = (T *)(memBegin + 2);
		}
	}
	void push_back(const T &x){
		if (this->size() >= this->capacity())
			this->reserve(this->size()*2);
		new(ptr + this->size()) T{x};
		++((uint32_t *)ptr)[-1];
	}
	void push_back(T &&x){
		if (this->size() >= this->capacity())
			this->reserve(this->size()*2);
		new(ptr + this->size()) T{std::move(x)};
		++((uint32_t *)ptr)[-1];
	}
	template<class ...Args>
	void emplace_back(Args &&...args){
		if (this->size() >= this->capacity())
			this->reserve(this->size()*2);
		new(ptr + this->size()) T{std::forward(args)...};
		++((uint32_t *)ptr)[-1];
	}


private:
	T *ptr;
};







#define TPL template<class T>
#define CLS ForwardList<T>

TPL CLS::ForwardList() noexcept : head{nullptr} {}

TPL CLS::ForwardList(const CLS &list) noexcept{
	Node *I = head;
	if (list.head){
		I = (Node *)malloc(sizeof(Node));
		I->data = list.head->data;
		for (Node *J=list.head->next; J!=nullptr; J=J->next){
			I->next = (Node *)malloc(sizeof(Node));
			I = I->next;
			I->data = J->data;
		}
	}
	I = nullptr;
}

TPL CLS::ForwardList(CLS &&list) noexcept : head{list.head}{
	~ForwardList();
	list.head = nullptr;
}

TPL CLS::~ForwardList(){
	Node *I = head;
	while (I){
		Node *const toDelete = I;
		I = I->next;
		free(toDelete);
	}
}

TPL CLS &CLS::operator =(const CLS &list) noexcept{
	Node *I = head;
	Node *J = list.head;
	Node *lastNode = head;
	for (; I && J; I=I->next, J=J->next){
		I->data = J->data;
		lastNode = I;
	}

	if (I){
		while (I){
			J = I;
			I = I->next;
			free(J);
		}
	} else{
		if (!head & J){
			head = (Node *)malloc(sizeof(Node));
			head->data = J->data;
			lastNode = head;
			J = J->next;
		}
		for (; J; J=J->next){
			I = (Node *)malloc(sizeof(Node));
			I->data = J->data;
			lastNode->next = I;
			lastNode = I;
		}
	}
	lastNode->next = nullptr;
	return *this;
}

TPL CLS &CLS::operator =(CLS &&list) noexcept{
	~ForwardList();
	head = list.head;
	list.head = nullptr;
	return *this;
}

TPL void CLS::pushFront(const T &newData){
	Node *newNode = (Node *)malloc(sizeof(Node));
	newNode->data = newData;
	newNode->next = head;
	head = newNode;
}

TPL void CLS::popFront(){
	Node *toDelete = head;
	head = head->next;
	free(toDelete);
}

TPL T &CLS::operator [](const uint32_t index) noexcept{
	Node *I = head;
	for (uint32_t i=0; i!=index; ++i)
		I = I->next;
	return I->data;
}

TPL T const &CLS::operator [](const uint32_t index) const noexcept{
	Node *I = head;
	for (uint32_t i=0; i!=index; ++i)
		I = I->next;
	return I->data;
}

TPL uint32_t CLS::size() const{
	uint32_t i = 0;
	for (Node *I=head; I; I=I->next, ++i);
	return i;
}

TPL void CLS::resize(const uint32_t size) noexcept{
	uint32_t lenght = 0;
	Node *I = head;
	Node *lastNode = head;
	for (; lenght<size && I; ++lenght, I=I->next)
		lastNode = I;

	if (I){
		lastNode->next = nullptr;
		while (I){
			lastNode = I;
			I = I->next;
			free(lastNode);
		}
	} else{
		if (!head && size){
			I = (Node *)malloc(sizeof(Node));
			lastNode = I;
			++lenght;
		}
		for (;lenght<size; ++lenght){
			I = (Node *)malloc(sizeof(Node));
			lastNode->next = I;
			lastNode = I;
		}
		lastNode->next = nullptr;
	}
}

TPL constexpr typename CLS::It CLS::begin() const noexcept{
	return CLS::It(*this);
};

TPL constexpr typename CLS::It CLS::end() const noexcept{
	CLS::It result;
	result.ptr = nullptr;
	return result;
};


TPL CLS::It::It() {}

TPL CLS::It::It(const It &iterator) noexcept{
	ptr = iterator.ptr;
}

TPL CLS::It::It(const CLS &list) noexcept{
	ptr = list.head;
}

TPL typename CLS::It &CLS::It::operator =(const It &iterator) noexcept{
	ptr = iterator.ptr;
	return *this;
}

TPL typename CLS::It &CLS::It::operator =(const CLS &list) noexcept{
	ptr = list.head;
	return *this;
}

TPL typename CLS::It CLS::It::operator +(const uint32_t steps) noexcept{
	CLS::It result;
	result.ptr = this->ptr;
	for (uint32_t i=0; i!=steps; ++i)
		result.ptr = result.ptr->next;
	return result;
}

TPL typename CLS::It &CLS::It::operator +=(const uint32_t steps) noexcept{
	for (uint32_t i=0; i!=steps; ++i)
		this->ptr = this->ptr->next;
	return *this;
}

TPL typename CLS::It &CLS::It::operator ++() noexcept{
	ptr = ptr->next;
	return *this;
}

TPL T &CLS::It::operator *() noexcept{
	return ptr->data;
}

TPL T &CLS::It::operator [](const uint32_t index) noexcept{
	Node *I = ptr;
	for (uint32_t i=0; i!=index; ++i)
		I = I->next;
	return I->data;
}

TPL uint32_t CLS::It::operator -(const It iter) const noexcept{
	uint32_t result = 0;
	for (Node *I=iter.ptr; I!=this->ptr; I=I->next, ++result);
	return result;
}

TPL bool CLS::It::operator ==(const It iterator) const{
	return ptr == iterator.ptr;
}

TPL bool CLS::It::operator !=(const It iterator) const{
	return ptr != iterator.ptr;
}

TPL void CLS::It::insertBefore(const T &newData) noexcept{
	Node *newNode = (Node *)malloc(sizeof(Node));
	newNode->data = newData;
	newNode->next = ptr->next;
	ptr->next = newNode;
}

TPL void CLS::It::removeNext() noexcept{
	Node *nodePtr = ptr->next;
	ptr->next = nodePtr->next;
	free(nodePtr);
}

#undef TPL
#undef CLS



#define TPL template<class T>
#define CLS LinkedList<T>

TPL CLS::LinkedList() noexcept{
	head = nullptr;
}

TPL CLS::LinkedList(const CLS &list) noexcept{
	Node *I = head;
	if (list.head){
		I = (Node *)malloc(sizeof(Node));
		I->prev = nullptr;
		I->data = list.head->data;
		for (Node *J=list.head->next; J; J=J->next){
			I->next = (Node *)malloc(sizeof(Node));
			I->next->prev = I;
			I = I->next;
			I->data = J->data;
		}
	}
	I = nullptr;
}

TPL CLS::LinkedList(CLS &&list) noexcept :head{list.head}{
	~LinkedList();
	list.head = nullptr;
}

TPL CLS::~LinkedList(){
	Node *I = head;
	while (I){
		Node *const toDelete = I;
		I = I->next;
		free(toDelete);
	}
}

TPL CLS &CLS::operator =(const CLS &list) noexcept{
	Node *I = head;
	Node *J = list.head;
	Node *lastNode = head;
	for (; I && J; I=I->next, J=J->next){
		I->data = J->data;
		lastNode = I;
	}

	if (I){
		while (I){
			J = I;
			I = I->next;
			free(J);
		}
	} else{
		if (!head && J){
			head = (Node *)malloc(sizeof(Node));
			head->data = J->data;
			head->prev = nullptr;
			lastNode = head;
			J = J->next;
		}
		for (; J; J=J->next){
			I = (Node *)malloc(sizeof(Node));
			I->data = J->data;
			I->prev = lastNode;
			lastNode->next = I;
			lastNode = I;
		}
	}
	lastNode->next = nullptr;
	return *this;
}

TPL CLS &CLS::operator =(CLS &&list) noexcept{
	~LinkedList();
	head = list.head;
	list.head = nullptr;
	return *this;
}

TPL void CLS::pushFront(const T &newData){
	Node *const newNode = (Node *)malloc(sizeof(Node));
	newNode->data = newData;
	newNode->next = head;
	newNode->prev = nullptr;
	if (head)
		head->prev = newNode;
	head = newNode;
}

TPL void CLS::popFront(){
	Node *const toDelete = head;
	head = head->next;
	head->prev = nullptr;
	free(toDelete);
}

TPL T &CLS::operator [](const uint32_t index) noexcept{
	Node *I = head;
	for (uint32_t i=0; i!=index; ++i)
		I = I->next;
	return I->data;
}

TPL T const &CLS::operator [](const uint32_t index) const noexcept{
	Node *I = head;
	for (uint32_t i=0; i!=index; ++i)
		I = I->next;
	return I->data;
}

TPL uint32_t CLS::size() const{
	uint32_t i = 0;
	for (Node *I=head; I; I=I->next, ++i);
	return i;
}

TPL void CLS::resize(const uint32_t size) noexcept{
	uint32_t lenght = 0;
	Node *I = head;
	Node *lastNode = head;
	for (; lenght<size && I; ++lenght, I=I->next)
		lastNode = I;

	if (I){
		lastNode->next = nullptr;
		while (I){
			lastNode = I;
			I = I->next;
			free(lastNode);
		}
	} else{
		if (!head && size){
			head = (Node *)malloc(sizeof(Node));
			head->prev = nullptr;
			lastNode = head;
			++lenght;
		}
		for (;lenght<size; ++lenght){
			I = (Node *)malloc(sizeof(Node));
			I->prev = lastNode;
			lastNode->next = I;
			lastNode = I;
		}
		lastNode->next = nullptr;
	}
}

TPL constexpr typename CLS::It CLS::begin() const noexcept{
	return CLS::It(*this);
};

TPL constexpr typename CLS::It CLS::end() const noexcept{
	CLS::It result;
	result.ptr = nullptr;
	return result;
};


TPL CLS::It::It() {}

TPL CLS::It::It(const It &iterator) noexcept{
	ptr = iterator.ptr;
}

TPL CLS::It::It(const CLS &list) noexcept{
	ptr = list.head;
}

TPL typename CLS::It &CLS::It::operator =(const It &iterator) noexcept{
	ptr = iterator.ptr;
	return *this;
}

TPL typename CLS::It &CLS::It::operator =(const CLS &list) noexcept{
	ptr = list.head;
	return *this;
}

TPL typename CLS::It CLS::It::operator +(const uint32_t steps) const noexcept{
	CLS::It result;
	result.ptr = this->ptr;
	for (uint32_t i=0; i!=steps; ++i)
		result.ptr = result.ptr->next;
	return result;
}

TPL typename CLS::It CLS::It::operator -(const uint32_t steps) const noexcept{
	CLS::It result;
	result.ptr = this->ptr;
	for (uint32_t i=0; i!=steps; ++i)
		result.ptr = result.ptr->prev;
	return result;
}

TPL typename CLS::It &CLS::It::operator +=(const uint32_t steps) noexcept{
	for (uint32_t i=0; i!=steps; ++i)
		this->ptr = this->ptr->next;
	return *this;
}

TPL typename CLS::It &CLS::It::operator -=(const uint32_t steps) noexcept{
	for (uint32_t i=0; i<steps; ++i)
		this->ptr = this->ptr->prev;
	return *this;
}

TPL typename CLS::It &CLS::It::operator ++() noexcept{
	ptr = ptr->next;
	return *this;
}

TPL typename CLS::It &CLS::It::operator --() noexcept{
	ptr = ptr->prev;
	return *this;
}

TPL T &CLS::It::operator *() noexcept{
	return ptr->data;
}

TPL uint32_t CLS::It::operator -(const It iter) const noexcept{
	uint32_t result = 0;
	for (Node *I=iter.ptr; I!=this->ptr; I=I->next, ++result);
	return result;
}

TPL bool CLS::It::operator ==(const It iterator) const{
	return ptr == iterator.ptr;
}

TPL bool CLS::It::operator !=(const It iterator) const{
	return ptr != iterator.ptr;
}

TPL void CLS::It::insertBefore(const T &newData) noexcept{
	Node *newNode = (Node *)malloc(sizeof(Node));
	newNode->data = newData;
	newNode->next = ptr->next;
	newNode->prev = ptr;
	ptr->next = newNode;
}

TPL void CLS::It::removeNext() noexcept{
	Node *nodePtr = ptr->next;
	ptr->next = nodePtr->next;
	nodePtr->next->prev = ptr;
	free(nodePtr);
}

TPL void CLS::It::insertBehind(const T &newData) noexcept{
	Node *newNode = (Node *)malloc(sizeof(Node));
	newNode->data = newData;
	newNode->next = ptr;
	newNode->prev = ptr->prev;
	ptr->prev = newNode;
}

TPL void CLS::It::removePrev() noexcept{
	Node *nodePtr = ptr->prev;
	ptr->prev = nodePtr->prev;
	nodePtr->prev->next = ptr;
	free(nodePtr);
}

#undef TPL
#undef CLS

}	// END OF NAMESPACE	///////////////////////////////////////////////////////////////////