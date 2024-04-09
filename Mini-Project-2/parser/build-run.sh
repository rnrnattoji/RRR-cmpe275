#!/bin/bash

#Change OMPI_CXX according to env
export OMPI_CXX=clang++-18

echo "Select the program you want to build and run:"
echo "1) v4"
echo "2) readCSV"
echo "3) readCSVMPI"
read -p "Enter your choice (1-3): " choice

threads=1

ask_threads() {
    read -p "How many threads do you want to use for the MPI program? " threads
    if ! [[ "$threads" =~ ^[0-9]+$ ]]; then
        echo "Invalid input. Please enter a number."
        ask_threads
    fi
}

case $choice in
  1)
    echo "Compiling v4.cpp with mpic++..."
    mpic++ v4.cpp -o ./build/execv4
    ask_threads
    echo "Running ./build/execv4 with $threads threads..."
    mpiexec -n $threads ./build/execv4
    ;;
  2)
    echo "Compiling readCSV.cpp with clang++-18..."
    clang++-18 readCSV.cpp -o ./build/execreadCSV
    echo "Running ./build/execreadCSV..."
    ./build/execreadCSV
    ;;
  3)
    echo "Compiling readCSVMPI.cpp with mpic++..."
    mpic++ readCSVMPI.cpp -o ./build/execreadCSVMPI
    ask_threads
    echo "Running ./build/exereadCSVMPI with $threads threads..."
    mpiexec -n $threads ./build/execreadCSVMPI
    ;;
  *)
    echo "Invalid selection. Please choose a number between 1 and 3."
    ;;
esac
