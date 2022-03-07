/**
 * NBLogger.cpp - Some tools for NewBreeze
 **/

#include <NBLogger.hpp>

FILE *NewBreeze::log = nullptr;
bool NewBreeze::SHOW_INFO_ON_CONSOLE     = true;
bool NewBreeze::SHOW_DEBUG_ON_CONSOLE    = true;
bool NewBreeze::SHOW_WARNING_ON_CONSOLE  = true;
bool NewBreeze::SHOW_CRITICAL_ON_CONSOLE = true;
bool NewBreeze::SHOW_FATAL_ON_CONSOLE    = true;

static const char *COLOR_INFO     = "\033[01;32m";
static const char *COLOR_DEBUG    = "\033[01;30m";
static const char *COLOR_WARN     = "\033[01;33m";
static const char *COLOR_CRITICAL = "\033[01;31m";
static const char *COLOR_FATAL    = "\033[01;41m";
static const char *COLOR_RESET    = "\033[00;00m";

void NewBreeze::Logger( QtMsgType type, const QMessageLogContext& context, const QString& msg ) {
    QByteArray localMsg  = msg.toLocal8Bit();
    const char *file     = context.file ? context.file : "";
    const char *function = context.function ? context.function : "";

    switch ( type ) {
        case QtInfoMsg: {
            if ( NewBreeze::SHOW_INFO_ON_CONSOLE ) {
                fprintf( stderr, "%s[I]: (%s:%u, %s) %s%s\n",
                         COLOR_INFO,
                         file,
                         context.line,
                         function,
                         localMsg.constData(),
                         COLOR_RESET
                );
                fflush( stderr );
            }

            if ( NewBreeze::log ) {
                fprintf( NewBreeze::log, "[I]: (%s:%u, %s) %s\n", file, context.line, function, localMsg.constData() );
                fflush( NewBreeze::log );
            }

            break;
        }

        case QtDebugMsg: {
            if ( NewBreeze::SHOW_DEBUG_ON_CONSOLE ) {
                fprintf( stderr, "%s[D]: (%s:%u, %s) %s%s\n",
                         COLOR_DEBUG,
                         file,
                         context.line,
                         function,
                         localMsg.constData(),
                         COLOR_RESET
                );
                fflush( stderr );
            }

            if ( NewBreeze::log ) {
                fprintf( NewBreeze::log, "[D]: (%s:%u, %s) %s\n", file, context.line, function, localMsg.constData() );
                fflush( NewBreeze::log );
            }

            break;
        }

        case QtWarningMsg: {
            if ( NewBreeze::SHOW_WARNING_ON_CONSOLE ) {
                fprintf( stderr, "%s[W]: (%s:%u, %s) %s%s\n",
                         COLOR_WARN,
                         file,
                         context.line,
                         function,
                         localMsg.constData(),
                         COLOR_RESET
                );
                fflush( stderr );
            }

            if ( NewBreeze::log ) {
                fprintf( NewBreeze::log, "[W]: (%s:%u, %s) %s\n", file, context.line, function, localMsg.constData() );
                fflush( NewBreeze::log );
            }

            break;
        }

        case QtCriticalMsg: {
            if ( NewBreeze::SHOW_CRITICAL_ON_CONSOLE ) {
                fprintf( stderr, "%s[E]: (%s:%u, %s) %s%s\n",
                         COLOR_CRITICAL,
                         file,
                         context.line,
                         function,
                         localMsg.constData(),
                         COLOR_RESET
                );
                fflush( stderr );
            }

            if ( NewBreeze::log ) {
                fprintf( NewBreeze::log, "[E]: (%s:%u, %s) %s\n", file, context.line, function, localMsg.constData() );
                fflush( NewBreeze::log );
            }

            break;
        }

        case QtFatalMsg: {
            if ( NewBreeze::SHOW_FATAL_ON_CONSOLE ) {
                fprintf( stderr, "%s[#]: (%s:%u, %s) %s%s\n",
                         COLOR_FATAL,
                         file,
                         context.line,
                         function,
                         localMsg.constData(),
                         COLOR_RESET
                );
                fflush( stderr );
            }

            if ( NewBreeze::log ) {
                fprintf( NewBreeze::log, "[#]: (%s:%u, %s) %s\n", file, context.line, function, localMsg.constData() );
                fflush( NewBreeze::log );
            }

            break;
        }
    }
}
