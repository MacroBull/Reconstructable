/*

Copyright (c) 2019 Macrobull

*/

#pragma once

#include <utility>

//// CloneBase: interface for clone-able container

template <typename T>
class CloneBase
{
public:
	CloneBase()          = default;
	virtual ~CloneBase() = default;

	CloneBase(const CloneBase&) = default;

	CloneBase& operator=(const CloneBase&) = default;

	virtual T*         instance() const = 0;
	virtual CloneBase* clone() const    = 0;
};

template <typename T, typename... CArgs>
CloneBase<T>* create_clone_impl(T* instance, CArgs... args);

//// ClonePtr: drop Params... from CloneImpl

template <typename T>
class ClonePtr
{
public:
	template <typename... CArgs>
	explicit ClonePtr(CArgs... args)
		: m_impl(create_clone_impl(
				  new T(std::forward<CArgs>(args)...), std::forward<CArgs>(args)...))
	{}

	template <typename U>
	explicit ClonePtr(std::initializer_list<U>&& args)
		: m_impl(create_clone_impl(new T(std::forward<std::initializer_list<U>>(args)),
								   std::forward<std::initializer_list<U>>(args)))
	{}

	virtual ~ClonePtr();

	ClonePtr(const ClonePtr& rvalue)
		: m_impl(rvalue.get_impl()->clone())
	{}

	ClonePtr(ClonePtr&& xvalue) noexcept
		: m_impl(nullptr)
	{
		std::swap(m_impl, xvalue.m_impl);
	}

	ClonePtr& operator=(const ClonePtr& rvalue);
	ClonePtr& operator=(ClonePtr&& xvalue) noexcept;

	inline const T& operator*() const
	{
		return *get_impl()->instance();
	}

	inline T& operator*()
	{
		return *get_impl()->instance();
	}

	inline const T* operator->() const
	{
		return get_impl()->instance();
	}

	inline T* operator->()
	{
		return get_impl()->instance();
	}

	ClonePtr& reconstruct();

protected:
	CloneBase<T>* get_impl() const;

private:
	CloneBase<T>* m_impl{};
};
