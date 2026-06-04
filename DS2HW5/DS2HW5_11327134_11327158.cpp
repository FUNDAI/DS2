#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>
#include <cstdio>

struct Record {
  char putID[10];
  char getID[10];
  float weight;
};

class ExternalSorter {
 private:
  int maxBufferSize = 300;

  // === 保持原樣：完全不更動原有 mergeTwoFiles ===
  void mergeTwoFiles(std::string file1, std::string file2, std::string outFile) {
    std::ifstream in1(file1, std::ios::binary);
    std::ifstream in2(file2, std::ios::binary);
    std::ofstream out(outFile, std::ios::binary);

    Record rec1;
    Record rec2;
    bool has1 = false;
    bool has2 = false;

    if (in1.read((char*)&rec1, sizeof(Record))) {
      has1 = true;
    }
    if (in2.read((char*)&rec2, sizeof(Record))) {
      has2 = true;
    }

    while (has1 && has2) {
      if (rec1.weight >= rec2.weight) {
        out.write((char*)&rec1, sizeof(Record));
        if (in1.read((char*)&rec1, sizeof(Record))) {
          has1 = true;
        } else {
          has1 = false;
        }
      } else {
        out.write((char*)&rec2, sizeof(Record));
        if (in2.read((char*)&rec2, sizeof(Record))) {
          has2 = true;
        } else {
          has2 = false;
        }
      }
    }

    while (has1) {
      out.write((char*)&rec1, sizeof(Record));
      if (in1.read((char*)&rec1, sizeof(Record))) {
        has1 = true;
      } else {
        has1 = false;
      }
    }

    while (has2) {
      out.write((char*)&rec2, sizeof(Record));
      if (in2.read((char*)&rec2, sizeof(Record))) {
        has2 = true;
      } else {
        has2 = false;
      }
    }

    in1.close();
    in2.close();
    out.close();
  }

 public:
  // === 保持原樣：完全不更動原有 mission1 ===
  void mission1(std::string fileNum) {
    std::string inFileName = "pairs" + fileNum + ".bin";
    std::string outFileName = "order" + fileNum + ".bin";

    std::ifstream inFile(inFileName, std::ios::binary);
    if (!inFile.is_open()) {
      std::cout << "\n### " << inFileName << " does not exist! ###\n";
      return;
    }

    // 記錄任務起點時間
    auto timeStart = std::chrono::high_resolution_clock::now();

    std::vector<Record> buffer;
    int runCount = 0;
    std::vector<std::string> current_runs;

    // --- 內部排序階段 (Internal Sort Phase) ---
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
        
        for (int i = 0; i < buffer.size(); ++i) {
          tempOut.write((char*)&buffer[i], sizeof(Record));
        }
        tempOut.close();
        
        buffer.clear();
        runCount++;
      }
    }

    // 處理最後不滿 300 筆的殘餘資料
    if (buffer.size() > 0) {
      std::stable_sort(buffer.begin(), buffer.end(), [](Record a, Record b) {
        return a.weight > b.weight;
      });

      std::string tempName = "temp_initial_" + std::to_string(runCount) + ".bin";
      current_runs.push_back(tempName);
      std::ofstream tempOut(tempName, std::ios::binary);
      
      for (int i = 0; i < buffer.size(); ++i) {
        tempOut.write((char*)&buffer[i], sizeof(Record));
      }
      tempOut.close();
      
      buffer.clear();
    }
    inFile.close();

    // 記錄內部排序完成時間
    auto timeInternalDone = std::chrono::high_resolution_clock::now();

    std::cout << "\nThe internal sort is completed. Check the initial sorted runs! \n";
    std::cout << "\nNow there are " << current_runs.size() << " runs.\n";

    // --- 外部合併排序階段 (External Merge Sort Phase) ---
    int pass = 1;
    while (current_runs.size() > 1) {
      std::vector<std::string> next_runs;
      
      for (int i = 0; i < current_runs.size(); i += 2) {
        if (i + 1 < current_runs.size()) {
          std::string mergedName = "temp_pass_" + std::to_string(pass) + "_" + std::to_string(i) + ".bin";
          mergeTwoFiles(current_runs[i], current_runs[i + 1], mergedName);
          next_runs.push_back(mergedName);
          
          // 刪除已經合併完畢的舊檔案
          std::remove(current_runs[i].c_str());
          std::remove(current_runs[i + 1].c_str());
        } else {
          // 落單的檔案直接推進下一回合
          next_runs.push_back(current_runs[i]);
        }
      }
      
      current_runs = next_runs;
      std::cout << "\nNow there are " << current_runs.size() << " runs.\n";
      pass++;
    }

    // 將最終的單一檔案重新命名為目標輸出檔
    if (current_runs.size() == 1) {
      std::remove(outFileName.c_str());
      std::rename(current_runs[0].c_str(), outFileName.c_str());
    }

    // 記錄外部排序完成時間
    auto timeExternalDone = std::chrono::high_resolution_clock::now();

    // 計算各階段時間 (ms)
    double internalTimeMs = std::chrono::duration<double, std::milli>(timeInternalDone - timeStart).count();
    double externalTimeMs = std::chrono::duration<double, std::milli>(timeExternalDone - timeInternalDone).count();
    double totalTimeMs = std::chrono::duration<double, std::milli>(timeExternalDone - timeStart).count();

    std::cout << "\nThe execution time ...\n";
    std::cout << "Internal Sort = " << internalTimeMs << " ms\n";
    std::cout << "External Sort = " << externalTimeMs << " ms\n";
    std::cout << "Total Execution Time = " << totalTimeMs << " ms\n";
  }

  // === 任務二：建立主索引函數 （計算結果調整為與您的 FILE* 版本一致的筆數 offset） ===
  void mission2(std::string fileNum) {
    std::string outFileName = "order" + fileNum + ".bin";
    std::ifstream inFile(outFileName, std::ios::binary);

    if (!inFile.is_open()) {
      std::cout << "\n### " << outFileName << " does not exist! ###\n";
      return;
    }

    std::cout << "\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n";
    std::cout << "Mission 2: Build the primary index \n";
    std::cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n\n";
    std::cout << "<Primary index>: (key, offset)\n";

    struct IndexEntry {
      float key;
      int offset; // 改回 int 型態，對應筆數編號
    };

    std::vector<IndexEntry> primaryIndex;
    std::vector<Record> chunkBuffer;
    Record tempRecord;

    int currentOffset = 0; // 筆數累計序號計算器，完全對應原 FILE* 版本的計數邏輯
    float lastWeight = -1.0f;

    while (true) {
      // 分批讀取，每次最多 300 筆（符合不一次載入記憶體之規範）
      if (inFile.read((char*)&tempRecord, sizeof(Record))) {
        chunkBuffer.push_back(tempRecord);

        if (chunkBuffer.size() == maxBufferSize) {
          for (size_t i = 0; i < chunkBuffer.size(); ++i) {
            // 與您的原 FILE* 計算式同：第一個元素，或是當 weight 改變時
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
      } else {
        // 處理最後殘餘不滿 300 筆的資料
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
        break;
      }
    }
    inFile.close();

    // 依序輸出主索引內容（完全複製原 printf 的輸出樣式 %zu、%g、%d）
    for (size_t i = 0; i < primaryIndex.size(); ++i) {
      printf("[%zu] (%g, %d)\n", i + 1, primaryIndex[i].key, primaryIndex[i].offset);
    }
  }
};

int main() {
  std::string fileNum;
  ExternalSorter sorter;

  while (true) {
    // 輸出指定的選單介面
    std::cout << "* Data Structures and Algorithms *\n";
    std::cout << "**********************************\n";
    std::cout << "* 1. External merge sort on file *\n";
    std::cout << "* 2: Construct the primary index *\n";
    std::cout << "**********************************\n";
    std::cout << "*** The buffer size is 300\n";
    std::cout << "##################################\n";
    std::cout << "Mission 1: External merge sort \n";
    std::cout << "##################################\n\n";
    
    // 檔名防呆與重新輸入迴圈
    while (true) {
      std::cout << "Input the file name: [0]Quit\n";
      std::cin >> fileNum;

      if (fileNum == "0") {
        break; // 若輸入 0 則跳出輸入迴圈
      }

      std::string inFileName = "pairs" + fileNum + ".bin";
      std::ifstream testFile(inFileName, std::ios::binary);

      if (testFile.is_open()) {
        testFile.close(); 
        break; // 檔案存在，跳出輸入迴圈，準備執行任務
      } else {
        // 檔案不存在，印出錯誤格式並繼續迴圈重新輸入
        std::cout << "\n" << inFileName << " does not exist!!!\n\n";
      }
    }

    // 只有在輸入不是 0 的情況下，才去依序執行任務一與任務二
    if (fileNum != "0") {
      sorter.mission1(fileNum);
      sorter.mission2(fileNum); // 直接在這裡呼叫你寫好的任務二
    }
    
    // 任務執行完畢（或輸入 0 放棄任務）後，詢問是否繼續
    std::string cont;
    std::cout << "\n[0]Quit or [Any other key]continue?\n";
    std::cin >> cont;
    
    if (cont == "0") {
      break; 
    }
    
    std::cout << "\n";
  }

  return 0;
}
