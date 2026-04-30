#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

.SECONDARY:
.SECONDEXPANSION:

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

include $(DEVKITARM)/ds_rules

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
#---------------------------------------------------------------------------------
TARGET      :=  $(shell basename $(CURDIR))
BUILD       :=  build
SOURCES     :=  source source/views source/controllers source/core source/dialogue source/models source/environments
DATA        :=  
INCLUDES    :=  include source
GRAPHICS    :=  assets/graphics
SFX         :=  assets/sfx
NITRODATA   :=  nitrofiles

GAME_TITLE     := Persona 3 Dual
GAME_SUBTITLE1 := A Fan Recreation
GAME_SUBTITLE2 := Demake by Taha Rashid
export GAME_ICON := $(CURDIR)/../icon.bmp

#---------------------------------------------------------------------------------
# Python tool configuration
#---------------------------------------------------------------------------------
TOOLS_DIR       := $(CURDIR)/tools
VENV_PYTHON     := $(HOME)/.venv/bin/python3

ASSETS_DIALOGUE := $(CURDIR)/assets/dialogue
ASSETS_MUSIC    := $(CURDIR)/assets/music
ASSETS_VIDEO    := $(CURDIR)/assets/video
ASSETS_ENVIRONMENTS := $(CURDIR)/assets/environments
ASSETS_MODELS   := $(CURDIR)/assets/models
ASSETS_MAPS     := $(CURDIR)/assets/maps

NITRO_MUSIC     := $(CURDIR)/nitrofiles/music
NITRO_VIDEO     := $(CURDIR)/nitrofiles/video

#---------------------------------------------------------------------------------
# Collect source files
#---------------------------------------------------------------------------------
DLG_FILES       := $(wildcard $(ASSETS_DIALOGUE)/*.dlg)
MP3_FILES       := $(wildcard $(ASSETS_MUSIC)/*.mp3)
MP4_FILES       := $(wildcard $(ASSETS_VIDEO)/*.mp4)
ENV_OBJ_FILES   := $(wildcard $(ASSETS_ENVIRONMENTS)/*/*.obj)
MAP_FILES       := $(wildcard $(ASSETS_MAPS)/*.png)
MODEL_JSON_FILES := $(wildcard $(ASSETS_MODELS)/*/*.json)

#---------------------------------------------------------------------------------
# Derive output paths & dynamically add environment output dirs to SOURCES
#---------------------------------------------------------------------------------
DIALOGUE_OUT := $(DLG_FILES:$(ASSETS_DIALOGUE)/%.dlg=$(CURDIR)/source/dialogue/%_dialogue.cpp)
MUSIC_OUT    := $(MP3_FILES:$(ASSETS_MUSIC)/%.mp3=$(NITRO_MUSIC)/%.pcm)
VIDEO_OUT    := $(MP4_FILES:$(ASSETS_VIDEO)/%.mp4=$(NITRO_VIDEO)/%.vid)
MAP_OUT      := $(MAP_FILES:$(ASSETS_MAPS)/%.png=$(CURDIR)/source/maps/%.h)
MODEL_OUT    := $(foreach file,$(MODEL_JSON_FILES),$(CURDIR)/source/models/$(notdir $(file:.json=.h)))

# Keep track of environment directories so Make knows where to find the generated .s files later
ENV_OUT_DIRS    := $(foreach file,$(ENV_OBJ_FILES),source/environments/$(notdir $(patsubst %/,%,$(dir $(file)))))
ENVIRONMENT_OUT := $(foreach file,$(ENV_OBJ_FILES),$(CURDIR)/source/environments/$(notdir $(patsubst %/,%,$(dir $(file))))/$(notdir $(file:.obj=_env.h)))

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
ARCH    :=  -march=armv5te -mtune=arm946e-s -mthumb

CFLAGS  :=  -g -Wall -O3 -flto -ffunction-sections -fdata-sections\
        $(ARCH)

CFLAGS  +=  $(INCLUDE) -DARM9
CXXFLAGS    := $(CFLAGS) -fno-rtti -fno-exceptions

ASFLAGS :=  -g $(ARCH)
LDFLAGS =   -specs=ds_arm9.specs -g $(ARCH) -flto -Wl,--gc-sections -Wl,-Map,$(notdir $*.map)

#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project
#---------------------------------------------------------------------------------
LIBS    := -lmm9 -lfat -lnds9

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS :=  $(LIBNDS) $(PORTLIBS)

#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------
export TOPDIR   :=  $(CURDIR)
export OUTPUT   :=  $(CURDIR)/$(TARGET)

export VPATH    :=  $(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
                    $(foreach dir,$(DATA),$(CURDIR)/$(dir)) \
                    $(foreach dir,$(GRAPHICS),$(CURDIR)/$(dir)) \
                    $(foreach dir,$(ENV_OUT_DIRS),$(CURDIR)/$(dir))

export DEPSDIR  :=  $(CURDIR)/$(BUILD)

ifneq ($(strip $(NITRODATA)),)
    export NITRO_FILES  :=  $(CURDIR)/$(NITRODATA)
endif

CFILES      :=  $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES    :=  $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
CPPFILES    +=  $(notdir $(DLG_FILES:$(ASSETS_DIALOGUE)/%.dlg=%_dialogue.cpp))
CPPFILES    :=  $(sort $(CPPFILES))
SFILES      :=  $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
BINFILES    :=  soundbank.bin
PNGFILES    :=  $(foreach dir,$(GRAPHICS),$(notdir $(wildcard $(dir)/*.png)))

export SFXFILES :=  $(foreach dir,$(notdir $(wildcard $(SFX)/*.*)),$(CURDIR)/$(SFX)/$(dir))

ifeq ($(strip $(CPPFILES)),)
    export LD := $(CC)
else
    export LD := $(CXX)
endif

# FIX: Removed the .obj=_env.o compilation requirement for environments
ENV_S_OFILES    :=  $(foreach file,$(wildcard $(ASSETS_ENVIRONMENTS)/*/*.png),$(notdir $(file:.png=.o)))

SFILES          :=  $(filter-out $(ENV_S_OFILES:.o=.s),$(SFILES))

export OFILES   :=  $(addsuffix .o,$(BINFILES)) \
                    $(PNGFILES:.png=.o) \
                    $(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o) \
                    $(ENV_S_OFILES)

export INCLUDE  :=  $(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
                    $(foreach dir,$(LIBDIRS),-I$(dir)/include) \
                    $(foreach dir,$(LIBDIRS),-I$(dir)/include) \
                    -I$(CURDIR)/$(BUILD)

export LIBPATHS :=  $(foreach dir,$(LIBDIRS),-L$(dir)/lib)

.PHONY: $(BUILD) clean assets dialogue music video environments maps models help

#---------------------------------------------------------------------------------
$(BUILD):
	@$(MAKE) --no-print-directory assets
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

help:
	@echo "  make              Build everything"
	@echo "  make assets       Run all asset converters"

assets: dirs dialogue music video environments maps models

# FIX: Added nitrofiles/environments directory creation
dirs:
	@mkdir -p $(CURDIR)/source/dialogue $(CURDIR)/source/maps $(CURDIR)/source/models $(CURDIR)/source/environments $(NITRO_MUSIC) $(NITRO_VIDEO) $(CURDIR)/nitrofiles/models $(CURDIR)/nitrofiles/environments

#---------------------------------------------------------------------------------
$(CURDIR)/source/dialogue/%_dialogue.cpp: $(ASSETS_DIALOGUE)/%.dlg $(wildcard $(ASSETS_DIALOGUE)/%.build.json)
	@echo "  DLG   $(notdir $<)"
	@mkdir -p $(dir $@)
	@cd $(CURDIR)/source/dialogue && \
		$(VENV_PYTHON) $(TOOLS_DIR)/build_asset.py $< $*
dialogue: $(DIALOGUE_OUT)

#---------------------------------------------------------------------------------
$(NITRO_MUSIC)/%.pcm: $(ASSETS_MUSIC)/%.mp3
	@echo "  PCM   $(notdir $<)"
	@mkdir -p $(dir $@)
	@ffmpeg -i $< -f s16le -ar 32000 -ac 2 $@ -y -loglevel error
music: $(MUSIC_OUT)

#---------------------------------------------------------------------------------
$(NITRO_VIDEO)/%.vid: $(ASSETS_VIDEO)/%.mp4 $(wildcard $(ASSETS_VIDEO)/%.build.json)
	@echo "  VID   $(notdir $<)"
	@mkdir -p $(dir $@)
	@$(VENV_PYTHON) $(TOOLS_DIR)/build_asset.py $< $(basename $@)
video: $(VIDEO_OUT)

#---------------------------------------------------------------------------------
# FIX: Adjusted output to nitrofiles and added the mv command for the .h file
define ENV_TEMPLATE
$$(CURDIR)/source/environments/$$(notdir $$(patsubst %/,%,$$(dir $(1))))/$$(notdir $(1:.obj=_env.h)): $(1) $$(wildcard $$(dir $(1))/*.png) $$(wildcard $$(dir $(1))/*.mtl) $$(wildcard $(1:.obj=.build.json)) $$(wildcard $$(patsubst %/,%,$$(dir $(1))).build.json) $$(TOOLS_DIR)/build_asset.py
	@echo "  ENV   $$(notdir $$<)"
	@mkdir -p $$(dir $$@) $$(CURDIR)/nitrofiles/environments/$$(notdir $$(patsubst %/,%,$$(dir $(1))))
	@$$(VENV_PYTHON) $$(TOOLS_DIR)/build_asset.py $$< $$(CURDIR)/nitrofiles/environments/$$(notdir $$(patsubst %/,%,$$(dir $(1))))
	@mv $$(CURDIR)/nitrofiles/environments/$$(notdir $$(patsubst %/,%,$$(dir $(1))))/$$(notdir $(1:.obj=_env.h)) $$@
	@touch $$@
endef

$(foreach file,$(ENV_OBJ_FILES),$(eval $(call ENV_TEMPLATE,$(file))))
environments: $(ENVIRONMENT_OUT)

#---------------------------------------------------------------------------------
define MODEL_TEMPLATE
$$(CURDIR)/source/models/$$(notdir $(1:.json=.h)): $(1) $$(wildcard $(1:.json=.build.json)) $$(wildcard $$(patsubst %/,%,$$(dir $(1))).build.json) $$(TOOLS_DIR)/build_asset.py
	@echo "  MODEL $$(notdir $$<)"
	@mkdir -p $$(dir $$@) $$(CURDIR)/nitrofiles/models
	@$$(VENV_PYTHON) $$(TOOLS_DIR)/build_asset.py $$< $$(CURDIR)/nitrofiles/models/$$(notdir $(1:.json=.bin))
	@mv $$(CURDIR)/nitrofiles/models/$$(notdir $(1:.json=.h)) $$@
	@touch $$@
endef

$(foreach file,$(MODEL_JSON_FILES),$(eval $(call MODEL_TEMPLATE,$(file))))
models: $(MODEL_OUT)

#---------------------------------------------------------------------------------
$(CURDIR)/source/maps/%.h: $(ASSETS_MAPS)/%.png $(wildcard $(ASSETS_MAPS)/%.build.json)
	@echo "  MAP   $(notdir $<)"
	@mkdir -p $(dir $@)
	@$(VENV_PYTHON) $(TOOLS_DIR)/build_asset.py $< $@

maps: $(MAP_OUT)

#---------------------------------------------------------------------------------
# FIX: Added nitrofiles/environments to the clean targets
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(TARGET).elf $(TARGET).nds $(TARGET).ds.gba
	@rm -f $(DIALOGUE_OUT) $(MUSIC_OUT) $(VIDEO_OUT) $(MAP_OUT) $(MODEL_OUT) $(CURDIR)/source/dialogue/*_dialogue.cpp $(CURDIR)/source/dialogue/*_dialogue.h
	@rm -rf $(CURDIR)/source/environments/* $(CURDIR)/nitrofiles/models/* $(CURDIR)/nitrofiles/environments/*

#---------------------------------------------------------------------------------
else

DEPENDS :=  $(OFILES:.o=.d)

$(OUTPUT).nds   :   $(OUTPUT).elf
$(OUTPUT).nds   :   $(shell find $(TOPDIR)/$(NITRODATA))
$(OUTPUT).elf   :   $(OFILES)

soundbank.bin soundbank.h : $(SFXFILES)
	@mmutil $^ -d -osoundbank.bin -hsoundbank.h

%.bin.o :   %.bin
	@echo $(notdir $<)
	@$(bin2o)

%.mp3.o :   %.mp3
	@echo $(notdir $<)
	@$(bin2o)

%.s %.h : %.png %.grit
	grit $< -fts -o$*

-include $(DEPENDS)
endif