
#include "clone_ptr.inl.hpp"
#include "reconstructable.hpp"

struct NonCopyable
{
	explicit NonCopyable(int a)
		: x(a)
		, y(a)
	{}

	NonCopyable(int a, int b)
		: x(a)
		, y(b)
	{}

	NonCopyable(const NonCopyable&) = delete;
	NonCopyable& operator=(const NonCopyable&) = delete;

	int x, y;
};

#include <iostream>

int main()
{
	{
		ClonePtr<NonCopyable> x(1);
		auto                  y = x;
		y->y                    = 3;
		auto z                  = y;
		std::cout << "x = {1}               : " << x->x << ", " << x->y << std::endl;
		std::cout << "y = x.ctor(), modified: " << y->x << ", " << y->y << std::endl;
		std::cout << "z = y.ctor()          : " << z->x << ", " << z->y << std::endl;
		std::cout << std::endl;
	}
	{
		ClonePtr<NonCopyable> x(1, 2);
		auto                  y = x;
		y->y                    = 3;
		auto z                  = y;
		std::cout << "x = {1, 2}            : " << x->x << ", " << x->y << std::endl;
		std::cout << "y = x.ctor(), modified: " << y->x << ", " << y->y << std::endl;
		std::cout << "z = y.ctor()          : " << z->x << ", " << z->y << std::endl;

		y.reconstruct();
		std::cout << "y.reconstruct()       : " << y->x << ", " << y->y << std::endl;
		std::cout << std::endl;
	}
	{
		NonCopyable x(1, 2);
		std::cout << "x = {1, 2}            : " << x.x << ", " << x.y << std::endl;
		ReconstructorImpl<NonCopyable, int, int>{std::make_tuple(3, 4)}.reconstruct(x);
		std::cout << "reconstruct(x, 3, 4)  : " << x.x << ", " << x.y << std::endl;
		std::cout << std::endl;
	}
	{
		Reconstructable<NonCopyable> x(1, 2);
		x.x = 3;
		x.y = 4;
		std::cout << "create with {1, 2}, modified: " << x.x << ", " << x.y << std::endl;
		x.reconstruct();
		std::cout << "reconstructed               : " << x.x << ", " << x.y << std::endl;
		std::cout << std::endl;
	}
	{
		ReconstructableImpl<NonCopyable> x(true, 1, 2);
		std::cout << "YOU SHOULD NOT CALL ReconstructableImpl(true, ...): " << std::endl;
		x.reconstruct();
		std::cout << std::endl;
	}
	return 0;
}
