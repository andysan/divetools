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

#include "serialize/uddf.hh"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <iostream>
#include <string>
#include <ctime>
#include <list>
#include <boost/foreach.hpp>

#include "valid_value.hh"
#include "serialize/saxlite.hh"

using namespace std;

// TODO: Implement support for alarms

namespace uddf {
    const static char *alarm_names[] = {
	"ascent",
	"breath",
	"deco",
	"error",
	"link",
	"microbubbles",
	"rbt",
	"skincooling",
	"surface",
    };

    enum AlarmType {
	ALARM_ASCENT = 0,
	ALARM_BREATH,
	ALARM_DECO,
	ALARM_ERROR,
	ALARM_LINK_LOST,
	ALARM_MICROBUBBLES,
	ALARM_RBT,
	ALARM_SKIN_COOLING,
	ALARM_SURFACE,
    };

    struct Alarm
    {
	AlarmType type;
	ValidValue<double> value;
	ValidValue<string> tankref;
    };

    struct Waypoint
    {
	Waypoint(dcxx::Duration _divetime, dcxx::Length _depth)
	    : divetime(_divetime), depth(_depth) {}

	dcxx::Duration divetime;
	dcxx::Length depth;

	ValidValue<double> heading;
	ValidValue<dcxx::Temperature> temperature;
    };

    struct InformationBeforeDive
    {
	time_t datetime;
    };

    struct InformationAfterDive
    {
	dcxx::Duration diveDuration;
	dcxx::Length greatestDepth;
	ValidValue<double> highestPO2;
	ValidValue<dcxx::Temperature> lowestTemperature;

	// TODO: Support global alarms
    };

    struct ApplicationData
    {
	// TODO: Implement storage of raw data from dive computer
    };

    struct Dive
    {
	Dive(const string &_id)
	    : id(_id) {}

	string id;
	InformationBeforeDive infoBefore;
	InformationAfterDive infoAfter;
	ApplicationData applicationData;
	list<Waypoint> samples;
    };

    struct RepetitionGroup
    {
	RepetitionGroup(const string &_id)
	    : id(_id) {}

	string id;
	list<Dive> dives;
    };

    struct ProfileData
    {
	list<RepetitionGroup> repetitionGroups;
    };

    struct Manufacturer
    {
	Manufacturer()
	    : id("andreas.sandberg.pp.se"),
	      name("Andreas Sandberg"),
	      contact(PACKAGE_BUGREPORT) {}

	string id;
	string name;
	string contact;
    };

    struct Generator
    {
	Generator();

	string name;
	string type;
	Manufacturer manufacturer;
	string version;
	time_t datetime;
    };

    struct File
    {
	File()
	    : version("3.0.0") {}

	string version;
	Generator generator;
	ProfileData profileData;
    };


    Generator::Generator()
	: name(PACKAGE_NAME),
	  type("converter"),
	  manufacturer(),
	  version(PACKAGE_VERSION)
    {
	datetime = time(NULL);
	if (datetime == (time_t)-1)
	    cerr << "Warning: Can't determine time of day" << endl;
    }
}

xml::ContentHandler &
operator<<(xml::ContentHandler &out, const uddf::Waypoint &wp)
{
    xml::ScopedElement ibd(out, "waypoint");

    out.simpleTextElement("divetime", wp.divetime.seconds());
    out.simpleTextElement("depth", wp.depth.metre());

    if (wp.heading)
	out.simpleTextElement("heading", wp.heading.get());

    if (wp.temperature)
	out.simpleTextElement("temperature", wp.temperature.get().kelvin());

    return out;
}

xml::ContentHandler &
operator<<(xml::ContentHandler &out, const uddf::InformationBeforeDive &info)
{
    xml::ScopedElement ibd(out, "informationbeforedive");

    out.simpleTextElement("datetime", info.datetime);

    return out;
}


xml::ContentHandler &
operator<<(xml::ContentHandler &out, const uddf::InformationAfterDive &info)
{
    xml::ScopedElement e(out, "informationafterdive");

    out.simpleTextElement("diveduration", info.diveDuration.seconds());
    out.simpleTextElement("greatestdepth", info.greatestDepth.metre());

    if (info.highestPO2)
	out.simpleTextElement("highestpo2", info.highestPO2.get());

    if (info.lowestTemperature)
	out.simpleTextElement("lowesttemperature", info.lowestTemperature.get().kelvin());

    return out;
}

xml::ContentHandler &
operator<<(xml::ContentHandler &out, const uddf::ApplicationData &info)
{
    xml::ScopedElement e(out, "applicationdata");

    return out;
}

xml::ContentHandler &
operator<<(xml::ContentHandler &out, const uddf::Dive &dive)
{
    xml::ScopedElement e(out, "dive");
    out.attribute("id", dive.id);

    out << dive.infoBefore
	<< dive.infoAfter
	<< dive.applicationData;

    BOOST_FOREACH(const uddf::Waypoint &wp, dive.samples)
	out << wp;

    return out;
}

xml::ContentHandler &
operator<<(xml::ContentHandler &out, const uddf::RepetitionGroup &group)
{
    xml::ScopedElement e(out, "repetitiongroup");
    out.attribute("id", group.id);

    BOOST_FOREACH(const uddf::Dive &dive, group.dives)
	out << dive;

    return out;
}

xml::ContentHandler &
operator<<(xml::ContentHandler &out, const uddf::ProfileData &prof)
{
    xml::ScopedElement e(out, "profiledata");

    BOOST_FOREACH(const uddf::RepetitionGroup &rg, prof.repetitionGroups)
	out << rg;

    return out;
}

xml::ContentHandler &
operator<<(xml::ContentHandler &out, const uddf::Manufacturer &mfgr)
{
    xml::ScopedElement e(out, "manufacturer");
    out.attribute("id", mfgr.id);
    out.simpleTextElement("name", mfgr.name);

    return out;
}

xml::ContentHandler &
operator<<(xml::ContentHandler &out, const uddf::Generator &gen)
{
    xml::ScopedElement e(out, "generator");
    out.simpleTextElement("name", gen.name);
    out << gen.manufacturer;
    out.simpleTextElement("version", gen.version);
    out.simpleTextElement("datetime", gen.datetime);
    out.simpleTextElement("type", gen.type);

    return out;
}

xml::ContentHandler &
operator<<(xml::ContentHandler &out, const uddf::File &uddf)
{
    xml::ScopedElement e(out, "uddf");
    out.attribute("version", uddf.version);
    out << uddf.generator
	<< uddf.profileData;

    return out;
}


SerializeUDDF::SerializeUDDF(ostream &_out, dcxx::Parser &parser)
    : SampleBuilder(),
      out(_out)
{
    uddf.reset(new uddf::File());

    uddf->profileData.repetitionGroups.push_back(
	uddf::RepetitionGroup(repetitionGroupID(parser)));
    currentRG = &uddf->profileData.repetitionGroups.front();

    currentRG->dives.push_back(uddf::Dive(diveID(parser)));
    currentDive = &currentRG->dives.front();

    uddf::InformationAfterDive &infoAfter(currentDive->infoAfter);
    uddf::InformationBeforeDive &infoBefore(currentDive->infoBefore);
    infoAfter.greatestDepth = parser.getMaxDepth();
    infoAfter.diveDuration = parser.getDiveTime();
    infoBefore.datetime = parser.getDateTime();


}

SerializeUDDF::~SerializeUDDF()
{
    xml::StreamSerializer ser(out);
    ser.startDocument();
    ser << *uddf;
    ser.endDocument();
}

void
SerializeUDDF::onSample(const Sample &sample)
{
    assert(sample.time);
    assert(sample.depth);

    uddf::InformationAfterDive &infoAfter(currentDive->infoAfter);
    uddf::Waypoint wp(sample.time.get(),
		      sample.depth.get());
    if (sample.temperature) {
	wp.temperature = sample.temperature;
	if (!infoAfter.lowestTemperature ||
	    infoAfter.lowestTemperature.get() > sample.temperature.get())
	    infoAfter.lowestTemperature = sample.temperature;
    }

    assert(currentDive);
    currentDive->samples.push_back(wp);
}

string
SerializeUDDF::repetitionGroupID(dcxx::Parser &parser)
{
    stringstream ss;
    time_t time(parser.getDateTime());
    ss << "rg-" << time;

    return ss.str();
}

string
SerializeUDDF::diveID(dcxx::Parser &parser)
{
    stringstream ss;
    time_t time(parser.getDateTime());
    ss << "dive-" << time;

    return ss.str();
}
