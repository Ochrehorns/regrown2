HEART_FILES:=\
	$(BUILD_DIR)/src/HeartPieceMod/Usuba.o\
	$(BUILD_DIR)/src/HeartPieceMod/UsubaMgr.o\
	$(BUILD_DIR)/src/HeartPieceMod/UsubaState.o\
	$(BUILD_DIR)/src/HeartPieceMod/UsubaAnimator.o\

DEPENDS += $(HEART_FILES:.o=.d)

$(BUILD_DIR)/src/HeartPieceMod/HeartPieceMod.a: $(HEART_FILES)
	@echo Linking... $@
	$(QUIET) mkdir -p $(dir $@)
	@echo $(HEART_FILES) > build/HEART_ofiles
	$(QUIET) $(LD) -library $(LIBRARY_LDFLAGS) -o $@ -lcf ldscript.lcf @build/HEART_ofiles
