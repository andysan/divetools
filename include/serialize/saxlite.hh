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

#ifndef _SERIALIZE_SAXLITE_HH
#define _SERIALIZE_SAXLITE_HH

#include <string>
#include <ostream>
#include <stack>
#include <ctime>
#include <boost/lexical_cast.hpp>

#define BEGIN_SAXLITE_NS namespace xml {
#define END_SAXLITE_NS }

BEGIN_SAXLITE_NS

class ContentHandler {
public:
    virtual ~ContentHandler() {};

    template<typename T>
    void simpleTextElement(const char *name, const T &value) {
	startElement(name);
	text(value);
	endElement();
    }

    virtual void startElement(const char *name) = 0;
    virtual void endElement() = 0;

    virtual void startDocument() = 0;
    virtual void endDocument() = 0;

    virtual void attribute(const char *name, const char *value) = 0;
    void attribute(const char *name, const std::string &value) {
	attribute(name, value.c_str());
    }
    template<typename T>
    void attribute(const char *name, const T &value) {
	attribute(name, boost::lexical_cast<std::string>(value));
    }

    virtual void text(const char *value) = 0;
    void text(const std::string &value) {
	text(value.c_str());
    }
    void text(const time_t &time);
    template<typename T>
    void text(const T &value) {
	text(boost::lexical_cast<std::string>(value));
    }
};

class StreamSerializer
    : public ContentHandler
{
public:
    StreamSerializer(std::ostream &out);
    ~StreamSerializer();

    void startElement(const char *name);
    void endElement();

    void startDocument();
    void endDocument();

    void attribute(const char *name, const char *value);
    void text(const char *text);

private:
    struct ElementState {
	ElementState(const char *_name)
	    : name(_name), hasText(false), hasChildElements(false) {}

	bool open() { return !hasText && !hasChildElements; }

	std::string name;
	bool hasText;
	bool hasChildElements;
    };

    std::stack<ElementState> elementStack;
    std::ostream &out;
    bool seenStartDocument;
    bool seenEndDocument;
};

class ScopedElement {
public:
    ScopedElement(ContentHandler &_handler, const char *_name)
	: handler(_handler) {
	handler.startElement(_name);
    }

    ~ScopedElement() {
	handler.endElement();
    }

private:
    ContentHandler &handler;
};

END_SAXLITE_NS

#endif
