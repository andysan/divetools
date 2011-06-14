bin_PROGRAMS = dcsync

CPPFLAGS = -I$(top_srcdir)/include $(BOOST_CPPFLAGS)
LDFLAGS = $(BOOST_LDFLAGS)
LDADD = $(top_builddir)/lib/libcommon.a $(top_builddir)/lib/dcxx/libdcxx.a
LIBS = -ldivecomputer \
	$(BOOST_PROGRAM_OPTIONS_LIB) \
	$(BOOST_FILESYSTEM_LIB)

dcsync_SOURCES = dcsync.cc
