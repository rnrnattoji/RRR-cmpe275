#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <set>
#include <filesystem>
#include <chrono> 
#include <map>

struct AQIEntry {
    std::string city;
    std::string date;
    double aqi;
};

std::vector<std::string> splitByStringDelimiter(const std::string& s, const std::string& delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
};


std::vector<AQIEntry> parseAQIData(const std::string& data) {
    std::vector<AQIEntry> entries;
    auto cityDataSegments = splitByStringDelimiter(data, "\n");

    for (auto& cityData : cityDataSegments) {
        if (cityData.empty()) continue;

        size_t posCityNameEnd = cityData.find("CITY_NAME");
        if (posCityNameEnd == std::string::npos) continue;

        std::string city = cityData.substr(0, posCityNameEnd);
        std::string cityRecords = cityData.substr(posCityNameEnd + 9);

        auto records = splitByStringDelimiter(cityRecords, "NEW_DAY");
        for (auto& record : records) {
            if (record.empty()) continue;

            std::istringstream recordStream(record);
            std::string date, aqiStr;
            getline(recordStream, date, ',');
            getline(recordStream, aqiStr, ',');

            if (!date.empty() && !aqiStr.empty()) {
                entries.push_back({city, date, std::stod(aqiStr)});
            }
        }
    }

    return entries;
};


void writeCSV(const std::vector<AQIEntry>& entries, const std::string& filename) {
    std::ofstream file(filename);
    file << "City,Date,AQI\n";
    for (const auto& entry : entries) {
        file << "\"" << entry.city << "\"," << "\"" << entry.date << "\"" << "," << "\"" << entry.aqi << "\"" << "\n";
    }
}

int main(int argc, char *argv[])
{
   
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

    std::string location_names = "";
    std::map<std::string, std::vector<std::string>> locationDataMap;

    for (int i = 0; i < entry_count; ++i) {

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
                std::vector<std::string> rowData;
                std::string cell;

                std::string row = rowStream.str();

                while (std::getline(rowStream, cell, ',')) {
                    rowData.push_back(cell);
                }

                std::string locationName = rowData[9];
                
                if (!locationName.empty()) {
                    // Remove the last character
                    locationName.erase(locationName.length() - 1, 1);
                }

                if (!locationName.empty()) {
                    // Remove the first character, which is now at index 0
                    locationName.erase(0, 1);
                }

                if(rowData[7] != "\"-999\"" ) {
                    locationDataMap[locationName].push_back(rowData[2]+','+rowData[7]);
                } 
            }

            // Close the file
            csvFile.close();
        }
    }

    // Generting CSV files
    for(auto &row: locationDataMap) {
        std::ofstream file;
        location_names += row.first + ",";
        file.open("./newDataSequential/" + row.first + ".csv");
           for(auto &rowdata: row.second) {
            file<<rowdata<<"\n";
        }

        file.close();
    }

    std::string all_data_str = location_names;

    std::set<std::string> uniqueLocations;
    std::stringstream ss(all_data_str);
    while (ss.good()) {
        std::string substr;
        std::getline(ss, substr, ',');
        uniqueLocations.insert(substr);
    }
    
    // int loc_count = uniqueLocations.size();
    std::string csv_loc_base_filepath = "./newDataSequential/";

    struct AqiData {
        double average = 0; 
        int count = 0;      
    };

    std::map<std::string, std::map<std::string, AqiData>> locDateAqiMap;

    for (const std::string& location : uniqueLocations) {
        const auto &csv_loc_filename = location;

        std::string csv_loc_filepath = csv_loc_base_filepath + csv_loc_filename + ".csv";

        std::ifstream fileCSV(csv_loc_filepath);

        if (!fileCSV.is_open()) {
            continue;
        }

        std::string line;
        while (std::getline(fileCSV, line))
        {
            std::stringstream rowStream(line);
            std::vector<std::string> rowData;
            std::string cell;

            std::string row = rowStream.str();

            while (std::getline(rowStream, cell, ',')) {
                rowData.push_back(cell);
            }

            std::string locDate = rowData[0];
            size_t position = locDate.find('T');

            if (position != std::string::npos) {
                locDate.erase(position);
            }

            if (!locDate.empty()) {
                locDate.erase(0, 1);
            }

            std::string locAqi = rowData[1];

            if (!locAqi.empty()) {
                locAqi.erase(locAqi.length() - 1, 1);
            }

            if (!locAqi.empty()) {
                locAqi.erase(0, 1);
            }

            int aqiValue = std::stoi(locAqi);
            auto& aqiData = locDateAqiMap[csv_loc_filename][locDate];
            aqiData.count += 1;
            aqiData.average = aqiData.average + (aqiValue - aqiData.average) / aqiData.count;
        }

        fileCSV.close();   
        
    }

    std::ostringstream flattenedData;
    bool firstCity = true;

    for (const auto& locEntry : locDateAqiMap) {
        if (!firstCity) {
            flattenedData << "\n";
        }
        firstCity = false;

        flattenedData << locEntry.first << "CITY_NAME";

        bool firstDate = true;
        for (const auto& dateEntry : locEntry.second) {
            if (!firstDate) {
                flattenedData << ",NEW_DAY";
            }
            firstDate = false;
            flattenedData << dateEntry.first << "," << dateEntry.second.average;
        }
    }

    std::string result = flattenedData.str();

    auto final_entries = parseAQIData(result);
    writeCSV(final_entries, "./output_sequential/aqi_data.csv");

    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Total Elapsed time: " << elapsed.count() << " microseconds\n";

    return 0;
}
