# Mandelpool
Mandelpool - visualization and parallelized calculation of the mandelbrot set by implementing a threadpool

**COURSE**  
Course project in Multicore and Process Oriented Programming (1DT049) Spring 2014  
Department of Information Technology  
Uppsala university  


**MAY THE SOURCE BE WITH YOU**

To run the GUI SFML libraries needs to be installed.
On Debian / Ubuntu this can be done with apt-get install libsfml-dev.
For other operating systems we refer to http://www.sfml-dev.org/download/sfml/2.1/

**C VERSION**

This software was developed and tested using gcc version 4.8.2 and G++ version 4.8.2 (Ubuntu 4.8.2-19ubuntu1)
     	      	  	    	       
**MAKE IT HAPPEN**

Using the make utility you can perform the following actions:
```makefile
make         	==> Compiles the mandelbrot GUI  
make start   	==> Runs the GUI  
make prototype	==> Compiles protoype which renders one ppm image of the Mandelbrot set to the hard drive (without use of SFML)  
make archive	==> Creates a gziped tar archive of this directory   
make clean   	==> Removes all binaries and html generated by doxygen  
make doc     	==> Generates doxygen documentation in the doc/html directory  
make test    	==> Runs all check tests  
make beautify 	==> Makes code formatting coherent with astyle
```

For performance-tests we refer to the makefile comments.

**MORE INFORMATION**

Please read the documentation and the report in the doc/ folder
