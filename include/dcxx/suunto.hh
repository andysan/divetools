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
#include <dcxx/parser.hh>
#include <dcxx/types.hh>

#define DCXX_BEGIN_NS_SUUNTO DCXX_BEGIN_NS(suunto)

DCXX_BEGIN_NS_DC
DCXX_BEGIN_NS_SUUNTO

class Vyper
    : public Device
{
public:
    Vyper(const char *name) throw(DeviceException);

    enum HardwareType {
	HW_STINGER = 0x03,
	HW_MOSQUITO = 0x04,
	HW_VYPER_NEW = 0x0A,
	HW_VYTEC = 0x0B,
	HW_VYPER_OR_COBRA = 0x0C,
	HW_GEKKO = 0x0D,
    };

    Length getMaxDepth();
    Duration getTotalDiveTime();
    unsigned int getTotalDives();
    unsigned int getSerial();
    std::string getPersonalInfo();

    HardwareType getHWType();
    static std::string getHWTypeName(HardwareType type);
    std::string getHWTypeName();
    uint8_t getFWVersion();
    uint8_t getLogInterval();

    Duration getAlarmTime();
    bool getAlarmTimeOn();
    Length getAlarmDepth();
    bool getAlarmDepthOn();

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
	uint16_t unknown0;
	char personal[30];
	char unknown1[7];
	uint16_t ptrRingBuff;
	uint8_t logInterval;
	uint8_t modelModifiers;
	char unknown2[7];
	uint16_t maxFreedivingDepth;
	uint16_t totalFreedivingTime;
	uint8_t timeFlags;
	uint8_t unknown3;
	uint8_t displayUnits;
	uint8_t model;
	uint8_t lightSettings;
	uint8_t alarmFlags;
	uint16_t alarmTime;
	uint16_t alarmDepth;
	char unknown4[7];
    } __attribute__((packed));

    const Info &getInfo();

    Info infoCache;
    bool infoCacheValid;
};

class VyperParser
    : public Parser
{
public:
    VyperParser();
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
