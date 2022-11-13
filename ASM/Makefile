build\program.exe: build\main.o build\ASM.o build\Buffer.o build\Logger.o
		g++ -o build\ASM.exe $^
build\main.o: source\main.cpp
		g++ -c -o build\main.o source\main.cpp
build\ASM.o: source\ASM.cpp
		g++ -c -o build\ASM.o source\ASM.cpp
build\Buffer.o: source\Buffer.cpp
		g++ -c -o build\Buffer.o source\Buffer.cpp
build\Logger.o: source\Logger.cpp
		g++ -c -o build\Logger.o source\Logger.cpp