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
    
    // --- 新增：錯誤檔名防呆與重新輸入迴圈 ---
    while (true) {
      std::cout << "Input the file name: [0]Quit\n";
      std::cin >> fileNum;

      if (fileNum == "0") {
        break; // 若輸入 0 則跳出輸入迴圈
      }

      std::string inFileName = "pairs" + fileNum + ".bin";
      std::ifstream testFile(inFileName, std::ios::binary);

      if (testFile.is_open()) {
        testFile.close(); // 檔案存在，關閉測試用的串流
        break;            // 跳出輸入迴圈，繼續往下執行任務
      } else {
        // 檔案不存在，印出你指定的錯誤格式，並繼續迴圈重新輸入
        std::cout << "\n" << inFileName << " does not exist!!!\n\n";
      }
    }

    if (fileNum == "0") {
      break; // 若剛才是因為輸入 0 退出輸入迴圈，則直接結束整個程式
    }

    // 確定檔案存在後，將檔名交給 mission1 執行
    sorter.mission1(fileNum);
    
    // 任務執行完畢後詢問是否繼續
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