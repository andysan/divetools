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

#ifndef DCXX_DEVICE_HH
#define DCXX_DEVICE_HH

#include <dcxx/utils.hh>
#include <libdivecomputer/device.h>


#define DCXX_TRY(call)				\
    do {					\
	device_status_t status = call;		\
	if (status != DEVICE_STATUS_SUCCESS)	\
	    throw DeviceException(status);	\
    } while(0)

DCXX_BEGIN_NS_DC

class DeviceException {
public:
    DeviceException(device_status_t status);

    const char *what() const throw();
    device_status_t getStatus() const throw();

private:
    const device_status_t status;
};

class Device;

class DeviceCallbacks {
public:
    DeviceCallbacks() {}
    virtual ~DeviceCallbacks() {};

    virtual void onEvent(Device &device, device_event_t event, const void *data);
    virtual void onEventWaiting(Device &device) {}
    virtual void onEventProgress(Device &device, const device_progress_t &progress) {}
    virtual void onEventDevInfo(Device &device, const device_devinfo_t &info) {}
    virtual void onEventClock(Device &device, const device_clock_t &clock) {}

    /** Return true to cancel */
    virtual bool onCancel(Device &device) { return false; }
    /** Return false to cancel */
    virtual bool onDive(Device &device,
			const void *data, int size,
			const void *fingerprint, int fsize) { return false; }
};

class Device {
public:
    virtual ~Device() throw(DeviceException);

    void setCallbackHandler(DeviceCallbacks *handler);

    device_type_t getType();

    void setEventMask(unsigned int events) throw(DeviceException);

    void setFingerprint(const void *data, unsigned int size)
	throw(DeviceException);

    void version(unsigned char *data, unsigned int size)
	throw(DeviceException);

    void read(unsigned int addr, void *data, unsigned int size)
	throw(DeviceException);

    void write(unsigned int addr, void *data, unsigned int size)
	throw(DeviceException);

    void forEach() throw(DeviceException);

protected:
    Device(device_t *device);
    Device();

    void init() throw(DeviceException);
    void close() throw(DeviceException);

    device_t *device;

private:
    static void eventCallback(device_t *device,
			      device_event_t event, const void *data,
			      void *userdata);
    static int cancelCallback(void *userdata);
    static int diveCallback(const unsigned char *data, unsigned int size,
			    const unsigned char *fingerprint, unsigned int fsize,
			    void *userdata);

    DeviceCallbacks *callbacks;
};

DCXX_END_NS

#endif
