CC     =gcc
CXX    =c++
LIBS   =-I../../adl3/include/ -I. -L../../adl3/lib/ -lADL-3
PROG   =CreatePA SimulatePulse

all: $(PROG)

%: %.c ; $(CXX) $< -o $@ $(LIBS)

clean: ; rm -f $(PROG) *.pdf *.pa* *.txt
