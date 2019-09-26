/*

Copyright (c) 2019 Macrobull

*/

#pragma once

#include <tuple>
#include <type_traits>

//// hepler traits

template <bool P, typename T = void>
using enable_if_t = typename std::enable_if<P, T>::type;

//// CloneBase: interface for clone-able container

template <typename T>
class CloneBase
{
public:
	CloneBase(){};

	CloneBase(const CloneBase&) = delete;
	CloneBase& operator=(const CloneBase&) = delete;

	virtual ~CloneBase(){};
	virtual T*         instance() const = 0;
	virtual CloneBase* clone() const    = 0;
};

//// CloneImpl: implement CloneBase with constructor parameter storage

template <typename T, typename... Params>
class CloneImpl : public CloneBase<T>
{
public:
	inline static T* create(Params... args)
	{
		return new T{std::forward<Params>(args)...};
	}

	template <typename... CArgs> // make_tuple perfect forward
	explicit CloneImpl(T* const instance, CArgs... args)
		: m_instance{instance}, m_params{std::make_tuple(std::forward<CArgs>(args)...)} {};

	CloneImpl(T* const instance, const std::tuple<Params...>& params)
		: m_instance{instance}, m_params{params} {};

	~CloneImpl() override
	{
		if (m_instance != nullptr)
		{
			delete m_instance;
		}
	}

	inline T* instance() const override
	{
		return m_instance;
	}

	inline CloneImpl* clone() const override
	{
		return new CloneImpl{create(), m_params};
	}

protected:
	template <typename... Args>
	inline enable_if_t<sizeof...(Args) != sizeof...(Params), T*> create(Args&&... args) const
	{
		return create(std::forward<Args>(args)..., std::get<sizeof...(Args)>(m_params));
	}

private:
	T* const              m_instance{nullptr};
	std::tuple<Params...> m_params;
};

//// ClonePtr: drop Params... from CloneImpl

template <typename T>
class ClonePtr
{
public:
	template <typename... CArgs>
	explicit ClonePtr(CArgs... args)
		: m_impl{new CloneImpl<T, CArgs...>{new T{args...}, args...}} {};

	ClonePtr(const ClonePtr& rvalue) : m_impl{rvalue.m_impl->clone()} {};

	ClonePtr(ClonePtr&& xvalue) = delete;
	//		: m_impl{xvalue.m_impl}
	//	{
	//		xvalue.m_impl = nullptr;
	//	}

	ClonePtr& operator=(const ClonePtr& rvalue)
	{
		auto impl = rvalue.m_impl->clone();
		// assert(m_impl != nullptr);
		delete m_impl;
		m_impl = impl;
		return *this;
	}

	ClonePtr& operator=(ClonePtr&& xvalue) = delete;
	//	{
	//      // assert(m_impl != nullptr);
	//		std::swap(m_impl, xvalue.m_impl);
	//		delete xvalue.m_impl;
	//		xvalue.m_impl = nullptr;
	//		return *this;
	//	}

	~ClonePtr()
	{
		// assert(m_impl != nullptr);
		delete m_impl;
	}

	inline const T& operator*() const
	{
		return *m_impl->instance();
	}

	inline T& operator*()
	{
		return *m_impl->instance();
	}

	inline const T* operator->() const
	{
		return m_impl->instance();
	}

	inline T* operator->()
	{
		return m_impl->instance();
	}

	inline ClonePtr& reconstruct()
	{
		auto impl = m_impl->clone();
		// assert(m_impl != nullptr);
		delete m_impl;
		m_impl = impl;
		return *this;
	}

private:
	CloneBase<T>* m_impl;
};
