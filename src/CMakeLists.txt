#Copyright (C) 2014 David Leiter
#
#This program is free software: you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation, either version 3 of the License, or
#(at your option) any later version.
#
#This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.
#
#You should have received a copy of the GNU General Public License
#along with this program.  If not, see <http://www.gnu.org/licenses/>.

add_executable(sh2ck main.c)
target_sources(sh2ck PRIVATE
	            "${CMAKE_CURRENT_SOURCE_DIR}/image.h"
				"${CMAKE_CURRENT_SOURCE_DIR}/image.c"
				"${CMAKE_CURRENT_SOURCE_DIR}/gm1.h"
				"${CMAKE_CURRENT_SOURCE_DIR}/gm1.c"
				"${CMAKE_CURRENT_SOURCE_DIR}/tgx.h"
				"${CMAKE_CURRENT_SOURCE_DIR}/tgx.c")

target_include_directories(sh2ck PRIVATE ${CMAKE_SOURCE_DIR})

target_compile_features(sh2ck PRIVATE c_std_11)

target_link_libraries (sh2ck PRIVATE PNG::PNG)

if(UNIX)
	target_link_libraries (sh2ck PRIVATE m)
endif()

