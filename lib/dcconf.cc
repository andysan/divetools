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

#include "dcconf.hh"

#include <iostream>

#include "dev_common.hh"

namespace po = boost::program_options;
namespace bfs = boost::filesystem;

using namespace std;

DCConf::DCConf()
    : optsCommon(), cfgCommon(),
      devPort(), devType(DEVICE_TYPE_NULL), devSerial(0), devSerialValid(false)
{
    po::options_description optsDevice("Device");
    optsDevice.add_options()
	("dev-type", po::value<string>(),
	 "set device type")
	("dev-port", po::value<string>(),
	 "set device port")
	;

    optsCommon.add(optsDevice);


    po::options_description cfgDev("device");
    cfgDev.add_options()
	("device.type", po::value<string>())
	("device.port", po::value<string>())
	("device.serial", po::value<unsigned int>())
	;
    cfgCommon.add(cfgDev);
}

DCConf::~DCConf()
{
}

void
DCConf::handleConf(po::variables_map vm)
{
    if (vm.count("device.type")) {
	device_type_t confDevType =
	    getDevType(vm["device.type"].as<string>().c_str());
	if (confDevType == DEVICE_TYPE_NULL) {
	    cerr << "Error: Unknown device type in configuration" << endl;
	    exit(EXIT_FAILURE);
	}

	if (devType == DEVICE_TYPE_NULL)
	    devType = confDevType;
    }
    if (devPort.empty() && vm.count("device.port"))
	devPort = vm["device.port"].as<string>();

    if (!devSerialValid && vm.count("device.serial"))
	setDevSerial(vm["device.serial"].as<unsigned int>());
}

void
DCConf::handleArgs(po::variables_map vm)
{
    if (vm.count("dev-port"))
	devPort = vm["dev-port"].as<string>();

    if (vm.count("dev-type")) {
	devType = (getDevType(vm["dev-type"].as<string>().c_str()));
	if (devType == DEVICE_TYPE_NULL) {
	    cerr << "Error: Unknown device type specified" << endl;
	    exit(EXIT_FAILURE);
	}
    }
}

ostream &
operator<<(ostream &out, const DCConf &conf)
{
    const char *devName = getDevName(conf.devType);
    assert(devName);
    out << "[device]" << endl
	<< "type = " << devName << endl
	<< "port = " << conf.devPort << endl
	<< "serial = " << conf.devSerial;
}
