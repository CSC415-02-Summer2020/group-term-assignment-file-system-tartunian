
ROOTNAME=fsshell
HW=
FOPTION=
VOLUMENAME=SampleVolume
VOLUMESIZE=30000
BLOCKSIZE=512
RUNOPTIONS=$(VOLUMENAME) $(VOLUMESIZE) $(BLOCKSIZE)
CC=gcc
CFLAGS= -g -lm -lreadline -I.
LIBS =pthread
DEPS = 
ADDOBJ= fsLow.o b_io.o bitMap.o mfs.o fsMakeVol.o
OBJ = $(ROOTNAME)$(HW)$(FOPTION).o $(ADDOBJ)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) 

$(ROOTNAME)$(HW)$(FOPTION): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) -l $(LIBS)

fsFormat: fsFormat.o $(ADDOBJ)
	$(CC) -o $@ $^ $(CFLAGS) -l $(LIBS)

fileExplorer: fileExplorer.o $(ADDOBJ)
	$(CC) -o $@ $^ $(CFLAGS) -l $(LIBS)

clean:
	rm *.o $(ROOTNAME)$(HW)$(FOPTION) fsshell fileExplorer fsFormat

format:
	./fsFormat $(RUNOPTIONS)

wipe:
	rm $(VOLUMENAME)

run: $(ROOTNAME)$(HW)$(FOPTION)
	./$(ROOTNAME)$(HW)$(FOPTION) $(RUNOPTIONS)

# ROOTNAME=fsDriver
# HW=
# FOPTION=
# RUNOPTIONS = SampleVolume 8192 512
# CC=gcc
# CFLAGS= -g -lm -I.
# LIBS =pthread
# DEPS = 
# ADDOBJ= fsLow.o b_io.o bitMap.o mfs.o fsMakeVol.o fsFileOrg.o
# OBJ1 = fsDriver.o $(ADDOBJ)

# %.o: %.c $(DEPS)
# 	$(CC) -c -o $@ $< $(CFLAGS)

# fsDriver: $(OBJ1)
# 	$(CC) -o $@ $^ $(CFLAGS) -l $(LIBS)

# clean:
# 	rm *.o $(ROOTNAME)$(HW)$(FOPTION)

# run: $(ROOTNAME)$(HW)$(FOPTION)	
# 	./fsDriver $(RUNOPTIONS)


