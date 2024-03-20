EPOCH_FILES:=\
	$(BUILD_DIR)/src/epochMod/blowPiki.o\
	$(BUILD_DIR)/src/epochMod/smokyFrog.o\
	$(BUILD_DIR)/src/epochMod/smokyFrogState.o\
	$(BUILD_DIR)/src/epochMod/SmokyFrogMgr.o\
	$(BUILD_DIR)/src/epochMod/SleepyBulbear.o\
	$(BUILD_DIR)/src/epochMod/SleepyBulbearMgr.o\

DEPENDS += $(EPOCH_FILES:.o=.d)

$(BUILD_DIR)/src/epochMod/epochMod.a: $(EPOCH_FILES)
	@echo Linking... $@
	$(QUIET) mkdir -p $(dir $@)
	@echo $(EPOCH_FILES) > build/EPOCH_ofiles
	$(QUIET) $(LD) -library $(LIBRARY_LDFLAGS) -o $@ -lcf ldscript.lcf @build/EPOCH_ofiles
