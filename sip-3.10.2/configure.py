# This script handles the SIP configuration and generates the Makefiles.
#
# Copyright (c) 2004
# 	Riverbank Computing Limited <info@riverbankcomputing.co.uk>
# 
# This file is part of SIP.
# 
# This copy of SIP is licensed for use under the terms of the SIP License
# Agreement.  See the file LICENSE for more details.
# 
# SIP is supplied WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.


import sys
import os
import string
import glob
import getopt

import siputils


# Initialise the globals.
sip_version = 0x030a02
sip_version_str = "3.10.2"
qt_version = 0
qt_edition = ""
qt_winconfig = ""
qt_threaded = 0
qt_dir = None
qt_incdir = None
qt_libdir = None
py_version = sys.hexversion >> 8
plat_py_site_dir = None
plat_py_inc_dir = None
plat_py_lib_dir = None
plat_sip_dir = None
plat_bin_dir = None
platform_specs = []
default_platform = None

# Command line options.
opt_platform = None
opt_qtlib = None
opt_sipbindir = None
opt_sipmoddir = None
opt_sipincdir = None
opt_sipsipdir = None
opt_static = 0
opt_debug = 0

# The names of build macros extracted from the platform specific configuration
# files.
build_macros = [
    "DEFINES",
    "CC",
    "CFLAGS",
    "CFLAGS_RELEASE", "CFLAGS_DEBUG",
    "CFLAGS_SHLIB", "CFLAGS_THREAD",
    "CFLAGS_MT", "CFLAGS_MT_DBG", "CFLAGS_MT_DLL", "CFLAGS_MT_DLLDBG",
    "CFLAGS_EXCEPTIONS_ON", "CFLAGS_EXCEPTIONS_OFF",
    "CFLAGS_RTTI_ON", "CFLAGS_RTTI_OFF",
    "CFLAGS_STL_ON", "CFLAGS_STL_OFF",
    "CFLAGS_WARN_ON", "CFLAGS_WARN_OFF",
    "CHK_DIR_EXISTS", "COPY",
    "CXX",
    "CXXFLAGS",
    "CXXFLAGS_RELEASE", "CXXFLAGS_DEBUG",
    "CXXFLAGS_SHLIB", "CXXFLAGS_THREAD",
    "CXXFLAGS_MT", "CXXFLAGS_MT_DBG", "CXXFLAGS_MT_DLL", "CXXFLAGS_MT_DLLDBG",
    "CXXFLAGS_EXCEPTIONS_ON", "CXXFLAGS_EXCEPTIONS_OFF",
    "CXXFLAGS_RTTI_ON", "CXXFLAGS_RTTI_OFF",
    "CXXFLAGS_STL_ON", "CXXFLAGS_STL_OFF",
    "CXXFLAGS_WARN_ON", "CXXFLAGS_WARN_OFF",
    "DEL_FILE",
    "EXTENSION_SHLIB",
    "INCDIR", "INCDIR_X11", "INCDIR_QT", "INCDIR_OPENGL",
    "LINK", "LINK_SHLIB", "AIX_SHLIB", "LINK_SHLIB_CMD",
    "LFLAGS", "LFLAGS_CONSOLE", "LFLAGS_CONSOLE_DLL", "LFLAGS_DEBUG",
    "LFLAGS_PLUGIN", "LFLAGS_RELEASE", "LFLAGS_SHLIB", "LFLAGS_SONAME",
    "LFLAGS_THREAD", "LFLAGS_WINDOWS", "LFLAGS_WINDOWS_DLL", "LFLAGS_OPENGL",
    "LIBDIR", "LIBDIR_X11", "LIBDIR_QT", "LIBDIR_OPENGL",
    "LIBS", "LIBS_CONSOLE", "LIBS_OPENGL", "LIBS_OPENGL", "LIBS_THREAD",
    "LIBS_WINDOWS", "LIBS_X11",
    "MAKEFILE_GENERATOR",
    "MKDIR", "MOC",
    "RPATH",
    "AR", "RANLIB", "LIB"
]


def usage(rcode = 2):
    """Display a usage message and exit.

    rcode is the return code passed back to the calling process.
    """
    print "Usage:"
    print "    python configure.py [-h] [-b dir] [-d dir] [-e dir] [-k] [-l Qt-library] [-p platform] [-u] [-v dir] [-x] option=value option+=value ..."
    print "where:"
    print "    -h             display this help message"
    print "    -b dir         where the SIP code generator will be installed [default %s]" % opt_sipbindir
    print "    -d dir         where the SIP module will be installed [default %s]" % opt_sipmoddir
    print "    -e dir         where the SIP header file will be installed [default %s]" % opt_sipincdir
    print "    -k             build the SIP module as a static library"
    print "    -l Qt-library  explicitly specify the type of Qt library, either qt, qt-mt, qte or qtmt"
    print "    -p platform    the platform/compiler configuration [default the Qt configuration if enabled or %s]" % default_platform
    print "    -u             build with debugging symbols"
    print "    -v dir         where .sip files are normally installed [default %s]" % opt_sipsipdir
    print "    -x             Qt support is disabled"

    # Display the different platform/compilers.
    print
    print "The following platform/compiler configurations are supported:"
    print

    platform_specs.sort()

    ss = ""
    sep = ""
    for s in platform_specs:
        ss = ss + sep + s
        sep = ", "

    print siputils.format(ss, 2)

    # Display the different build macros.
    print
    print "The following options may be used to adjust the compiler configuration:"
    print

    build_macros.sort()

    ss = ""
    sep = ""
    for s in build_macros:
        ss = ss + sep + s
        sep = " "

    print siputils.format(ss, 2)

    sys.exit(rcode)


def set_defaults():
    """Set up the defaults for values that can be set on the command line.
    """
    global default_platform, qt_dir
    global opt_sipbindir, opt_sipmoddir, opt_sipincdir, opt_sipsipdir

    # Set the platform specific default specification.
    platdefaults = {
        "aix":          "aix-xlc",
        "bsd":          "bsdi-g++",
        "cygwin":       "cygwin-g++",
        "darwin":       "macx-g++",
        "dgux":         "dgux-g++",
        "freebsd":      "freebsd-g++",
        "gnu":          "hurd-g++",
        "hp-ux":        "hpux-acc",
        "irix":         "irix-cc",
        "linux":        "linux-g++",
        "lynxos":       "lynxos-g++",
        "netbsd":       "netbsd-g++",
        "openbsd":      "openbsd-g++",
        "openunix":     "unixware-cc",
        "osf1":         "tru64-cxx",
        "qnx":          "qnx-g++",
        "reliantunix":  "reliant-cds",
        "sco_sv":       "sco-cc",
        "sinix":        "reliant-cds",
        "sunos5":       "solaris-cc",
        "ultrix":       "ultrix-g++",
        "unix_sv":      "unixware-g++",
        "unixware":     "unixware-cc",
        "win32":        "win32-msvc"
    }

    default_platform = "none"

    for pd in platdefaults.keys():
        if sys.platform[:len(pd)] == pd:
            default_platform = platdefaults[pd]
            break

    # If the environment variable is set assume that Qt is installed and
    # support for it enabled.
    try:
        qt_dir = os.environ["QTDIR"]
    except KeyError:
        pass

    opt_sipbindir = plat_bin_dir
    opt_sipmoddir = plat_py_site_dir
    opt_sipincdir = plat_py_inc_dir
    opt_sipsipdir = plat_sip_dir


def inform_user():
    """Tell the user the option values that are going to be used.
    """
    siputils.inform("The SIP code generator will be installed in %s." % opt_sipbindir)
    siputils.inform("The SIP module will be installed in %s." % opt_sipmoddir)
    siputils.inform("The SIP header file will be installed in %s." % opt_sipincdir)
    siputils.inform("The default directory to install .sip files in is %s." % opt_sipsipdir)

    if qt_version:
        if qt_edition:
            edstr = qt_edition + " edition "
        else:
            edstr = ""

        siputils.inform("Qt v%s %sis being used." % (siputils.version_to_string(qt_version), edstr))
        siputils.inform("The Qt header files are in %s." % qt_incdir)
        siputils.inform("The %s Qt library is in %s." % (opt_qtlib, qt_libdir))
    else:
        siputils.inform("Qt support is disabled.")

    if opt_platform:
        siputils.inform("The platform/compiler configuration is %s." % opt_platform)
    else:
        siputils.inform("Qt's default platform/compiler configuration will be used.")


def set_platform_directories():
    """Initialise the global variables relating to platform specific
    directories.
    """
    global plat_py_site_dir, plat_py_inc_dir, plat_bin_dir, plat_sip_dir
    global plat_py_lib_dir

    if sys.platform == "win32":
        plat_py_site_dir = sys.prefix + "\\Lib"
        if py_version >= 0x020200:
            plat_py_site_dir = plat_py_site_dir + "\\site-packages"

        plat_py_inc_dir = sys.prefix + "\\include"
        plat_py_lib_dir = sys.prefix + "\\libs"
        plat_bin_dir = sys.exec_prefix
        plat_sip_dir = sys.prefix + "\\sip"
    else:
        vers = "%d.%d" % ((py_version >> 16) & 0xff, (py_version >> 8) & 0xff)

        plat_py_site_dir = sys.prefix + "/lib/python" + vers
        if py_version >= 0x020000:
            plat_py_site_dir = plat_py_site_dir + "/site-packages"

        plat_py_inc_dir = sys.prefix + "/include/python" + vers
        plat_py_lib_dir = sys.prefix + "/lib/python" + vers + "/config"
        plat_bin_dir = sys.exec_prefix + "/bin"
        plat_sip_dir = sys.prefix + "/share/sip"


def create_config(module, template, macros):
    """Create the SIP configuration module so that it can be imported by build
    scripts.

    module is the module file name.
    template is the template file name.
    macros is the dictionary of build macros.
    """
    siputils.inform("Creating %s..." % module)

    content = {
        "sip_version":      sip_version,
        "sip_version_str":  sip_version_str,
        "sip_bin":          os.path.join(opt_sipbindir, "sip"),
        "sip_inc_dir":      opt_sipincdir,
        "sip_mod_dir":      opt_sipmoddir,
        "default_bin_dir":  plat_bin_dir,
        "default_mod_dir":  plat_py_site_dir,
        "default_sip_dir":  opt_sipsipdir,
        "py_version":       py_version,
        "py_inc_dir":       plat_py_inc_dir,
        "py_lib_dir":       plat_py_lib_dir,
        "qt_version":       qt_version
    }

    if qt_version:
        content["qt_edition"] = qt_edition
        content["qt_winconfig"] = qt_winconfig
        content["qt_threaded"] = qt_threaded
        content["qt_dir"] = qt_dir
        content["qt_inc_dir"] = qt_incdir
        content["qt_lib"] = opt_qtlib
        content["qt_lib_dir"] = qt_libdir

    # Write the (nicely formatted) dictionary.
    content = siputils.create_content(content) + "_default_macros = {\n"

    names = macros.keys()
    names.sort()

    width = 0
    for c in names:
        clen = len(c)
        if width < clen:
            width = clen

    for c in names:
        if c == names[-1]:
            sep = ""
        else:
            sep = ","

        k = "'" + c + "':"
        content = content + "    %-*s  '%s'%s\n" % (1 + width + 2, k, string.replace(macros[c], "\\", "\\\\"), sep)

    content = content + "}\n"

    siputils.create_config_module(module, template, content)


def check_qt_installation(macros):
    """Check the Qt installation and get the version number and edition.

    macros is the dictionary of macros.
    """
    # Get the Makefile generator.
    try:
        generator = macros["MAKEFILE_GENERATOR"]
    except KeyError:
        generator = "UNIX"

    # Check the Qt header files have been installed.
    global qt_incdir

    try:
        qt_incdir = macros["INCDIR_QT"]
    except KeyError:
        qt_incdir = os.path.join(qt_dir, "include")

    qglobal = os.path.join(qt_incdir, "qglobal.h")

    if not os.access(qglobal, os.F_OK):
        siputils.error("qglobal.h could not be found in %s." % qt_incdir)

    # Get the Qt version number.
    global qt_version

    qt_version, ignore = siputils.read_version(qglobal, "Qt", "QT_VERSION")

    # Early versions of Qt for the Mac didn't include everything.  Rather than
    # maintain these in the future we just mandate a later version.
    if sys.platform == "darwin" and qt_version < 0x030100:
        siputils.error("PyQt for MacOS/X requires Qt v3.1.0 or later.")

    # Try and work out which edition it is.
    global qt_edition

    if qt_version >= 0x030000:
        qconfig = os.path.join(qt_incdir, "qconfig.h")

        if not os.access(qconfig,os.F_OK):
            siputils.error("qconfig.h could not be found in %s." % qt_incdir)

        f = open(qconfig)
        l = f.readline()

        while l:
            wl = string.split(l)
            if len(wl) == 3 and wl[0] == "#define" and wl[1] == "QT_PRODUCT_LICENSE":
                    qt_edition = wl[2][4:-1]
                    break

            l = f.readline()

        f.close()

        if not qt_edition:
            siputils.error("The Qt edition could not be determined by parsing %s." % qconfig)

    # See which version of the Qt library can be found.  (We can't trust the
    # configuration files.)
    global qt_libdir, opt_qtlib

    try:
        qt_libdir = macros["LIBDIR_QT"]
    except KeyError:
        qt_libdir = os.path.join(qt_dir, "lib")

    if opt_qtlib:
        if not is_qt_library(generator, opt_qtlib):
            siputils.error("The %s Qt library could not be found in %s." % (opt_qtlib, qt_libdir))
    else:
        stlib = is_qt_library(generator, "qt")
        mtlib = is_qt_library(generator, "qt-mt")
        edlib = is_qt_library(generator, "qt-mtedu")
        evlib = is_qt_library(generator, "qt-mteval")
        emlib = is_qt_library(generator, "qte")

        # Borland likes to be a little different.
        bmtlib = is_qt_library(generator, "qtmt")
        bedlib = is_qt_library(generator, "qtmtedu")

        names = []

        if stlib:
            opt_qtlib = "qt"
            names.append(opt_qtlib)

        if mtlib:
            opt_qtlib = "qt-mt"
            names.append(opt_qtlib)

        if edlib:
            opt_qtlib = "qt-mtedu"
            names.append(opt_qtlib)

        if evlib:
            opt_qtlib = "qt-mteval"
            names.append(opt_qtlib)

        if emlib:
            opt_qtlib = "qte"
            names.append(opt_qtlib)

        if bmtlib:
            opt_qtlib = "qtmt"
            names.append(opt_qtlib)

        if bedlib:
            opt_qtlib = "qtmtedu"
            names.append(opt_qtlib)

        if not names:
            siputils.error("No Qt libraries could be found in %s." % qt_libdir)

        if len(names) > 1:
            siputils.error("These Qt libraries were found: %s. Use the -l argument to explicitly specify which you want to use." % string.join(names))

    # See if Qt has thread support.
    if opt_qtlib in ("qt-mt", "qt-mtedu", "qt-mteval", "qtmt", "qtmtedu"):
        global qt_threaded
        qt_threaded = 1

    # Try and work out how Qt was built on Windows.
    if sys.platform == "win32":
        global qt_winconfig

        try:
            f = open(os.path.join(qt_dir, ".qtwinconfig"), "r")
        except IOError:
            f = None

        if f:
            cfg = f.readline()
            f.close()

            val = string.find(cfg, "=")

            if val >= 0:
                qt_winconfig = string.strip(cfg[val + 1:])
        else:
            # Assume it was built as a DLL.
            qt_winconfig = "shared"


def is_qt_library(generator, lib):
    """See if a particular Qt library is installed.

    generator is the name of the Makefile generator.
    lib is the name of the library.
    """
    if generator != "UNIX" and generator != "MINGW":
        lpatts = [lib + "[0-9]*.lib", lib + ".lib"]
    else:
        lpatts = ["lib" + lib + ".*"]

    for lpatt in lpatts:
        lmatch = glob.glob(os.path.join(qt_libdir, lpatt))

        if lmatch:
            return lmatch

    return []


def get_build_macros(overrides):
    """Return a dictionary of build macros extracted from the user's Qt
    configuration, or what we bundle with SIP.

    overrides is the dictionary of modified build macros.
    """
    macro_prefix = "QMAKE_"

    # Get the name of the file we want to read the macros from.
    if opt_platform:
        fname = os.path.join("specs", opt_platform)
    elif "QMAKESPEC" in os.environ.keys():
        fname = os.path.join(qt_dir, "mkspecs", os.environ["QMAKESPEC"], "qmake.conf")
    else:
        qname = os.path.join(qt_dir, "mkspecs", "default", "qmake.conf")

        if os.access(qname, os.F_OK):
            fname = qname
        else:
            try:
                tmakepath = os.environ["TMAKEPATH"]
            except KeyError:
                siputils.error("This version of Qt seems to use tmake but the TMAKEPATH environment is not set.")

            tname = os.path.join(tmakepath, "tmake.conf")

            if not os.access(tname, os.F_OK):
                siputils.error("Unable to find either %s for qmake or %s for tmake." % (qname, tname))

            fname = tname
            macro_prefix = "TMAKE_"

    try:
        f = open(fname, "r")
    except IOError, detail:
        siputils.error("Unable to open %s: %s" % (fname, detail))

    # Get everything into a dictionary.
    raw = {
        "LITERAL_WHITESPACE":   " ",
        "LITERAL_DOLLAR":       "$",
        "LITERAL_HASH":         "#"
    }

    line = f.readline()
    while line:
        # Handle line continuations.
        while len(line) > 1 and line[-2] == "\\":
            line = line[:-2]

            next = f.readline()

            if next:
                line = line + next
            else:
                break

        line = string.strip(line)

        # Ignore comments.
        if line and line[0] != "#":
            assstart = string.find(line, "+")
            if assstart > 0 and line[assstart + 1] == '=':
                assend = assstart + 1
            else:
                assstart = string.find(line, "=")
                assend = assstart

            if assstart > 0:
                lhs = string.strip(line[:assstart])
                rhs = string.strip(line[assend + 1:])

                raw[lhs] = rhs

        line = f.readline()

    # Go through the raw dictionary extracting the macros we need and resolving
    # any macro expansions.  First of all, make sure every macro has a value.
    refined = {}

    for m in build_macros:
        refined[m] = ""

    for lhs in raw.keys():
        # Strip any prefix.
        if string.find(lhs, macro_prefix) == 0:
            reflhs = lhs[len(macro_prefix):]
        else:
            reflhs = lhs

        # See if we are interested in this one.
        if reflhs not in build_macros:
            continue

        rhs = raw[lhs]

        # Resolve any references.
        mstart = string.find(rhs, "$$")

        while mstart >= 0:
            rstart = mstart + 2
            if rstart < len(rhs) and rhs[rstart] == "{":
                rstart = rstart + 1
                term = "}"
            else:
                term = string.whitespace

            mend = rstart
            while mend < len(rhs) and rhs[mend] not in term:
                mend = mend + 1

            lhs = rhs[rstart:mend]

            if term == "}":
                mend = mend + 1

            try:
                rhs = rhs[:mstart] + raw[lhs] + rhs[mend:]
            except KeyError:
                siputils.error("%s: macro '%s' is not defined." % (fname, lhs))

            mstart = string.find(rhs, "$$")

        # Expand any POSIX style environment variables.
        estart = string.find(rhs, "$(")

        while estart >= 0:
            eend = string.find(rhs[estart + 2:], ")")

            if eend < 0:
                break

            eend = estart + 2 + eend

            name = rhs[estart + 2:eend]

            try:
                env = os.environ[name]
            except KeyError:
                env = ""

            rhs = rhs[:estart] + env + rhs[eend + 1:]

            estart = string.find(rhs, "$(")

        # Expand any Windows style environment variables.
        estart = string.find(rhs, "%")

        while estart >= 0:
            eend = string.find(rhs[estart + 1:], "%")

            if eend < 0:
                break

            eend = estart + 1 + eend

            name = rhs[estart + 1:eend]

            try:
                env = os.environ[name]
            except KeyError:
                env = ""

            rhs = rhs[:estart] + env + rhs[eend + 1:]

            estart = string.find(rhs, "%")

        refined[reflhs] = rhs

    # Handle the user overrides.
    for lhs in overrides.keys():
        rhs = refined[lhs]
        oride = overrides[lhs]

        if string.find(oride, "+") == 0:
            if rhs:
                rhs = rhs + " " + oride[1:]
            else:
                rhs = oride[1:]
        else:
            rhs = oride

        refined[lhs] = rhs

    return refined


def create_makefiles():
    """Create the Makefiles.
    """
    # Bootstrap.
    import sipconfig

    cfg = sipconfig.Configuration()

    sipconfig.inform("Creating top level Makefile...")

    sipconfig.ParentMakefile(
        configuration=cfg,
        subdirs=["sipgen", "siplib"],
        installs=("sipconfig.py", cfg.sip_mod_dir)
    ).generate()

    sipconfig.inform("Creating sip code generator Makefile...")

    sipconfig.ProgramMakefile(
        configuration=cfg,
        build_file="sipgen.sbf",
        dir="sipgen",
        install_dir=os.path.dirname(cfg.sip_bin),
        console=1,
        warnings=0
    ).generate()

    sipconfig.inform("Creating sip module Makefile...")

    hfiles = ["sip.h"]

    if cfg.sip_version < 0x040000:
        hfiles.append("sipQt.h")

    if cfg.sip_version >= 0x040000 or cfg.qt_version:
        sbf = "siplib.sbf"
    else:
        sbf = "siplib-noqt.sbf"

    makefile = sipconfig.ModuleMakefile(
        configuration=cfg,
        build_file=sbf,
        dir="siplib",
        install_dir=cfg.sip_mod_dir,
        installs=(hfiles, cfg.sip_inc_dir),
        qt=cfg.qt_version,
        warnings=0,
        static=opt_static,
        debug=opt_debug
    )

    if cfg.qt_version:
        makefile.extra_defines.append("SIP_QT_SUPPORT")

    if cfg.sip_version < 0x040000 and makefile.generator != "UNIX":
        if opt_static:
            libdef = "SIP_MAKE_STATIC"
        else:
            libdef = "SIP_MAKE_DLL"

        makefile.extra_defines.append(libdef)

    makefile.generate()


def main(argv):
    """Create the configuration module module.

    argv is the list of command line arguments.
    """
    # Basic initialisation.
    set_platform_directories()

    # Build up the list of valid specs.
    for s in os.listdir("specs"):
        platform_specs.append(s)

    # Handle the command line.
    set_defaults()

    try:
        optlist, args = getopt.getopt(argv[1:], "hb:d:e:kl:p:uv:x")
    except getopt.GetoptError:
        usage()

    global qt_dir, opt_sipbindir, opt_sipmoddir, opt_sipincdir, opt_sipsipdir
    global opt_platform, opt_qtlib, opt_static, opt_debug

    for opt, arg in optlist:
        if opt == "-h":
            usage(0)
        elif opt == "-b":
            opt_sipbindir = arg
        elif opt == "-d":
            opt_sipmoddir = arg
        elif opt == "-e":
            opt_sipincdir = arg
        elif opt == "-k":
            opt_static = 1
        elif opt == "-l":
            if arg in ("qt", "qt-mt", "qt-mtedu", "qt-mteval", "qte", "qtmt", "qtmtedu"):
                opt_qtlib = arg
            else:
                usage()
        elif opt == "-p":
            if arg not in platform_specs:
                usage()
            
            opt_platform = arg
        elif opt == "-q":
            opt_qtdir = arg
        elif opt == "-u":
            opt_debug = 1
        elif opt == "-v":
            opt_sipsipdir = arg
        elif opt == "-x":
            qt_dir = None

    if qt_dir is None and opt_platform is None:
        opt_platform = default_platform

    # Allow the user to modify the build macros.
    overrides = {}

    for arg in args:
        prefix = ""
        name_end = string.find(arg, "+=")

        if name_end >= 0:
            prefix = "+"
            val_start = name_end + 2
        else:
            name_end = string.find(arg, "=")

            if name_end >= 0:
                val_start = name_end + 1
            else:
                usage()

        name = arg[:name_end]

        if name not in build_macros:
            usage()

        overrides[name] = prefix + arg[val_start:]

    # Get the platform specific macros for building.
    macros = get_build_macros(overrides)

    siputils.inform("This is SIP %s for Python %s on %s." % (sip_version_str, string.split(sys.version)[0], sys.platform))

    if sip_version >= 0x040000 and py_version < 0x020300:
        siputils.error("This version of SIP requires Python v2.3 or later.")

    # Check the Qt installation.
    if qt_dir:
        check_qt_installation(macros)

    # Tell the user what's been found.
    inform_user()

    # Install the configuration module.
    create_config("sipconfig.py", "siputils.py", macros)

    # Create the Makefiles.
    create_makefiles()


###############################################################################
# The script starts here.
###############################################################################

if __name__ == "__main__":
    try:
        main(sys.argv)
    except SystemExit:
        raise
    except:
        print \
"""An internal error occured.  Please report all the output from the program,
including the following traceback, to support@riverbankcomputing.co.uk.
"""
        raise
