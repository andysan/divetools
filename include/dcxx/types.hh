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

#ifndef DCXX_TYPES_HH
#define DCXX_TYPES_HH

#include <cmath>
#include <dcxx/utils.hh>

DCXX_BEGIN_NS_DC

/** One foot in metre */
#define DCXX_FOOT 0.3048

struct Duration {
    Duration(double val)
	: val(val) {}

    static Duration seconds(double val) {
	return Duration(val);
    }

    double seconds() const {
	return val;
    }

    static Duration minutes(double val) {
	return Duration(val * 60);
    }

    double minutes() const {
	return val / 60;
    }

    static Duration hours(double val) {
	return Duration(val * 60 * 60);
    }

    double hours() const {
	return val / 60 / 60;
    }

    operator double() { return val; }

    /** Duration in seconds */
    double val;
};

struct Length {
    Length(double val)
	: val(val) {}

    static Length metre(double val) {
	return Length(val);
    }

    double metre() const {
	return val;
    }

    static Length feet(double val) {
	return Length(DCXX_FOOT * val);
    }

    double feet() const {
	return val / DCXX_FOOT;
    }

    operator double() { return val; }

    /** Length, always in metre */
    double val;
};

DCXX_END_NS

#endif
