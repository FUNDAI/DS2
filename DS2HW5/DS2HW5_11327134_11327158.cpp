#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <map>
#include <iomanip>
#include <limits> // 用於清空輸入緩衝區

struct Record {
    char putID[10];
    char getID[10];
    float weight;
};

struct IndexEntry {
    float key;
    int offset; // 該權重在排序檔中的起始筆數編號
};

class ExternalSorter {
private:
    int maxBufferSize = 300;
    std::vector<IndexEntry> primaryIndex;

    // 輔助索引：學號 -> 該學號在排序檔中所有對應的 record offsets
    std::map<std::string, std::vector<int>> secondaryIndex;

    void mergeTwoFiles(std::string file1, std::string file2, std::string outFile) {
        std::ifstream in1(file1, std::ios::binary);
        std::ifstream in2(file2, std::ios::binary);
        std::ofstream out(outFile, std::ios::binary);

        Record rec1, rec2;
        bool has1 = false, has2 = false;

        if (in1.read((char*)&rec1, sizeof(Record))) has1 = true;
        if (in2.read((char*)&rec2, sizeof(Record))) has2 = true;

        while (has1 && has2) {
            if (rec1.weight >= rec2.weight) {
                out.write((char*)&rec1, sizeof(Record));
                has1 = (bool)in1.read((char*)&rec1, sizeof(Record));
            } else {
                out.write((char*)&rec2, sizeof(Record));
                has2 = (bool)in2.read((char*)&rec2, sizeof(Record));
            }
        }
        while (has1) {
            out.write((char*)&rec1, sizeof(Record));
            has1 = (bool)in1.read((char*)&rec1, sizeof(Record));
        }
        while (has2) {
            out.write((char*)&rec2, sizeof(Record));
            has2 = (bool)in2.read((char*)&rec2, sizeof(Record));
        }
        in1.close(); in2.close(); out.close();
    }

public:
    void setBufferSize(int size) {
        maxBufferSize = size;
    }

    // === 任務一 ===
    void mission1(std::string fileNum) {
        std::string inFileName = "pairs" + fileNum + ".bin";
        std::string outFileName = "order" + fileNum + ".bin";

        std::ifstream inFile(inFileName, std::ios::binary);
        if (!inFile.is_open()) {
            std::cout << "\n### " << inFileName << " does not exist! ###\n";
            return;
        }

        std::cout << "\n##################################\n";
        std::cout << "* 1. External merge sort on file *\n";
        std::cout << "##################################\n";

        auto timeStart = std::chrono::high_resolution_clock::now();

        std::vector<Record> buffer;
        int runCount = 0;
        std::vector<std::string> current_runs;

        while (true) {
            Record tempRecord;
            if (inFile.read((char*)&tempRecord, sizeof(Record))) {
                buffer.push_back(tempRecord);
            } else {
                break;
            }

            if (buffer.size() == maxBufferSize) {
                std::stable_sort(buffer.begin(), buffer.end(), [](Record a, Record b) {
                    return a.weight > b.weight;
                });

                std::string tempName = "temp_initial_" + std::to_string(runCount) + ".bin";
                current_runs.push_back(tempName);
                std::ofstream tempOut(tempName, std::ios::binary);
                tempOut.write((char*)buffer.data(), buffer.size() * sizeof(Record));
                tempOut.close();

                buffer.clear();
                runCount++;
            }
        }

        if (!buffer.empty()) {
            std::stable_sort(buffer.begin(), buffer.end(), [](Record a, Record b) {
                return a.weight > b.weight;
            });

            std::string tempName = "temp_initial_" + std::to_string(runCount) + ".bin";
            current_runs.push_back(tempName);
            std::ofstream tempOut(tempName, std::ios::binary);
            tempOut.write((char*)buffer.data(), buffer.size() * sizeof(Record));
            tempOut.close();
            buffer.clear();
        }
        inFile.close();

        auto timeInternalDone = std::chrono::high_resolution_clock::now();
        std::cout << "\nThe internal sort is completed. Check the initial sorted runs! \n";

        int pass = 1;
        while (current_runs.size() > 1) {
            std::cout << "\nNow there are " << current_runs.size() << " runs.\n";
            std::vector<std::string> next_runs;

            for (size_t i = 0; i < current_runs.size(); i += 2) {
                if (i + 1 < current_runs.size()) {
                    std::string mergedName = "temp_pass_" + std::to_string(pass) + "_" + std::to_string(i) + ".bin";
                    mergeTwoFiles(current_runs[i], current_runs[i + 1], mergedName);
                    next_runs.push_back(mergedName);

                    std::remove(current_runs[i].c_str());
                    std::remove(current_runs[i + 1].c_str());
                } else {
                    next_runs.push_back(current_runs[i]);
                }
            }
            current_runs = next_runs;
            pass++;
        }

        if (current_runs.size() == 1) {
            std::cout << "\nNow there are " << current_runs.size() << " runs.\n";
            std::remove(outFileName.c_str());
            std::rename(current_runs[0].c_str(), outFileName.c_str());
        }

        auto timeExternalDone = std::chrono::high_resolution_clock::now();

        double internalTimeMs = std::chrono::duration<double, std::milli>(timeInternalDone - timeStart).count();
        double externalTimeMs = std::chrono::duration<double, std::milli>(timeExternalDone - timeInternalDone).count();
        double totalTimeMs = std::chrono::duration<double, std::milli>(timeExternalDone - timeStart).count();

        std::cout << "\nThe execution time ...\n";
        std::cout << "Internal Sort = " << internalTimeMs << " ms\n";
        std::cout << "External Sort = " << externalTimeMs << " ms\n";
        std::cout << "Total Execution Time = " << totalTimeMs << " ms\n";
    }

    // === 任務二 ===
    void mission2(std::string fileNum) {
        std::string outFileName = "order" + fileNum + ".bin";
        std::ifstream inFile(outFileName, std::ios::binary);

        if (!inFile.is_open()) {
            return;
        }

        std::cout << "\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n";
        std::cout << "* 2: Construct the primary index *\n";
        std::cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n\n";
        std::cout << "<Primary index>: (key, offset)\n";

        primaryIndex.clear();
        std::vector<Record> chunkBuffer;
        Record tempRecord;

        int currentOffset = 0;
        float lastWeight = -1.0f;

        while (inFile.read((char*)&tempRecord, sizeof(Record))) {
            chunkBuffer.push_back(tempRecord);

            if (chunkBuffer.size() == maxBufferSize) {
                for (size_t i = 0; i < chunkBuffer.size(); ++i) {
                    if (currentOffset == 0 || chunkBuffer[i].weight != lastWeight) {
                        IndexEntry entry;
                        entry.key = chunkBuffer[i].weight;
                        entry.offset = currentOffset;
                        primaryIndex.push_back(entry);
                        lastWeight = chunkBuffer[i].weight;
                    }
                    currentOffset++;
                }
                chunkBuffer.clear();
            }
        }

        if (!chunkBuffer.empty()) {
            for (size_t i = 0; i < chunkBuffer.size(); ++i) {
                if (currentOffset == 0 || chunkBuffer[i].weight != lastWeight) {
                    IndexEntry entry;
                    entry.key = chunkBuffer[i].weight;
                    entry.offset = currentOffset;
                    primaryIndex.push_back(entry);
                    lastWeight = chunkBuffer[i].weight;
                }
                currentOffset++;
            }
            chunkBuffer.clear();
        }
        inFile.close();

        for (size_t i = 0; i < primaryIndex.size(); ++i) {
            std::cout << "[" << std::setw(3) << std::right << i + 1 << "] ("
                      << primaryIndex[i].key << ", " << primaryIndex[i].offset << ")\n";
        }
    }

    // === 任務三 ===
    void mission3(std::string fileNum) {
        if (primaryIndex.empty()) {
            std::cout << "\n### Primary index is empty! Please run Mission 2 first. ###\n";
            return;
        }

        std::string outFileName = "order" + fileNum + ".bin";
        std::ifstream inFile(outFileName, std::ios::binary);
        if (!inFile.is_open()) {
            return;
        }

        // 算出排序檔總資料筆數
        inFile.seekg(0, std::ios::end);
        int totalRecords = inFile.tellg() / sizeof(Record);
        inFile.seekg(0, std::ios::beg);

        std::cout << "\n##################################\n";
        std::cout << "* 3: Range search to build index *\n";
        std::cout << "##################################\n\n";

        float lowerBound, upperBound;
        std::cout << "Input two values in (0,1] for range search.\n\n";
        std::cout << "Input a floating number in [0.01, 1.00]: ";
        std::cin >> lowerBound;
        std::cout << "Input a floating number in [0.01, 1.00]: ";
        std::cin >> upperBound;
        std::cout << "\n";

        if (lowerBound > upperBound) {
            std::swap(lowerBound, upperBound);
        }

        secondaryIndex.clear(); // 每次範圍檢索皆重新建立

        // 使用主索引分段加速定位 (大到小排序)
        int startOffset = totalRecords;
        for (size_t i = 0; i < primaryIndex.size(); ++i) {
            if (primaryIndex[i].key <= upperBound) {
                startOffset = primaryIndex[i].offset;
                break;
            }
        }

        int endOffset = totalRecords;
        for (size_t i = 0; i < primaryIndex.size(); ++i) {
            if (primaryIndex[i].key < lowerBound) {
                endOffset = primaryIndex[i].offset;
                break;
            }
        }

        int totalInBounds = endOffset - startOffset;
        if (totalInBounds < 0) totalInBounds = 0;

        // 緩衝區分批讀入指定的這段區間
        if (startOffset < endOffset) {
            inFile.seekg(startOffset * sizeof(Record), std::ios::beg);
            int recordsToRead = totalInBounds;
            int recordsRead = 0;

            std::vector<Record> chunkBuffer(maxBufferSize);

            while (recordsRead < recordsToRead) {
                int currentChunkSize = std::min(maxBufferSize, recordsToRead - recordsRead);
                inFile.read((char*)chunkBuffer.data(), currentChunkSize * sizeof(Record));

                for (int i = 0; i < currentChunkSize; ++i) {
                    int globalOffset = startOffset + recordsRead + i;
                    std::string sID(chunkBuffer[i].putID);
                    secondaryIndex[sID].push_back(globalOffset);
                }
                recordsRead += currentChunkSize;
            }
        }

        std::cout << "There are " << totalInBounds << " records in total.\n";
        std::cout << "There are " << secondaryIndex.size() << " senders in total.\n";

        int serialNum = 1;
        for (auto const& [studentID, offsets] : secondaryIndex) {
            std::cout << "[" << std::setw(4) << std::right << serialNum++ << "]  "
                      << std::setw(8) << std::left << studentID << "         "
                      << offsets.size() << "\n";
        }
        inFile.close();
    }

    // === 任務四 ===
    void mission4(std::string fileNum) {
        if (secondaryIndex.empty()) {
            std::cout << "\n### Secondary index is empty! Please run Mission 3 first. ###\n";
            return;
        }

        std::string outFileName = "order" + fileNum + ".bin";
        std::ifstream inFile(outFileName, std::ios::binary);
        if (!inFile.is_open()) {
            return;
        }

        std::string searchID;
        while (true) {
            std::cout << "\nInput a student ID ([4] Quit): ";
            std::cin >> searchID;

            if (searchID == "4") {
                break;
            }

            auto it = secondaryIndex.find(searchID);
            if (it == secondaryIndex.end()) {
                std::cout << "Sender{" << searchID << "} does not exist\n";
            } else {
                std::cout << "Sender " << searchID << " has " << it->second.size() << " records.\n";
                int itemNum = 1;

                for (int offset : it->second) {
                    inFile.seekg(offset * sizeof(Record), std::ios::beg);
                    Record rec;
                    inFile.read((char*)&rec, sizeof(Record));

                    std::cout << "[" << std::setw(3) << std::right << itemNum++ << "]   "
                              << std::setw(8) << std::left << rec.getID << "        "
                              << std::fixed << std::setprecision(2) << rec.weight << "\n";
                }
            }
        }
        inFile.close();
    }
};

int main() {
    std::string fileNum;
    ExternalSorter sorter;
    int bufSize = 300;

    while (true) {
        // 1. 印出主選單
        std::cout << "* Data Structures and Algorithms *\n";
        std::cout << "**********************************\n";
        std::cout << "* 1. External merge sort on file *\n";
        std::cout << "* 2: Construct the primary index *\n";
        std::cout << "* 3: Range search to build index *\n";
        std::cout << "* 4: Retrieve records from index *\n";
        std::cout << "**********************************\n";
        std::cout << "*** The buffer size is " << bufSize << "\n";

        // 2. 詢問 Buffer Size
        while (true) {
            std::cout << "Input a new buffer size in [300, 60000]: ";
            if (std::cin >> bufSize) {
                if (bufSize >= 300 && bufSize <= 60000) {
                    sorter.setBufferSize(bufSize);
                    break;
                }
            } else {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
            std::cout << "Invalid size! Please input a value between 300 and 60000.\n";
        }

        // 3. 詢問檔名
        while (true) {
            std::cout << "\nInput the file name: [0]Quit\n";
            std::cin >> fileNum;

            if (fileNum == "0") {
                return 0; // 直接退出程式
            }

            std::string inFileName = "pairs" + fileNum + ".bin";
            std::ifstream testFile(inFileName, std::ios::binary);

            if (testFile.is_open()) {
                testFile.close();
                break;
            } else {
                std::cout << "\n" << inFileName << " does not exist!!!\n";
            }
        }

        // 4. 先執行任務 1 和 任務 2 (同一個檔案與Buffer下只需跑一次)
        sorter.mission1(fileNum);
        sorter.mission2(fileNum);

        // 【核心修改】：內層無窮迴圈，專門負責重複執行任務 3、4
        while (true) {
            sorter.mission3(fileNum);
            sorter.mission4(fileNum);

            // 5. 雙階退出詢問
            std::string cont3 = "0";
            std::cout << "\n[3]Quit or [Any other key]continue?\n";
            std::cin >> cont3;

            if (cont3 != "3") {
                // 如果輸入的「不是 3」（例如 33 或任何其他鍵），利用 continue 直接跳過後方判斷，
                // 回到內層迴圈開頭，重新執行任務 3！
                std::cout << "\nContinuing range search...\n";
                continue;
            }

            // 如果輸入的是 3，才進到這裡詢問是否要完全離開程式 [0]
            std::string cont0 = "0";
            std::cout << "\n[0]Quit or [Any other key]continue?\n";
            std::cin >> cont0;

            if (cont0 == "0") {
                return 0; // 直接結束整個程式
            } else {
                // 如果在 [0] 選了 continue，代表想更換「別的測試檔案」或「Buffer Size」
                break; // 跳出內層迴圈，回到外層大選單
            }
        }

        // 清除殘留換行，確保回到最上方選單時輸入正常
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "\n";
    }

    return 0;
}
