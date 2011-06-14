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
    DCXX_TRY(suunto_vyper_device_open(&device, name));
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
    DCXX_TRY(suunto_vyper_device_set_delay(device, delay));
}

void
Vyper::readDive(dc_buffer_t *buffer, int init) throw(DeviceException)
{
    DCXX_TRY(suunto_vyper_device_read_dive(device, buffer, init));
}



Vyper2::Vyper2(const char *name) throw(DeviceException)
    : Device()
{
    DCXX_TRY(suunto_vyper2_device_open(&device, name));
    init();
}

void
Vyper2::reset_maxdepth() throw(DeviceException)
{
    DCXX_TRY(suunto_vyper2_device_reset_maxdepth(device));
}


DCXX_END_NS
DCXX_END_NS
