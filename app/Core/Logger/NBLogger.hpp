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

void NBMessageOutput( QtMsgType, const QMessageLogContext&, const QString& );
