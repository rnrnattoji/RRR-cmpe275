#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <filesystem>
#include <mpi.h>

// Function to trim leading/trailing whitespaces from a string
std::string trim(const std::string &str)
{
    std::string trimmedString(str);
    trimmedString.erase(0, trimmedString.find_first_not_of(" \t\n\r"));
    trimmedString.erase(trimmedString.find_last_not_of(" \t\n\r") + 1);
    return trimmedString;
}

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if (world_size != 4) {
        std::cerr << "This program requires 4 MPI processes to run." << std::endl;
        MPI_Finalize();
        return 1;
    }

    std::filesystem::path rootFolderPath = "../airnow-2020fire/data"; // Adapt this path if needed

    // Define key for shared memory segment
    key_t key = 12345;

    // Create shared memory segment
    int shmid = shmget(key, sizeof(std::vector<std::vector<std::string>>) * world_size, IPC_CREAT | 0666);
    if (shmid == -1) {
        std::cerr << "Failed to create shared memory segment." << std::endl;
        MPI_Finalize();
        return 1;
    }

    // Attach shared memory segment
    std::vector<std::vector<std::string>> *all_data = (std::vector<std::vector<std::string>> *)shmat(shmid, NULL, 0);
    if (all_data == (void *)-1) {
        std::cerr << "Failed to attach shared memory segment." << std::endl;
        MPI_Finalize();
        return 1;
    }

    // Initialize shared memory
    if (world_rank == 0) {
        *all_data = std::vector<std::vector<std::string>>();
    }

    // Barrier synchronization to ensure initialization is complete
    MPI_Barrier(MPI_COMM_WORLD);

    // Iterate through the top-level directory
    for (const auto &dateDir : std::filesystem::directory_iterator(rootFolderPath))
    {
        if (dateDir.is_directory())
        {
            // Iterate through subdirectories (dates)
            for (const auto &entry : std::filesystem::directory_iterator(dateDir))
            {
                if (entry.is_regular_file() && entry.path().extension() == ".csv")
                {
                    // Process the CSV file
                    std::string filePath = entry.path().string();

                    // Open the CSV file
                    std::ifstream csvFile(filePath);
                    if (!csvFile.is_open())
                    {
                        std::cerr << "Error: Could not open file '" << filePath << "'!" << std::endl;
                        MPI_Finalize();
                        return 1;
                    }

                    // Read data line by line
                    std::string line;
                    while (std::getline(csvFile, line))
                    {
                        std::stringstream rowStream(line);
                        std::vector<std::string> row;
                        std::string cell;

                        // Split the line by comma (delimiter) and store each trimmed cell
                        while (std::getline(rowStream, cell, ','))
                        {
                            row.push_back(trim(cell));
                        }

                        // Add the row vector to the shared data structure
                        if (world_rank == 0) {
                            all_data->push_back(row);
                        }
                    }

                    // Close the file
                    csvFile.close();
                }
            }
        }
    }

    // Barrier synchronization to ensure all data is written
    MPI_Barrier(MPI_COMM_WORLD);

    // Calculate total number of rows across all processes
    int total_rows;
    int local_rows = all_data->size();
    MPI_Reduce(&local_rows, &total_rows, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (world_rank == 0) {
        std::cout << "Total number of rows: " << total_rows << std::endl;
    }

    // Detach shared memory segment
    shmdt(all_data);

    // Remove shared memory segment (only done by one process)
    if (world_rank == 0) {
        shmctl(shmid, IPC_RMID, NULL);
    }

    MPI_Finalize();
    return 0;
}
