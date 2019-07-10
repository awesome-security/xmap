##########################################################
#Copyright(C) 2012 WAF PROJECT TEAM
#Author(A) shajianfeng
##########################################################

include ../make.include
CFLAGS  = ${BUILD_CFLAGS}  -O0 -rdynamic -D_GNU_SOURCE -pthread
CFLAGS +=  -fPIC

xm_common_SOURCES = xm_mpool.c \
			 xm_printf.c \
			 xm_strtok.c \
			 xm_strnatcmp.c \
			 xm_cpystrn.c \
			 xm_tables.c \
			 xm_fnmatch.c \
			 xm_string.c \
			 xm_errno.c \
			 xm_file_readwrite.c \
			 xm_file_fullrw.c \
			 xm_dir.c \
			 xm_fileacc.c \
			 xm_filepath.c \
			 xm_config.c \
			 xm_time.c \
			 xm_strtime.c \
			 xm_filestat.c \
			 xm_file_open.c \
			 xm_getopt.c \
			 xm_signal.c \
			 xm_util.c \
			 xm_log.c \
			 xm_object_pool.c \
			 xm_net_util.c \
			 xm_uri.c

xm_common_OBJECTS = $(patsubst %.c,%.o,$(xm_common_SOURCES))
xm_common_DEPENDS = $(patsubst %.c,%.d,$(xm_common_SOURCES))
xm_common_ASMFILE = $(patsubst %.c,%.s,$(xm_common_SOURCES))

.PHONY: all clean

all: $(xm_common_OBJECTS)

clean:
	@rm -fr $(xm_common_OBJECTS) $(xm_common_DEPENDS) $(xm_common_ASMFILE) $(xm_common_package)
	@rm -fr *.d *.o *.s 

