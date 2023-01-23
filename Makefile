CXX        = g++
CXXFLAGS   = -g3 -Wall
CXXPATHS   = -I.
LDFLAGS    =
RM         = rm -f

SRCS       = cell.cc main.cc sudoku.cc

BUILD      = build
OBJS       = $(patsubst %.cc,$(BUILD)/%.o,$(SRCS))
DEPS       = $(patsubst %.o,%.d,$(OBJS))
EXE        = sudoku

.PHONY: all
all: $(EXE)

$(EXE): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJS)

$(OBJS): $(BUILD)/%.o: %.cc
	@mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS) $(CXXPATHS) -MMD -MP -o $@ -c $<

.PHONY: clean
clean:
	$(RM) $(OBJS) $(DEPS) $(EXE)

-include $(DEPS)
