QMAKE_TARGET  = SachesiCascades
LUPDATE       = $(QNX_HOST)/usr/bin/lupdate
LRELEASE      = $(QNX_HOST)/usr/bin/lrelease

update: $(QMAKE_TARGET).pro FORCE
	$(LUPDATE) $(QMAKE_TARGET).pro

release: $(QMAKE_TARGET).pro $(QMAKE_TARGET).ts $(QMAKE_TARGET)_Spanish.ts $(QMAKE_TARGET)_Russian.ts $(QMAKE_TARGET)_French.ts
	$(LRELEASE) $(QMAKE_TARGET).pro

FORCE:

