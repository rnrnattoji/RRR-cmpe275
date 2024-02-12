## CLANG 18 install
https://phoenixnap.com/kb/install-gcc-ubuntu for gcc-13 and g++-13 (needed for libstd++)
https://ubuntuhandbook.org/index.php/2023/09/how-to-install-clang-17-or-16-in-ubuntu-22-04-20-04/
$ sudo ./llvm.sh 18 all
Update clang-tidy to clang-tidy-18 in CMakeLists.txt

## BOOST
sudo apt install libboost-all-dev
https://www.baeldung.com/linux/boost-install-on-ubuntu


## To RUN.

Note :- To run commands in VS CODE ( Ctrl+P -> Type (>) first and then CMake: commands )

-> DELETE BUILD FOLDER if it exists (cmd CMake:clean should work, if not just delete the build folder manually)
-> Next go inside cpp-src/basic (Cmake config is written in CMAKELists.txt) (Make sure to go inside first (root folder) and then open VS Code)
-> RUN cmd CMake: Configure (The build folder gets created)
-> RUN cmd CMake: Build
-> RUN cmd CMake: Install
-> Next go inside cpp-src/basic/build/bin. Inside you can see executables.
-> Run ./serverApp and ./clientApp in side by side terminal.

