all: 2d 3d

2d:
	$(MAKE) -C 2d_model

3d:
	$(MAKE) -C 3d_model

clean:
	$(MAKE) -C 2d_model clean
	$(MAKE) -C 3d_model clean

realclean:
	$(MAKE) -C 2d_model realclean
	$(MAKE) -C 3d_model realclean

.PHONY: all 2d 3d clean realclean
