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

#include <iostream>
#include <memory>

#include <boost/program_options.hpp>

#include "dcxx/suunto.hh"

using namespace std;
using namespace dcxx;

namespace po = boost::program_options;

#define VYPER_CONFIG_OFFSET 0x1E
#define VYPER_CONFIG_LEN 0x53

#define VYPER_MEM_SIZE 0x2000

enum Mode {
    MODE_INFO,
    MODE_RAW_CONFIG,
    MODE_RAW_MEMORY,
};

/* Configuration options */
static std::string devPort;
static Mode optMode = MODE_INFO;

static void
parse_args(int argc, char **argv)
{
    po::options_description optsGeneral("General options");
    optsGeneral.add_options()
	("help", "produce help message")
	("raw-config", "dump raw configuration")
	("raw-memory", "dump dive computer memory")
	;

    po::options_description optsHidden("Hidden");
    optsHidden.add_options()
	("port", po::value<string>(), "");

    po::options_description optsVisible;
    optsVisible.add(optsGeneral);

    po::options_description optsAll;
    optsAll.add(optsVisible).add(optsHidden);

    po::positional_options_description args;
    args.add("port", 1);

    po::variables_map vm;

    try {
	po::store(po::command_line_parser(argc, argv).
		  options(optsAll).positional(args).run(), vm);
	po::notify(vm);

	if (vm.count("help")) {
	    cout << "Usage: dcvyper [OPTION]... PORT" << endl;
	    cout << optsVisible << endl;
	    exit(EXIT_SUCCESS);
	}

	if (vm.count("raw-config") > 0)
	    optMode = MODE_RAW_CONFIG;
	if (vm.count("raw-memory") > 0)
	    optMode = MODE_RAW_MEMORY;

	if (vm.count("raw-memory") &&
	    vm.count("raw-config")) {
	    cerr << "Can't both dump memory and raw configuration." << endl;
	    exit(EXIT_FAILURE);
	}

	if (vm.count("port"))
	    devPort = vm["port"].as<string>();
	else {
	    cerr << "Missing port name" << endl;
	    exit(EXIT_FAILURE);
	}
    } catch (po::error e) {
	cerr << "Error: " << e.what() << endl;
	exit(EXIT_FAILURE);
    }
}

static void
printInfo(suunto::Vyper &vyper)
{
    cout << "Max depth: " << vyper.getMaxDepth() << endl;
    cout << "Total dive time: " << vyper.getTotalDiveTime() << endl;
    cout << "No dives: " << vyper.getTotalDives() << endl;
    cout << "Serial: " << vyper.getSerial() << endl;
    cout << "Personal: " << vyper.getPersonalInfo() << endl;

    cout << "Hardware type: " << vyper.getHWTypeName() << endl;
    cout << "FW Version: " << (int)vyper.getFWVersion() << endl;
    cout << "Log Interval: " << (int)vyper.getLogInterval() << endl;

    cout << "Alarm - Time: " << vyper.getAlarmTime()
	 << " (" << (vyper.getAlarmTimeOn() ? "active" : "inactive") << ")" << endl;
    cout << "Alarm - Depth: " << vyper.getAlarmDepth()
	 << " (" << (vyper.getAlarmDepthOn() ? "active" : "inactive") << ")" << endl;
}

static void
dumpRaw(suunto::Vyper &vyper)
{
    char config[VYPER_CONFIG_LEN];
    vyper.read(VYPER_CONFIG_OFFSET, config, sizeof(config));
    cout.write(config, sizeof(config));
}

static void
dumpMemory(suunto::Vyper &vyper)
{
    char mem[VYPER_MEM_SIZE];
    vyper.read(0, mem, sizeof(mem));
    cout.write(mem, sizeof(mem));
}

int
main(int argc, char **argv)
{
    parse_args(argc, argv);

    try {
	suunto::Vyper vyper(devPort.c_str());

	switch (optMode) {
	case MODE_INFO:
	    printInfo(vyper);
	    break;

	case MODE_RAW_CONFIG:
	    dumpRaw(vyper);
	    break;

	case MODE_RAW_MEMORY:
	    dumpMemory(vyper);
	    break;
	}

	return 0;
    } catch (DeviceException e) {
	cerr << "Error: " << e.what() << endl;
	return 1;
    }
}
