echo $@ > valgrind-output.txt
./valgrind/bin/valgrind --tool=memcheck --leak-check=full $@ >> valgrind-output.txt 2>&1
