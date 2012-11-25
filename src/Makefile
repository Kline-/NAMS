## Compiler to use. Either g++ or your own flavor of colorgcc, ccache, etc.
# Default: g++
CXX  = g++

### Name of binary file to generate.
# Default: nams
PROG = nams

## Compile mode. Either "RELEASE" or "DEBUG". Defaults to DEBUG otherwise.
# Default: DEBUG
MODE = DEBUG


### END CONFIGURATION OPTIONS ###


ifeq '$(MODE)' 'RELEASE'
	C_FLAGS = -w -O3 -std=c++0x
else
	C_FLAGS = -O0 -ggdb3 -pg -std=c++0x
endif

MAKEFLAGS = -j2
W_FLAGS = -Wall -Wformat-security -Wpointer-arith -Wredundant-decls -Wcast-align -Wshadow -Wwrite-strings -Werror
L_FLAGS = -lcrypt -ldl -lm -lpthread -L`pwd` -rdynamic

C_FILES = $(wildcard *.c *.cpp)
O_FILES = $(patsubst %.cpp,o/%.o,$(C_FILES))
H_FILES = $(wildcard h/includes.h)
G_FILES = $(patsubst h/%.h,h/%.h.gch,$(H_FILES))
DEPS = o/dependencies.d
VERS =  $(shell grep 'define CFG_STR_VERSION' h/config.h | cut -d\" -f2)

CMD_C_FILES = $(wildcard ../command/*.cpp)
CMD_O_FILES = $(patsubst ../command/%.cpp,../obj/%.so,$(CMD_C_FILES))
PLG_O_FILES = $(CMD_O_FILES)
PLG_OPT = -I../src/h -fpic -ldl -rdynamic -shared

# Trickery to run a script on the -first ever- compile and re-create a directory
# structure that may be missing due to Git not tracking empty directories.
$(shell if [ -x ./.dirbuild ]; then ./.dirbuild; rm -f ./.dirbuild; fi )

.PHONY: help $(PROG) clean commands depend doxygen cbuild pclean plugins
.SILENT: help $(PROG) clean commands depend doxygen cbuild pclean plugins $(G_FILES) $(O_FILES) $(PLG_O_FILES)

help:
	echo -e "\n### $(VERS) Makefile Options ###"
	echo -e "    help     Displays this help menu."
	echo -e "    $(PROG)     Compiles the game into binary file $(PROG)."
	echo -e "    cbuild   Equivalent to: make clean && make depend && make $(PROG)"
	echo -e "    clean    Removes files: $(PROG) o/* d/* ../report/core ../obj/*"
	echo -e "    depend   Generate dependencies for all source code."
	echo -e "    doxygen  Generate Doxygen output in ../etc/gh-pages"
	echo -e "    pclean   Removes files: ../obj/*"
	echo -e "    plugins  Compiles all available plugins.\n"

$(PROG): $(G_FILES) $(O_FILES)
	$(RM) $(PROG)
	$(CXX) -o $(PROG) $(O_FILES) $(L_FLAGS)
	echo "Finished building $(VERS) ($(MODE))."
	chmod +x $(PROG)

clean:
	$(RM) $(O_FILES) $(G_FILES) $(DEPS) $(PROG) ../report/core $(PLG_O_FILES)

commands: $(CMD_O_FILES)
	echo "Finished building all command plugins."

depend:
	$(RM) $(DEPS)
	echo "Generating dependencies for $(VERS) ... "
	$(CXX) -MM $(C_FLAGS) $(C_FILES) > $(DEPS)
	perl -pi -e 's.^([a-z]).o/$$1.g' $(DEPS)
	echo "Finished writing dependencies to $(DEPS)"

cbuild:
	$(MAKE) clean
	$(MAKE) depend
	$(MAKE) $(PROG)

pclean:
	$(RM) $(PLG_O_FILES)

plugins: commands

# pull in dependency info for *existing* .o files
-include $(DEPS)

h/%.h.gch: h/%.h
	echo "Compiling `echo $@ | cut -c 3-` ... ";
	$(CXX) -c $(C_FLAGS) $(W_FLAGS) $< -o $@

o/%.o: %.cpp
	echo "Compiling `echo $@ | cut -c 3-` ...";
	$(CXX) -c $(C_FLAGS) $(W_FLAGS) $< -o $@

../obj/%.so: ../command/%.cpp
	echo "Compiling `echo $@ | cut -c 8-` ... ";
	$(CXX) -c $(PLG_OPT) $< -o $@
