
clean:
	rm -f *.o *~ core
	rm -f *.d *~ core
	rm -f *.so  *.a
	rm -f *.0
	rm -f $(TARGET_NAME) $(LIBNAME)
#	rm -fv ../bin/*.so ../bin/*.so.0 ../bin/*.so.0.0 ../bin/*.ko
#	rm -fv ../bin/ex*_* ../bin/tint ../bin/tprog ../bin/tstream

distclean: clean