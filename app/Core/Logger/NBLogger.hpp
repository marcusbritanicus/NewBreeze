/*
	*
	* NBLogger.hpp - NBLogger.cpp header
	*
*/

#pragma once

#include "Global.hpp"

namespace DbgMsgPart {
	enum MsgPart {
		HEAD,
		BODY,
		TAIL,
		ONESHOT
	};
};

#if QT_VERSION >= 0x050000
	void NBMessageOutput5( QtMsgType, const QMessageLogContext&, const QString& );
#else
	void NBMessageOutput( QtMsgType, const char* );
#endif
