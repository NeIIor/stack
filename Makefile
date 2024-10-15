all : main.o stack.o
	g++ main.o stack.o -o result.exe


# defining and redefining pattern rules
# variables
# automatic variables

# functions for file names
# the wildcard function

# generating prerequisites automatically
# g++ preprocessing options (-MM, -MMD)

main.o : main.cpp stack.h
	g++ -c main.cpp -o $@ -g -Wall -Wextra -Wno-unused-function -Wmissing-field-initializers
stack.o : stack.cpp stack.h
	g++ -c stack.cpp -o $@ -g -Wall -Wextra -Wno-unused-function -Wmissing-field-initializers

clean:
	del main.o stack.o result.exe