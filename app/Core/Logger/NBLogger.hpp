/**
 * NBLogger.hpp - NBLogger.cpp header
 **/

#pragma once

#include "Global.hpp"

namespace DbgMsgPart {
    enum MsgPart {
        HEAD,
        BODY,
        TAIL,
        ONESHOT
    };
}

void NBMessageOutput( QtMsgType, const QMessageLogContext&, const QString& );

namespace NewBreeze {
    extern FILE *log;
    extern bool SHOW_INFO_ON_CONSOLE;
    extern bool SHOW_DEBUG_ON_CONSOLE;
    extern bool SHOW_WARNING_ON_CONSOLE;
    extern bool SHOW_CRITICAL_ON_CONSOLE;
    extern bool SHOW_FATAL_ON_CONSOLE;

    void Logger( QtMsgType type, const QMessageLogContext& context, const QString& message );
}
