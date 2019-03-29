
all:
	cd sipgen
	$(MAKE)
	@cd ..
	cd siplib
	$(MAKE)
	@cd ..

install:
	cd sipgen
	$(MAKE) install
	@cd ..
	cd siplib
	$(MAKE) install
	@cd ..
	@if not exist C:\Python35\Lib\site-packages mkdir C:\Python35\Lib\site-packages
	copy /y sipconfig.py C:\Python35\Lib\site-packages\sipconfig.py
	copy /y C:\Users\peili\OneDrive\Desktop\obdyn\sip-4.19.8\sipdistutils.py C:\Python35\Lib\site-packages\sipdistutils.py

clean:
	cd sipgen
	$(MAKE) clean
	@cd ..
	cd siplib
	$(MAKE) clean
	@cd ..
