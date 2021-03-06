#
# CMake configuration file for VMime
#
# Usage:
#
# . 'cmake -LH' to list build settings variable
#
# . 'cmake -G <generator>' to generate makefiles for a build system
#   eg. cmake -G "Unix Makefiles"
#
# For more information, please visit:
# http://www.cmake.org
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.6 FATAL_ERROR)

INCLUDE(cmake/Utils.cmake)

# CMake configuration
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY build/bin)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY build/lib)

SET(CMAKE_VERBOSE_MAKEFILE ON)


# Project
PROJECT(vmime)

# Package version number
SET(VMIME_VERSION_MAJOR  0)
SET(VMIME_VERSION_MINOR  9)
SET(VMIME_VERSION_MICRO  2)

# API version number (libtool)
#
# Increment this number only immediately before a public release.
# This is independent from package version number.
#
# See: http://www.gnu.org/software/libtool/manual.html#Libtool-versioning
#
# . Implementation changed (eg. bug/security fix):  REVISION++
# . Interfaces added/removed/changed:               CURRENT++, REVISION=0
# . Interfaces added (upward-compatible changes):   AGE++
# . Interfaces removed:                             AGE=0
SET(VMIME_API_VERSION_CURRENT   0)
SET(VMIME_API_VERSION_REVISION  0)
SET(VMIME_API_VERSION_AGE       0)


SET(VMIME_VERSION ${VMIME_VERSION_MAJOR}.${VMIME_VERSION_MINOR}.${VMIME_VERSION_MICRO})
SET(VMIME_API_VERSION ${VMIME_API_VERSION_CURRENT}.${VMIME_API_VERSION_REVISION}.${VMIME_API_VERSION_AGE})


##############################################################################
# VMime Library

# Set base name
SET(VMIME_LIBRARY_NAME vmime)

# Source files
FILE(
	GLOB_RECURSE
	VMIME_LIBRARY_SRC_FILES
	src/*
)

FILE(
	GLOB_RECURSE
	VMIME_LIBRARY_INCLUDE_FILES
	vmime/*
)

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}
)

# Automatically set lib suffix
IF(UNIX AND NOT APPLE AND NOT CMAKE_CROSSCOMPILING AND NOT EXISTS "/etc/debian_version")
	IF(CMAKE_SIZEOF_VOID_P EQUAL 8 AND NOT LIB_SUFFIX)
		SET(LIB_SUFFIX 64)
	ENDIF()
ENDIF()

# Shared library
ADD_LIBRARY(
	${VMIME_LIBRARY_NAME}
	SHARED
	${VMIME_LIBRARY_SRC_FILES}
	${VMIME_LIBRARY_INCLUDE_FILES}
)

SET_TARGET_PROPERTIES(
	${VMIME_LIBRARY_NAME}
	PROPERTIES
	VERSION    "${VMIME_VERSION}"
	SOVERSION  "${VMIME_API_VERSION}"
)

# Static library
#
# Note: cannot have two targets with the same name so the static version has
# '-static' appended and then the name of the output file is set separately.
ADD_LIBRARY(
	${VMIME_LIBRARY_NAME}-static
	STATIC
	${VMIME_LIBRARY_SRC_FILES}
	${VMIME_LIBRARY_INCLUDE_FILES}
)

SET_TARGET_PROPERTIES(
	${VMIME_LIBRARY_NAME}-static
	PROPERTIES
	OUTPUT_NAME  ${VMIME_LIBRARY_NAME}
)

# These next two lines are required but it is unclear exactly what they do.
# The CMake FAQ mentions they are necessary and it does not work otherwise.
SET_TARGET_PROPERTIES(${VMIME_LIBRARY_NAME} PROPERTIES CLEAN_DIRECT_OUTPUT 1)
SET_TARGET_PROPERTIES(${VMIME_LIBRARY_NAME}-static PROPERTIES CLEAN_DIRECT_OUTPUT 1)

SET(CMAKE_INSTALL_LIBDIR lib CACHE PATH "Output directory for libraries")

# Installation of libraries
INSTALL(
	TARGETS ${VMIME_LIBRARY_NAME}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT sharedlibs
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT sharedlibs
)

INSTALL(
	TARGETS ${VMIME_LIBRARY_NAME}-static
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT staticlibs
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT staticlibs
)

# Installation of header files
INSTALL_HEADERS_WITH_DIRECTORY(VMIME_LIBRARY_INCLUDE_FILES headers)

#INSTALL(
#	FILES ${VMIME_LIBRARY_INCLUDE_FILES}
#	DESTINATION include
#	COMPONENT headers
#)


##############################################################################
# Tests

OPTION(
	VMIME_BUILD_TESTS
	"Build unit tests (this will create a 'run-tests' binary)"
	OFF
)

IF(VMIME_BUILD_TESTS)

	FILE(
		GLOB_RECURSE
		VMIME_TESTS_SRC_FILES
		tests/*Test.cpp tests/testRunner.cpp tests/testUtils.cpp
	)

	ADD_EXECUTABLE(
		"run-tests"
		${VMIME_TESTS_SRC_FILES}
	)

	INCLUDE(cmake/FindCppUnit.cmake)

	INCLUDE_DIRECTORIES(${CPPUNIT_INCLUDE_DIR})

	TARGET_LINK_LIBRARIES(
		"run-tests"
		${VMIME_LIBRARY_NAME}
		${CPPUNIT_LIBRARY}
	)

	ADD_DEPENDENCIES(
		"run-tests"
		${VMIME_LIBRARY_NAME}
	)

ENDIF()


##############################################################################
# Packaging / Distribution

# Package information
SET(VMIME_PACKAGE_NAME         ${VMIME_LIBRARY_NAME})
SET(VMIME_PACKAGE_VERSION      ${VMIME_VERSION})
SET(VMIME_PACKAGE_CONTACT      "Vincent Richard <vincent@vmime.org>")
SET(VMIME_PACKAGE_DESCRIPTION  "VMime C++ Mail Library (http://www.vmime.org)")

SET(CPACK_PACKAGE_NAME                 "${VMIME_PACKAGE_NAME}")
SET(CPACK_PACKAGE_CONTACT              "${VMIME_PACKAGE_CONTACT}")
SET(CPACK_PACKAGE_DESCRIPTION          "${VMIME_PACKAGE_DESCRIPTION}")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY  "${CPACK_PACKAGE_DESCRIPTION}")
SET(CPACK_PACKAGE_VERSION              "${VMIME_PACKAGE_VERSION}")

# Package settings
SET(CPACK_RESOURCE_FILE_LICENSE        ${CMAKE_SOURCE_DIR}/COPYING)

IF(APPLE)
	# CPack/PackageManager won't allow file without recognized extension
	# to be used as license file.
	CONFIGURE_FILE("${CMAKE_SOURCE_DIR}/COPYING" "${CMAKE_BINARY_DIR}/COPYING.txt" COPYONLY)
	SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_BINARY_DIR}/COPYING.txt")
ELSE()
	SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING")
ENDIF(APPLE)

SET(CPACK_SOURCE_GENERATOR             TGZ)
SET(CPACK_SOURCE_IGNORE_FILES          "\\\\.git;~$;build/")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME     ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION})

# Set components
SET(CPACK_COMPONENTS_ALL sharedlibs staticlibs headers)

SET(CPACK_COMPONENT_SHAREDLIBS_DISPLAY_NAME "Shared libraries")
SET(CPACK_COMPONENT_SHAREDLIBS_DESCRIPTION
	"Shared library for general use.")

SET(CPACK_COMPONENT_STATICLIBS_DISPLAY_NAME "Static libraries")
SET(CPACK_COMPONENT_STATICLIBS_DESCRIPTION
	"Static library, good if you want to embed VMime in your application.")

SET(CPACK_COMPONENT_HEADERS_DISPLAY_NAME "C++ Headers")
SET(CPACK_COMPONENT_HEADERS_DESCRIPTION
	"C/C++ header files.")

SET(CPACK_COMPONENT_SHAREDLIBS_GROUP "Development")
SET(CPACK_COMPONENT_STATICLIBS_GROUP "Development")
SET(CPACK_COMPONENT_HEADERS_GROUP    "Development")
SET(CPACK_COMPONENT_GROUP_DEVELOPMENT_DESCRIPTION "")

# Make a target "dist" to generate tarball
SET(ARCHIVE_NAME ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION})

ADD_CUSTOM_TARGET(
	dist
	COMMAND git archive --prefix=${ARCHIVE_NAME}/ HEAD
	| bzip2 > ${CMAKE_BINARY_DIR}/${ARCHIVE_NAME}.tar.bz2
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

# PkgConfig
SET(prefix      ${CMAKE_INSTALL_PREFIX})
SET(exec_prefix ${CMAKE_INSTALL_PREFIX}/bin)
SET(includedir  ${CMAKE_INSTALL_PREFIX}/include/vmime)

SET(VMIME_PKGCONFIG_LIBS     "")
SET(VMIME_PKGCONFIG_CFLAGS   "")
SET(VMIME_PKGCONFIG_REQUIRES "")

IF(${UNIX})
	SET(libdir ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX})
ENDIF(${UNIX})
IF(${WIN32})
	SET(libdir ${CMAKE_INSTALL_PREFIX}/bin)
ENDIF(${WIN32})

CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/libvmime.pc.in ${CMAKE_BINARY_DIR}/libvmime.pc @ONLY)
INSTALL(FILES ${CMAKE_BINARY_DIR}/libvmime.pc DESTINATION lib${LIB_SUFFIX}/pkgconfig COMPONENT headers)


##############################################################################
# Build type

IF(NOT CMAKE_BUILD_TYPE)
	SET(
		CMAKE_BUILD_TYPE
		"Debug"
		CACHE STRING
		"Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
		FORCE
	)
ENDIF(NOT CMAKE_BUILD_TYPE)

# Set a default build type for single-configuration
# CMake generators if no build type is set.
IF(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE Debug)
ENDIF()

# Debug build
MESSAGE("-- Build type: ${CMAKE_BUILD_TYPE}")
IF(${CMAKE_BUILD_TYPE} STREQUAL Debug)
	ADD_DEFINITIONS(-DDEBUG)
ENDIF(${CMAKE_BUILD_TYPE} STREQUAL Debug)


IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
	SET(VMIME_DEBUG 1)
ELSE()
	SET(VMIME_DEBUG 0)
ENDIF()


##############################################################################
# Test endianness and basic type sizes

INCLUDE(TestBigEndian)
TEST_BIG_ENDIAN(BIGENDIAN)

IF(BIGENDIAN EQUAL 0)
	set(VMIME_BYTE_ORDER_BIG_ENDIAN     0)
	set(VMIME_BYTE_ORDER_LITTLE_ENDIAN  1)
ELSE(BIGENDIAN EQUAL 0)
	SET(VMIME_BYTE_ORDER_BIG_ENDIAN     1)
	SET(VMIME_BYTE_ORDER_LITTLE_ENDIAN  0)
ENDIF(BIGENDIAN EQUAL 0)

SET(VMIME_DEFAULT_8BIT_TYPE  "char")
SET(VMIME_DEFAULT_16BIT_TYPE "short")
SET(VMIME_DEFAULT_32BIT_TYPE "int")


SET(
	VMIME_8BIT_TYPE
	${VMIME_DEFAULT_8BIT_TYPE}
	CACHE STRING
	"The C-language 8-bit type for your platform"
)

SET(
	VMIME_16BIT_TYPE
	${VMIME_DEFAULT_16BIT_TYPE}
	CACHE STRING
	"The C-language 16-bit type for your platform"
)

SET(
	VMIME_32BIT_TYPE
	${VMIME_DEFAULT_32BIT_TYPE}
	CACHE STRING
	"The C-language 32-bit type for your platform"
)


INCLUDE(cmake/TargetArch.cmake)
TARGET_ARCHITECTURE(CMAKE_TARGET_ARCHITECTURES)


##############################################################################
# Sendmail path

FOREACH (SENDMAIL_PATH /usr/sbin/sendmail /usr/lib/sendmail /usr/bin/sendmail /bin/sendmail /var/qmail/bin/qmail-inject /bin/cgimail)
	IF(EXISTS ${SENDMAIL_PATH})
		MESSAGE(STATUS "Sendmail binary found at ${SENDMAIL_PATH}")
		SET(VMIME_DEFAULT_SENDMAIL_PATH ${SENDMAIL_PATH})
	ENDIF()
ENDFOREACH(SENDMAIL_PATH)

SET(
	VMIME_SENDMAIL_PATH
	${VMIME_DEFAULT_SENDMAIL_PATH}
	CACHE
	STRING
	"Specifies the path to sendmail binary"
)


##############################################################################
# Messaging features

# Module
OPTION(
	VMIME_HAVE_MESSAGING_FEATURES
	"Enable messaging features (connection to IMAP, POP3, SMTP...)"
	ON
)

# Protocols
OPTION(
	VMIME_HAVE_MESSAGING_PROTO_POP3
	"Enable POP3 protocol"
	ON
)

OPTION(
	VMIME_HAVE_MESSAGING_PROTO_SMTP
	"Enable SMTP protocol"
	ON
)

OPTION(
	VMIME_HAVE_MESSAGING_PROTO_IMAP
	"Enable IMAP protocol"
	ON
)

OPTION(
	VMIME_HAVE_MESSAGING_PROTO_MAILDIR
	"Enable Maildir protocol"
	ON
)

OPTION(
	VMIME_HAVE_MESSAGING_PROTO_SENDMAIL
	"Enable Sendmail protocol"
	ON
)


##############################################################################
# File-system features

OPTION(
	VMIME_HAVE_FILESYSTEM_FEATURES
	"Enable file-system features (required for file attachments and Maildir)"
	ON
)


##############################################################################
# SASL support

INCLUDE(cmake/FindGSasl.cmake)

OPTION(
	VMIME_HAVE_SASL_SUPPORT
	"Enable SASL support (requires GNU SASL library)"
	ON
)

IF(VMIME_HAVE_SASL_SUPPORT)

	INCLUDE_DIRECTORIES(
		${INCLUDE_DIRECTORIES}
		${GSASL_INCLUDE_DIR}
	)

	TARGET_LINK_LIBRARIES(
		${VMIME_LIBRARY_NAME}
		${TARGET_LINK_LIBRARIES}
		${GSASL_LIBRARIES}
	)

	SET(VMIME_PKGCONFIG_LIBS "${VMIME_PKGCONFIG_LIBS} ${GSASL_LIBRARIES}")
	SET(VMIME_PKGCONFIG_CFLAGS "${VMIME_PKGCONFIG_CFLAGS} ${GSASL_INCLUDE_DIR}")
	SET(VMIME_PKGCONFIG_REQUIRES "${VMIME_PKGCONFIG_REQUIRES} libgsasl")

ENDIF()


##############################################################################
# SSL/TLS support

INCLUDE(FindGnuTLS)
INCLUDE(FindOpenSSL)


INCLUDE(CheckFunctionExists)
SET(CMAKE_REQUIRED_LIBRARIES "${GNUTLS_LIBRARY}")
CHECK_FUNCTION_EXISTS(gnutls_priority_set_direct VMIME_HAVE_GNUTLS_PRIORITY_FUNCS)


OPTION(
	VMIME_HAVE_TLS_SUPPORT
	"SSL/TLS support (requires either GNU TLS or OpenSSL library)"
	ON
)

OPTION(
	VMIME_TLS_SUPPORT_LIB_IS_GNUTLS
	"Use GNU TLS library for SSL/TLS support"
	ON
)

OPTION(
	VMIME_TLS_SUPPORT_LIB_IS_OPENSSL
	"Use OpenSSL library for SSL/TLS support"
	OFF
)

IF(VMIME_HAVE_TLS_SUPPORT)

	IF(VMIME_TLS_SUPPORT_LIB_IS_GNUTLS)

		INCLUDE_DIRECTORIES(
			${INCLUDE_DIRECTORIES}
			${GNUTLS_INCLUDE_DIR}
		)

		LINK_DIRECTORIES(
			${LINK_DIRECTORIES}
			${GNUTLS_LIBRARY_DIRS}
		)

		TARGET_LINK_LIBRARIES(
			${VMIME_LIBRARY_NAME}
			${TARGET_LINK_LIBRARIES}
			${GNUTLS_LIBRARY}
		)

		SET(VMIME_PKGCONFIG_LIBS "${VMIME_PKGCONFIG_LIBS} ${GNUTLS_LIBRARY_DIRS} ${GNUTLS_LIBRARY}")
		SET(VMIME_PKGCONFIG_CFLAGS "${VMIME_PKGCONFIG_CFLAGS} ${GNUTLS_INCLUDE_DIR}")
		SET(VMIME_PKGCONFIG_REQUIRES "${VMIME_PKGCONFIG_REQUIRES} libgnutls")

	ELSEIF(VMIME_TLS_SUPPORT_LIB_IS_OPENSSL)

		INCLUDE_DIRECTORIES(
			${INCLUDE_DIRECTORIES}
			${OPENSSL_INCLUDE_DIR}
		)

		TARGET_LINK_LIBRARIES(
			${VMIME_LIBRARY_NAME}
			${TARGET_LINK_LIBRARIES}
			${OPENSSL_LIBRARIES}
		)

		SET(VMIME_PKGCONFIG_LIBS "${VMIME_PKGCONFIG_LIBS} ${OPENSSL_LIBRARIES}")
		SET(VMIME_PKGCONFIG_CFLAGS "${VMIME_PKGCONFIG_CFLAGS} ${OPENSSL_INCLUDE_DIR}")
		SET(VMIME_PKGCONFIG_REQUIRES "${VMIME_PKGCONFIG_REQUIRES} libopenssl")

	ELSE()

		MESSAGE(FATAL_ERROR "TLS support is enabled, but no TLS/SSL library was selected/found")

	ENDIF()

ENDIF(VMIME_HAVE_TLS_SUPPORT)


##############################################################################
# iconv

INCLUDE(cmake/FindIconv.cmake)

INCLUDE_DIRECTORIES(
	${INCLUDE_DIRECTORIES}
	${ICONV_INCLUDE_DIR}
)

TARGET_LINK_LIBRARIES(
	${VMIME_LIBRARY_NAME}
	${TARGET_LINK_LIBRARIES}
	${ICONV_LIBRARIES}
)


##############################################################################
# Platform

SET(VMIME_PLATFORM_IS_POSIX OFF)
SET(VMIME_PLATFORM_IS_WINDOWS OFF)

IF(WIN32)
	SET(VMIME_PLATFORM_IS_WINDOWS ON)
ELSE()
	SET(VMIME_PLATFORM_IS_POSIX ON)
ENDIF()


##############################################################################
# POSIX-specific checks

INCLUDE(CheckFunctionExists)
INCLUDE(CheckSymbolExists)

CHECK_FUNCTION_EXISTS(getaddrinfo VMIME_HAVE_GETADDRINFO)

CHECK_FUNCTION_EXISTS(gettid VMIME_HAVE_GETTID)
CHECK_FUNCTION_EXISTS(syscall VMIME_HAVE_SYSCALL)
CHECK_SYMBOL_EXISTS(SYS_gettid sys/syscall.h VMIME_HAVE_SYSCALL_GETTID)

FIND_PACKAGE(Threads)
FIND_LIBRARY(PTHREAD pthread)

IF(PTHREAD)
	SET(VMIME_HAVE_PTHREAD 1)
ELSE(PTHREAD)
	SET(VMIME_HAVE_PTHREAD 0)
ENDIF(PTHREAD)


##############################################################################
# Additional compiler flags

IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")

	SET(
		CMAKE_CXX_FLAGS
		"${CMAKE_CXX_FLAGS} -D_REENTRANT=1 -W -Wall -pedantic -Warray-bounds-pointer-arithmetic -Wold-style-cast -Wconversion -Wcast-align -Wno-sign-conversion"
		CACHE STRING
		"g++ Compiler Flags"
		FORCE
	)

	SET(CMAKE_CXX_FLAGS_RELEASE "-O2")
	SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
	SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

	#SET(CMAKE_EXE_LINKER_FLAGS "-s")

ELSE()

	IF(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)

		SET(
			CMAKE_CXX_FLAGS
			"${CMAKE_CXX_FLAGS} -D_REENTRANT=1 -W -Wall -ansi -pedantic -Wpointer-arith -Wold-style-cast -Wconversion -Wcast-align"
			CACHE STRING
			"g++ Compiler Flags"
			FORCE
		)

		SET(CMAKE_CXX_FLAGS_RELEASE "-O2")
		SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
		SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

		#SET(CMAKE_EXE_LINKER_FLAGS "-s")

	ENDIF()

ENDIF()


##############################################################################
# Documentation

FIND_PACKAGE(Doxygen)

IF(DOXYGEN_FOUND)
	CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/Doxyfile.in ${CMAKE_BINARY_DIR}/Doxyfile @ONLY)

	# Make a target so that documentation can be generated by running "make doc"
	ADD_CUSTOM_TARGET(
		doc
		${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/Doxyfile
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		COMMENT "Generating API documentation with Doxygen" VERBATIM
	)
ENDIF(DOXYGEN_FOUND)


##############################################################################
# Sanity checks

# Maildir protocol is available only if file-system features are enabled
IF(VMIME_HAVE_MESSAGING_FEATURES AND VMIME_HAVE_MESSAGING_PROTO_MAILDIR AND NOT VMIME_HAVE_FILESYSTEM_FEATURES)
	MESSAGE(FATAL_ERROR "Maildir protocol requires file-system support (VMIME_HAVE_FILESYSTEM_FEATURES must be set to ON).")
ENDIF()

# Path to 'sendmail' must be specified if Sendmail protocol is enabled
IF(VMIME_HAVE_MESSAGING_PROTO_SENDMAIL)
	IF(NOT VMIME_SENDMAIL_PATH OR VMIME_SENDMAIL_PATH STREQUAL "")
		MESSAGE(FATAL_ERROR "Enabling Sendmail protocol requires that you specify path to 'sendmail' binary.")
	ENDIF()
ENDIF()


# Set our configure file
CONFIGURE_FILE(cmake/config.hpp.cmake vmime/config.hpp)

INCLUDE(CPack)

