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


if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
	message(FATAL_ERROR "Builds within the source directory are not allowed!")
endif()

cmake_minimum_required (VERSION 2.6)

project (sh2ck)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, 
		options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

set(CMAKE_C_FLAGS "-std=c99")

find_package(PNG REQUIRED)

include_directories(${PNG_INCLUDE_DIR})

set(LIBRARIES ${PNG_LIBRARIES})

include_directories(${CMAKE_SOURCE_DIR}/src)

add_subdirectory(src)

