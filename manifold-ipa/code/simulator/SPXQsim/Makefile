CXX = mpic++
QSIM_DIR = ../../../../../qsim-root
MAIN_DIR = ../..
MODELS_DIR = $(MAIN_DIR)/models
CPPFLAGS += -DUSE_QSIM -DSTATS -Wall -g -I ../.. -I$(MODELS_DIR)/cache -I$(MODELS_DIR)/network -I$(MODELS_DIR)/processor -I$(MODELS_DIR)/memory  -I$(QSIM_DIR)/include -I$(MODELS_DIR)/energy_introspector -I$(MODELS_DIR)/energy_introspector/ENERGYLIB_MCPAT/mcpat0.8_r274  -I$(MODELS_DIR)/energy_introspector/THERMALLIB_3DICE/3d-ice/include -DEI_CTRL=1 #-DDEBUG_IRIS_INTERFACE
LDFLAGS = -lspx -L$(MODELS_DIR)/processor/spx -lmcp-cache -L$(MODELS_DIR)/cache/mcp-cache -liris -L$(MODELS_DIR)/network/iris -lcaffdram -L$(MODELS_DIR)/memory/CaffDRAM -lDRAMSim2 -L$(MODELS_DIR)/memory/DRAMSim2 -lDRAMSim2proper -L$(MODELS_DIR)/memory/DRAMSim2/DRAMSim2-2.2.2 -L../../kernel -lmanifold -L$(QSIM_DIR)/lib  -lqsim -ldistorm64 -lconfig++ -pthread -lEI -L$(MODELS_DIR)/energy_introspector  -lmcpat -L$(MODELS_DIR)/energy_introspector/ENERGYLIB_MCPAT/mcpat0.8_r274 -lthreed-ice-2.1 -L$(MODELS_DIR)/energy_introspector/THERMALLIB_3DICE/3d-ice/lib -lsuperlu_4.3 -lblas -L$(MODELS_DIR)/energy_introspector/THERMALLIB_3DICE/SuperLU_4.3/lib -ldl

EXECS = smp_llp


ALL: $(EXECS)


#$(CXX) $(LDFLAGS)  $^ -o$@   NOT working
#$(CXX) $(LDFLAGS)  -o$@ $^   NOT working
# it seems LDFLAGS must be the last parameter

smp_llp: smp_llp.o sysBuilder_llp.o ei_wrapper.o common.o n_ei_wrapper.o
	$(CXX) $^ -o$@   $(LDFLAGS)


%.o: %.cc
	@[ -d dep ] || mkdir dep
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MF dep/$*.d -c $< -o $*.o

-include $(wildcard dep/*.d)

.PHONY: clean
clean:
	rm -f $(EXECS) *.d *.o DBG* core*.out
	rm -rf dep
