CXX = g++
CXXFLAGS = -O3 -Wall -Wextra -std=c++17

recom: recom.cxx recom_main.cxx
	$(CXX) $(CXXFLAGS) $^ -o $@.out
clean:
	rm -f *.out
