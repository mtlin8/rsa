CXX = g++
CPPFLAGS = -Wall -g

all: rsa

rsa: rsa.cpp
	$(CXX) $(CPPFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -rf rsa

# Makefile found, no longer a worry