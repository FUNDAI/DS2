// 11327134 曾苡綾 11327158 謝亞諺
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>

// 任務零指定的二進位資料結構
struct Student {
  char sid[10];
  char sname[10];
  unsigned char score[6];
  float average;
};

// 任務一指定的雜湊表節點結構
struct HashNode {
  bool isEmpty;
  int hvalue;
  char sid[10];
  char sname[10];
  float mean;

  HashNode() {
    isEmpty = true;
    hvalue = 0;
    mean = 0.0f;
    for (int i = 0; i < 10; i++) {
      sid[i] = '\0';
      sname[i] = '\0';
    }
  }
};

class ForDC {
  int temp;
};

// 任務零：文字檔轉二進位檔   
bool executeTask0(std::string fileNumber) {
  std::string txtName = "input" + fileNumber + ".txt";
  std::string binName = "input" + fileNumber + ".bin";

  std::ifstream inFile(txtName);
  if (!inFile.is_open()) {
    std::cout << "\n### " << txtName << " does not exist! ###\n\n";
    return false;
  }

  std::ofstream outFile(binName, std::ios::binary);

  std::string line;
  while (std::getline(inFile, line)) {
    if (line.length() > 0) {
      if (line[line.length() - 1] == '\r') {
        line.erase(line.length() - 1);
      }
    }
    if (line.length() == 0) {
      continue;
    }

    std::stringstream ss(line);
    std::string token;
    std::vector<std::string> tokens;

    // 修復：使用 Tab (\t) 精準切割欄位，避免名字中有空格造成錯位
    while (std::getline(ss, token, '\t')) {
      tokens.push_back(token);
    }

    if (tokens.size() >= 9) {
      std::string sid_str = tokens[0];
      std::string sname_str = tokens[1];
      int sc[6];
      float avg;

      for (int i = 0; i < 6; i++) {
        std::stringstream(tokens[2 + i]) >> sc[i];
      }
      std::stringstream(tokens[8]) >> avg;

      Student s;
      for (int i = 0; i < 10; i++) {
        s.sid[i] = '\0';
        s.sname[i] = '\0';
      }

      for (int i = 0; i < sid_str.length() && i < 9; i++) {
        s.sid[i] = sid_str[i];
      }
      for (int i = 0; i < sname_str.length() && i < 9; i++) {
        s.sname[i] = sname_str[i];
      }
      for (int i = 0; i < 6; i++) {
        s.score[i] = (unsigned char)sc[i];
      }
      s.average = avg;

      outFile.write((char*)&s, sizeof(Student));
    }
  }
  
  inFile.close();
  outFile.close();
  return true;
}

// 判斷是否為質數的輔助函式
bool isPrime(int n) {
  if (n <= 1) {
    return false;
  }
  if (n == 2) {
    return true;
  }
  if (n % 2 == 0) {
    return false;
  }
  for (int i = 3; i * i <= n; i = i + 2) {
    if (n % i == 0) {
      return false;
    }
  }
  return true;
}

// 尋找大於指定數值的最小質數
int getNextPrime(int n) {
  while (true) {
    if (isPrime(n) == true) {
      return n;
    }
    n = n + 1;
  }
}

// ASCII 編碼相乘的雜湊函數
int hashFunction(std::string key, int tableSize) {
  unsigned long long product = 1;
  for (int i = 0; i < key.length(); i++) {
    product = product * (unsigned char)(key[i]);
  }
  return product % tableSize;
}

// 將字元陣列安全轉換為字串
std::string charArrayToString(char arr[], int size) {
  std::string res = "";
  for (int i = 0; i < size; i++) {
    if (arr[i] == '\0') {
      break;
    }
    res = res + arr[i];
  }
  return res;
}


unsigned long long getKeyProduct(std::string key) {
    unsigned long long product = 1;
    for (int i = 0; i < key.length(); i++) {
        product = product * (unsigned char)(key[i]);
    }
    return product;
}



bool executeTask1(std::string fileNumber, std::vector<Student> &students) {
  students.clear();
  std::string binName = "input" + fileNumber + ".bin";
  std::ifstream inFile(binName, std::ios::binary);

  // 檢查二進位檔是否存在，若不存在則先執行任務零
  if (!inFile.is_open()) {
    std::cout << "\n### " << binName << " does not exist! ###\n";
    if (executeTask0(fileNumber) == false) {
      return false;
    }
    inFile.open(binName, std::ios::binary);
    if (!inFile.is_open()) {
      return false;
    }
  }

  Student temp;
  while (inFile.read((char*)&temp, sizeof(Student))) {
    students.push_back(temp);
  }
  inFile.close();

  if (students.size() == 0) {
    std::cout << "### No data in file! ###\n";
    return false;
  }

  int totalRecords = students.size();
  // 雜湊表大小 = 大於 1.15 倍資料總筆數的最小質數
  int minVal = totalRecords * 1.15;
  int tableSize = getNextPrime(minVal + 1);

  std::vector<HashNode> hashTable(tableSize);

  int successfulSearches = 0;
  int successfulComparisons = 0;

  // 依序將資料加入雜湊表
  for (int i = 0; i < totalRecords; i++) {
    std::string sid_str = charArrayToString(students[i].sid, 10);
    int hvalue = hashFunction(sid_str, tableSize);
    int j = 0;
    bool inserted = false;
    int pos = 0;
    while (true) {
      // 防呆檢查：若迴圈次數超過表格大小，代表找不到空位
      if (j >= tableSize) {
        break;
      }
      
      pos = (hvalue + j * j) % tableSize;
      if (hashTable[pos].isEmpty == true) {
        hashTable[pos].isEmpty = false;
        hashTable[pos].hvalue = hvalue;
        for (int k = 0; k < 10; k++) {
          hashTable[pos].sid[k] = students[i].sid[k];
          hashTable[pos].sname[k] = students[i].sname[k];
        }
        hashTable[pos].mean = students[i].average;

        successfulSearches = successfulSearches + 1;
        successfulComparisons = successfulComparisons + (j + 1);
        inserted = true;
        break;
      }
      j = j + 1;
    }

    if (inserted == false) {
      std::cout << "### Failed at [" << i << "]. ###\n";
    }
  }

  // 計算搜尋不存在值的比較次數
  int unsuccessfulComparisons = 0;
  for (int i = 0; i < tableSize; i++) {
    int j = 0;
    int count = 0;
    while (true) {
      int pos = (i + j * j) % tableSize;
      
      if (hashTable[pos].isEmpty == true) {
        break;
      }
      count = count + 1;
      // 防呆機制：避免無限迴圈
      if (j >= tableSize) {
        break; 
      }
      j = j + 1;
    }
    unsuccessfulComparisons = unsuccessfulComparisons + count;
  }

  double avgSuccess = (double)successfulComparisons / successfulSearches;
  double avgUnsuccess = (double)unsuccessfulComparisons / tableSize;

  // 輸出檔案
  std::string outTxtName = "quadratic" + fileNumber + ".txt";
  std::ofstream outFile(outTxtName);
  
  // 輸出標頭
outFile << " --- Hash table created by Quadratic probing ---\n";
  
  for (int i = 0; i < tableSize; i++) {
    outFile << "[" << std::setw(3) << std::right << i << "] ";
    
    if (hashTable[i].isEmpty == true) {
      outFile << "\n";
    } else {
      std::string sid_out = charArrayToString(hashTable[i].sid, 10);
      std::string sname_out = charArrayToString(hashTable[i].sname, 10);
      
      // 統一設定寬度 10，向右對齊
      outFile << std::setw(10) << std::right << hashTable[i].hvalue << ", "
              << std::setw(10) << std::right << sid_out << ", "
              << std::setw(10) << std::right << sname_out << ", ";
              
      if (hashTable[i].mean == (int)hashTable[i].mean) {
        outFile << std::setw(10) << std::right << (int)hashTable[i].mean << "\n";
      } else {
        std::stringstream ss;
        ss << hashTable[i].mean;
        outFile << std::setw(10) << std::right << ss.str() << "\n";
      }
    }
  }
  outFile << " ----------------------------------------------------- \n";
  outFile.close();

  // 螢幕輸出
  std::cout << "\nHash table has been successfully created by Quadratic probing\n";
  std::cout << "unsuccessful search: " << std::fixed << std::setprecision(4) << avgUnsuccess << " comparisons on average\n";
  std::cout << "successful search: " << std::fixed << std::setprecision(4) << avgSuccess << " comparisons on average\n";

  std::string targetId;
  while (true) {
    std::cout << "Input a student ID to search ([0] Quit): ";
    std::cin >> targetId;

    if (targetId == "0") {
      std::cout << std::endl;
      break;
    }

    int hvalue = hashFunction(targetId, tableSize);
    int j = 0;
    bool found = false;
    int probes = 0;

    // 開始平方探測搜尋
    while (true) {
      if (j >= tableSize) {
        break;
      }

      probes = probes + 1;
      int pos = (hvalue + j * j) % tableSize;

      if (hashTable[pos].isEmpty == true) {
        break; 
      }
       
      std::string currentSid = charArrayToString(hashTable[pos].sid, 10);
      if (currentSid == targetId) {
        found = true;
        std::string sname_out = charArrayToString(hashTable[pos].sname, 10);

        std::cout << "\n{ " << currentSid << ", " << sname_out << ", ";
        if (hashTable[pos].mean == (int)hashTable[pos].mean) {
          std::cout << (int)hashTable[pos].mean;
        } else {
          std::stringstream ss;
          ss << hashTable[pos].mean;
          std::cout << ss.str();
        }
        std::cout << " } is found after " << probes << " probes.\n\n";
        break;
      }

      j = j + 1;
    }

    if (found == false) {
      std::cout << std::endl << targetId << " is not found after " << probes << " probes.\n\n";
    }
  }

  return true;
}

bool executeTask2(std::string fileNumber, std::vector<Student>& students) {
    if (students.empty()) {
        std::cout << "### Command 1 first. ###\n\n";
        return false;
    }
    
    int successfulSearches2 = 0;
    int successfulComparisons2 = 0;

    int tableSize = getNextPrime((int)(students.size() * 1.15) + 1);
    int maxStep = getNextPrime((int)students.size() / 5 + 1);
    std::vector<HashNode> hashTable(tableSize);
    int successComp = 0;

    for (int i = 0; i < students.size(); i++) {
        unsigned long long prod = getKeyProduct(charArrayToString(students[i].sid, 10));
        int hvalue = prod % tableSize;
        int step = maxStep - (int)(prod % maxStep);
        bool inserted = false;

        for (int j = 0; j < tableSize; j++) {
            int pos = (hvalue + j * step) % tableSize;
            if (hashTable[pos].isEmpty) {
                hashTable[pos].isEmpty = false;
                hashTable[pos].hvalue = hvalue;
                hashTable[pos].mean = students[i].average;
                for (int k = 0; k < 10; k++) {
                    hashTable[pos].sid[k] = students[i].sid[k];
                    hashTable[pos].sname[k] = students[i].sname[k];
                }        
                successfulSearches2 = successfulSearches2 + 1;
                successfulComparisons2 = successfulComparisons2 + (j + 1);
                inserted = true;
                break;
            }
        }
        if (!inserted) std::cout << "### Failed at [" << i << "]. ###\n";
    }
   double avgSuccess2 = (double)successfulComparisons2 / successfulSearches2;


  std::ofstream outFile(("double" + fileNumber + ".txt").c_str());
outFile << " --- Hash table created by Double hashing    ---\n";
  
  for (int i = 0; i < tableSize; i++) {
    // 輸出陣列索引，寬度 3
    outFile << "[" << std::setw(3) << std::right << i << "] ";
    
    if (!hashTable[i].isEmpty) {
      std::string sid_out = charArrayToString(hashTable[i].sid, 10);
      std::string sname_out = charArrayToString(hashTable[i].sname, 10);

      // hvalue, sid, sname 統一設定寬度 10，向右對齊
      outFile << std::setw(10) << std::right << hashTable[i].hvalue << ", " 
              << std::setw(10) << std::right << sid_out << ", " 
              << std::setw(10) << std::right << sname_out << ", ";
              
      // 分數處理
      if (hashTable[i].mean == (int)hashTable[i].mean) {
        outFile << std::setw(10) << std::right << (int)hashTable[i].mean << "\n";
      } else { 
        std::stringstream ss; 
        ss << hashTable[i].mean; 
        outFile << std::setw(10) << std::right << ss.str() << "\n"; 
      }
    } else {
      outFile << "\n";
    }
  }
  
  outFile << " ----------------------------------------------------- \n";
  outFile.close();

  std::cout << "\nHash table has been successfully created by Double hashing   \n";
  std::cout << "successful search: " << std::fixed << std::setprecision(4) << avgSuccess2 << " comparisons on average\n";

  std::string targetId;
  while (true) {
    std::cout << "Input a student ID to search ([0] Quit): ";
    std::cin >> targetId;

    if (targetId == "0") {
      std::cout << std::endl;
      break;
    }

    unsigned long long prod = getKeyProduct(targetId);
    int hvalue = prod % tableSize;
    int step = maxStep - (int)(prod % maxStep);
    int j = 0;
    bool found = false;
    int probes = 0;

    // 開始雙重雜湊搜尋
    while (true) {
      if (j >= tableSize) {
        break;
      }

      probes = probes + 1;
      int pos = (hvalue + j * step) % tableSize;

      if (hashTable[pos].isEmpty == true) {
        break; // 遇到空位，代表絕對不存在於表中
      }

      std::string currentSid = charArrayToString(hashTable[pos].sid, 10);
      if (currentSid == targetId) {
        found = true;
        std::string sname_out = charArrayToString(hashTable[pos].sname, 10);

        std::cout << "\n{ " << currentSid << ", " << sname_out << ", ";
        if (hashTable[pos].mean == (int)hashTable[pos].mean) {
          std::cout << (int)hashTable[pos].mean;
        } else {
          std::stringstream ss;
          ss << hashTable[pos].mean;
          std::cout << ss.str();
        }
        std::cout << " } is found after " << probes << " probes.\n\n";
        break;
      }

      j = j + 1;
    }

    if (found == false) {
      std::cout << std::endl << targetId << " is not found after " << probes << " probes.\n\n";
    }
  }

  return true;
}

int main() {
    ForDC wut;
    std::string cmd, fileNum;
    std::vector<Student> students;
    while (true) {
        std::cout << "* Data Structures and Algorithms *\n";
        std::cout << "************ Hash Table **********\n";
        std::cout << "* 0. QUIT                        *\n";
        std::cout << "* 1. Quadratic probing           *\n";
        std::cout << "* 2. Double hashing              *\n";
        std::cout << "**********************************\n";
        std::cout << "Input a choice(0, 1, 2): ";
        std::cin >> cmd;
        if (cmd == "0") break;
        else if (cmd == "1") {
            std::cout << "\nInput a file number ([0] Quit): "; 
            std::cin >> fileNum;
            if (fileNum != "0") {
              executeTask1(fileNum, students);
            } else {
              std::cout << "\n";
            }
            std::cout << "\n";
        } else if (cmd == "2") {
            executeTask2(fileNum, students);
            std::cout << "\n";
        } else std::cout << "\nCommand does not exist!\n\n\n";
    }
    return 0;
}