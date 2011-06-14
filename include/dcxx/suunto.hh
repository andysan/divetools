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

#ifndef DCXX_SUUNTO_HH
#define DCXX_SUUNTO_HH

#include <memory>
#include <string>
#include <stdint.h>

#include <libdivecomputer/suunto.h>

#include <dcxx/utils.hh>
#include <dcxx/device.hh>
#include <dcxx/types.hh>

#define DCXX_BEGIN_NS_SUUNTO DCXX_BEGIN_NS(suunto)

DCXX_BEGIN_NS_DC
DCXX_BEGIN_NS_SUUNTO

class Vyper
    : public Device
{
public:
    Vyper(const char *name) throw(DeviceException);

    Length getMaxDepth();
    Duration getTotalDiveTime();
    unsigned int getTotalDives();
    unsigned int getSerial();
    std::string getPersonalInfo();

    void setDelay(unsigned int delay) throw(DeviceException);
    void readDive(dc_buffer_t *buffer, int init) throw(DeviceException);

private:
    struct Info {
	uint16_t maxDepth;
	uint16_t totalDiveTime;
	uint16_t totalDives;
	uint8_t hwType;
	uint8_t fwVersion;
	uint32_t serial;
	uint16_t unknown;
	char personal[30];
    } __attribute__((packed));

    const Info &getInfo();

    Info infoCache;
    bool infoCacheValid;
};

class Vyper2
    : public Device
{
public:
    Vyper2(const char *name) throw(DeviceException);

    void reset_maxdepth() throw(DeviceException);
};

DCXX_END_NS
DCXX_END_NS

#endif
