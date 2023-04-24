main:
	g++ -std=c++11 -o main main.cpp -lpthread
clean:
	@rm -rf main.o
cleanall:
	@rm -rf main.o go