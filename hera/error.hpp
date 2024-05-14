// hera
// Copyright (C) 2024  Cole Reynolds
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef HERA_ERROR_HPP
#define HERA_ERROR_HPP

#if HERA_STACKTRACE
#include <cpptrace/cpptrace.hpp>
#else
#include <stdexcept>
#endif

namespace hera {

[[noreturn]] inline void rethrow()
{
#if HERA_STACKTRACE
    cpptrace::rethrow_and_wrap_if_needed(1);
#else
    throw;
#endif
}

#if HERA_STACKTRACE
using cpptrace::domain_error;
using cpptrace::invalid_argument;
using cpptrace::length_error;
using cpptrace::logic_error;
using cpptrace::out_of_range;
using cpptrace::overflow_error;
using cpptrace::range_error;
using cpptrace::runtime_error;
using cpptrace::underflow_error;
#else
using std::domain_error;
using std::invalid_argument;
using std::length_error;
using std::logic_error;
using std::out_of_range;
using std::overflow_error;
using std::range_error;
using std::runtime_error;
using std::underflow_error;
#endif
} // namespace hera

#endif
