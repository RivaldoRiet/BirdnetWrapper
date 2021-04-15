#include <iostream>
#include <string>
#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <vector>

namespace py = boost::python;
namespace np = boost::python::numpy;

template<typename T>
inline
std::vector< T > py_list_to_std_vector(const boost::python::object& iterable)
{
	return std::vector< T >(boost::python::stl_input_iterator< T >(iterable),
		boost::python::stl_input_iterator< T >());
}

int main(int argc, char* argv[]) {
	// Initialize the Python interpreter.
	Py_Initialize();

	// Add the current dir//r("path").attr("append")(".");

	//py::object array_from_python = py::import("analyze").attr("writeResultsToArray");
	py::object array_from_python = py::import("analyze").attr("loadModel");

	py::list abc = py::extract<py::list>((array_from_python()));
	py::ssize_t n = py::len(abc);
	for (py::ssize_t i = 0; i < n; i++) {
		py::object elem = abc[i];
		std::string up = py::extract<std::string>(elem);
		std::cout << "Elem value: '" << up << "'" << std::endl;
	}

	//std::string return_value1 = py::extract<std::string>(hello_from_python());

	// Print out the return value.//
	std::cout << "Return value: '" << len(abc) << "'" << std::endl;
}

boost::python::list extract_list(py::object x)
{
	py::extract<py::list> get_list((x));

	// Make sure we always have the right idea about whether it's a list
	bool is_list_1 = get_list.check();
	bool is_list_2 = PyObject_IsInstance(x.ptr(), (PyObject*)&PyList_Type);
	if (is_list_1 != is_list_2) {
		throw std::runtime_error("is_list_1 == is_list_2 failure.");
	}
	return get_list();
}

template< typename T >
inline
std::vector< T > to_std_vector(const py::object& iterable)
{
	return std::vector< T >(py::stl_input_iterator< T >(iterable),
		py::stl_input_iterator< T >());
}