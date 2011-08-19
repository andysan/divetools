/*
 * Copyright (c) 2011, Andreas Sandberg
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <dcxx/parser.hh>
#include <dcxx/utils.hh>

#include <stdlib.h>
#include <cstring>

DCXX_BEGIN_NS_DC

ParserException::ParserException(parser_status_t status)
    : status(status)
{
}

const char *
ParserException::what() const throw()
{
    switch (status) {
    case PARSER_STATUS_SUCCESS:
	return "No Error";

    case PARSER_STATUS_UNSUPPORTED:
	return "Unsupported feature";

    case PARSER_STATUS_TYPE_MISMATCH:
	return "Type mismatch";

    case PARSER_STATUS_MEMORY:
	return "Memory allocation failed";

    case PARSER_STATUS_ERROR:
    default:
	return "An unknown error occured";
    }

}

parser_status_t
ParserException::getStatus() const throw()
{
    return status;
}


ParserCallbacks::ParserCallbacks()
    : inSample(false)
{
}

ParserCallbacks::~ParserCallbacks()
{
}

void
ParserCallbacks::onSample(Parser &parser,
			  parser_sample_type_t type,
			  parser_sample_value_t value)
{
    switch (type) {
    case SAMPLE_TYPE_TIME:
	terminateSample();
	beginSample();
	onTime(Duration::seconds(value.time));
	break;

    case SAMPLE_TYPE_DEPTH:
	onDepth(Length::metre(value.depth));
	break;

    case SAMPLE_TYPE_PRESSURE:
	onPressure(value.pressure.tank, value.pressure.value);
	break;

    case SAMPLE_TYPE_TEMPERATURE:
	onTemperature(Temperature::celsius(value.temperature));
	break;

    case SAMPLE_TYPE_EVENT:
	onEvent((parser_sample_event_t)value.event.type,
		Duration::seconds(value.event.time),
		value.event.flags, value.event.value);
	break;

    case SAMPLE_TYPE_RBT:
	onRBT(value.rbt);
	break;

    case SAMPLE_TYPE_HEARTBEAT:
	onHeartBeat(value.heartbeat);
	break;

    case SAMPLE_TYPE_BEARING:
	onBearing(value.bearing);
	break;

    case SAMPLE_TYPE_VENDOR:
	onVendor(value.vendor.type, value.vendor.size, value.vendor.data);
	break;
    }
}

void
ParserCallbacks::beginSample()
{
    inSample = true;
    onBeginSample();
}

void
ParserCallbacks::terminateSample()
{
    if (inSample) {
	onEndSample();
	inSample = false;
    }
}

Parser::Parser(parser_t *parser)
    : parser(parser), callbacks(NULL)
{
    init();
}

Parser::Parser()
    : parser(NULL)
{
}

Parser::~Parser() throw(ParserException)
{
    if (parser)
	DCXX_PARSER_TRY(parser_destroy(parser));
}

void
Parser::init() throw(ParserException)
{
}

void
Parser::setData(const void *data, unsigned int size) throw(ParserException)
{
    DCXX_PARSER_TRY(parser_set_data(parser, (const unsigned char *)data, size));
}

void
Parser::setCallbackHandler(ParserCallbacks *handler)
{
    callbacks = handler;
}

parser_type_t
Parser::getType()
{
    return parser_get_type(parser);
}

void
Parser::forEachSample() throw(ParserException)
{
    DCXX_PARSER_TRY(
	parser_samples_foreach(parser, &Parser::sampleCallback, (void *)this));
    callbacks->terminateSample();
}

Duration
Parser::getDiveTime() throw(ParserException)
{
    return Duration::seconds(getField<unsigned int>(FIELD_TYPE_DIVETIME, 0));
}

Length
Parser::getMaxDepth() throw(ParserException)
{
    return Length::metre(getField<double>(FIELD_TYPE_MAXDEPTH, 0));
}

Parser::GasMixVector &
Parser::getGasMixes(GasMixVector &mixes) throw(ParserException)
{
    unsigned int count = getField<unsigned int>(FIELD_TYPE_GASMIX_COUNT, 0);
    mixes.resize(count);
    for (unsigned int i = 0; i < count; i++)
	getField(FIELD_TYPE_GASMIX, i, &mixes[i]);
}

time_t
Parser::getDateTime() throw(ParserException)
{
    dc_datetime_t dt;
    struct tm tm;

    DCXX_PARSER_TRY(parser_get_datetime(parser, &dt));

    memset(&tm, 0, sizeof(tm));
    tm.tm_year = dt.year - 1900;
    tm.tm_mon = dt.month - 1;
    tm.tm_mday = dt.day;
    tm.tm_hour = dt.hour;
    tm.tm_min = dt.minute;
    tm.tm_sec = dt.second;
    tm.tm_isdst = -1;

    return mktime(&tm);
}

void
Parser::getField(parser_field_type_t type, unsigned int flags, void *value)
    throw(ParserException)
{
    DCXX_PARSER_TRY(parser_get_field(parser, type, flags, value));
}

void
Parser::sampleCallback(parser_sample_type_t type,
		       parser_sample_value_t value,
		       void *userdata)
{
    Parser *_this = static_cast<Parser *>(userdata);

    if (_this->callbacks)
	_this->callbacks->onSample(*_this, type, value);
}

DCXX_END_NS
