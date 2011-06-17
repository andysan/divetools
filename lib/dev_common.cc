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

#include "dev_common.hh"
#include "dcxx/suunto.hh"

#include <boost/foreach.hpp>

const DeviceInfo devDevices[] = {
    { DEVICE_TYPE_SUUNTO_SOLUTION, PARSER_TYPE_SUUNTO_SOLUTION,
      "suunto-solution" },
    { DEVICE_TYPE_SUUNTO_EON, PARSER_TYPE_SUUNTO_EON,
      "suunto-eon" },
    { DEVICE_TYPE_SUUNTO_VYPER, PARSER_TYPE_SUUNTO_VYPER,
      "suunto-vyper" },
    { DEVICE_TYPE_SUUNTO_VYPER2, PARSER_TYPE_NULL,
      "suunto-vyper2" },
    { DEVICE_TYPE_SUUNTO_D9, PARSER_TYPE_SUUNTO_D9,
      "suunto-d9" },

    { DEVICE_TYPE_REEFNET_SENSUS, PARSER_TYPE_REEFNET_SENSUS,
      "reefnet-sensus" },
    { DEVICE_TYPE_REEFNET_SENSUSPRO, PARSER_TYPE_REEFNET_SENSUSPRO,
      "reefnet-sensuspro" },
    { DEVICE_TYPE_REEFNET_SENSUSULTRA, PARSER_TYPE_REEFNET_SENSUSULTRA,
      "reefnet-sensusultra" },

    { DEVICE_TYPE_UWATEC_ALADIN, PARSER_TYPE_NULL,
      "uwatec-aladin" },
    { DEVICE_TYPE_UWATEC_MEMOMOUSE, PARSER_TYPE_UWATEC_MEMOMOUSE,
      "uwatec-memomouse" },
    { DEVICE_TYPE_UWATEC_SMART, PARSER_TYPE_UWATEC_SMART,
      "uwatec-smart" },

    { DEVICE_TYPE_OCEANIC_ATOM2, PARSER_TYPE_OCEANIC_ATOM2,
      "oceanic-atom2" },
    { DEVICE_TYPE_OCEANIC_VEO250, PARSER_TYPE_OCEANIC_VEO250,
      "oceanic-veo250" },
    { DEVICE_TYPE_OCEANIC_VTPRO, PARSER_TYPE_OCEANIC_VTPRO,
      "oceanic-vtpro" },

    { DEVICE_TYPE_MARES_NEMO, PARSER_TYPE_MARES_NEMO,
      "mares-nemo" },
    { DEVICE_TYPE_MARES_PUCK, PARSER_TYPE_NULL,
      "mares-puck" },
    { DEVICE_TYPE_MARES_ICONHD, PARSER_TYPE_MARES_ICONHD,
      "mares-iconhd" },

    { DEVICE_TYPE_HW_OSTC, PARSER_TYPE_HW_OSTC,
      "hw-ostc" },

    { DEVICE_TYPE_CRESSI_EDY, PARSER_TYPE_CRESSI_EDY,
      "cressi-edy" },

    { DEVICE_TYPE_ZEAGLE_N2ITION3, PARSER_TYPE_NULL,
      "zeagle-n2ition3" },

    { DEVICE_TYPE_ATOMICS_COBALT, PARSER_TYPE_ATOMICS_COBALT,
      "atomics-cobalt" },
};

const DeviceInfo *
getDeviceInfo(const char *devName)
{
    BOOST_FOREACH(const DeviceInfo &dev, devDevices) {
	if (strcmp(devName, dev.name) == 0)
	    return &dev;
    }
    return NULL;
}

device_type_t
getDevType(const char *devName)
{
    const DeviceInfo *devInfo(getDeviceInfo(devName));
    return devInfo ? devInfo->device : DEVICE_TYPE_NULL;
}

parser_type_t
getParserType(const char *devName)
{
    const DeviceInfo *devInfo(getDeviceInfo(devName));
    return devInfo ? devInfo->parser : PARSER_TYPE_NULL;
}

dcxx::Device *
devCreate(device_type_t type, const char *port)
{
    switch (type) {
    case DEVICE_TYPE_SUUNTO_VYPER:
	return new dcxx::suunto::Vyper(port);
    case DEVICE_TYPE_SUUNTO_VYPER2:
	return new dcxx::suunto::Vyper2(port);
    default:
	return NULL;
    }
}

dcxx::Parser *
parserCreate(parser_type_t type)
{
    switch (type) {
    case PARSER_TYPE_SUUNTO_VYPER:
	return new dcxx::suunto::VyperParser();
    default:
	return NULL;
    }
}
