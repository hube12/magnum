/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
              Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include <sstream>
#include <pybind11/pybind11.h>
#include <pybind11/operators.h>

#include "Magnum/Magnum.h"
#include "Magnum/Math/Angle.h"
#include "Magnum/Math/BoolVector.h"

#include "magnum.h"

using namespace Magnum;
namespace py = pybind11;

namespace {

template<class T> std::string repr(const T& value) {
    std::ostringstream out;
    Debug{&out, Debug::Flag::NoNewlineAtTheEnd} << value;
    return out.str();
}

template<class T> void angle(py::class_<T>& c) {
    /*
        Missing APIs:

        Type
    */

    c
        /* Constructors */
        .def(py::init(), "Default constructor")
        .def(py::init<Math::ZeroInitT>(), "Construct a zero value")
        .def(py::init<typename T::Type>(), "Explicit conversion from a unitless type")

        /* Explicit conversion to an underlying type */
        .def("__float__", &T::operator typename T::Type, "Conversion to underlying type")

        /* Comparison */
        .def(py::self == py::self, "Equality comparison")
        .def(py::self != py::self, "Non-equality comparison")
        .def(py::self < py::self, "Less than comparison")
        .def(py::self > py::self, "Greater than comparison")
        .def(py::self <= py::self, "Less than or equal comparison")
        .def(py::self >= py::self, "Greater than or equal comparison")

        /* Arithmetic ops. Need to use lambdas because the C++ functions return
           the Unit base class :( */
        .def("__neg__", [](const T& self) -> T {
                return -self;
            }, "Negated value")
        .def("__iadd__", [](T& self, const T& other) -> T& {
                self += other;
                return self;
            }, "Add and assign a value")
        .def("__add__", [](const T& self, const T& other) -> T {
                return self + other;
            }, "Add a value")
        .def("__isub__", [](T& self, const T& other) -> T& {
                self -= other;
                return self;
            }, "Subtract and assign a value")
        .def("__sub__", [](const T& self, const T& other) -> T {
                return self - other;
            }, "Subtract a value")
        .def("__imul__", [](T& self, typename T::Type other) -> T& {
                self *= other;
                return self;
            }, "Multiply with a number and assign")
        .def("__mul__", [](const T& self, typename T::Type other) -> T {
                return self * other;
            }, "Multiply with a number")
        .def("__itruediv__", [](T& self, typename T::Type other) -> T& {
                self /= other;
                return self;
            }, "Divide with a number and assign")
        .def("__truediv__", [](const T& self, typename T::Type other) -> T {
                return self / other;
            }, "Divide with a number")
        .def("__truediv__", [](const T& self, const T& other) -> typename T::Type {
                return self / other;
            }, "Ratio of two values")

        .def("__repr__", repr<T>, "Object representation");
}

template<class T> void boolVector(py::class_<T>& c) {
    c
        /* Constructors */
        .def(py::init(), "Default constructor")
        .def(py::init<Math::ZeroInitT>(), "Construct a zero-filled boolean vector")
        .def(py::init<bool>(), "Construct a boolean vector with one value for all fields")
        .def(py::init<UnsignedByte>(), "Construct a boolean vector from segment values")

        /* Explicit conversion to bool */
        .def("__bool__", &T::operator bool, "Boolean conversion")

        /* Comparison */
        .def(py::self == py::self, "Equality comparison")
        .def(py::self != py::self, "Non-equality comparison")

        /* Member functions */
        .def("all", &T::all, "Whether all bits are set")
        .def("none", &T::none, "Whether no bits are set")
        .def("any", &T::any, "Whether any bit is set")

        /* Set / get */
        .def("__setitem__", &T::set, "Set a bit at given position", py::arg("i"), py::arg("value"))
        .def("__getitem__", &T::operator[], "Bit at given position")

        /* Operators */
        .def(~py::self, "Bitwise inversion")
        .def(py::self &= py::self, "Bitwise AND and assign")
        .def(py::self & py::self, "Bitwise AND")
        .def(py::self |= py::self, "Bitwise OR and assign")
        .def(py::self | py::self, "Bitwise OR")
        .def(py::self ^= py::self, "Bitwise XOR and assign")
        .def(py::self ^ py::self, "Bitwise XOR")

        .def("__repr__", repr<T>, "Object representation");

    /* Constants */
    c.attr("SIZE") = int(T::Size);
}

}

void math(py::module& root, py::module&& m) {
    m.doc() = "Math library";

    /* Tags */
    py::class_<Math::ZeroInitT> zeroInit{m, "ZeroInitT", "Zero initialization tag type"};

    m.attr("ZeroInit") = Math::ZeroInit;

    /* Deg, Rad, Degd, Radd */
    py::class_<Deg> deg{root, "Deg", "Float degrees"};
    py::class_<Rad> rad{root, "Rad", "Float radians"};
    py::class_<Degd> degd{root, "Degd", "Double degrees"};
    py::class_<Radd> radd{root, "Radd", "Double radians"};
    deg.def(py::init<Degd>(), "Conversion from a double representation")
       .def(py::init<Rad>(), "Conversion from radians");
    rad.def(py::init<Radd>(), "Conversion from a double representation")
       .def(py::init<Deg>(), "Conversion from degrees");
    degd.def(py::init<Deg>(), "Conversion from a float representation")
        .def(py::init<Radd>(), "Conversion from radians");
    radd.def(py::init<Rad>(), "Conversion from a float representation")
        .def(py::init<Degd>(), "Conversion from degrees");
    angle(deg);
    angle(rad);
    angle(degd);
    angle(radd);

    /* BoolVector */
    py::class_<Math::BoolVector<2>> boolVector2{root, "BoolVector2", "Two-component bool vector"};
    py::class_<Math::BoolVector<3>> boolVector3{root, "BoolVector3", "Three-component bool vector"};
    py::class_<Math::BoolVector<4>> boolVector4{root, "BoolVector4", "Four-component bool vector"};
    boolVector(boolVector2);
    boolVector(boolVector3);
    boolVector(boolVector4);
}
