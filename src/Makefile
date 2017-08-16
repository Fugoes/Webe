CXX = g++
SOFLAG = -Wall -shared -fPIC
CXXFLAG = -O3 -DNDEBUG -std=c++11
LIBS = dl
SOS = module_404 module_info module_file_server
OBJS = client.o http_response.o http_request.o server.o read_buffer.o

lib%.so: modules/%.cpp modules/module_base.h modules/module_utils.h
	$(CXX) $(CXXFLAG) $(SOFLAG) $< -o $@
%.o: %.cpp *.h
	$(CXX) $(CXXFLAG) -c $< -o $@
.PHONY: webed webectl clean all
webed: main.cpp $(OBJS) $(SOS:%=lib%.so)
	$(CXX) $(CXXFLAG) $(OBJS) $(LIBS:%=-l%) -L. $(SOS:%=-l%) $< -o $@ -Wl,-rpath=.
webectl: webectl.cpp $(OBJS) $(SOS:%=lib%.so)
	$(CXX) $(CXXFLAG) $(OBJS) $(LIBS:%=-l%) -L. $(SOS:%=-l%) $< -o $@ -Wl,-rpath=.
clean:
	-rm -rf *.o *.so webed webectl
all: webed webectl
