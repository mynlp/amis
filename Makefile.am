HTML_DIR = html

SUBDIRS = src include test manual

pkgdata_DATA = doxygen.conf

EXTRA_DIST = $(pkgdata_DATA)

.PHONY : doc

doc:
	doxygen doxygen.conf

