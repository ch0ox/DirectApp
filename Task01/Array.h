#pragma once
#ifndef __CHAE_Array_H
#define __CHAE_Array_H

#include <windows.h>
#include <D3dx9tex.h>
#include <string>
#include <vector>
#include <map>
#include <assert.h>

template<typename T> 
class CArray
{
public:
	CArray() { m_pData = nullptr; }
	//~CArray();

	int GetSize() const { return m_size; }
	void Remove()		{ SetSize(0); }
	T* GetData()		{ return m_pData; }
	T& Get(int index) const
	{
		assert(index >= 0 && index < m_size);		// index 가 말이 안되면 중단시켜버리기~
		return m_pData[index];
	}

	HRESULT Add(const T& value);
	HRESULT SetSize(int size);
	HRESULT SetResize(int size);
	HRESULT Set(int index, const T& value);

private:

protected:
	T* m_pData = nullptr;
	int m_size = 0;
	int m_maxSize = 0;
};


template<typename T> 
HRESULT CArray<T>::Add(const T& value)
{
	HRESULT hr;
	// hash table resize
	if (FAILED(hr = SetResize(m_size + 1)))
	{
		MessageBox(NULL, TEXT("Resize Failed!"), TEXT("Error"), MB_OK);
		return hr;
	}

	assert(m_pData != NULL);

	::new (&m_pData[m_size]) T;

	m_pData[m_size] = value;
	++m_size;

	return S_OK;
}

template<typename T> 
HRESULT CArray<T>::Set(int index, const T& value)
{
	if (index < 0 || index >= m_size)
	{
		MessageBox(NULL, TEXT("Set Value Error. Wrong index!"), TEXT("Error"), MB_OK);
		assert(FALSE);
		return E_INVALIDARG;
	}

	// Set Data
	m_pData[index] = value;
	return S_OK;
}

template<typename T>
HRESULT CArray<T>::SetSize(int newSize)
{
	int preSize = m_size;

	// 새로 지정할 크기보다 이전 크기가 더 클 경우 
	if (preSize > newSize)
	{
		for (int i = newSize; i < preSize; ++i)
		{
			m_pData[i].~T();
		}
	}

	HRESULT hr = SetResize(newSize);

	// 새로 지정할 크기가 더 클 경우 공간을 더 생성
	if (preSize < newSize)
	{
		assert(m_pData);
		if (m_pData)
		{
			for (int i = preSize; i < newSize; ++i)
			{
				::new (&m_pData[i]) T;
			}
		}
	}

	return hr;
}

template<typename T> 
HRESULT CArray<T>::SetResize(int newSize)
{
	// Error
	if (newSize < 0)
	{
		MessageBox(NULL, TEXT("Resize Error. Wrong newSize!"), TEXT("Error"), MB_OK);
		assert(FALSE);
		return E_INVALIDARG;
	}

	// Init
	if (newSize == 0)
	{
		if (m_pData)
		{
			free(m_pData);
			m_pData = nullptr;
		}
		m_size = 0;
		m_maxSize = 0;
	}
	// Resize
	else if (m_pData == nullptr || newSize > m_maxSize)
	{
		int initSize = 16;
		int growBy = (m_maxSize == 0) ? initSize : m_maxSize;

		if ((UINT)m_maxSize + (UINT)growBy > (UINT)INT_MAX)
			growBy = INT_MAX - m_maxSize;

		// more bigger ?
		newSize = __max(newSize, m_maxSize + growBy);

		if (sizeof(T) > UINT_MAX / (UINT)newSize)
			return E_INVALIDARG;

		T* pDataNew = (T*)realloc(m_pData, newSize * sizeof(T));

		if (pDataNew == nullptr)
		{
			MessageBox(NULL, TEXT("Resize Error. New data is Nullptr"), TEXT("Error"), MB_OK);
			return E_INVALIDARG;
		}

		// change
		m_pData = pDataNew;
		m_maxSize = newSize;
	}

	return S_OK;
}

#endif //__CHAE_Array_H