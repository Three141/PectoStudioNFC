#include "All.h"
#include "Main.h"

Error::Error(ErrorType et, int code)
{
	this->et = et;
	this->code = code;
	App::ins->lastError = toString();
}

const char* Error::what()
{
	return NULL;
}

string Error::toString()
{
	ostringstream oss;
	string out = "";
	switch (et)
	{
	case Error::HTTP_REQ_ERROR:
		oss << code;
		out = string("HTTP: Req failed - #"+oss.str());
		break;
	case Error::DW_ERROR_CREATE_TEXT_FORMAT:
		out = string("DirectWrite: Unable to create text format");
		break;
	default:
		out = string("Generic Error");
		break;
	}
	return out;
}

const Error::ErrorType Error::GetErrorType()
{
	return et;
}

const int Error::GetNumCode()
{
	return code;
}