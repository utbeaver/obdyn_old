TARGET = sip.exe
OFILES = main.obj transform.obj gencode.obj heap.obj parser.obj lexer.obj
HFILES = sip.h parser.h

CC = cl
CXX = cl
LINK = link
CPPFLAGS = -DUNICODE -I.
CFLAGS = -nologo -Zm200 -O1 -W0
CXXFLAGS = -nologo -Zm200 -O1 -W0
LFLAGS = /NOLOGO /INCREMENTAL:NO /SUBSYSTEM:console delayimp.lib /DELAYLOAD:comdlg32.dll /DELAYLOAD:oleaut32.dll /DELAYLOAD:winmm.dll /DELAYLOAD:wsock32.dll /DELAYLOAD:winspool.dll
LIBS = kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib imm32.lib winmm.lib wsock32.lib winspool.lib
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
	@if not exist C:\Python27 mkdir C:\Python27
	copy $(TARGET) C:\Python27\$(TARGET)

clean:
	-del $(TARGET)
	-del main.obj
	-del transform.obj
	-del gencode.obj
	-del heap.obj
	-del parser.obj
	-del lexer.obj
