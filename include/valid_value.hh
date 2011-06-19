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

#ifndef VALID_VALUE_HH
#define VALID_VALUE_HH

struct InvalidValueException {};

template <typename T>
class ValidValue
{
public:
    ValidValue()
	: valid(false) {}

    ValidValue(const T &v)
	: value(v), valid(true){}

    ValidValue(const ValidValue<T> &that)
	: valid(that.valid), value(that.value) {}

    void set(const T &v) {
	value = v;
	valid = true;
    }

    T get() const {
	if (!valid)
	    throw InvalidValueException();
	return value;
    }

    operator bool() const {
	return valid;
    }

    ValidValue<T> &operator=(const ValidValue<T> &rhs) {
	if (this == &rhs)
	    return *this;

	value = rhs.value;
	valid = rhs.valid;
    }

    ValidValue<T> &operator=(const T &rhs) {
	set(rhs);
    }

    bool operator==(const ValidValue<T> &rhs) const {
	return rhs.valid == valid &&
	    value == rhs.value;
    }

    bool operator==(const T &rhs) const {
	return valid && value == rhs.value;
    }

private:
    T value;
    bool valid;
};

#endif
