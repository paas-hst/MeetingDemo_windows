#pragma once

#include "fsp_common.h"

namespace fsp{

template<typename T>
class TStringBase
{
public:
	TStringBase();
	TStringBase(const T* str);
	TStringBase(const TStringBase<T>& oth);
	~TStringBase();

	TStringBase<T>&		operator=(const TStringBase<T>& rhs);
	TStringBase<T>&		operator=(const T* rhs);

	bool				operator==(const TStringBase<T>& rhs) const;
	bool				operator==(const T* rhs) const;

	TStringBase<T>&		operator+=(const TStringBase<T>& str);
	TStringBase<T>&		operator+=(const T* str);

	int					length() const;
	void				clear();
	bool				empty() const;
	const T*			c_str() const;
	void				format(const T* format, ...);
	void				vformat(const T* format, va_list argptr);

	int					compare(const TStringBase<T>& oth) const;
	int					compare(const T* oth) const;

	bool				beginwith(const TStringBase<T>& s) const;
	bool				beginwith(const T* s) const;
	bool				endwith(const TStringBase<T>& s) const;
	bool				endwith(const T* s) const;

	friend bool			operator == (const T* lhs, const TStringBase& rhs)
	{
		return rhs.compare(lhs ? lhs : "") == 0;
	}

	friend bool			operator != (const T* lhs, const TStringBase& rhs)
	{
		return !(lhs == rhs);
	}

	static TStringBase<T>	Format(const T* format, ...);

protected:
	struct StringData
	{
		T*			getData();
		const T*	getData() const;
		T*			refCopy();
		int			getSize() const;
		void		unRef();
		void		freeMemory();
		StringData*		clone() const;
		static StringData*	Create(int len);

		int			m_length;
		int			m_capacity;
		int			m_references;
	};

	explicit		TStringBase(StringData* d);
	StringData&			_getData() const;
	static StringData&		_GetEmptyStringData();

protected:
	T*						m_string;
	static unsigned char	ms_emptyStringRep[sizeof(int) + sizeof(int) + sizeof(int) + sizeof(T)];
};

typedef TStringBase<char>	String;

template<typename T> inline T*
TStringBase<T>::StringData::getData()
{
	return reinterpret_cast<T*>(this + 1);
}

template<typename T> inline const T*
TStringBase<T>::StringData::getData() const
{
	return reinterpret_cast<const T*>(this + 1);
}

template<typename T> inline T*
TStringBase<T>::StringData::refCopy()
{
	++m_references;
	return getData();
}

template<typename T> inline int
TStringBase<T>::StringData::getSize() const
{
	return sizeof(Rep) + sizeof(T) * (m_capacity + 1);
}

template<typename T> inline void
TStringBase<T>::StringData::unRef()
{
	if (--m_references < 0)
	{
		if (this != reinterpret_cast<StringData*>(ms_emptyStringRep))
		{
			freeMemory();
		}
	}
}

template<typename T> inline void
TStringBase<T>::StringData::freeMemory()
{
	assert(this != reinterpret_cast<StringData*>(ms_emptyStringRep));
	FspFree(this);
}

template<typename T> inline typename TStringBase<T>::StringData*
TStringBase<T>::StringData::clone() const
{
	Rep* r = reinterpret_cast<Rep*>(iliveMalloc(getSize()));
	r->m_length = m_length;
	r->m_capacity = m_capacity;
	r->m_references = 0;
	iliveMemCpy(r->getData(), getData(), m_length + 1);
	return r;
}

template<typename T> inline typename TStringBase<T>::StringData*
TStringBase<T>::StringData::Create(int len)
{
	unsigned int scapacity = len;
	if (scapacity < 32)
	{
		scapacity = 32;
	}
	StringData* r = reinterpret_cast<StringData*>(FspMalloc(sizeof(StringData) + sizeof(T)*(scapacity + 1)));
	r->m_length = len;
	r->m_capacity = scapacity;
	r->m_references = 0;
	return r;
}

template<typename T> unsigned char TStringBase<T>::ms_emptyStringRep[sizeof(int) + sizeof(int) + sizeof(int) + sizeof(T)] = { 0 };

template<typename T> inline TStringBase<T>
	TStringBase<T>::Format(const T* format, ...)
	{
		TStringBase<T> str;
		va_list argptr;
		va_start(argptr, format);
		str.vformat(format, argptr);
		va_end(argptr);
		return str;
	}

	template<typename T> inline typename TStringBase<T>::StringData&
		TStringBase<T>::_GetEmptyStringData()
	{
		return *reinterpret_cast<StringData*>(ms_emptyStringRep);
	}

	template<typename T> inline
		TStringBase<T>::TStringBase()
	{
		m_string = _GetEmptyStringData().refCopy();
	}

	template<typename T> inline
		TStringBase<T>::TStringBase(typename TStringBase<T>::StringData* r)
		:m_string(r->getData())
	{

	}

	template<typename T> inline
		TStringBase<T>::TStringBase(const T* str)
	{
		if (NULL != str && str[0] != 0)
		{
			int slen = FspStrLen(str);
			m_string = StringData::Create(slen)->getData();
			FspMemCpy(m_string, str, slen + 1);
		}
		else
		{
			m_string = _GetEmptyStringData().refCopy();
		}
	}

	template<typename T> inline
		TStringBase<T>::TStringBase(const TStringBase<T>& oth)
	{
		m_string = oth._getData().refCopy();
	}

	template<typename T> inline
		TStringBase<T>::~TStringBase()
	{
		_getData().unRef();
	}

	template<typename T> inline	TStringBase<T>&
		TStringBase<T>::operator = (const TStringBase<T>& rhs)
	{
		_getData().unRef();
		m_string = rhs._getData().refCopy();
		return *this;
	}

	template<typename T> inline TStringBase<T>&
		TStringBase<T>::operator = (const T* rhs)
	{
		if (rhs != NULL && 0 != *rhs)
		{
			int slen = FspStrLen(rhs);
			if (_getData().m_capacity < slen || _getData().m_references > 0)
			{
				_getData().unRef();
				m_string = StringData::Create(slen)->getData();
			}
			FspMemCpy(m_string, rhs, slen + 1);
			_getData().m_length = slen;
		}
		else
		{
			_getData().unRef();
			m_string = _GetEmptyStringData().refCopy();
		}
		return *this;
	}

	template<typename T> inline bool
		TStringBase<T>::operator == (const TStringBase<T>& rhs) const
	{
		return length() == rhs.length() ? compare(rhs) == 0 : false;
	}

	template<typename T> inline bool
		TStringBase<T>::operator == (const T* rhs) const
	{
		return compare(rhs ? rhs : "") == 0;
	}

	template<typename T> inline int
		TStringBase<T>::length() const
	{
		return _getData().m_length;
	}

	template<typename T> inline void
		TStringBase<T>::clear()
	{
		_getData().unRef();
		m_string = _GetEmptyStringData().refCopy();
	}

	template<typename T> inline bool
		TStringBase<T>::empty() const
	{
		return 0 == length();
	}

	template<typename T> inline const T*
		TStringBase<T>::c_str() const
	{
		return m_string;
	}


	template<typename T> inline	void
		TStringBase<T>::format(const T* format, ...)
	{
		va_list argptr;
		va_start(argptr, format);
		vformat(format, argptr);
		va_end(argptr);
	}

	template<typename T> inline void
		TStringBase<T>::vformat(const T* format, va_list argptr)
	{
		if (_getData().m_references > 0)
		{
			_getData().unRef();
			m_string = StringData::Create(FSP_MININUM_STRING_CAPACITY)->getData();
		}
		while (true)
		{
			int size = _getData().m_capacity;
			int nchars = iliveVsnprintf(m_string, size, format, argptr);
			if (nchars >= 0 && nchars < size)
			{
				_getData().m_length = nchars;
				break;
			}
			else
			{
				_getData().unRef();
				m_string = StringData::Create(size * 2 > 256 ? size * 2 : 256)->getData();
			}
		}
	}

	template<typename T> inline int
		TStringBase<T>::compare(const TStringBase<T>& oth) const
	{
		return FspStrCmp(c_str(), oth.c_str());
	}

	template<typename T> inline int
		TStringBase<T>::compare(const T* oth) const
	{
		return FspStrCmp(c_str(), oth);
	}

	template<typename T> inline	typename TStringBase<T>::StringData&
		TStringBase<T>::_getData() const
	{
		return *(reinterpret_cast<StringData*>(m_string)-1);
	}
} //namespace fsp