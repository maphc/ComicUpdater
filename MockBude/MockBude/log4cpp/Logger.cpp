#include "stdafx.h"
#include "Logger.h"

using namespace log4cpp;

Logger* Logger::_inst=NULL;

//[gboss.crm.CustServ][d:\workspace\visualc\mockbude\mockbude\mockbude.cpp(25)]:ohter
const char* Logger::fmt="[%s][%s(%d)]:%s\n";