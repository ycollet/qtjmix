#=============================================================================
#  MusE
#  Linux Music Editor
#  $Id:$
#
#  Copyright (C) 2002-2006 by Werner Schweer and others
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


option(AWL_BUILD_PLUGIN "Build the designer plugin for AWL" OFF)

qt5_wrap_cpp(mocs aslider.h
                  knob.h
		  panknob.h
		  checkbox.h
		  volknob.h
		  midipanknob.h
		  colorlabel.h
		  slider.h
		  volslider.h
		  mslider.h
		  pitchedit.h
		  pitchlabel.h
		  denomspinbox.h)
		
if (APPLE)
  file(GLOB_RECURSE INCS "*.h")
else (APPLE)
  set(INCS "")
endif (APPLE)

add_library(awl STATIC ${mocs}
                       ${INCS}
		       aslider.cpp
		       knob.cpp
		       panknob.cpp
		       checkbox.cpp
		       volknob.cpp
		       midipanknob.cpp
		       colorlabel.cpp
		       slider.cpp
		       volslider.cpp
		       mslider.cpp
		       pitchedit.cpp
		       pitchlabel.cpp
		       utils.cpp
		       denomspinbox.cpp)

qt5_use_modules(awl Widgets)

if (NOT MINGW AND NOT APPLE AND AWL_BUILD_PLUGIN)
  qt5_wrap_cpp(mocs2 aslider.h
                     knob.h
		     panknob.h
   		     checkbox.h
		     volknob.h
		     midipanknob.h
		     awlplugin.h
		     colorlabel.h
		     slider.h
		     volslider.h
		     mslider.h
		     denomspinbox.h
		     pitchedit.h
		     pitchlabel.h
		     awlplugin.h)
		   
  add_library(awlplugin SHARED ${mocs2}
                               awlplugin.cpp
			       aslider.cpp
			       knob.cpp
			       panknob.cpp
			       checkbox.cpp
			       volknob.cpp
			       midipanknob.cpp
			       colorlabel.cpp
			       slider.cpp
			       volslider.cpp
			       mslider.cpp
			       denomspinbox.cpp
			       pitchedit.cpp
			       pitchlabel.cpp
			       utils.cpp)
			     
  set_target_properties(awlplugin PROPERTIES COMPILE_FLAGS "-fPIC -D_GNU_SOURCE -D_REENTRANT -DHAVE_CONFIG_H -DQT_PLUGIN -DQT_SHARED -DQT_NO_DEBUG")

  qt5_use_modules(awlplugin Widgets)
 
  install(TARGETS awlplugin 
          DESTINATION ${QT_PLUGINS_DIR}/designer/)
endif (NOT MINGW AND NOT APPLE AND AWL_BUILD_PLUGIN)
