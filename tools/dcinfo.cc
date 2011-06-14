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

#include "dcxx/suunto.hh"

using namespace std;

/* Configuration options */
static const char *dev_port = "/dev/ttyUSB0";
static const char *dev_type = "suunto_vyper";

/* */
static void
handle_dive(const unsigned char *data, unsigned int size,
	    const unsigned char *fingerprint, unsigned int fsize,
	    void *userdata)
{
    std::cout << "Dive" << std::endl;
}

int
main(int argc, char **argv)
{
    dcxx::suunto::Vyper vyper(dev_port);

    cout << "Dive computer info:" << endl;
    cout << "Max depth: " << vyper.getMaxDepth() << endl;
    cout << "Total dive time: " << vyper.getTotalDiveTime() << endl;
    cout << "No dives: " << vyper.getTotalDives() << endl;
    cout << "Serial: " << vyper.getSerial() << endl;
    cout << "Personal: " << vyper.getPersonalInfo() << endl;

    return 0;
}
