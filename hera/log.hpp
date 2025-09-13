// hera
// Copyright (C) 2024-2025  Cole Reynolds
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

#ifndef HERA_LOG_HPP
#define HERA_LOG_HPP

#define QUILL_DISABLE_NON_PREFIXED_MACROS

#include <quill/Logger.h>
#include <quill/LogMacros.h>
#include <quill/std/FilesystemPath.h>
#include <quill/std/Vector.h>

namespace hera {

extern quill::Logger* global_log;

using quill::LogLevel;

} // namespace hera

#define LOG_TRACE_L3(fmt, ...)                                                 \
    QUILL_LOG_TRACE_L3(hera::global_log, fmt, ##__VA_ARGS__)
#define LOG_TRACE_L2(fmt, ...)                                                 \
    QUILL_LOG_TRACE_L2(hera::global_log, fmt, ##__VA_ARGS__)
#define LOG_TRACE_L1(fmt, ...)                                                 \
    QUILL_LOG_TRACE_L1(hera::global_log, fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...)                                                    \
    QUILL_LOG_DEBUG(hera::global_log, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) QUILL_LOG_INFO(hera::global_log, fmt, ##__VA_ARGS__)
#define LOG_WARNING(fmt, ...)                                                  \
    QUILL_LOG_WARNING(hera::global_log, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...)                                                    \
    QUILL_LOG_ERROR(hera::global_log, fmt, ##__VA_ARGS__)
#define LOG_CRITICAL(fmt, ...)                                                 \
    QUILL_LOG_CRITICAL(hera::global_log, fmt, ##__VA_ARGS__)

// value-based macros

#define LOGV_TRACE_L3(fmt, message, ...)                                       \
    QUILL_LOGV_TRACE_L3(hera::global_log, fmt, message, ##__VA_ARGS__)
#define LOGV_TRACE_L2(fmt, message, ...)                                       \
    QUILL_LOGV_TRACE_L2(hera::global_log, fmt, message, ##__VA_ARGS__)
#define LOGV_TRACE_L1(fmt, message, ...)                                       \
    QUILL_LOGV_TRACE_L1(hera::global_log, fmt, message, ##__VA_ARGS__)
#define LOGV_DEBUG(fmt, message, ...)                                          \
    QUILL_LOGV_DEBUG(hera::global_log, fmt, message, ##__VA_ARGS__)
#define LOGV_INFO(fmt, message, ...)                                           \
    QUILL_LOGV_INFO(hera::global_log, fmt, message, ##__VA_ARGS__)
#define LOGV_WARNING(fmt, message, ...)                                        \
    QUILL_LOGV_WARNING(hera::global_log, fmt, message, ##__VA_ARGS__)
#define LOGV_ERROR(fmt, message, ...)                                          \
    QUILL_LOGV_ERROR(hera::global_log, fmt, message, ##__VA_ARGS__)
#define LOGV_CRITICAL(fmt, message, ...)                                       \
    QUILL_LOGV_CRITICAL(hera::global_log, fmt, message, ##__VA_ARGS__)

#endif
