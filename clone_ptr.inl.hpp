/*

Copyright (c) 2019 Macrobull

*/

#pragma once

#include "clone_ptr.hpp"

#include <tuple>

#include <cassert>

#ifdef CLONE_PTR_DEF_PRIVATE_IMPL

#define CLONE_PTR_IMPL_NS
#define CLONE_PTR_IMPL_NS_
#define CLONE_PTR_IMPL_STORAGE

#else

#define CLONE_PTR_IMPL_NS detail
#define CLONE_PTR_IMPL_NS_ detail::
#define CLONE_PTR_IMPL_STORAGE inline

#endif

namespace CLONE_PTR_IMPL_NS
{

//// hepler traits

template <bool P, typename T = void>
using enable_if_t = typename std::enable_if<P, T>::type;

//// CloneImpl: implement CloneBase with constructor parameter storage

template <typename T, typename... Params>
class CloneImpl : public CloneBase<T>
{
public:
	inline static T* create(Params... args)
	{
		return new T(std::move(args)...);
	}

	template <typename... CArgs>
	explicit CloneImpl(T* const instance, CArgs... args)
		: m_instance(instance)
		, m_params{std::move(args)...}
	{
		assert_valid();
	}

	CloneImpl(T* const instance, std::tuple<Params...> params)
		: m_instance(instance)
		, m_params(std::move(params))
	{
		assert_valid();
	}

	~CloneImpl() override
	{
		delete m_instance;
		// m_instance = nullptr;
	}

	inline T* instance() const noexcept override
	{
		return m_instance;
	}

	inline CloneImpl* clone() const override
	{
		return new CloneImpl(create(), m_params);
	}

protected:
	inline void assert_valid() const
	{
		assert(m_instance != nullptr && "instance should not be nullptr");
	}

	template <typename... Args>
	inline enable_if_t<sizeof...(Args) != sizeof...(Params), T*> create(Args&&... args) const
	{
		return create(std::forward<Args>(args)..., std::get<sizeof...(Args)>(m_params));
	}

private:
	T* const                        m_instance{};
	/*const*/ std::tuple<Params...> m_params; // modifiable
};

} // namespace CLONE_PTR_IMPL_NS

//// factory for CloneImpl

template <typename T, typename... CArgs>
CLONE_PTR_IMPL_STORAGE CloneBase<T>* create_clone_impl(T* instance, CArgs... args)
{
	return new CLONE_PTR_IMPL_NS_ CloneImpl<T, CArgs...>(instance, std::move(args)...);
}

template <typename T>
ClonePtr<T>::~ClonePtr()
{
	delete m_impl;
	m_impl = nullptr;
}

template <typename T>
ClonePtr<T>& ClonePtr<T>::operator=(const ClonePtr& rvalue)
{
	// reconstruct when this == &rvalue
	const auto impl = rvalue.get_impl()->clone();
	delete m_impl;
	m_impl = impl;
	return *this;
}

template <typename T>
ClonePtr<T>& ClonePtr<T>::operator=(ClonePtr&& xvalue) noexcept
{
	m_impl        = xvalue.m_impl;
	xvalue.m_impl = nullptr;
	return *this;
}

template <typename T>
ClonePtr<T>& ClonePtr<T>::reconstruct()
{
	const auto impl = get_impl()->clone();
	delete m_impl;
	m_impl = impl;
	return *this;
}

template <typename T>
CloneBase<T>* ClonePtr<T>::get_impl() const
{
	assert(m_impl != nullptr && "accessing implementation after destruction");

	return m_impl;
}
