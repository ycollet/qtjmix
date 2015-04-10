#=============================================================================
#  QtJMix
#  Qt Jack Mixer
#  $Id:$
#
#  Copyright (C) 2013-2015 by Yann Collette and others
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License version 2.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#=============================================================================

option(UPDATE_TRANSLATIONS "Update source translation translations/*.ts files (WARNING: make clean will delete the source .ts files! Danger!)" OFF)

set(QTJMIX_SRCS main.cpp)

set(QTJMIX_UI_SRCS ./mixer_strip.cpp
                   ./gui_mixer.cpp
	           ./help_dialog.cpp
	           ./set_mixer_param_dialog.cpp)

set(QTJMIX_UI_HEADERS ./mixer_strip.h
                      ./gui_mixer.h
	              ./help_dialog.h
	              ./set_mixer_param_dialog.h)

set(QTJMIX_TRANS ./translations/qtjmix_en.ts
                 ./translations/qtjmix_fr.ts)

# Manage translation files
set(FILES_TO_TRANSLATE)
file(GLOB FTT_HEADER_FILES *.h widgets/*.h awl/*.h)
file(GLOB FTT_CPP_FILES *.cpp widgets/*.cpp awl/*.cpp)
file(GLOB FTT_UI_FILES *.ui widgets/*.ui awl/*.ui)
set(FILES_TO_TRANSLATE ${FTT_HEADER_FILES} ${FTT_CPP_FILES} ${FTT_UI_FILES})

file (GLOB TRANSLATIONS_FILES translations/*.ts)

if (UPDATE_TRANSLATIONS)
  qt5_create_translation(QTJMIX_QM ${FILES_TO_TRANSLATE} ${TRANSLATIONS_FILES})
else (UPDATE_TRANSLATIONS)
  qt5_add_translation(QTJMIX_QM ${TRANSLATIONS_FILES})
endif (UPDATE_TRANSLATIONS) 

add_custom_target(translations_target DEPENDS ${QTJMIX_QM})

qt5_wrap_cpp(QTJMIX_UI_HEADERS_MOC ${QTJMIX_UI_HEADERS})

qt5_add_resources(QTJMIX_RSRCS ./qtjmix.qrc)

add_library(libqtjmix STATIC ${QTJMIX_UI_SRCS} ${QTJMIX_UI_HEADERS_MOC})

add_executable(qtjmix ${QTJMIX_SRCS} ${QTJMIX_RSRCS} ${QTJMIX_QM})
target_link_libraries(qtjmix libqtjmix awl ${Qt5Widgets_LIBRARIES} ${Qt5WebKitWidgets_LIBRARIES} ${JACK_LIBRARIES})

qt5_use_modules(libqtjmix Widgets WebKitWidgets)
qt5_use_modules(qtjmix Widgets WebKitWidgets)

install(TARGETS qtjmix
        DESTINATION bin)

install(FILES ${QM_FILES} 
        DESTINATION ${CMAKE_INSTALL_PREFIX}/translations)
