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

    if (world_size < 2) {
        std::cerr << "This program requires atleast 2 MPI processes to run." << std::endl;
        MPI_Finalize();
        return 1;
    }    
   
    std::vector<std::string> csv_files_total;
    std::vector<std::string> csv_files_process;

    auto start = std::chrono::high_resolution_clock::now(); 

    if (world_rank == 0) {
        std::filesystem::path rootFolderPath = "../airnow-2020fire/data"; // Adapt this path if needed

        start = std::chrono::high_resolution_clock::now(); 
        // Get directory entries for this process
        
        std::string file_path; 
        int route_process_num = 0;
        int dist_rank;
        int send_size;

        for (const auto &dateDir : std::filesystem::directory_iterator(rootFolderPath))
        {
            if (dateDir.is_directory())
            {
                // Iterate through subdirectories (dates)
                for (const auto &entry : std::filesystem::directory_iterator(dateDir))
                {   
                    file_path = entry.path().string();
                    csv_files_total.push_back(file_path);
                    //Round-robin
                    route_process_num = (route_process_num % (world_size - 1)) + 1;

                    send_size = file_path.size() + 1;
                    MPI_Send(&send_size, 1, MPI_INT, route_process_num, 0, MPI_COMM_WORLD);
                    MPI_Send(file_path.c_str(), send_size, MPI_CHAR, route_process_num, 0, MPI_COMM_WORLD);
                }
            }
        }

        for (int i = 1; i < world_size; ++i) {
            send_size = 0;
            MPI_Send(&send_size, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    } else {
        int recv_size = 1;
        while (recv_size > 0){
            MPI_Recv(&recv_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            if (recv_size > 0) {
                char recvPath[recv_size];
                MPI_Recv(recvPath, recv_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                csv_files_process.push_back(recvPath);
            }
        }  
    }

    // Barrier synchronization to ensure initialization is complete
    MPI_Barrier(MPI_COMM_WORLD);

    std::vector<std::string> all_data;
    if (world_rank == 0) {
        int active_process = world_size - 1;
        int max_row_recv_size = 1024;
        MPI_Status status;
        
        while(active_process > 0) {
            char received_row[max_row_recv_size];
            MPI_Recv(received_row, max_row_recv_size, MPI_CHAR, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            if (strcmp(received_row, "END") == 0) {
                active_process--;
            } else {
                all_data.push_back(received_row);
            }
        }
    } else {
        // Iterate through directory entries assigned to this process
        int num_threads = 4;

        #pragma omp parallel for num_threads(num_threads)
        for (int i = 0; i < csv_files_process.size(); ++i) {

            // Process the CSV file
            std::string filePath = csv_files_process[i];
    
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

                int row_size = row.size() + 1;
                MPI_Send(row.c_str(), row_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD);

                // // Add the row vector to the shared data structure if world_rank == 0
                // #pragma omp critical // Ensure only one thread accesses shared data structure at a time
                // {
                //     MPI_Send(row.c_str(), row_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
                // }
            }

            // Close the file
            csvFile.close();
            
        }

        MPI_Send("END", 4, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }

    // Create a shared memory segment
    int shmid;
    key_t key = 1234; // Change this key as needed
    int shared_mem_size;
    char *shm;
    
    if (world_rank == 0) {

        std::string all_data_str;
        for (const auto& row_data : all_data) {
            all_data_str += row_data + '\n';
        }

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

        
        for (int i = 1; i < world_size; ++i) {
            MPI_Send(&shared_mem_size, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

    } else {
        // Wait for shared memory to be initialized
        // MPI_Barrier(MPI_COMM_WORLD);

        MPI_Recv(&shared_mem_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Detach shared memory
        if ((shmid = shmget(key, shared_mem_size, 0666)) < 0) {
            perror("shmget");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        if ((shm = (char*)shmat(shmid, NULL, 0)) == (char*) -1) {
            perror("shmat");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }
    
    int f_count = 0;

    if (world_rank == 2) {
        char* row_shm = std::strtok(shm, "\n");
        while (row_shm != NULL) {
            // std::cout << row_shm << std::endl;
            f_count++;
            row_shm = std::strtok(NULL, "\n");
        }
    }
    std::cout << "RANK: " << world_rank << " COUNT: " << f_count << std::endl;
    
    // Detach shared memory segment
    shmdt(shm);

    MPI_Barrier(MPI_COMM_WORLD);
    // Remove shared memory segment (only done by one process)
    if (world_rank == 0) {
        shmctl(shmid, IPC_RMID, NULL);
        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "Total Elapsed time: " << elapsed.count() << " microseconds\n";
    }

    MPI_Finalize();
    return 0;
}
