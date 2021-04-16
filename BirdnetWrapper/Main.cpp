#include <iostream>
#include <string>
#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <vector>

namespace py = boost::python;
namespace np = boost::python::numpy;

// decode a Python exception into a string
std::string handle_pyerror()
{
	using namespace boost::python;
	using namespace boost;

	PyObject* exc, * val, * tb;
	object formatted_list, formatted;
	PyErr_Fetch(&exc, &val, &tb);
	handle<> hexc(exc), hval(allow_null(val)), htb(allow_null(tb));
	object traceback(import("traceback"));
	if (!tb) {
		object format_exception_only(traceback.attr("format_exception_only"));
		formatted_list = format_exception_only(hexc, hval);
	}
	else {
		object format_exception(traceback.attr("format_exception"));
		formatted_list = format_exception(hexc, hval, htb);
	}
	formatted = str("\n").join(formatted_list);
	return extract<std::string>(formatted);
}


int main(int argc, char* argv[]) {
	// Initialize the Python interpreter.
	Py_Initialize();

	// Add the current dir//r("path").attr("append")(".");
	try {
	//py::object array_from_python = py::import("analyze").attr("writeResultsToArray");
	py::object array_from_python = py::import("analyze").attr("loadModel");

	py::list abc = py::extract<py::list>((array_from_python()));
	py::ssize_t n = py::len(abc);
	for (py::ssize_t i = 0; i < n; i++) {
		py::object elem = abc[i];
		std::string up = py::extract<std::string>(elem);
		std::cout << "Elem value: '" << up << "'" << std::endl;
	}
std::cout << "Return value: '" << len(abc) << "'" << std::endl;
	}
	
	catch (const py::error_already_set&) {
		if (PyErr_Occurred()) {
			std::string msg = handle_pyerror();
			std::cout << "Error value: '" << msg << "'" << std::endl;
		}

		py::handle_exception();
		PyErr_Clear();
	}

	//std::string return_value1 = py::extract<std::string>(hello_from_python());

	// Print out the return value.//
	
}