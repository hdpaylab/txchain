#ifndef ZERO_AFTER_FREE_ALLOCATOR_H
#define ZERO_AFTER_FREE_ALLOCATOR_H

#include <openssl/crypto.h> // for OPENSSL_cleanse()

//
// Allocator that clears its contents before deletion.
//
template <typename T>
struct zero_after_free_allocator : public std::allocator<T> {
	// MSVC8 default copy constructor is broken
	typedef std::allocator<T> base;
	typedef typename base::size_type size_type;
	typedef typename base::difference_type difference_type;
	typedef typename base::pointer pointer;
	typedef typename base::const_pointer const_pointer;
	typedef typename base::reference reference;
	typedef typename base::const_reference const_reference;
	typedef typename base::value_type value_type;
	zero_after_free_allocator() throw() {}
	zero_after_free_allocator(const zero_after_free_allocator& a) throw() : base(a) {}
	template <typename U>
	zero_after_free_allocator(const zero_after_free_allocator<U>& a) throw() : base(a)
	{
	}
	~zero_after_free_allocator() throw() {}
	template <typename _Other>
	struct rebind {
		typedef zero_after_free_allocator<_Other> other;
	};

	void deallocate(T* p, std::size_t n)
	{
		if (p != NULL)
			OPENSSL_cleanse(p, sizeof(T) * n);
		std::allocator<T>::deallocate(p, n);
	}
};

#endif
