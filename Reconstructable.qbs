import qbs

CppApplication {
	consoleApplication: true
	files: ["main.cpp", "reconstructable.hpp", "clone_ptr.hpp", "clone_ptr.inl.hpp"]
}
