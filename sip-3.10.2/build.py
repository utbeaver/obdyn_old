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
#
# This is the build script for SIP.  It should be run in the top level
# directory of the source distribution and by the Python interpreter for which
# it is being built.  It uses either qmake or tmake to do the hard work of
# generating the platform specific Makefiles.


import os
import string
import glob
import tempfile
import shutil
import py_compile
import re


# Initialise the globals.
sipVersion = 0x030a02
qtDir = None
qtIncDir = ""
qtLibDir = ""
qtVersion = 0
qtEdition = ""
qtLib = ""
platPyIncDir = None
platPyLibDir = None
platPyLib = None
platBinDir = None
platSiteDir = None
platSipDir = None
sipModDir = None
sipIncDir = None
sipBinDir = None
sipSipDir = None
platMake = None
platQTDIRName = None
pyVersNr = None
makefileGen = None
makeBin = None
makeBinPath = None
makefilePatches = {}
tempBuildDir = None
usingTmake = 0
enableOldPython = 0
staticModule = 0
debugMode = "release"
gccFlags = []
bli_config = []
bli_config_lib = None


def usage(rcode = 2):
    """Display a usage message and exit.

    rcode is the return code passed back to the calling process.
    """
    print "Usage:"
    print "    %s [-h] [-b dir] [-d dir] [-e dir] [-f gccflag] [-g prog] [-i dir] [-k] [-l Qt-library] [-m prog] [-p dir] [-q dir] [-r dir] [-t dir] [-u] [-v dir] [-w] [-x]" % (script())
    print "where:"
    print "    -h             display this help message"
    print "    -b dir         where the SIP code generator will be installed [default %s]" % platBinDir
    print "    -d dir         where the SIP module will be installed [default %s]" % platSiteDir
    print "    -e dir         where the SIP header files will be installed [default %s]" % sipIncDir
    print "    -f gccflag     additional GCC flag, eg. -fno-exceptions"
    print "    -g prog        the name of the Makefile generator"
    print "    -i dir         the directory containing the Qt header files [default %s]" % os.path.join(platQTDIRName, "include")
    print "    -k             build the SIP module as a static library"
    print "    -l Qt-library  explicitly specify the type of Qt library, either qt, qt-mt, qt-mtedu or qte"
    print "    -m prog        the name of the Make program [default %s]" % platMake
    print "    -q dir         the Qt base directory [default %s]" % platQTDIRName
    print "    -r dir         the directory containing the Qt library [default %s]" % os.path.join(platQTDIRName, "lib")
    print "    -t dir         the directory containing the Python library directories [default %s]" % (platPyLibDir)
    print "    -u             build with debugging symbols"
    print "    -v dir         where .sip files are normally installed [default %s]" % (platSipDir)
    print "    -w             enable the use of Python 1.5.x under Windows"
    print "    -x             disable Qt support"

    sys.exit(rcode)


#<BootstrapCode>
import sys
import types
import time


_script = None


def script():
    """Return the basename of the build/installation script.
    """
    return _script


def _set_script():
    """Internal: Remember the basename of the build/install script and check
    it is in the current directory.
    """
    global _script

    _script = os.path.basename(sys.argv[0])

    # Check we are in the top level directory.
    if not os.access(_script,os.F_OK):
        error("This program must be run from the top level directory of the package, ie. the directory containing this program.")


def _get_license(package,ltype=None,lname=None):
    """Internal: Returns a tuple of the license type and the name of the
    license file (if any).

    ltype is the default license type, the default default is the GPL.
    lname is the default license name.
    """
    try:
        import license
        ltype = license.LicenseType
        lname = license.LicenseName

        try:
            lfile = license.LicenseFile
        except AttributeError:
            lfile = None
    except ImportError:
        pass

    if ltype is None:
        ltype = "GPL"
        lname = "GNU General Public License"
        lfile = None

    inform("This is the %s version of %s (licensed under the %s) for Python %s on %s." % (ltype,package,lname,string.split(sys.version)[0],sys.platform),0)

    if ltype == "GPL" and sys.platform == "win32":
        error("You cannot use the GPL version of %s under Windows.\n" % (package))

    return (ltype, lfile)


def confirm_license():
    """The user is to be asked to confirm their acceptance of the license.
    There should be a text file called LICENSE in the current directory
    that contains the full text of the license.  If the user doesn't
    accept the license then this method will not return.  Returns the
    type of the license.
    """
    print
    print "Type 'L' to view the license."
    print "Type 'yes' to accept the terms of the license."
    print "Type 'no' to decline the terms of the license."
    print

    while 1:
        try:
            resp = raw_input("Do you accept the terms of the license? ")
        except:
            resp = ""

        resp = string.lower(string.strip(resp))

        if resp == "yes":
            break

        if resp == "no":
            sys.exit(0)

        if resp == "l":
            os.system("more LICENSE")


def _patch_file(name,pdict):
    """Internal: Apply the patches to a file.

    name is the name of the file.
    pdict is the patch dictionary.
    """
    fi = open(name + ".in","r")
    fo = open(name,"w")

    for line in fi.readlines():
        # Patches come in two forms.  In the first, the key is the pattern to
        # be replaced and the value is the replacement text.  In the second,
        # the key is a unique identifier of the patch and the value is a two
        # element list.  The first element is the pattern, and the second
        # element is the replacement text.  When we find either type we convert
        # it to the second type with the additional difference that the pattern
        # is compiled.
        for p in pdict.keys():
            v = pdict[p]

            if type(v) == types.ListType:
                pattern, repl = v

                if type(pattern) == types.StringType:
                    pattern = re.compile(pattern,re.M)
                    pdict[p] = [pattern, repl]
            else:
                pattern = re.compile(p,re.M)
                repl = v
                pdict[p] = [pattern, repl]

            # Escape the replacement again as (if it needs it) it will go into
            # a string.
            line = re.sub(pattern, string.replace(repl, "\\", "\\\\"), line)

        fo.write(line)
    
    fi.close()
    fo.close()


def _create_config(cfgfile,pdict):
    """Internal: Create a configuration module based on a template file and
    bootstrap code in this file.

    cfgfile is the name of the file to create.
    pdict is the patch dictionary.
    """
    inform("Creating the %s configuration module." % (cfgfile))
    _patch_file(cfgfile,pdict)

    # Append the bootstrap code from this file.
    fi = open(_script,"r")
    fo = open(cfgfile,"a")

    bootcode = 0

    bootstart = "#<BootstrapCode>"
    bootend = "#</BootstrapCode>"

    for line in fi.readlines():
        if bootcode:
            if line[:len(bootend)] == bootend:
                bootcode = 0
            else:
                fo.write(line)
        else:
            if line[:len(bootstart)] == bootstart:
                bootcode = 1
                fo.write("\n\n")

    fi.close()
    fo.close()


def _format(mess,delin = 1):
    """Internal: Format a message by inserting line breaks at appropriate
    places.

    mess is the text of the message.
    delin is non-zero if the text should be deliniated.

    Return the formatted message.
    """
    margin = 78
    curs = 0

    if delin:
        msg = "*" * margin + "\n"
    else:
        msg = ""

    for w in string.split(mess):
        l = len(w)
        if curs != 0 and curs + l > margin:
            msg = msg + "\n"
            curs = 0

        if curs > 0:
            msg = msg + " "
            curs = curs + 1

        msg = msg + w
        curs = curs + l

    msg = msg + "\n"

    if delin:
        msg = msg + ("*" * margin) + "\n"

    return msg


def error(mess):
    """Display an error message and exit.

    mess is the text of the message.
    """
    sys.stderr.write(_format("Error: %s" % (mess)))
    sys.exit(1)


def inform(mess,delin = 1):
    """Display an information message.

    mess is the text of the message.
    delin is non-zero if the text should be deliniated.
    """
    sys.stdout.write(_format(mess,delin))


def escape(s):
    """Escape all the backslashes (usually Windows directory separators) in a
    string.  Also enclose in double quotes if the string contains spaces.

    s is the string to escape.

    Returns the escaped string.
    """
    s = string.replace(s, "\\", "\\\\")

    if string.find(s, " ") >= 0:
        s = "\"" + s + "\""

    return s


def version_to_string(v):
    """Convert a 3 part version number encoded as a hexadecimal value to a
    string.
    """
    return "%u.%u.%u" % (((v >> 16) & 0xff), ((v >> 8) & 0xff), (v & 0xff))


def read_version(fname, desc, numdefine=None, strdefine=None):
    """Read the version information for a package from a file. The information
    is specified as #defines of a numeric (hexadecimal or decimal) value and/or
    a string value.

    fname is the name of the file.
    desc is the descriptive name of the package.
    numdefine is the name of the #define of the numeric version. It is ignored
    if it is None.
    strdefine is the name of the #define of the string version. It is ignored
    if it is None.

    Returns a tuple of the version as a number and as a string.
    """
    need_num = numdefine is not None
    need_str = strdefine is not None

    vers = None
    versstr = None

    f = open(fname)
    l = f.readline()

    while l and (need_num or need_str):
        wl = string.split(l)
        if len(wl) >= 3 and wl[0] == "#define":
            if need_num and wl[1] == numdefine:
                v = wl[2]

                if v[0:2] == "0x":
                    vers = string.atoi(v,16)
                else:
                    dec = int(v)
                    maj = dec / 100
                    min = (dec % 100) / 10
                    bug = (dec % 10)
                    vers = (maj << 16) + (min << 8) + bug

                need_num = 0

            if need_str and wl[1] == strdefine:
                versstr = wl[2][1:-1]
                need_str = 0

        l = f.readline()

    f.close()

    if need_num or need_str:
        error("The %s version number could not be determined by parsing %s." % (desc, fname))

    return (vers, versstr)
#</BootstrapCode>


def initGlobals():
    """Initialises the values of globals that need more than a simple
    assignment before the parsing of the command line.
    """
    global pyVersNr

    fullVers = string.split(sys.version)[0]
    vl = string.split(re.findall("[0-9.]*",fullVers)[0],".")
    major = vl[0]
    minor = vl[1]

    try:
        bug = vl[2]
    except IndexError:
        bug = 0

    pyVers = major + "." + minor
    pyVersNr = (int(major) << 16) + (int(minor) << 8) + int(bug)

    global platMake, platQTDIRName, platSipDir, platBinDir, platPyIncDir
    global platPyLibDir, platSiteDir

    if sys.platform == "win32":
        platMake = "nmake"
        platQTDIRName = "%QTDIR%"
        platSipDir = sys.prefix + "\\sip"
        platBinDir = sys.exec_prefix
        platPyIncDir = sys.prefix + "\\include"
        platPyLibDir = sys.prefix

        if pyVersNr < 0x020200:
            platSiteDir = platPyLibDir + "\\Lib"
        else:
            platSiteDir = platPyLibDir + "\\Lib\\site-packages"
    else:
        platMake = "make"
        platQTDIRName = "$QTDIR"
        platSipDir = "/usr/local/share/sip"
        platBinDir = "/usr/local/bin"
        platPyIncDir = sys.prefix + "/include/python" + pyVers
        platPyLibDir = sys.prefix + "/lib/python" + pyVers

        if pyVersNr < 0x020000:
            platSiteDir = platPyLibDir
        else:
            platSiteDir = platPyLibDir + "/site-packages"

    global sipModDir, sipSipDir, sipIncDir, sipBinDir

    sipModDir = platSiteDir
    sipSipDir = platSipDir
    sipIncDir = platPyIncDir
    sipBinDir = platBinDir


def updateGlobals():
    """Updates the values of globals after parsing the command line.
    """
    if sys.platform == "win32":
        global platPyLib

        if debugMode == "debug":
            dbg = "_d"
        else:
            dbg = ""

        platPyLib = platPyLibDir + "\\libs\\python%s%s%s.lib" % (pyVersNr >> 16, (pyVersNr >> 8) & 0xff, dbg)

    global bli_config_lib

    if staticModule:
        bli_config_lib = "staticlib"
    else:
        bli_config_lib = "dll"

    bli_config.append(debugMode)


def isProgram(f):
    """Return non-zero if a filename refers to an executable file.
    """
    return os.access(f,os.X_OK) and not os.path.isdir(f)


def checkPath(prog, xtradir=None):
    """Search the PATH and an optional extra directory for a program.  The
    full pathname of the program is returned.

    prog is the name of the program.
    xtradir is the name of the optional extra directory.
    """
    if sys.platform == "win32":
        base = prog + ".exe"
    else:
        base = prog

    # Create the list of directories to search.
    try:
        path = os.environ["PATH"]
    except KeyError:
        path = os.defpath

    dlist = string.split(path, os.pathsep)

    if xtradir:
        dlist.insert(0, xtradir)

    # Create the list of files to check.
    flist = []

    for d in dlist:
        flist.append(os.path.abspath(os.path.join(d, base)))

    # Search for the file.
    for f in flist:
        if isProgram(f):
            return f

    # Nothing found.
    return None


def isQtLibrary(dir,lib):
    """See if a Qt library appears in a particular directory.

    dir is the name of the directory.
    lib is the name of the library.

    Returns the basename of the library file that was found or None if nothing
    was found.
    """
    if sys.platform == "win32":
        lpatt = "\\" + lib + "[0-9]*.lib"
    else:
        lpatt = "/lib" + lib + ".*"

    l = glob.glob(dir + lpatt)

    if len(l) == 0:
        return None

    return os.path.basename(l[0])


def checkQtDirAndVersion():
    """Check that Qt can be found and what its version is.
    """
    global qtDir

    if qtDir is None:
        try:
            qtDir = os.environ["QTDIR"]
        except KeyError:
            error("Please set the name of the Qt base directory, either by using the -q argument or setting the QTDIR environment variable.")
    else:
        os.environ["QTDIR"] = qtDir

    if not os.access(qtDir,os.F_OK):
        error("The %s Qt base directory does not seem to exist. Use the -q argument or the QTDIR environment variable to set the correct directory or use the -x argument to disable Qt support." % (qtDir))

    inform("%s is the Qt base directory." % (qtDir))

    # Check that the Qt header files have been installed.
    global qtIncDir

    if not qtIncDir:
        qtIncDir = os.path.join(qtDir, "include")

    qglobal = os.path.join(qtIncDir, "qglobal.h")

    if not os.access(qglobal,os.F_OK):
        error("qglobal.h could not be found in %s. Use the -i argument to explicitly specify the correct directory." % (qtIncDir))

    inform("%s contains qglobal.h." % (qtIncDir))

    # Get the Qt version number.
    global qtVersion

    qtVersion, qtversstr = read_version(qglobal, "Qt", "QT_VERSION", "QT_VERSION_STR")

    # Try and work out which Qt edition it is.
    global qtEdition

    qteditionstr = ""

    if qtVersion >= 0x030000:
        qconfig = os.path.join(qtIncDir, "qconfig.h")

        if not os.access(qconfig,os.F_OK):
            error("qconfig.h could not be found in %s." % (qtIncDir))

        f = open(qconfig)
        l = f.readline()

        while l:
            wl = string.split(l)
            if len(wl) == 3 and wl[0] == "#define" and wl[1] == "QT_PRODUCT_LICENSE":
                    qtEdition = wl[2][4:-1]
                    break

            l = f.readline()

        f.close()

        if not qtEdition:
            error("The Qt edition could not be determined by parsing %s." % (qconfig))

        qteditionstr = qtEdition + " edition "

    inform("Qt %s %sis being used." % (qtversstr,qteditionstr))


def checkQtLibrary():
    """Check which Qt library is to be used.
    """
    global qtLibDir

    if not qtLibDir:
        qtLibDir = os.path.join(qtDir, "lib")

    global qtLib

    if qtLib:
        lib = isQtLibrary(qtLibDir, qtLib)

        if not lib:
            error("The %s Qt library could not be found in %s." % (qtLib, qtLibDir))
    else:
        mtlib = isQtLibrary(qtLibDir,"qt-mt")
        stlib = isQtLibrary(qtLibDir,"qt")
        edlib = isQtLibrary(qtLibDir,"qt-mtedu")
        emlib = isQtLibrary(qtLibDir,"qte")

        nrlibs = 0
        names = ""

        if stlib:
            nrlibs = nrlibs + 1
            names = names + ", qt"
            qtLib = "qt"
            lib = stlib

        if mtlib:
            nrlibs = nrlibs + 1
            names = names + ", qt-mt"
            qtLib = "qt-mt"
            lib = mtlib

        if edlib:
            nrlibs = nrlibs + 1
            names = names + ", qt-mtedu"
            qtLib = "qt-mtedu"
            lib = edlib

        if emlib:
            nrlibs = nrlibs + 1
            names = names + ", qte"
            qtLib = "qte"
            lib = emlib

        if nrlibs == 0:
            error("No Qt libraries could be found in %s." % (qtLibDir))

        if nrlibs > 1:
            error("These Qt libraries were found: %s. Use the -l argument to explicitly specify which you want to use." % (names[2:]))

        inform("The %s Qt library was found." % (qtLib))

    if qtLib in ("qt-mt", "qt-mtedu"):
        bli_config.append("thread")
        inform("Qt thread support is enabled.")
    else:
        inform("Qt thread support is disabled.")

    # tmake under Windows doesn't know the version of the Qt library, so we
    # have to patch it.
    if sys.platform == "win32" and usingTmake:
        makefilePatches[TMAKE_WIN] = ["qt.lib", lib]


def checkConfiguration():
    """Carry out basic checks about the installation.
    """
    # We can't be sure that we can run programs under Windows (because it might
    # be 95 or 98) with Python 1.5.x and get the return codes.  So we force the
    # user to explicitly say that's what they want to do.
    if sys.platform == "win32" and pyVersNr < 0x010600:
        if not enableOldPython:
            error("You are using Python %s under Windows. If you are running Windows95 or Windows98 then there may be problems trying to configure PyQt. It is recomended that you upgrade to Python 1.6 or later. Use the -w argument to suppress this check.\n" % (version_to_string(pyVersNr)))

    # Check the installation directory is valid.
    if not os.access(sipModDir,os.F_OK):
        error("The %s SIP module destination directory does not seem to exist. Use the -d argument to set the correct directory." % (sipModDir))

    inform("%s is the SIP module installation directory." % (sipModDir))

    # Check that the Python header files have been installed.
    if not os.access(platPyIncDir + "/Python.h",os.F_OK):
        error("Python.h could not be found in %s. Make sure Python is fully installed, including any development packages." % (platPyIncDir))

    inform("%s contains Python.h." % (platPyIncDir))

    # Check that the Python library can be found.
    if platPyLib:
        if not os.access(platPyLib,os.F_OK):
            error("%s could not be found. Make sure Python is fully installed, including any development packages." % (platPyLib))

        inform("%s is installed." % (platPyLib))

    # Check the Qt installation.
    if qtVersion >= 0:
        checkQtDirAndVersion()
        checkQtLibrary()

    # Look for the Makefile generator.
    checkMakefileGenerator()

    # Look for Make.
    global makeBin, makeBinPath

    if makeBin is None:
        makeBin = platMake
        makeBinPath = checkPath(makeBin)
    elif isProgram(makeBin):
        makeBinPath = makeBin
    else:
        makeBin = None

    if makeBin is None:
        error("A make program could not be found. Use the -m argument to set the correct program.")

    inform("%s will be used as the make program." % makeBin)


def checkMakefileGenerator():
    """Check that the Makefile generator can be found:
    """
    global makefileGen, usingTmake

    if makefileGen is None:
        mfg = "tmake"

        if qtVersion >= 0:
            xtradir = qtDir + "/bin"

            if qtVersion >= 0x030000:
                mfg = "qmake"
        else:
            xtradir = None

        makefileGen = checkPath(mfg, xtradir)
    elif not isProgram(makefileGen):
        mfg = makefileGen
        makefileGen = None

    if makefileGen is None:
        error("The Makefile generator %s could not be found. Use the -g argument to set the correct program." % (mfg))

    if os.path.basename(makefileGen) in ("tmake", "tmake.exe"):
        usingTmake = 1

        # Make sure that references to tmake use the correct absolute pathname.
        makefilePatches["TMAKE1"] = ["^TMAKE[ \\t]*=.*$", "TMAKE = " + escape(makefileGen)]
        makefilePatches["TMAKE2"] = ["^\\ttmake", "\\t" + escape(makefileGen)]

        # Add the install target for Makefiles generated from the subdirs
        # template.
        makefilePatches["TMAKE3"] = ["^clean release debug:", "clean release debug install:"]

        # Remove the invocation of the tmake_all target to prevent tmake
        # regenerating the Makefile and overwriting the patches.
        makefilePatches["TMAKE4"] = [" tmake_all", ""]
    else:
        # Make sure that references to qmake use the correct absolute pathname.
        makefilePatches["QMAKE1"] = ["^QMAKE[ \\t]*=.*$", "QMAKE = " + escape(makefileGen)]

        # Remove the invocation of the qmake_all target.  This doesn't seem to
        # have the same problem as with tmake, but we do it just in case.
        makefilePatches["QMAKE2"] = [" qmake_all", ""]

        # qmake seems to have a bug where the it generates the wrong name for
        # the .prl file for the Qt library.  This only affects SIP v4 when
        # compiled as a plugin.  Rather than fix the name we just remove the
        # re-building of Makefile.in from the 'all' target.
        makefilePatches["QMAKE3"] = ["^all: Makefile.in", "all:"]

    inform("%s will be used to generate Makefiles." % (makefileGen))

    # If it is tmake then make sure TMAKEPATH is set.
    if usingTmake:
        try:
            os.environ["TMAKEPATH"]
        except:
            error("tmake requires that the TMAKEPATH environment variable is properly set.")


def tuneCompiler():
    """Tune the compiler flags.
    """
    if len(gccFlags) > 0:
        subst = "CXXFLAGS ="

        for f in gccFlags:
            subst = subst + " -f" + f

        makefilePatches["CXXFLAGS"] = ["^CXXFLAGS[ \\t]*=", subst]


def createConfigurationModule():
    """Create the configuration module.
    """
    cfgPatches = {}

    # Later versions of qmake add the current directory, but do it just to make
    # sure.
    bli_includepath = [".", escape(platPyIncDir)]
    bli_includepath_sip = [".", escape(sipIncDir)]

    if sipIncDir != platPyIncDir:
        bli_includepath_sip.append(escape(platPyIncDir))

    bli_defines = []
    bli_defines_sip = []

    if not staticModule:
        bli_defines.append("SIP_MAKE_DLL")
        bli_defines_sip.append("SIP_MAKE_MODULE_DLL")

    if qtVersion >= 0:
        if qtIncDir != os.path.join(qtDir, "include"):
            bli_includepath.append(escape(qtIncDir))
            bli_includepath_sip.append(escape(qtIncDir))

        bli_config.append("qt")

        bli_defines.append("SIP_QT_SUPPORT")

        if usingTmake and sys.platform == "win32":
            bli_defines.append("QT_DLL")
            bli_defines_sip.append("QT_DLL")

    bli_config_app = string.join(bli_config)
    cfgPatches["@BLI_CONFIG_APP@"] = bli_config_app
    cfgPatches["@BLI_CONFIG_LIB@"] = bli_config_lib + " " + bli_config_app

    cfgPatches["@BLI_DEFINES@"] = string.join(bli_defines)
    cfgPatches["@BLI_DEFINES_SIP@"] = string.join(bli_defines_sip)
    cfgPatches["@BLI_INCLUDEPATH@"] = string.join(bli_includepath)
    cfgPatches["@BLI_INCLUDEPATH_SIP@"] = string.join(bli_includepath_sip)

    bli_libs = ""
    bli_libs_sip = ""

    if sys.platform == "win32":
        bli_libs_sip = escape(sipModDir + "\\libsip.lib") + " "
        bli_libs_sip = bli_libs_sip + escape(platPyLib)
        bli_libs = escape(platPyLib)
    else:
        bli_libs_sip = "-L" + escape(sipModDir) + " -lsip"

    cfgPatches["@BLI_LIBS@"] = bli_libs
    cfgPatches["@BLI_LIBS_SIP@"] = bli_libs_sip

    cfgPatches["@BLI_PY_VERSION@"] = "0x%06x" % (pyVersNr)

    if qtVersion <= 0:
        qtv = "0"
    else:
        qtv = "0x%06x" % (qtVersion)

    cfgPatches["@BLI_QT_VERSION@"] = qtv
    cfgPatches["@BLI_QT_EDITION@"] = qtEdition
    cfgPatches["@BLI_QT_INC_DIR@"] = escape(qtIncDir)
    cfgPatches["@BLI_QT_LIB_DIR@"] = escape(qtLibDir)
    cfgPatches["@BLI_QT_LIB@"] = qtLib

    cfgPatches["@BLI_DEF_BINDIR@"] = escape(platBinDir)
    cfgPatches["@BLI_DEF_MODDIR@"] = escape(platSiteDir)
    cfgPatches["@BLI_DEF_SIPDIR@"] = escape(sipSipDir)

    bli_sip_bin = os.path.join(sipBinDir, "sip")

    if sys.platform == "win32":
        bli_sip_bin = bli_sip_bin + ".exe"

    cfgPatches["@BLI_SIP_BIN@"] = escape(bli_sip_bin)

    cfgPatches["@BLI_SIP_VERSION@"] = "0x%06x" % sipVersion

    cfgPatches["@BLI_MFG_BIN@"] = escape(makefileGen)
    cfgPatches["@BLI_MAKE_BIN@"] = escape(makeBin)
    cfgPatches["@BLI_MAKE_BIN_PATH@"] = escape(makeBinPath)

    _create_config("sipconfig.py",cfgPatches)

    # qmake generates Makefiles that refer to $(QTDIR) so that the value is
    # picked up from the environment.  This is no good if the directory has
    # been passed on the command line, so we add patches to explicitly define
    # it.

    if qtVersion >= 0:
        # For lib template project files.
        makefilePatches["QTDIR_LIB"] = ["^CC", "QTDIR = " + escape(qtDir) + "\\nCC"]

        # For subdir template project files.
        makefilePatches["QTDIR_SUBDIR"] = ["^MAKEFILE", "QTDIR = " + escape(qtDir) + "\\nMAKEFILE"]

    # Add the Makefile patches.
    f = open("sipconfig.py","a")
    f.write("\n\n# The Makefile patches.\n_makefile_patches = {\n")

    for p in makefilePatches.keys():
        pattern, repl = makefilePatches[p]

        f.write("    '%s': ['%s', '%s'],\n" % (p, pattern, repl))

    f.write("}\n")
    f.close()


def main(argv):
    """The main function of the script.

    argv is the list of command line arguments.
    """
    import getopt

    # Check we are running from the right directory.
    _set_script()

    # Get the license.
    global licType

    # Make sure everything is initialised.
    initGlobals()

    # Parse the command line.
    try:
        optlist, args = getopt.getopt(argv[1:],"hb:d:e:f:g:i:kl:m:q:r:t:uvwx")
    except getopt.GetoptError:
        usage()

    for opt, arg in optlist:
        if opt == "-h":
            usage(0)
        elif opt == "-b":
            global sipBinDir
            sipBinDir = arg
        elif opt == "-d":
            global sipModDir
            sipModDir = arg
        elif opt == "-e":
            global sipIncDir
            sipIncDir = arg
        elif opt == "-f":
            global gccFlags
            gccFlags.append(arg)
        elif opt == "-g":
            global makefileGen
            makefileGen = arg
        elif opt == "-i":
            global qtIncDir
            qtIncDir = arg
        elif opt == "-k":
            global staticModule
            staticModule = 1
        elif opt == "-l":
            global qtLib

            if arg in ("qt", "qt-mt", "qt-mtedu", "qte"):
                qtLib = arg
            else:
                usage()
        elif opt == "-m":
            global makeBin
            makeBin = arg
        elif opt == "-q":
            global qtDir
            qtDir = arg
        elif opt == "-r":
            global qtLibDir
            qtLibDir = arg
        elif opt == "-t":
            global platPyLibDir
            platPyLibDir = arg
        elif opt == "-u":
            global debugMode
            debugMode = "debug"
        elif opt == "-v":
            global sipSipDir
            sipSipDir = arg
        elif opt == "-w":
            global enableOldPython
            enableOldPython = 1
        elif opt == "-x":
            global qtVersion
            qtVersion = -1

    # Complete the configuration from the command line.
    updateGlobals()
    tuneCompiler()

    if sys.platform == "darwin":
        error("MacOS/X requires SIP v4.x or later.")

    # Validate the configuration.
    checkConfiguration()

    # Create the configuration module.
    createConfigurationModule()

    # Bootstrap the configuration module.
    import sipconfig
    config = sipconfig.SIPConfigBase()

    # Add the non-exported patches needed to complete the configuration.
    config.patches["@SIP_BINDIR@"] = escape(sipBinDir)
    config.patches["@SIP_MODDIR@"] = escape(sipModDir)

    sipconfig.inform("Creating SIP code generator Makefile.")
    olddir = sipconfig.push_dir("sipgen")
    config.create_makefile("sipgen.pro")
    config.add_install_target([("copy", os.path.join("..", "sipconfig.py"), platSiteDir)])
    sipconfig.pop_dir(olddir)

    sipconfig.inform("Creating SIP module Makefile.")
    olddir = sipconfig.push_dir("siplib")
    config.create_makefile("siplib.pro", "sip")

    icmds = [("copy", "sip.h", sipIncDir)]

    icmds.append(("copy", "sipQt.h", sipIncDir))

    config.add_install_target(icmds)

    sipconfig.pop_dir(olddir)

    # Generate the top-level Makefile.
    sipconfig.inform("Creating top level Makefile.")
    sipconfig.copy_to_file("sip.pro.in","TEMPLATE = subdirs\nSUBDIRS = sipgen siplib\n")
    config.create_makefile("sip.pro")

    # Tell the user what to do next.
    inform("The configuration of SIP for your system is now complete. To compile and install SIP run \"%s\" and \"%s install\" with appropriate user privileges." % (makeBin, makeBin))


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
