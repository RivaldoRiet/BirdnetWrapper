#include <iostream>
#include <string>
#include <boost/python.hpp>

namespace py = boost::python;

int main(int argc, char* argv[]) {
	// Initialize the Python interpreter.
	Py_Initialize();

	// Add the current directory to the path.
	py::import("sys").attr("path").attr("append")(".");

	// Get the "hello_from_python" function from "hello.py".
	py::object hello_from_python = py::import("mytest").attr("hello_from_python");

	// Call the "hello_from_python" and extract the return value as a string.
	std::string return_value = py::extract<std::string>(hello_from_python());

	// Print out the return value.
	std::cout << "Return value: '" << return_value << "'" << std::endl;
}