
AMISSOURCES = AmisDriver.cc \
              EventReader.cc \
              AmisModelFormat.cc \
              DataFormat.cc \
              Estimator.cc \
              EventSpace.cc \
              Feature.cc \
              GaussianPrior.cc \
              GzStream.cc \
              LogStream.cc \
              Model.cc \
              OptionManager.cc \
              Profile.cc \
              Property.cc \
              Tokenizer.cc \
\
              BLMVMBC.cc \
              BLMVMBCMAP.cc \
\
              LMVM.cc \
              LMVMMAP.cc \
\
              BFGS.cc \
              BFGSMAP.cc \
\
              GIS.cc \
              GISMAP.cc \
              AP.cc




pkglib_LIBRARIES = libamis.a

libamis_a_SOURCES = $(AMISSOURCES)

bin_PROGRAMS = amis
noinst_PROGRAMS = forestcheck

amis_SOURCES = amis.cc $(AMISSOURCES)

# amis_DEPENDENCIES = libamis.a
# amis_LDFLAGS = -Wl,--whole-archive libamis.a -Wl,--no-whole-archive

forestcheck_SOURCES = forestcheck.cc $(AMISSOURCES)

AM_CXXFLAGS = -I../include

# AM_CXXFLAGS = -Wall -fomit-frame-pointer -fstrict-aliasing
# AM_CXXFLAGS = -Wall -g -O0 -fstrict-aliasing -ftemplate-depth-22

