# #MAKEFILE 1
FILES:= MainServer Socket ConfigFile  Game Player  TournomentServer PantallaServidor TTF \
	Utility Json Semaphore MsgFields PantallaResumen

BINARY:=D-RalphTournoment

OBJS:=$(patsubst %,obj/%.o,$(FILES))

CC=g++
FLAGS=-Wall -ggdb 
LIBS=-lpthread -lSDL -lSDL_image -lSDL_ttf

.PHONY: all clean dirs
all: dirs $(OBJS) CopyImages CopyConfig CopyFonts
#@echo "Linking..."
#@g++ $(OBJS) $(LFLAGS) -o bin/$(BINARY)
#@echo "Done."	
	@echo " #----------------------------------------------------------------------------#"
	@echo " | \033[1;34mLinking ${BINARY}...\033[0m |"
	@echo " #----------------------------------------------------------------------------#\n"
	
	@${CC}  $(OBJS) $(FLAGS) ${LIBS} -o bin/$(BINARY) 2> .tmp.log || cp .tmp.log .tmp.errors
	@if test -e .tmp.errors; then \
	echo "\033[31m" && echo "${CC} $(OBJS) $(FLAGS) -o bin/$(BINARY) \033[1m [ ERROR ]" && cat .tmp.errors ;\
	elif test -s .tmp.log; then \
	echo "\033[33m" && echo "$${CC} $(OBJS) $(FLAGS) -o bin/$(BINARY) \033[1m [ WARN ]" && cat .tmp.log ;\
	else \
	echo "\033[32m$${CC} $(OBJS) $(FLAGS) -o bin/$(BINARY) \033[1m [ OK ]" ;\
	fi
	@echo "\033[0m"
	@rm -f .tmp.errors .tmp.log 2>/dev/null


clean:
	@rm -rf *.o bin/$(BINARY) bin/$(BINARY).exe obj/
	@rm -R bin
	@echo " #----------------------------------------------------------------------------#"
	@echo " | \033[1;34mCleaned\033[0m |"
	@echo " #----------------------------------------------------------------------------#\n"
	@echo "\033[0m"
	
dirs: obj/ bin/
obj/:
	@echo "Creating obj/ directory"
	@mkdir -p obj/
bin/:
	@echo "Creating bin/ directory"
	@mkdir -p bin/

obj/%.o: src/%.cpp
#@echo "Compiling $<"
#@g++ -c $< $(CFLAGS) -o $@
	@echo " #----------------------------------------------------------------------------#"
	@echo " | \033[1;34mcompiling $<...\033[0m |"
	@echo " #----------------------------------------------------------------------------#\n"
	@${CC} -std=c++0x -c $< $(FLAGS) -o $@ 2> .tmp.log || cp .tmp.log .tmp.errors
	@if test -e .tmp.errors; then \
	echo "\033[31m" && echo "${CC} ${FLAGS} -o ${BINARY} ${FILES} \033[1m [ ERROR ]" && cat .tmp.errors ;\
	elif test -s .tmp.log; then \
	echo "\033[33m" && echo "${CC} ${FLAGS} -o ${BINARY} ${FILES} \033[1m [ WARN ]" && cat .tmp.log ;\
	else \
	echo "\033[32m${CC} ${FLAGS} -o ${BINARY} ${FILES} \033[1m [ OK ]" ;\
	fi
	@echo "\033[0m"
	@rm -f .tmp.errors .tmp.log 2>/dev/null


CopyFonts:
	@echo " #----------------------------------------------------------------------------#"
	@echo " | \033[1;34mCopiando fuentes...\033[0m |"
	@echo " #----------------------------------------------------------------------------#\n"
	cp -R fonts/ bin/
	@echo "\033[0m"

CopyImages:
	@echo " #----------------------------------------------------------------------------#"
	@echo " | \033[1;34mCopiando Imagenes...\033[0m |"
	@echo " #----------------------------------------------------------------------------#\n"
	cp -R images/ bin/
	@echo "\033[0m"

CopyConfig:
	@echo " #----------------------------------------------------------------------------#"
	@echo " | \033[1;34mCopiando Configuracion...\033[0m |"
	@echo " #----------------------------------------------------------------------------#\n"
	cp  conf/config.conf bin/config.conf
	@echo "\033[0m"