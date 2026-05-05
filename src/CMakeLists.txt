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

set(QTJMIX_SRCS main.cpp)

set(QTJMIX_UI_SRCS ./mixer_strip.cpp
                   ./gui_mixer.cpp
	           ./help_dialog.cpp
	           ./nsm_client.cpp
	           ./set_mixer_param_dialog.cpp)

set(QTJMIX_UI_HEADERS ./mixer_strip.h
                      ./gui_mixer.h
	              ./help_dialog.h
	              ./nsm_client.h
	              ./set_mixer_param_dialog.h)

set(QTJMIX_TRANS
    translations/qtjmix_en.ts
    translations/qtjmix_fr.ts)

qt6_wrap_cpp(QTJMIX_UI_HEADERS_MOC ${QTJMIX_UI_HEADERS})

qt6_add_resources(QTJMIX_RSRCS ./qtjmix.qrc)

add_library(libqtjmix STATIC ${QTJMIX_UI_SRCS} ${QTJMIX_UI_HEADERS_MOC})
target_link_libraries(libqtjmix Qt6::Widgets Qt6::WebEngineWidgets)

add_executable(qtjmix ${QTJMIX_SRCS} ${QTJMIX_RSRCS})
target_link_libraries(qtjmix libqtjmix awl Qt6::Widgets Qt6::WebEngineWidgets ${JACK_LIBRARIES} ${LIBLO_LIBRARIES})
target_include_directories(qtjmix PRIVATE ${JACK_INCLUDE_DIRS} ${LIBLO_INCLUDE_DIRS})
target_link_directories(qtjmix PRIVATE ${JACK_LIBRARY_DIRS} ${LIBLO_LIBRARY_DIRS})

qt_add_translations(qtjmix
    TS_FILES ${QTJMIX_TRANS}
    QM_FILES_OUTPUT_VARIABLE QTJMIX_QM
    LUPDATE_TARGET update_translations)

install(TARGETS qtjmix
        DESTINATION bin)

install(FILES ${QTJMIX_QM}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/translations)
