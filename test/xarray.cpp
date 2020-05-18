////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////


#include "xarray.h"
 
 
template <typename T>
xarray<T>::xarray() : m_size(100), m_used(0)
{
	m_array = new T[m_size];
}

 
template <typename T>
xarray<T>::xarray(int size) : m_size(size), m_used(0)
{
	m_array = new T[m_size];
}
 

template <typename T>
xarray<T>::~xarray() 
{
	delete[] m_array;
}
 
 
template <typename T>
void xarray<T>::resize(size_t newsize)
{
	T *newarr = new T[newsize];
	T *oldarr = m_array; 
 
	if (newsize < m_used)
		memcpy(newarr, oldarr, sizeof(T) * newsize);
	else
		memcpy(newarr, oldarr, sizeof(T) * m_used);
	m_size = newsize;
	m_array = newarr;
	printf("xarray resize: %d => %d\n", m_size, newsize);

	delete[] oldarr;
}

 
template <typename T>
void xarray<T>::add(T& data)
{
	if (m_size > m_used)
	{
		m_array[m_used] = data;
		m_used++;
		return;
	}
	else
	{
		resize((size_t)(m_size * 1.5) + 1);	// 50% 증가 
		m_array[m_used] = data;
		m_used++;
	}
}

 
template <typename T>
void xarray<T>::print()
{
	for(int ii = 0; ii < this->m_used; ii++)
		printf("%d ", this->m_array[ii]);
	printf("\n");
}
 

template <typename T>
T& xarray<T>::operator[](int index)
{
	return this->m_array[index];
}

