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
#include <set>
#include <boost/foreach.hpp>

#include "valid_value.hh"
#include "serialize/saxlite.hh"

using namespace std;

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
	ALARM_ASCENT = 0,   //< Ascent too fast
	ALARM_BREATH,       //< Breatingh frequency too high
	ALARM_DECO,         //< Deco stop announced
	ALARM_ERROR,        //< General error (e.g. deco stop missed)
	ALARM_LINK_LOST,    //< Wireless link to pressure gauge lost
	ALARM_MICROBUBBLES, //< Estimated degree of microbubble dagner (see UDDF manual)
	ALARM_RBT,          //< Remaining bottom time exceeded
	ALARM_SKIN_COOLING, //< Estimated degree of skin cooling (see UDDF manual)
	ALARM_SURFACE,      //< Surface reached
    };

    const static char *galarm_names[] = {
	"ascent-warning-too-long",
	"sos-mode",
	"work-too-hard",
    };

    enum GlobalAlarmType {
	GALARM_ASCENT_WARNING = 0, //< Ascent velocity too high or decompression stops disregarded
	GALARM_SOS_MODE,           //< Dive computer locked
	GALARM_HARD_WORK,          //< Physical work too high for a prolonged time
    };

    struct Alarm
    {
	Alarm(AlarmType _type)
	    : type(_type) {}

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
	list<Alarm> alarms;
    };

    struct InformationBeforeDive
    {
	time_t datetime;
    };

    struct GlobalAlarm
    {
	GlobalAlarm(GlobalAlarmType _type)
	    : type(_type) {}

	bool operator<(const GlobalAlarm &rhs) const {
	    return type < rhs.type;
	}

	GlobalAlarmType type;
    };

    struct InformationAfterDive
    {
	dcxx::Duration diveDuration;
	dcxx::Length greatestDepth;
	ValidValue<double> highestPO2;
	ValidValue<dcxx::Temperature> lowestTemperature;

	set<GlobalAlarm> globalAlarmsGiven;
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
operator<<(xml::ContentHandler &out, const uddf::Alarm &alarm)
{
    xml::ScopedElement e(out, "alarm");

    if (alarm.value)
	out.attribute("value", alarm.value.get());

    if (alarm.tankref)
	out.attribute("tankref", alarm.tankref.get());

    assert(alarm.type < sizeof(uddf::alarm_names) / sizeof(*uddf::alarm_names));
    out.text(uddf::alarm_names[alarm.type]);
}

xml::ContentHandler &
operator<<(xml::ContentHandler &out, const uddf::Waypoint &wp)
{
    xml::ScopedElement e(out, "waypoint");

    out.simpleTextElement("divetime", wp.divetime.seconds());
    out.simpleTextElement("depth", wp.depth.metre());

    if (wp.heading)
	out.simpleTextElement("heading", wp.heading.get());

    if (wp.temperature)
	out.simpleTextElement("temperature", wp.temperature.get().kelvin());

    BOOST_FOREACH(const uddf::Alarm &alarm, wp.alarms)
	out << alarm;

    return out;
}

xml::ContentHandler &
operator<<(xml::ContentHandler &out, const uddf::InformationBeforeDive &info)
{
    xml::ScopedElement e(out, "informationbeforedive");

    out.simpleTextElement("datetime", info.datetime);

    return out;
}

xml::ContentHandler &
operator<<(xml::ContentHandler &out, const uddf::GlobalAlarm &alarm)
{
    xml::ScopedElement e(out, "globalalarm");

    assert(alarm.type < sizeof(uddf::galarm_names) / sizeof(*uddf::galarm_names));
    out.text(uddf::galarm_names[alarm.type]);
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

    if (!info.globalAlarmsGiven.empty()) {
	xml::ScopedElement e_alarms(out, "globalalarmsgiven");
	BOOST_FOREACH(const uddf::GlobalAlarm &alarm, info.globalAlarmsGiven)
	    out << alarm;
    }

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

void
SerializeUDDF::onEvent(parser_sample_event_t type, dcxx::Duration time,
		       unsigned int flags, unsigned int value)
{
    assert(type != SAMPLE_EVENT_NONE);
    assert(currentDive);
    assert(!currentDive->samples.empty());

    uddf::Waypoint &wp(currentDive->samples.back());
    set<uddf::GlobalAlarm> &galarms(currentDive->infoAfter.globalAlarmsGiven);

    switch (type) {
    case SAMPLE_EVENT_NONE:
	/* This shouldn't happen */
	break;

    case SAMPLE_EVENT_ASCENT:
	/* Ascent too fast */
	wp.alarms.push_back(uddf::Alarm(uddf::ALARM_ASCENT));
	galarms.insert(uddf::GlobalAlarm(uddf::GALARM_ASCENT_WARNING));
	break;

    case SAMPLE_EVENT_DECOSTOP:
	/* Deco stop needed */
	wp.alarms.push_back(uddf::Alarm(uddf::ALARM_DECO));
	break;

    case SAMPLE_EVENT_SURFACE:
	/* Surface reached */
	wp.alarms.push_back(uddf::Alarm(uddf::ALARM_SURFACE));
	break;

    case SAMPLE_EVENT_CEILING:
	/* Deco voilation */
	wp.alarms.push_back(uddf::Alarm(uddf::ALARM_ERROR));
	galarms.insert(uddf::GlobalAlarm(uddf::GALARM_ASCENT_WARNING));
	break;

    case SAMPLE_EVENT_CEILING_SAFETYSTOP:
	/* Safety stop voilation */
	/* TODO: This should probably be some other type */
	wp.alarms.push_back(uddf::Alarm(uddf::ALARM_ASCENT));
	galarms.insert(uddf::GlobalAlarm(uddf::GALARM_ASCENT_WARNING));
	break;

    case SAMPLE_EVENT_VIOLATION:
	/* General violation (warning?) */
	galarms.insert(uddf::GlobalAlarm(uddf::GALARM_ASCENT_WARNING));
	break;

    case SAMPLE_EVENT_WORKLOAD:
	/* Work too hard */
	galarms.insert(uddf::GlobalAlarm(uddf::GALARM_HARD_WORK));
	break;

    case SAMPLE_EVENT_TRANSMITTER:
    case SAMPLE_EVENT_RBT:
    case SAMPLE_EVENT_BOOKMARK:
    case SAMPLE_EVENT_SAFETYSTOP:
    case SAMPLE_EVENT_GASCHANGE:
    case SAMPLE_EVENT_SAFETYSTOP_VOLUNTARY:
    case SAMPLE_EVENT_SAFETYSTOP_MANDATORY:
    case SAMPLE_EVENT_DEEPSTOP:
    case SAMPLE_EVENT_UNKNOWN:
    case SAMPLE_EVENT_DIVETIME:
    case SAMPLE_EVENT_MAXDEPTH:
    case SAMPLE_EVENT_OLF:
    case SAMPLE_EVENT_PO2:
    case SAMPLE_EVENT_AIRTIME:
    case SAMPLE_EVENT_RGBM:
    case SAMPLE_EVENT_HEADING:
    case SAMPLE_EVENT_TISSUELEVEL:
	cerr << "Warning: Ignored unhandled event" << endl;
	break;
    }
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
