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

	// Add the current directory to the path.
	py::import("sys").attr("path").attr("append")(".");

	// Get the "hello_from_python" function from "hello.py".
	py::object hello_from_python = py::import("mytest").attr("hello_from_python");
	py::object array_from_python = py::import("mytest").attr("writeResultsToArray");

	// Call the "hello_from_python" and extract the return value as a string.
	//std::vector return_value = py::extract<std::string>(array_from_python());
	std::string return_value1 = py::extract<std::string>(hello_from_python());

	// Print out the return value.//
	//std::cout << "Return value: '" << return_value << "'" << std::endl;
}

template< typename T >
inline
std::vector< T > to_std_vector(const py::object& iterable)
{
	return std::vector< T >(py::stl_input_iterator< T >(iterable),
		py::stl_input_iterator< T >());
}