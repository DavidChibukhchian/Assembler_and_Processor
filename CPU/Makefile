build\program.exe: build\main.o build\CPU.o build\Stack.o build\Logger.o
		g++ -o build\CPU.exe $^
build\main.o: source\main.cpp
		g++ -c -o build\main.o source\main.cpp
build\CPU.o: source\CPU.cpp
		g++ -c -o build\CPU.o source\CPU.cpp
build\Stack.o: source\Stack.cpp
		g++ -c -o build\Stack.o source\Stack.cpp
build\Logger.o: source\Logger.cpp
		g++ -c -o build\Logger.o source\Logger.cpp