#
# Copyright (C) 2012 NEC Corporation
#

TREMA = ../../trema
GEM_BIN = `ruby -e 'require "rubygems"; puts Gem::bindir'`
SHELL = /bin/sh

CC = gcc
AR = ar
RANLIB = ranlib

CFLAGS = $(shell $(TREMA)/trema-config --cflags) -g -std=gnu99 -D_GNU_SOURCE -fno-strict-aliasing -Werror -Wall -Wextra -Wformat=2 -Wcast-qual -Wcast-align -Wwrite-strings -Wconversion -Wfloat-equal -Wpointer-arith -I..
LDFLAGS = -L.. -lpathresolver $(shell $(TREMA)/trema-config --libs) 

TARGET = test_heap
SRCS = test_heap.c
OBJS = $(SRCS:.c=.o)

FEATURES = path_resolver.feature

DEPENDS = .depends

.PHONY: all clean

.SUFFIXES: .c .o

all: depend $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $<

depend: 
	$(CC) -MM $(CFLAGS) $(SRCS) > $(DEPENDS)

clean:
	@rm -rf $(DEPENDS) $(OBJS) $(TARGET) *~

run_acceptance_test:
	@for feature in $(FEATURES); do \
	  ( \
	    file=`pwd`/$$feature; \
	    cd ${TREMA}; \
	    ${GEM_BIN}/cucumber -r features $$file; \
	  ) \
	done

-include $(DEPENDS)
