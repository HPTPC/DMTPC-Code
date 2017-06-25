all:
	mkdir -p doxygen
	doxygen Doxyfile
	cp htaccess doxygen/.htaccess
	cp htpasswd doxygen/.htpasswd

clean:
	rm -r doxygen/* 
	rm -r doxygen/.ht* 
