doc:
	doxygen Doxyfile
clean:
	rm docs/html/*
indent:
	@echo "Indent: Drivers/KETCube/core ..."
	@for file in $$( ls Drivers/KETCube/core ) ; do \
	    dos2unix Drivers/KETCube/core/$$file ; \
	    indent -kr -nut Drivers/KETCube/core/$$file ; \
	    rm Drivers/KETCube/core/$$file~ ; \
	done
	@echo "Indent: Drivers/KETCube/modules ..."
	@for file in $$( ls Drivers/KETCube/modules ) ; do \
	    dos2unix Drivers/KETCube/modules/$$file ; \
	    indent -kr -nut Drivers/KETCube/modules/$$file ; \
	    rm Drivers/KETCube/modules/$$file~ ; \
	done
	@echo "Indent: KETCube/core ..."
	@for file in $$( ls KETCube/core ) ; do \
	    dos2unix KETCube/core/$$file ; \
	    indent -kr -nut KETCube/core/$$file ; \
	    rm KETCube/core/$$file~ ; \
	done
	@echo "Indent: KETCube/modules/actuation ..."
	@for file in $$( ls KETCube/modules/actuation ) ; do \
	    dos2unix KETCube/modules/actuation/$$file ; \
	    indent -kr -nut KETCube/modules/actuation/$$file ; \
	    rm KETCube/modules/actuation/$$file~ ; \
	done
	@echo "Indent: KETCube/modules/sensing ..."
	@for file in $$( ls KETCube/modules/sensing ) ; do \
	    dos2unix KETCube/modules/sensing/$$file ; \
	    indent -kr -nut KETCube/modules/sensing/$$file ; \
	    rm KETCube/modules/sensing/$$file~ ; \
	done
	@echo "Indent: KETCube/modules/communication ..."
	@for file in $$( ls KETCube/modules/communication ) ; do \
	    dos2unix KETCube/modules/communication/$$file ; \
	    indent -kr -nut KETCube/modules/communication/$$file ; \
	    rm KETCube/modules/communication/$$file~ ; \
	done
