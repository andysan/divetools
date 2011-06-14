#include <dcxx/utils.hh>
#include <dcxx/device.hh>

DCXX_BEGIN_NS_DC

DeviceException::DeviceException(device_status_t status)
    : status(status)
{
}

const char *
DeviceException::what() const throw()
{
    switch (status) {
    case DEVICE_STATUS_SUCCESS:
	return "No Error";

    case DEVICE_STATUS_UNSUPPORTED:
	return "Unsupported feature";

    case DEVICE_STATUS_TYPE_MISMATCH:
	return "Type mismatch";

    case DEVICE_STATUS_IO:
	return "IO Error";

    case DEVICE_STATUS_TIMEOUT:
	return "Timeout while accssing dive computer";

    case DEVICE_STATUS_PROTOCOL:
	return "Protocol error";

    case DEVICE_STATUS_MEMORY:
	return "Memory allocation failed";

    case DEVICE_STATUS_CANCELLED:
	return "Request cancelled";

    case DEVICE_STATUS_ERROR:
    default:
	return "An unknown error occured";
    }

}

device_status_t
DeviceException::getStatus() const throw()
{
    return status;
}

void
DeviceCallbacks::onEvent(Device &device,
			 device_event_t event, const void *data)
{
    switch (event) {
    case DEVICE_EVENT_WAITING:
	onEventWaiting(device);
	break;

    case DEVICE_EVENT_PROGRESS:
	onEventProgress(device, *(device_progress_t *)data);
	break;

    case DEVICE_EVENT_DEVINFO:
	onEventDevInfo(device, *(device_devinfo_t *)data);
	break;

    case DEVICE_EVENT_CLOCK:
	onEventClock(device, *(device_clock_t *)data);
	break;
    }
}


Device::Device(device_t *device)
    : device(device), callbacks(NULL)
{
    init();
}

Device::Device()
    : device(NULL), callbacks(NULL)
{
}

Device::~Device() throw(DeviceException)
{
    if (device)
	close();
}

void
Device::init() throw(DeviceException)
{
    if (device) {
	DCXX_TRY(device_set_cancel(device, &Device::cancelCallback, (void *)this));
	setEventMask((unsigned int)-1);
	
    }
}

void
Device::close() throw(DeviceException)
{
    DCXX_TRY(device_close(device));
}

void
Device::setCallbackHandler(DeviceCallbacks *handler)
{
    callbacks = handler;
}

device_type_t
Device::getType()
{
    return device_get_type(device);
}

void
Device::setEventMask(unsigned int events) throw(DeviceException)
{
    DCXX_TRY(device_set_events(device, events,
			       &Device::eventCallback, (void *)this));
}

void
Device::setFingerprint(const void *data, unsigned int size)
    throw(DeviceException)
{
    DCXX_TRY(device_set_fingerprint(device, (unsigned char *)data, size));
}

void
Device::version(unsigned char *data, unsigned int size) throw(DeviceException)
{
    DCXX_TRY(device_version(device, data, size));
}

void
Device::read(unsigned int addr, void *data, unsigned int size)
    throw(DeviceException)
{
    DCXX_TRY(device_read(device, addr, (unsigned char *)data, size));
}

void
Device::write(unsigned int addr, void *data, unsigned int size)
    throw(DeviceException)
{
    DCXX_TRY(device_write(device, addr, (unsigned char *)data, size));
}

void
Device::forEach() throw(DeviceException)
{
    DCXX_TRY(device_foreach(device, &Device::diveCallback, (void *)this));
}

void
Device::eventCallback(device_t *device, device_event_t event, const void *data,
		      void *userdata)
{
    Device *_this = static_cast<Device *>(userdata);

    if (_this->callbacks)
	_this->callbacks->onEvent(*_this, event, data);
}

int
Device::cancelCallback(void *userdata)
{
    Device *_this = static_cast<Device *>(userdata);

    if (_this->callbacks)
	return _this->callbacks->onCancel(*_this) ? 1 : 0;
    else
	return 0;
}

int
Device::diveCallback(const unsigned char *data, unsigned int size,
		     const unsigned char *fingerprint, unsigned int fsize,
		     void *userdata)
{
    Device *_this = static_cast<Device *>(userdata);

    if (_this->callbacks)
	return _this->callbacks->onDive(*_this, data, size, fingerprint, fsize)
	    ? 1 : 0;
    else
	return false;
}

DCXX_END_NS
