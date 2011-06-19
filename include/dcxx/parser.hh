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

#ifndef DCXX_PARSER_HH
#define DCXX_PARSER_HH

#include <dcxx/utils.hh>
#include <dcxx/types.hh>
#include <libdivecomputer/parser.h>

#include <vector>

#define DCXX_PARSER_TRY(call)			\
    do {					\
	parser_status_t status = call;		\
	if (status != PARSER_STATUS_SUCCESS)	\
	    throw ParserException(status);	\
    } while(0)

DCXX_BEGIN_NS_DC

class ParserException {
public:
    ParserException(parser_status_t status);

    const char *what() const throw();
    parser_status_t getStatus() const throw();

private:
    const parser_status_t status;
};

class Parser;

class ParserCallbacks {
public:
    friend class Parser;

    ParserCallbacks();
    virtual ~ParserCallbacks();

    virtual void onSample(Parser &parser,
			  parser_sample_type_t type, parser_sample_value_t value);

    virtual void onBeginSample() {};
    virtual void onEndSample() {};

    virtual void onTime(Duration time) {}
    virtual void onDepth(Length depth) {}
    virtual void onPressure(unsigned int tank, double value) {}
    virtual void onTemperature(Temperature temp) {}
    virtual void onEvent(parser_sample_event_t type, Duration time,
			 unsigned int flags, unsigned int value) {}
    virtual void onRBT(unsigned int rbt) {}
    virtual void onHeartBeat(unsigned int heartbeat) {}
    virtual void onBearing(unsigned int bearing) {}
    virtual void onVendor(unsigned int type,
			  unsigned int size, const void *data) {}

protected:
    void beginSample();
    void terminateSample();

    bool inSample;
};

class Parser {
public:
    typedef std::vector<gasmix_t> GasMixVector;

    virtual ~Parser() throw(ParserException);

    void setCallbackHandler(ParserCallbacks *handler);

    parser_type_t getType();

    void setData(const void *data, unsigned int size) throw(ParserException);

    void forEachSample() throw(ParserException);

    Duration getDiveTime() throw(ParserException);
    Length getMaxDepth() throw(ParserException);
    GasMixVector &getGasMixes(GasMixVector &mixes) throw(ParserException);
    GasMixVector &getGasMixes() throw(ParserException) {
	GasMixVector mixes;
	return getGasMixes(mixes);
    }
    

protected:
    Parser(parser_t *parser);
    Parser();

    void init() throw(ParserException);
    void getField(parser_field_type_t type, unsigned int flags, void *value)
	throw(ParserException);

    template<typename T>
    T getField(parser_field_type_t type, unsigned int flags)
	throw(ParserException) {
	T val;
	getField(type, flags, &val);
	return val;
    }

    parser_t *parser;

private:
    static void sampleCallback(parser_sample_type_t type,
			       parser_sample_value_t value,
			       void *userdata);

    ParserCallbacks *callbacks;
    const void *data;
    unsigned int size;
};

DCXX_END_NS

#endif
