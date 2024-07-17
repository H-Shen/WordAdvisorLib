#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "WordAdvisor.cpp"

namespace py = pybind11;

PYBIND11_MODULE(WordAdvisor, m) {
    m.doc() = "My Word Advisor Implementation";
    py::class_<WordAdvisor>(m, "WordAdvisor")
            .def(py::init<const std::vector<std::string> &>())
            .def("get_best_match_by_multi_threads", &WordAdvisor::get_best_match_by_multi_threads)
            .def("get_best_match_of_query_list_by_multi_threads",&WordAdvisor::get_best_match_of_query_list_by_multi_threads);
}
