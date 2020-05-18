#ifndef __XARRAY_H__
#define __XARRAY_H__


#pragma once
#include <stdio.h>


template <typename T>
class xarray
{
private:
	T* m_array;
	int m_size;
	int m_used;

public:
	xarray();
	xarray(int size);
	~xarray();
 
	void resize(size_t newsize);
	void add(T& data);
	void print();
	T& operator [](int index);
};


#endif	// __XARRAY_H__
