#############################################################################
# Makefile for building vhvreaddb
#############################################################################

####### Compiler, tools and options
CC	=	gcc
CXX	=	g++
CFLAGS	=	-ggdb -pipe -D_REENTRANT -DQT_THREAD_SUPPORT
CXXFLAGS=	-ggdb -pipe -D_REENTRANT -DQT_THREAD_SUPPORT
INCPATH	=	-I. \
		-I/usr/local/veritas/include/VDB -I/usr/local/include/mysql \
		-I/usr/local/include/omniORB4 -I/usr/local/include/zthread

####### Libraries
LIBS	=	-L/usr/local/veritas/lib \
		-L/usr/local/lib/mysql/ \
		-lm \
		-lZThread \
		-lVDB \
		-lmysqlclient_r \
		-lcrypt \
		-lnsl \
		-lssl \
		-lcrypto \
#		-lomniDynamic4 \
#		`omni-config --libs`
# 		-lXext \
# 		-lX11 \
#		-L/usr/X11R6/lib \

####### Compile

VHVReadDB: hvreaddb.o
	$(CXX) -o hvreaddb $(CXXFLAGS) $(INCPATH) $(LIBS) hvreaddb.o libVDB.a

hvreaddb.o: VHVReadDB.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) VHVReadDB.cpp -o hvreaddb.o

clean: 
	rm hvreaddb.o
	rm hvreaddb
