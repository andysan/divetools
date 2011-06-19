bin_PROGRAMS = dcsync dcvyper dcparse

CPPFLAGS = -I$(top_srcdir)/include $(BOOST_CPPFLAGS)
LDFLAGS = $(BOOST_LDFLAGS)
LDADD = 						\
	$(top_builddir)/lib/libcommon.a			\
	$(top_builddir)/lib/serialize/libserialize.a	\
	$(top_builddir)/lib/dcxx/libdcxx.a
LIBS = -ldivecomputer					\
	$(BOOST_PROGRAM_OPTIONS_LIB)			\
	$(BOOST_FILESYSTEM_LIB)

dcsync_SOURCES = dcsync.cc
dcvyper_SOURCES = dcvyper.cc
dcparse_SOURCES = dcparse.cc
