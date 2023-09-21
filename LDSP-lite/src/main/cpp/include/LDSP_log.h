#pragma once

#include <android/log.h>

#ifndef NDEBUG
#define LDSP_log(args...) \
__android_log_print(android_LogPriority::ANDROID_LOG_DEBUG, "LDSP-lite", args)
#else
#define LDSP_log(args...)
#endif
