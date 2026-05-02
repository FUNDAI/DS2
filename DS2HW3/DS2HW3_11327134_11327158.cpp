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

// 任務零：文字檔轉二進位檔   
bool executeTask0(std::string fileNumber) {
  std::string txtName = "input" + fileNumber + ".txt";
  std::string binName = "input" + fileNumber + ".bin";

  std::ifstream inFile(txtName);
  if (!inFile.is_open()) {
    std::cout << "\n### " << txtName << " does not exist! ###\n";
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
    std::string sid_str;
    std::string sname_str;
    int sc[6];
    float avg;

    if (ss >> sid_str >> sname_str) {
      for (int i = 0; i < 6; i++) {
        ss >> sc[i];
      }
      ss >> avg;

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

    while (true) {
      // 防呆檢查：若迴圈次數超過表格大小，代表找不到空位
      if (j >= tableSize) {
        break;
      }
      
      int pos = (hvalue + j * j) % tableSize;
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
      std::cout << "Cannot insert student: " << sid_str << " (Hash Table is full or probing failed)\n";
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
    // 輸出陣列索引，寬度 3，向右對齊
    outFile << "[" << std::setw(3) << std::right << i << "] ";
    
    if (hashTable[i].isEmpty == true) {
      outFile << "\n";
    } else {
      std::string sid_out = charArrayToString(hashTable[i].sid, 10);
      std::string sname_out = charArrayToString(hashTable[i].sname, 10);
      
      // hvalue 寬度 10
      outFile << std::setw(10) << std::right << hashTable[i].hvalue << ", ";
      // sid 寬度 10
      outFile << std::setw(10) << std::right << sid_out << ", ";
      // 姓名不設定寬度
      outFile << sname_out << ", ";
      
      // 平均分數
      // 判斷是否為整數，若是則不顯示小數點
      if (hashTable[i].mean == (int)hashTable[i].mean) {
        outFile << std::setw(10) << std::right << (int)hashTable[i].mean << "\n";
      } else {
        // 取兩位小數 (去除尾隨零) 的簡單作法
        std::stringstream temp_ss;
        temp_ss << hashTable[i].mean;
        outFile << std::setw(10) << std::right << temp_ss.str() << "\n";
      }
    }
  }
  
  // 輸出結尾分隔線
  outFile << " ----------------------------------------------------- \n";
  outFile.close();

  // 螢幕輸出
  std::cout << "\nHash table has been successfully created by Quadratic probing\n";
  std::cout << "unsuccessful search: " << std::fixed << std::setprecision(4) << avgUnsuccess << " comparisons on average\n";
  std::cout << "successful search: " << std::fixed << std::setprecision(4) << avgSuccess << " comparisons on average\n";

  return true;
}

bool executeTask2(std::string fileNumber, std::vector<Student>& students) {
    if (students.empty()) {
        std::cout << "\n### Please execute Task 1 first! ###\n";
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
                break;
            }
        }
    }
   double avgSuccess2 = (double)successfulComparisons2 / successfulSearches2;

    // 輸出檔案 (標頭改為 Double hashing)
    std::ofstream outFile(("double" + fileNumber + ".txt").c_str());
    outFile << " --- Hash table created by Double hashing ---\n";
    for (int i = 0; i < tableSize; i++) {
        outFile << "[" << std::setw(3) << std::right << i << "] ";
        if (!hashTable[i].isEmpty) {
            outFile << std::setw(10) << hashTable[i].hvalue << ", " << std::setw(10) << charArrayToString(hashTable[i].sid, 10) 
                    << ", " << charArrayToString(hashTable[i].sname, 10) << ", ";
            if (hashTable[i].mean == (int)hashTable[i].mean) {
                outFile << std::setw(10) << (int)hashTable[i].mean << "\n";
            } else { 
                std::stringstream ss; 
                ss << hashTable[i].mean; 
                outFile << std::setw(10) << ss.str() << "\n"; 
            }
        } else {
            outFile << "\n";
        }
    }
    outFile << " ----------------------------------------------------- \n";
    outFile.close();

    std::cout << "\nHash table has been successfully created by Double hashing\n";
    std::cout << "successful search: " << std::fixed << std::setprecision(4) << avgSuccess2 << " comparisons on average\n";
    return true;
}

int main() {
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
            std::cout << "\nInput a file number: "; std::cin >> fileNum;
            executeTask1(fileNum, students);
            std::cout << "\n";
        } else if (cmd == "2") {
            executeTask2(fileNum, students);
            std::cout << "\n";
        } else std::cout << "\nCommand does not exist!\n\n";
    }
    return 0;
}
