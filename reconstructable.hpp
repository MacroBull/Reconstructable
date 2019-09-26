/*

Copyright (c) 2019 Macrobull

*/

#pragma once

#include <tuple>
#include <type_traits>

//// hepler traits

template <bool P, typename T = void>
using enable_if_t = typename std::enable_if<P, T>::type;

//// reconstruct function

template <typename T, typename... CArgs>
inline T* reconstruct(T& target, CArgs... args)
{
	target.~T();
	return new (&target) T{std::forward<CArgs>(args)...};
}

//// ReconstructorBase: interface for ...

template <typename T>
struct ReconstructorBase
{
	virtual ~ReconstructorBase(){};
	virtual T* reconstruct(T&) = 0;
};

template <typename T, typename... Params>
struct ReconstructorImpl : ReconstructorBase<T>
{
	std::tuple<Params...> params;

	template <typename... CArgs> // make_tuple perfect forward
	explicit ReconstructorImpl(CArgs... args)
		: params{std::make_tuple(std::forward<CArgs>(args)...)} {};

	explicit ReconstructorImpl(const std::tuple<Params...>& rv_params) : params{rv_params} {};

	T* reconstruct(T& target) override
	{
		return reconstruct_impl(target);
	}

protected:
	inline T* reconstruct_impl(T& target, Params... args)
	{
		return ::reconstruct(target, args...);
	}

	template <typename... Args>
	inline enable_if_t<sizeof...(Args) != sizeof...(Params), T*>
	reconstruct_impl(T& target, Args&&... args)
	{
		return reconstruct_impl(target, std::forward<Args>(args)...,
								std::get<sizeof...(Args)>(params));
	}
};

template <typename T>
struct ReconstructorHelper
{
	template <typename... CArgs>
	inline static ReconstructorBase<T>* create(CArgs... args)
	{
		return new ReconstructorImpl<T, CArgs...>{args...};
	}

	template <typename... CArgs>
	inline static std::pair<ReconstructorBase<T>*, T*> create_with_instance(CArgs... args)
	{
		return std::make_pair(new ReconstructorImpl<T, CArgs...>{args...}, new T{args...});
	}
};

//// Reconstructable

template <typename T>
class ReconstructableImpl : public T
{
public:
	template <typename... CArgs>
	explicit ReconstructableImpl(bool init, CArgs... args)
		: T{args...}, m_reconstructor{({
			using Reconstructor = ReconstructorImpl<ReconstructableImpl, bool, CArgs...>;
			init ? new Reconstructor{false, args...} : nullptr;
		})} {};

	~ReconstructableImpl()
	{
		if (m_reconstructor != nullptr)
		{
			delete m_reconstructor;
		}
	}

	inline void reconstruct()
	{
		if (m_reconstructor != nullptr)
		{
			auto reconstructor = m_reconstructor;
			m_reconstructor    = nullptr;
			reconstructor->reconstruct(*this);
			// assert(m_reconstructor == nullptr);
			m_reconstructor = reconstructor;
		}
	}

private:
	ReconstructorBase<ReconstructableImpl>* m_reconstructor{nullptr};
};

template <typename T>
struct Reconstructable : ReconstructableImpl<T>
{
	template <typename... CArgs>
	explicit Reconstructable(CArgs... args) : ReconstructableImpl<T>{true, args...} {};
};
