PIKHACK_FILES:=\
	$(BUILD_DIR)/src/PikHackProject/Ptank.o\
	$(BUILD_DIR)/src/PikHackProject/PtankMgr.o\
	$(BUILD_DIR)/src/PikHackProject/Clam.o\
	$(BUILD_DIR)/src/PikHackProject/ClamMgr.o\
	$(BUILD_DIR)/src/PikHackProject/ClamState.o\
	$(BUILD_DIR)/src/PikHackProject/ClamAnimator.o\
	$(BUILD_DIR)/src/PikHackProject/Pearl.o\
	$(BUILD_DIR)/src/PikHackProject/PearlMgr.o\
	$(BUILD_DIR)/src/PikHackProject/PearlState.o\
	$(BUILD_DIR)/src/PikHackProject/PearlAnimator.o\

DEPENDS += $(PIKHACK_FILES:.o=.d)

$(BUILD_DIR)/src/PikHackProject/v.a: $(PIKHACK_FILES)
	@echo Linking... $@
	$(QUIET) mkdir -p $(dir $@)
	@echo $(PIKHACK_FILES) > build/PIKHACK_ofiles
	$(QUIET) $(LD) -library $(LIBRARY_LDFLAGS) -o $@ -lcf ldscript.lcf @build/PIKHACK_ofiles
