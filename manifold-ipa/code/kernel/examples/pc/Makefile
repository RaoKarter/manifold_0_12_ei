CXX = mpic++ -Wall
CXXFLAGS += -I ../../..

EXECS = example


VPATH = ../..

ALL: $(EXECS)


example: example.o  adder.o  register.o manifold.o clock.o component.o link.o messenger.o
	$(CXX) -o$@ $(LDFLAGS) $^

%.o: %.cc
	@[ -d dep ] || mkdir dep
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MF dep/$*.d -c $< -o $*.o

-include $(wildcard dep/*.d)

.PHONY: clean
clean:
	rm -f $(EXECS) *.d *.o DBG*
	rm -rf dep
