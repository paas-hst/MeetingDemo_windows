#pragma once

#include "fsp_common.h"

namespace fsp{

template<typename T, bool NeedConstruct=true, bool NeedDestruct=true>
	class Vector
	{
	public:
		typedef T*			iterator;
		typedef const T*	const_iterator;
		typedef T			value_type;

	public:
		Vector();
		Vector(const Vector<T, NeedConstruct, NeedDestruct>& rhs);
		
		~Vector();

		T&					operator [] (int idx);
		const T&			operator [] (int idx) const;
		Vector<T, NeedConstruct, NeedDestruct>& operator =  (const Vector<T, NeedConstruct, NeedDestruct>& rhs);
		

		int					size() const;
		
		void				push_back(const T& rhs);
		void				pop_back();

		bool				empty() const;
		void				clear();
		
 		iterator			begin();
 		const_iterator		begin() const;
 		iterator			end();
 		const_iterator		end() const;

		T&					front();
		const T&			front() const;
		T&					back();
		const T&			back() const;

	protected:
		void				_copy(const Vector<T, NeedConstruct, NeedDestruct>& rhs);
		void				_cleanup();
		void				_realloc(int capacity);
		void				_grow();
		void				_move(int from, int to);

	protected:
		int			m_size;
		int			m_capacity;
		T*			m_pData;
	};

	//------------------------------------------------------------------------
	template<typename T, bool NeedConstruct, bool NeedDestruct> inline
	Vector<T, NeedConstruct, NeedDestruct>::Vector()
	:m_size(0)
	,m_capacity(0)
	,m_pData(NULL)
	{

	}

	//------------------------------------------------------------------------
	template<typename T, bool NeedConstruct, bool NeedDestruct> inline
	Vector<T, NeedConstruct, NeedDestruct>::Vector(const Vector<T, NeedConstruct, NeedDestruct>& rhs)
	:m_size(0)
	,m_capacity(0)
	,m_pData(NULL)
	{
		_copy(rhs);
	}

	
	template<typename T, bool NeedConstruct, bool NeedDestruct> inline
	Vector<T, NeedConstruct, NeedDestruct>::~Vector()
	{
		_cleanup();
	}

	
	template<typename T, bool NeedConstruct, bool NeedDestruct> inline T&
	Vector<T, NeedConstruct, NeedDestruct>::operator [] (int idx)
	{
		assert(idx>=0 && idx<m_size);
		return m_pData[idx];
	}

	//------------------------------------------------------------------------
	template<typename T, bool NeedConstruct, bool NeedDestruct> inline const T&
	Vector<T, NeedConstruct, NeedDestruct>::operator [] (int idx) const
	{
		assert(idx>=0 && idx<m_size);
		return m_pData[idx];
	}

	//------------------------------------------------------------------------
	template<typename T, bool NeedConstruct, bool NeedDestruct> inline Vector<T, NeedConstruct, NeedDestruct>&
	Vector<T, NeedConstruct, NeedDestruct>::operator = (const Vector<T, NeedConstruct, NeedDestruct>& rhs)
	{
		if(this == &rhs) return *this;
		
		if(m_capacity >= rhs.m_size)
		{
			int i = 0;
			if(NeedConstruct)
			{
				for(; i < rhs.m_size && i < this->m_size; ++i )
				{
					m_pData[i] = rhs.m_pData[i]; //调用=操作符
				}
				for(; i < rhs.m_size; ++i)
				{
					new(m_pData+i) T(rhs.m_pData[i]); //调用拷贝构造
				}
			}
			else
			{
				FspMemCpy( m_pData, rhs.m_pData, rhs.m_size * sizeof(T) );
				i = rhs.m_size;
			}

			if(NeedDestruct)
			{
				for(; i < m_size; ++i)//本Vector比rhs的size大时，后面多余的元素需要释放掉;
				{
					m_pData[i].~T();
				}
			}
			m_size = rhs.m_size;
		}
		else
		{
			_cleanup();
			_copy(rhs);
		}

		return *this;
	}

	
	template<typename T, bool NeedConstruct, bool NeedDestruct> inline int
	Vector<T, NeedConstruct, NeedDestruct>::size() const
	{
		return m_size;
	}

	
	template<typename T, bool NeedConstruct, bool NeedDestruct> inline void
	Vector<T, NeedConstruct, NeedDestruct>::push_back(const T& rhs)
	{
		if( m_size >= m_capacity )
		{
			_grow();
		}
		new(m_pData+m_size) T(rhs);
		++m_size;
	}

	
	template<typename T, bool NeedConstruct, bool NeedDestruct> inline void
	Vector<T, NeedConstruct, NeedDestruct>::pop_back()
	{
		assert( m_size > 0 );
		--m_size;
		if(NeedDestruct)
		{
			m_pData[m_size].~T();
		}
	}

	
	template<typename T, bool NeedConstruct, bool NeedDestruct> inline bool
	Vector<T, NeedConstruct, NeedDestruct>::empty() const
	{
		return m_size==0;
	}

	
	template<typename T, bool NeedConstruct, bool NeedDestruct> inline void
	Vector<T, NeedConstruct, NeedDestruct>::clear()
	{
		if( 0 != m_size )
		{
			if( NeedDestruct )
			{
				for(int i=0; i<m_size; ++i)
				{
					m_pData[i].~T();
				}
			}
			m_size = 0;
		}
	}

	template<typename T, bool NeedConstruct, bool NeedDestruct> inline typename Vector<T, NeedConstruct, NeedDestruct>::iterator
	Vector<T, NeedConstruct, NeedDestruct>::begin()
	{
		return m_pData;
	}

	//------------------------------------------------------------------------
	template<typename T, bool NeedConstruct, bool NeedDestruct> inline typename Vector<T, NeedConstruct, NeedDestruct>::const_iterator
	Vector<T, NeedConstruct, NeedDestruct>::begin() const
	{
		return m_pData;
	}

	//------------------------------------------------------------------------
	template<typename T, bool NeedConstruct, bool NeedDestruct> inline typename Vector<T, NeedConstruct, NeedDestruct>::iterator
	Vector<T, NeedConstruct, NeedDestruct>::end()
	{
		return m_pData + m_size;
	}

	//------------------------------------------------------------------------
	template<typename T, bool NeedConstruct, bool NeedDestruct> inline typename Vector<T, NeedConstruct, NeedDestruct>::const_iterator
	Vector<T, NeedConstruct, NeedDestruct>::end() const
	{
		return m_pData + m_size;
	}

	//------------------------------------------------------------------------
	template<typename T, bool NeedConstruct, bool NeedDestruct> inline T&
	Vector<T, NeedConstruct, NeedDestruct>::front()
	{
		assert( m_size > 0 );
		return *m_pData;
	}

	//------------------------------------------------------------------------
	template<typename T, bool NeedConstruct, bool NeedDestruct> inline const T&
	Vector<T, NeedConstruct, NeedDestruct>::front() const
	{
		assert( m_size > 0 );
		return *m_pData;
	}

	//------------------------------------------------------------------------
	template<typename T, bool NeedConstruct, bool NeedDestruct> inline T&
	Vector<T, NeedConstruct, NeedDestruct>::back()
	{
		assert( m_size > 0 );
		return m_pData[m_size-1];
	}

	//------------------------------------------------------------------------
	template<typename T, bool NeedConstruct, bool NeedDestruct> inline const T&
	Vector<T, NeedConstruct, NeedDestruct>::back() const
	{
		assert( m_size > 0 );
		return m_pData[m_size-1];
	}

	template<typename T, bool NeedConstruct, bool NeedDestruct> inline void
	Vector<T, NeedConstruct, NeedDestruct>::_copy(const Vector<T, NeedConstruct, NeedDestruct>& rhs)
	{
		assert( 0 == m_size && 0 == m_capacity && NULL == m_pData );
		m_size		= rhs.m_size;
		m_capacity	= rhs.m_size;
		if( m_size > 0 )
		{
			m_pData	= reinterpret_cast<T*>( FspMalloc(m_size * sizeof(T)) );
			if(NeedConstruct)
			{
				for(int i=0; i<m_size; i++)
				{
					new(m_pData+i) T( rhs.m_pData[i] );//调用T类型的拷贝构造函数
				}
			}
			else
			{
				FspMemCpy(m_pData, rhs.m_pData, m_size * sizeof(T));
			}
		}
	}

	//------------------------------------------------------------------------
	template<typename T, bool NeedConstruct, bool NeedDestruct> inline void
	Vector<T, NeedConstruct, NeedDestruct>::_cleanup()
	{
		if(NeedDestruct)
		{
			for(int i=0; i<m_size; ++i)
			{
				m_pData[i].~T(); //调用析构函数(调试发现,基本类型会跳过此行);
			}
		}
		FspFree(m_pData);
		m_pData		= NULL;
		m_size		= 0;
		m_capacity	= 0;
	}

	//------------------------------------------------------------------------
	template<typename T, bool NeedConstruct, bool NeedDestruct> inline void
	Vector<T, NeedConstruct, NeedDestruct>::_realloc(int capacity)
	{
		T* pData = reinterpret_cast<T*>( FspMalloc( capacity * sizeof(T) ) );
		if(NeedConstruct)
		{
			for(int i=0; i<m_size; ++i)
			{
				new(pData+i) T(m_pData[i]);
			}
		}
		else
		{
			FspMemCpy( pData, m_pData, m_size * sizeof(T) );
		}
		if(NeedDestruct)
		{
			for(int i=0; i<m_size; i++)
			{
				m_pData[i].~T();
			}
		}
		FspFree(m_pData);
		m_pData		= pData;
		m_capacity	= capacity;
	}

	//------------------------------------------------------------------------
	template<typename T, bool NeedConstruct, bool NeedDestruct> inline void
	Vector<T, NeedConstruct, NeedDestruct>::_grow()
	{
		int growStep = m_capacity + 3*m_capacity/8 + 32;
		_realloc( growStep + m_capacity );
	}

	//------------------------------------------------------------------------
	template<typename T, bool NeedConstruct, bool NeedDestruct> inline void
	Vector<T, NeedConstruct, NeedDestruct>::_move(int from, int to)
	{
		FspMemMov( (char*)(m_pData+to), (char*)(m_pData+from), sizeof(T)*(m_size-from) );
	}

} //namespace fsp