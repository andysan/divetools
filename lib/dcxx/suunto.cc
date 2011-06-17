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

#include <dcxx/utils.hh>
#include <dcxx/suunto.hh>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef WORDS_BIGENDIAN
#define SUUNTO_SWAP16(v) (v)
#define SUUNTO_SWAP32(v) (v)
#else
#define SUUNTO_SWAP16(v) ((((v) << 8) & 0xFF00) | (((v) >> 8) & 0xFF))
#define SUUNTO_SWAP32(v) (SUUNTO_SWAP16((v) >> 16) | (SUUNTO_SWAP16(v) << 16))
#endif

DCXX_BEGIN_NS_DC
DCXX_BEGIN_NS_SUUNTO


Vyper::Vyper(const char *name) throw(DeviceException)
    : Device(), infoCacheValid(false)
{
    DCXX_DEVICE_TRY(suunto_vyper_device_open(&device, name));
    init();
}


Length
Vyper::getMaxDepth()
{
    return Length::feet(SUUNTO_SWAP16(getInfo().maxDepth) / 128.0);
}

Duration
Vyper::getTotalDiveTime()
{
    return Duration::minutes(SUUNTO_SWAP16(getInfo().totalDiveTime));
}

unsigned int
Vyper::getTotalDives()
{
    return SUUNTO_SWAP16(getInfo().totalDives);
}

unsigned int
Vyper::getSerial()
{
    return SUUNTO_SWAP32(getInfo().serial);
}

std::string
Vyper::getPersonalInfo()
{
    const Info &info(getInfo());
    return std::string(info.personal, sizeof(info.personal));
}

const Vyper::Info &
Vyper::getInfo()
{
    if (infoCacheValid)
	return infoCache;

    read(0x1E, &infoCache, sizeof(infoCache));
    infoCacheValid = true;

    return infoCache;
}

void
Vyper::setDelay(unsigned int delay) throw(DeviceException)
{
    DCXX_DEVICE_TRY(suunto_vyper_device_set_delay(device, delay));
}

void
Vyper::readDive(dc_buffer_t *buffer, int init) throw(DeviceException)
{
    DCXX_DEVICE_TRY(suunto_vyper_device_read_dive(device, buffer, init));
}



Vyper2::Vyper2(const char *name) throw(DeviceException)
    : Device()
{
    DCXX_DEVICE_TRY(suunto_vyper2_device_open(&device, name));
    init();
}

void
Vyper2::reset_maxdepth() throw(DeviceException)
{
    DCXX_DEVICE_TRY(suunto_vyper2_device_reset_maxdepth(device));
}


DCXX_END_NS
DCXX_END_NS
