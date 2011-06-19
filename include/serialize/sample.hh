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

#ifndef SERIALIZE_SAMPLE_HH
#define SERIALIZE_SAMPLE_HH

#include "dcxx/parser.hh"
#include "valid_value.hh"

struct Sample {
    Sample() {}
    Sample(dcxx::Duration _time)
	: time(_time) {}

    ValidValue<dcxx::Duration> time;
    ValidValue<dcxx::Length> depth;
    ValidValue<dcxx::Temperature> temperature;
};

class SampleBuilder
    : public dcxx::ParserCallbacks
{
public:
    SampleBuilder();
    virtual ~SampleBuilder();

    virtual void onSample(const Sample &sample) = 0;


    void onBeginSample();
    void onEndSample();
    void onTime(dcxx::Duration time);
    void onDepth(dcxx::Length depth);
    void onTemperature(dcxx::Temperature temp);

private:
    Sample sample;
};

#endif
