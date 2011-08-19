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

#include "serialize/saxlite.hh"

#include <sstream>
#include <iomanip>
#include <cassert>

BEGIN_SAXLITE_NS

using namespace std;

void
ContentHandler::text(const time_t &time)
{
    struct tm tm;
    stringstream ss;

    gmtime_r(&time, &tm);

    ss.fill('0');
    ss << 1900 + tm.tm_year << "-"
       << setw(2) << 1 + tm.tm_mon << "-"
       << setw(2) << tm.tm_mday << "T"
       << setw(2) << tm.tm_hour << ":"
       << setw(2) << tm.tm_min << ":"
       << setw(2) << tm.tm_sec << "Z";
    text(ss.str());
}

StreamSerializer::StreamSerializer(ostream &_out)
    : ContentHandler(),
      out(_out),
      seenStartDocument(false),
      seenEndDocument(false)
{
}

StreamSerializer::~StreamSerializer()
{
}

void
StreamSerializer::startElement(const char *name)
{
    assert(seenStartDocument);
    assert(!seenEndDocument);

    if (!elementStack.empty()) {
	ElementState &e(elementStack.top());
	if (e.open())
	    out << ">" << endl;

	e.hasChildElements = true;

	if (!e.hasText) {
	    for (int i = 0; i < elementStack.size(); i++)
		out << "  ";
	}
    }

    elementStack.push(ElementState(name));
    out << "<" << name;
}

void
StreamSerializer::endElement()
{
    assert(!elementStack.empty());

    ElementState &e(elementStack.top());

    if (e.open())
	out << "/>" << endl;
    else {
	if (!e.hasText) {
	    for (int i = 0; i < elementStack.size() - 1; i++)
		out << "  ";
	}
	out << "</" << e.name << ">" << endl;
    }

    elementStack.pop();
}

void
StreamSerializer::startDocument()
{
    assert(!seenStartDocument);
    assert(!seenEndDocument);

    seenStartDocument = true;
    out << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << endl
	<< endl;
}

void
StreamSerializer::endDocument()
{
    assert(seenStartDocument);
    assert(!seenEndDocument);
    assert(elementStack.empty());

    seenEndDocument = true;
}

void
StreamSerializer::attribute(const char *name, const char *value)
{
    assert(!elementStack.empty());
    ElementState &e(elementStack.top());
    assert(e.open());

    out << " " << name << "=\"" << value << "\"";
}

void
StreamSerializer::text(const char *text)
{
    assert(!elementStack.empty());
    ElementState &e(elementStack.top());

    if (e.open())
	out << ">" << text;
    else
	out << text;

    e.hasText = true;
}

END_SAXLITE_NS
