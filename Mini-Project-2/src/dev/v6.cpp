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
#include <omp.h>
#include <chrono> 



int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    /*

    if (world_size < 2) {
        std::cerr << "This program requires atleast 2 MPI processes to run." << std::endl;
        MPI_Finalize();
        return 1;
    }
    
    */
        
   
    auto start = std::chrono::high_resolution_clock::now(); 

    // Get directory entries for this process
    std::filesystem::path rootFolderPath = "../airnow-2020fire/data";
    std::vector<std::filesystem::directory_entry> entries;
    int entry_count = 0;
    for (const auto &dateDir : std::filesystem::directory_iterator(rootFolderPath))
    {
        if (dateDir.is_directory())
        {
            // Iterate through subdirectories (dates)
            for (const auto &entry : std::filesystem::directory_iterator(dateDir))
            {   
                entries.push_back(entry);
                entry_count++;
            }
        }
    }

    int start_index = (world_rank * entry_count) / world_size;
    int end_index = ((world_rank + 1) * entry_count) / world_size;


    std::string idv_parsed_data;

    int num_threads = 4;

    #pragma omp parallel for num_threads(num_threads)
    for (int i = start_index; i < end_index; ++i) {

        const auto &entry = entries[i];
        if (entry.is_regular_file() && entry.path().extension() == ".csv") {
            // Process the CSV file
            std::string filePath = entry.path().string();

            // Open the CSV file
            std::ifstream csvFile(filePath);
            if (!csvFile.is_open())
            {
                std::cerr << "Error: Could not open file '" << filePath << "'!" << std::endl;
                continue;
            }

            // Read data line by line
            std::string line;
            while (std::getline(csvFile, line))
            {
                std::stringstream rowStream(line);

                std::string row = rowStream.str();

                #pragma omp critical
                {
                    idv_parsed_data += row + '\n';
                }
                
            }

            // Close the file
            csvFile.close();
        }
    }


    // Create a shared memory segment
    int shmid;
    key_t key = 1234; // Change this key as needed
    int shared_mem_size;
    char *shm;

    if (world_rank == 0) {
        std::string all_data_str;
        all_data_str += idv_parsed_data;

        int data_recv_size;
        for (int i = 1; i < world_size; ++i) {
            MPI_Recv(&data_recv_size, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            std::string data_recv(data_recv_size, '\0');
            MPI_Recv(&data_recv[0], data_recv_size, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            all_data_str += data_recv;
        }

        /*

        std::cout << all_data_str << std::endl;

        int count = 0;
    
        Loop through the string and count the occurrences of "\n"
        for (size_t i = 0; i < all_data_str.length(); ++i) {
            if (all_data_str[i] == '\n') {
                count++;
            }
        }

        std::cout << "RANK: " << world_rank << " INITIAL COUNT: " << count << std::endl;
        
        */

        shared_mem_size = all_data_str.size();

        // Allocate shared memory for all_data
        if ((shmid = shmget(key, shared_mem_size, IPC_CREAT | 0666)) < 0) {
            perror("shmget");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        // Attach shared memory
        if ((shm = (char*)shmat(shmid, NULL, 0)) == (char*) -1) {
            perror("shmat");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        // Copy all_data to shared memory
        strncpy(shm, all_data_str.c_str(), shared_mem_size);
        
        // Broadcast shmid to all processes
        MPI_Bcast(&shmid, 1, MPI_INT, 0, MPI_COMM_WORLD);

    } else {
        // std::cout << idv_parsed_data << std::endl;

        // idv_parsed_data = "HELLO RNR";
        int idv_parsed_data_size = idv_parsed_data.size();
        MPI_Send(&idv_parsed_data_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(idv_parsed_data.c_str(), idv_parsed_data_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD);

        // Receive shmid from rank 0
        MPI_Bcast(&shmid, 1, MPI_INT, 0, MPI_COMM_WORLD);

        // Attach shared memory
        if ((shm = (char*)shmat(shmid, NULL, 0)) == (char*) -1) {
            perror("shmat");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    // Synchronization
    MPI_Barrier(MPI_COMM_WORLD);

    
    if (world_rank == 0) {
        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "Total Elapsed time: " << elapsed.count() << " microseconds\n";
    }

    if (world_rank >= 0){
        int data_size = strlen(shm);
        char* local_data = new char[data_size + 1];
        strncpy(local_data, shm, data_size);
        local_data[data_size] = '\0'; // Ensure null termination

        int f_count = 0;
        char* row_shm = std::strtok(local_data, "\n");
        while (row_shm != NULL) {
            f_count++;
            row_shm = std::strtok(NULL, "\n");
        }

        std::cout << "RANK: " << world_rank << " COUNT: " << f_count << std::endl;

        delete[] local_data;
    }

    // Detach shared memory
    shmdt(shm);

    // Remove shared memory segment (only done by one process)
    if (world_rank == 0) {
        shmctl(shmid, IPC_RMID, NULL);
    }

    MPI_Finalize();
    return 0;
}
