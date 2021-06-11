all: mxcan

install: driver_install

clean: driver_clean

uninstall: driver_uninstall

mxcan:
	@cd driver;\
	make

driver_install:
	@cd driver;\
	make install

driver_clean:
	@cd driver;\
	make clean

driver_uninstall:
	@cd driver;\
	make uninstall


