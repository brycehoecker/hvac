CXX	=	g++
INCPATH =       -I. \
                -I/usr/local/veritas/include/VDB -I/usr/local/include/mysql \
                -I/usr/local/include/omniORB4 -I/usr/local/include/zthread
LINK	=	g++
LIBS    =       -L/usr/local/veritas/lib \
                -L/usr/local/lib/mysql/ \
		-lVDB \
		-lm \
                -lZThread \
		-lmysqlclient_r \
		-lcrypt \
		-lnsl \
		-lssl \
		-lcrypto

rename: rename.cpp
	$(CXX) $(INCPATH) -o rename rename.cpp $(LIBS)

hvdb: hvdb.cpp
	$(CXX) $(INCPATH) -o hvdb hvdb.cpp $(LIBS)

#		-lZThread \
#		-lz \
