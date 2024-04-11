#!/bin/bash

#Change OMPI_CXX according to env
export OMPI_CXX=clang++-18

#cleanup

rm -rf ./newData/*
rm -rf ./build/*
rm -rf ./newDataSequential/*
rm -rf ./output_sequential/*
rm output/aqi_data.csv

echo "Select the program you want to build and run:"
echo "1) mainHybrid"
echo "2) mainSequential"
read -p "Enter your choice (1 or 2): " choice

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
    echo "Compiling mainHybrid.cpp with mpic++..."
    mpic++ mainHybrid.cpp -o ./build/execHybrid
    ask_threads
    echo "Running ./build/execHybrid with $threads threads..."
    mpiexec -n $threads ./build/execHybrid
    ;;
  2)
    echo "Compiling mainSequential.cpp with clang++-18..."
    clang++-18 mainSequential.cpp -o ./build/execSequential
    echo "Running ./build/execSequential..."
    ./build/execSequential
    ;;
  *)
    echo "Invalid selection. Please choose a number between 1 and 3."
    ;;
esac
