CXX = g++
cxxflags = -Wall -g -fsanitize=address -std=c++23
sources = src/*.cpp
target = bin/nvSQL

CXX_VERSION = $(shell $(CXX) -dumpfullversion)
VERSION = 13.0.0
RES = $(CXX) version dismatches

ifeq ($(firstword $(sort $(CXX_VERSION) $(VERSION))), $(VERSION))
	RES = $(CXX) version matches
endif

all:
	@echo $(RES)
	$(CXX)	$(cxxflags)	$(sources)	-o	$(target)