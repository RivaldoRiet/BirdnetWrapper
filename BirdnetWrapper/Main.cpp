#include <iostream>

#include <string>

#include <boost/python.hpp>

#include <boost/python/numpy.hpp>

#include <vector>

namespace py = boost::python;
namespace np = boost::python::numpy;

// decode a Python exception into a string
std::string handle_pyerror() {
    using namespace boost::python;
    using namespace boost;

    PyObject* exc, * val, * tb;
    object formatted_list, formatted;
    PyErr_Fetch(&exc, &val, &tb);
    handle < > hexc(exc), hval(allow_null(val)), htb(allow_null(tb));
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
    return extract < std::string >(formatted);
}

inline std::vector<std::string> get_birdnet_array() {
    std::vector<std::string> strVec = {};
    Py_Initialize();

    try {
        boost::python::object birdnet_array_from_python = boost::python::import("analyze").attr("loadModel");

        boost::python::list python_list_object = boost::python::extract < boost::python::list >((birdnet_array_from_python()));
        boost::python::ssize_t n = boost::python::len(python_list_object);
        for (boost::python::ssize_t i = 0; i < n; i++) {
            boost::python::object element = python_list_object[i];
            std::string single_element_string = boost::python::extract < std::string >(element);
            std::cout << "Array element value: '" << single_element_string << "'" << std::endl;
            strVec.push_back(single_element_string);
        }
        std::cout << "Amount of python array elements: '" << len(python_list_object) << "'" << std::endl;
    }
    catch (const boost::python::error_already_set&) {
        if (PyErr_Occurred()) {
            std::string msg = handle_pyerror();
            std::cout << "Error value: '" << msg << "'" << std::endl;
        }

        boost::python::handle_exception();
        PyErr_Clear();

    }
    return strVec;
}

int main(int argc, char* argv[]) {

    std::vector<std::string> results = get_birdnet_array();
    std::cout << "Vector length: '" << results.size() << "'" << std::endl;
    getchar();
}
