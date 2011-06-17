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
#include <string>
#include <list>

#include <boost/program_options.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/scoped_array.hpp>

#include "dev_common.hh"
#include "dcconf.hh"

#define DIVE_BASE "dive_"

using namespace std;
using namespace dcxx;

namespace po = boost::program_options;
namespace bfs = boost::filesystem;

DCConf dcconf;

bool optForce = false;
bool optInit = false;

bfs::path outputDir;
bfs::path configDir;
bfs::path configFile;

int lastDiveNo = -1;

static void setFingerprint(Device &dev);

class RawDive
{
public:
    RawDive(const void *_data, int _size,
	    const void *_fingerprint, int _fsize)
	: size(_size), fsize(_fsize) {

	data.reset(new char[size]);
	fingerprint.reset(new char[fsize]);

	memcpy(data.get(), _data, size);
	memcpy(fingerprint.get(), _fingerprint, fsize);
    }

    void writeDive(const bfs::path &path) const {
	bfs::ofstream ofDive(path);
	ofDive.write(data.get(), size);
	ofDive.close();
    }

    void writeFingerprint(const bfs::path &path) const {
	bfs::ofstream ofFingerprint(path);
	ofFingerprint.write(fingerprint.get(), fsize);
	ofFingerprint.close();
    }

    boost::shared_array<char> data;
    int size;

    boost::shared_array<char> fingerprint;
    int fsize;
};

typedef list<RawDive> RawDiveList;

class Callbacks
    : public DeviceCallbacks
{
public:
    void onEventWaiting(Device &device) {
	cerr << "Waiting..." << endl;
    }

    void onEventProgress(Device &device, const device_progress_t &progress) {
	//const double f = 100.0 * progress.current / progress.maximum;
	//cerr << "Progress: " << f << "%" << endl;
    }

    void onEventDevInfo(Device &device, const device_devinfo_t &info) {
	if (dcconf.devSerialValid && dcconf.devSerial != info.serial) {
	    cerr << "Error: Serial number mismatch" << endl;
	    exit(EXIT_FAILURE);
	}

	if (lastDiveNo >= 0) {
	    cerr << "Found previous dives, skipping the first " << lastDiveNo << " dives." << endl;
	    setFingerprint(device);
	}

	if (optInit) {
	    dcconf.setDevSerial(info.serial);

	    cerr << "Creating output directory structure..." << endl;
	    bfs::create_directory(outputDir);
	    bfs::create_directory(configDir);

	    cerr << "Storing configuration..." << endl;
	    bfs::ofstream conf(configFile);
	    conf << dcconf << endl;
	    conf.close();
	}
    }

    void onEventClock(Device &device, const device_clock_t &clock) {
	cerr << "Error: Unhandled clock event, this device isn't supported." << endl;
	exit(EXIT_FAILURE);
    }

    bool onDive(Device &device,
		const void *data, int size,
		const void *fingerprint, int fsize) {
	cerr << "Reading dive " << dives.size() << endl;
	RawDive dive(data, size, fingerprint, fsize);
	dives.push_front(dive);

	return true;
    }

    void saveDives() {
	BOOST_FOREACH(const RawDive &dive, dives) {
	    stringstream diveName;
	    stringstream fpName;
	    int diveNo = ++lastDiveNo;

	    diveName << DIVE_BASE << diveNo << ".raw";
	    fpName << DIVE_BASE << diveNo << ".fp";

	    dive.writeDive(outputDir / diveName.str());
	    dive.writeFingerprint(outputDir / fpName.str());
	}
    }

private:
    static void setFingerprint(Device &dev) {
	stringstream fpFName;
	fpFName << DIVE_BASE << lastDiveNo << ".fp";
	bfs::ifstream fin(outputDir / fpFName.str());
	int length;

	// Figure out the length of the fingerprint
	fin.seekg (0, ios::end);
	length = fin.tellg();
	fin.seekg (0, ios::beg);

	boost::scoped_array<char> data;
	data.reset(new char[length]);

	fin.read(data.get(), length);
	fin.close();

	dev.setFingerprint(data.get(), length);
    }

    RawDiveList dives;
};

static int
findLastDive()
{
    BOOST_FOREACH(bfs::path path,
		  make_pair(bfs::directory_iterator(outputDir),
			    bfs::directory_iterator())) {

	string name(path.filename());

	if (name.compare(0, sizeof(DIVE_BASE) - 1, DIVE_BASE) == 0 &&
	    name.compare(name.length() - 3, 3, ".fp") == 0) {
	    const char *cname = name.c_str();
	    char *endptr;
	    errno = 0;
	    long no = strtol(cname + sizeof(DIVE_BASE) - 1, &endptr, 10);

	    if (errno != 0 || endptr != cname + name.length() - 3) {
		cerr << "Error: Invalid dive file in output directory ("
		     << name << ")" << endl;
		exit(EXIT_FAILURE);
	    }

	    if (no < 0 || no > 0x8FFFFFFF) {
		cerr << "Error: Found dive file with out of range number ("
		     << name << ")" << endl;
		exit(EXIT_FAILURE);
	    }

	    if (no > lastDiveNo)
		lastDiveNo = no;
	}
    }
}

static void
parse_conf()
{
    if (!bfs::exists(configFile) ||
	!bfs::is_regular_file(configFile))
	return;

    bfs::ifstream fin(configFile);

    po::options_description cfg_all;
    cfg_all.add(dcconf.cfgCommon);

    try {
	po::variables_map vm;
	po::store(parse_config_file(fin, cfg_all), vm);
	po::notify(vm);

	dcconf.handleConf(vm);
    } catch (po::error e) {
	cerr << "Error: " << e.what() << endl;
	exit(EXIT_FAILURE);
    }
}

static void
parse_args(int argc, char **argv)
{
    po::options_description optsGeneral("General options");
    optsGeneral.add_options()
	("help", "produce help message")
	("force", "don't treat some errors as fatal")
	("init", "setup directory for device synchronization")
	;

    po::options_description optsHidden("Hidden");
    optsHidden.add_options()
	("output-dir", po::value<string>(), "");

    po::options_description optsVisible;
    optsVisible.add(optsGeneral).add(dcconf.optsCommon);

    po::options_description optsAll;
    optsAll.add(optsVisible).add(optsHidden);

    po::positional_options_description args;
    args.add("output-dir", 1);

    po::variables_map vm;

    try {
	po::store(po::command_line_parser(argc, argv).
		  options(optsAll).positional(args).run(), vm);
	po::notify(vm);

	if (vm.count("help")) {
	    cout << "Usage: dcsync [OPTION]... [DIR]" << endl;
	    cout << optsVisible << endl;
	    exit(EXIT_SUCCESS);
	}

	optInit = vm.count("init") > 0;

	if (vm.count("output-dir"))
	    outputDir = vm["output-dir"].as<string>();
	else
	    outputDir = bfs::current_path();

	dcconf.handleArgs(vm);

	configDir = outputDir / bfs::path(".divetools");
	configFile = configDir / bfs::path("config");
    } catch (po::error e) {
	cerr << "Error: " << e.what() << endl;
	exit(EXIT_FAILURE);
    }
}

int
main(int argc, char **argv)
{
    parse_args(argc, argv);
    if (optInit) {
	if (bfs::exists(outputDir)) {
	    cerr << "Error: Output directory already exists" << endl;
	    return 1;
	}
    } else {
	if (!bfs::exists(outputDir)) {
	    cerr << "Error: Output directory does not exist" << endl;
	    return 1;
	}

	if (!bfs::exists(configDir)) {
	    cerr << "Error: Output directory does not exist" << endl;
	    return 1;
	}
    }

    parse_conf();

    if (!dcconf.devInfo) {
	cerr << "Error: Unknown device type specified" << endl;
	return 1;
    }

    if (dcconf.devPort.empty()) {
	cerr << "Error: No port specified for device" << endl;
	return 1;
    }

    if (!optInit) {
	if (!dcconf.devSerialValid) {
	    if(!optForce) {
		cerr << "Error: No serial number specified in configuration." << endl;
		cerr << "Use --force to ignore this error." << endl;
		return 1;
	    } else {
		cerr << "Warning: No serial number specified in configuration." << endl;
		cerr << "Warning: Continuing anyway... Good luck!" << endl;
	    }
	}

	findLastDive();
    }

    try {
	Callbacks callbacks;
	boost::scoped_ptr<Device> device;
	device.reset(devCreate(dcconf.devInfo->device, dcconf.devPort.c_str()));
	if (!device.get()) {
	    cerr << "Error: Device type unsupported" << endl;
	    return 1;
	}

	device->setCallbackHandler(&callbacks);

	device->forEach();

	cerr << "Storing dives..." << endl;
	callbacks.saveDives();
    } catch (DeviceException e) {
	cerr << "Error: " << e.what() << endl;
	return 1;
    }
    return 0;
}
