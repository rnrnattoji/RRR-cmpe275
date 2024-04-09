#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <filesystem>

// Function to trim leading/trailing whitespaces from a string
std::string trim(const std::string &str)
{
    std::string trimmedString(str);
    trimmedString.erase(0, trimmedString.find_first_not_of(" \t\n\r"));
    trimmedString.erase(trimmedString.find_last_not_of(" \t\n\r") + 1);
    return trimmedString;
}

int main()
{
    std::filesystem::path rootFolderPath = "../airnow-2020fire/data"; // Adapt this path if needed

    // Iterate through the folder structure recursively
    for (const auto &entry : std::filesystem::recursive_directory_iterator(rootFolderPath))
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
                return 1;
            }

            // Store the parsed data
            std::vector<std::vector<std::string> > data;

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

                // Add the row vector to the main data vector
                data.push_back(row);
            }

            // Close the file
            csvFile.close();

            //   Print the parsed data (optional)
            if (!data.empty())
            {
                for (const std::vector<std::string> &row : data)
                {
                    for (const std::string &cell : row)
                    {
                        std::cout << cell << " ";
                    }
                    std::cout << std::endl;
                }
            }

            std::cout << "Processed file: " << filePath << std::endl;
        }
    }

    return 0;
}