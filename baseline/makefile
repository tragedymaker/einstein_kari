all:
	g++ -std=c++11 -D CONSERVATIVE src/baseline.cpp -o conservative
	g++ -std=c++11 -D GREEDY src/baseline.cpp -o greedy
	g++ -std=c++11 -D RANDOM src/baseline.cpp -o random
	g++ -std=c++11 src/r07522316.cpp -o r07522316
conservative:
	g++ -std=c++11 -D CONSERVATIVE src/baseline.cpp -o conservative

random:
	g++ -std=c++11 -D RANDOM src/baseline.cpp -o random

greedy:
	g++ -std=c++11 -D GREEDY src/baseline.cpp -o greedy

clean:
	rm -rf greedy
	rm -rf conservative
	rm -rf random
	rm -rf .log.*
	rm -rf r07522316
