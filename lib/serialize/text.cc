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

#include "serialize/text.hh"

#include <iostream>

using namespace std;
using namespace dcxx;

SerializeText::SerializeText(ostream &_out)
    : ParserCallbacks(),
      out(_out), separator(',')
{
}

SerializeText::~SerializeText()
{
}

void
SerializeText::onBeginSample()
{
    out << "Begin Sample" << endl;
}

void
SerializeText::onEndSample()
{
    out << "End Sample" << endl;
}

void
SerializeText::onTime(Duration time)
{
    out << "  Time: " << time << endl;
}

void
SerializeText::onDepth(Length depth)
{
    out << "  Depth: " << depth << endl;
}

void
SerializeText::onPressure(unsigned int tank, double value)
{
    out << "  Pressure [" << tank << "]: " << value << endl;
}

void
SerializeText::onTemperature(Temperature temp)
{
    out << "  Temperature: " << temp << endl;
}

void
SerializeText::onEvent(parser_sample_event_t type, Duration time,
		       unsigned int flags, unsigned int value)
{
    out << "  Event: [ type: " << type << " duration: " << time
	<< " flags: " << flags << " value: " << value << endl;
}

void
SerializeText::onRBT(unsigned int rbt)
{
    out << "  RBT: " << rbt << endl;
}

void
SerializeText::onHeartBeat(unsigned int heartbeat)
{
    out << "  Heart Beat: " << heartbeat << endl;
}

void
SerializeText::onBearing(unsigned int bearing)
{
    out << "  Bearing: " << bearing << endl;
}

void
SerializeText::onVendor(unsigned int type,
			unsigned int size, const void *data)
{
    out << "  Vendor specific data" << endl;
}
