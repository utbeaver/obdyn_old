TARGET = libsip.pyd
OFILES = siplib.obj sipQt.obj threads.obj objmap.obj
HFILES = sip.h sipint.h 

CC = cl
CXX = cl
LINK = link
CPPFLAGS = -DSIP_MAKE_DLL -DUNICODE -I. -IC:\Python27\include
CFLAGS = -nologo -Zm200 -O1 -W0
CXXFLAGS = -nologo -Zm200 -O1 -W0
LFLAGS = /NOLOGO /SUBSYSTEM:windows /DLL delayimp.lib /DELAYLOAD:comdlg32.dll /DELAYLOAD:oleaut32.dll /DELAYLOAD:winmm.dll /DELAYLOAD:wsock32.dll /DELAYLOAD:winspool.dll
LIBS = /LIBPATH:C:\Python27\libs python27.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib imm32.lib winmm.lib wsock32.lib winspool.lib
.SUFFIXES: .c .cpp .cc .cxx .C


{.}.cpp{}.obj::
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -Fo @<<
	$<
<<

{.}.cc{}.obj::
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -Fo @<<
	$<
<<

{.}.cxx{}.obj::
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -Fo @<<
	$<
<<

{.}.C{}.obj::
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -Fo @<<
	$<
<<

{.}.c{}.obj::
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -Fo @<<
	$<
<<

$(TARGET): $(OFILES)
	$(LINK) $(LFLAGS) /OUT:$(TARGET) @<<
	  $(OFILES) $(LIBS)
<<

$(OFILES): $(HFILES)

install: $(TARGET)
	@if not exist C:\Python27\Lib\site-packages mkdir C:\Python27\Lib\site-packages
	copy $(TARGET) C:\Python27\Lib\site-packages\$(TARGET)
	@if not exist C:\Python27\Lib\site-packages mkdir C:\Python27\Lib\site-packages
	copy libsip.lib C:\Python27\Lib\site-packages\libsip.lib
	@if not exist C:\Python27\Lib\site-packages mkdir C:\Python27\Lib\site-packages
	copy libsip.exp C:\Python27\Lib\site-packages\libsip.exp
	@if not exist C:\Python27\include mkdir C:\Python27\include
	copy sip.h C:\Python27\include\sip.h
	copy sipQt.h C:\Python27\include\sipQt.h

clean:
	-del $(TARGET)
	-del siplib.obj
	-del sipQt.obj
	-del threads.obj
	-del objmap.obj
