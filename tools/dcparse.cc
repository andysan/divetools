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
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/scoped_array.hpp>
#include <boost/foreach.hpp>

#include "dev_common.hh"
#include "dcconf.hh"
#include "serialize/csv.hh"

using namespace std;
using namespace dcxx;

namespace po = boost::program_options;
namespace bfs = boost::filesystem;

enum OutputFormat {
    FMT_TEXT,
    FMT_CSV,
};

DCConf dcconf;

bool optForce = false;
OutputFormat optFormat = FMT_TEXT;

bfs::path diveFile;
bfs::path projectDir;
bfs::path configDir;
bfs::path configFile;

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
	("format", po::value<string>(), "output format ('help' to list formats)")
	;

    po::options_description optsHidden("Hidden");
    optsHidden.add_options()
	("dive-file", po::value<string>(), "");

    po::options_description optsVisible;
    optsVisible.add(optsGeneral).add(dcconf.optsCommon);

    po::options_description optsAll;
    optsAll.add(optsVisible).add(optsHidden);

    po::positional_options_description args;
    args.add("dive-file", 1);

    po::variables_map vm;

    try {
	po::store(po::command_line_parser(argc, argv).
		  options(optsAll).positional(args).run(), vm);
	po::notify(vm);

	if (vm.count("help")) {
	    cout << "Usage: dcparse [OPTION]... FILE" << endl;
	    cout << optsVisible << endl;
	    exit(EXIT_SUCCESS);
	}

	if (vm.count("format")) {
	    string fmt = vm["format"].as<string>();

	    if (fmt == "text")
		optFormat = FMT_TEXT;
	    else if (fmt == "csv")
		optFormat = FMT_CSV;
	    else if (fmt == "help") {
		cout << "Supported output formats:" << endl;
		cout << "\ttext\tOutput dive in plain text" << endl;
		cout << "\tcsv\tOutput dive in CSV format" << endl;
		exit(EXIT_SUCCESS);
	    } else {
		cerr << "Unknown output format specified." << endl;
		exit(EXIT_FAILURE);
	    }
	}

	if (vm.count("dive-file"))
	    diveFile = vm["dive-file"].as<string>();
	else {
	    cerr << "Error: No input file specified" << endl;
	    exit(EXIT_FAILURE);
	}

	dcconf.handleArgs(vm);

	projectDir = diveFile.parent_path();
	configDir = projectDir / bfs::path(".divetools");
	configFile = configDir / bfs::path("config");
    } catch (po::error e) {
	cerr << "Error: " << e.what() << endl;
	exit(EXIT_FAILURE);
    }
}

static void
setFileData(Parser &parser, boost::scoped_array<char> &data)
{
    bfs::ifstream fin(diveFile);
    int length;

    // Figure out the length of the fingerprint
    fin.seekg (0, ios::end);
    length = fin.tellg();
    fin.seekg (0, ios::beg);

    data.reset(new char[length]);

    fin.read(data.get(), length);
    fin.close();
    
    parser.setData(data.get(), length);
}

static void
outputText(Parser &parser)
{
    Parser::GasMixVector mixes;
    parser.getGasMixes(mixes);

    cerr << "Dive info:" << endl
	 << "  Dive time: " << parser.getDiveTime() << endl
	 << "  Max Depth: " << parser.getMaxDepth() << endl;

    cerr << "Gas Mixes:" << endl;

    BOOST_FOREACH(gasmix_t mix, mixes)
	cerr << "  He: " << mix.helium * 100.0 << "%"
	     << " O2: " << mix.oxygen * 100.0 << "%"
	     << " N2: " << mix.nitrogen * 100.0 << "%" << endl;
}

static void
outputCSV(Parser &parser)
{
    SerializeCSV csv(cout);
    parser.setCallbackHandler(&csv);
    parser.forEachSample();
}

int
main(int argc, char **argv)
{
    parse_args(argc, argv);
    if (!bfs::exists(diveFile)) {
	cerr << "Error: Input file does not exist" << endl;
	return 1;
    }

    parse_conf();

    if (!dcconf.devInfo) {
	cerr << "Error: Unknown device type specified" << endl;
	return 1;
    }

    try {
	boost::scoped_ptr<Parser> parser;
	boost::scoped_array<char> data;

	parser.reset(parserCreate(dcconf.devInfo->parser));
	if (!parser.get()) {
	    cerr << "Error: Device type unsupported" << endl;
	    return 1;
	}

	setFileData(*parser, data);

	switch(optFormat) {
	case FMT_TEXT:
	    outputText(*parser);
	    break;
	case FMT_CSV:
	    outputCSV(*parser);
	    break;
	}
    } catch (DeviceException e) {
	cerr << "Error: " << e.what() << endl;
	return 1;
    }
    return 0;
}
