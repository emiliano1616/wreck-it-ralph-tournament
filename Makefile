all:
	cd Client; make -s
	cd TournomentServer; make -s
	cd GameServer; make -s

clean:
	cd Client; make clean -s
	cd TournomentServer; make clean -s
	cd GameServer; make clean -s
