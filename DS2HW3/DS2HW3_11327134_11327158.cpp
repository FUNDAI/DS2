// 11327134 曾苡綾 11327158 謝亞諺
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <cstring>
#include <cmath> // 為了使用 round()

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
    if (line.length() == 0) continue;

    std::stringstream ss(line);
    std::string token;
    std::vector<std::string> tokens;

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

      for (int i = 0; i < sid_str.length() && i < 9; i++) s.sid[i] = sid_str[i];
      for (int i = 0; i < sname_str.length() && i < 9; i++) s.sname[i] = sname_str[i];
      for (int i = 0; i < 6; i++) s.score[i] = (unsigned char)sc[i];
      s.average = avg;

      outFile.write((char*)&s, sizeof(Student));
    }
  }
  
  inFile.close();
  outFile.close();
  return true;
}

bool isPrime(int n) {
  if (n <= 1) return false;
  if (n == 2) return true;
  if (n % 2 == 0) return false;
  for (int i = 3; i * i <= n; i = i + 2) if (n % i == 0) return false;
  return true;
}

int getNextPrime(int n) {
  while (true) {
    if (isPrime(n)) return n;
    n = n + 1;
  }
}

int hashFunction(std::string key, int tableSize) {
  unsigned long long product = 1;
  for (int i = 0; i < key.length(); i++) product = product * (unsigned char)(key[i]);
  return product % tableSize;
}

std::string charArrayToString(char arr[], int size) {
  std::string res = "";
  for (int i = 0; i < size; i++) {
    if (arr[i] == '\0') break;
    res = res + arr[i];
  }
  return res;
}

unsigned long long getKeyProduct(std::string key) {
    unsigned long long product = 1;
    for (int i = 0; i < key.length(); i++) product = product * (unsigned char)(key[i]);
    return product;
}

std::string formatScore(float val) {
    double rounded = std::round(val * 100.0) / 100.0;
    std::stringstream ss;
    ss << rounded;
    return ss.str();
}

void searchTask3(const std::vector<HashNode>& hashTable, int tableSize) {
    std::string searchSid;
    while (true) {
        std::cout << "Input a student ID to search ([0] Quit): ";
        std::cin >> searchSid;
        if (searchSid == "0") break;

        int hvalue = hashFunction(searchSid, tableSize);
        int probeCount = 0;
        bool found = false;

        for (int j = 0; j < tableSize; j++) {
            probeCount++;
            int pos = (hvalue + (int)pow(j, 2)) % tableSize;
            if (hashTable[pos].isEmpty) break; 
            if (charArrayToString((char*)hashTable[pos].sid, 10) == searchSid) {
                std::cout << "{ " << charArrayToString((char*)hashTable[pos].sid, 10) << ", " 
                          << charArrayToString((char*)hashTable[pos].sname, 10) << ", " 
                          << formatScore(hashTable[pos].mean) << " } is found after " << probeCount << " probes.\n";
                found = true;
                break;
            }
        }
        if (!found) std::cout << searchSid << " is not found after " << probeCount << " probes.\n";
    }
}

void searchTask4(const std::vector<HashNode>& hashTable, int tableSize, int maxStep) {
    std::string searchSid;
    while (true) {
        std::cout << "Input a student ID to search ([0] Quit): ";
        std::cin >> searchSid;
        if (searchSid == "0") break;

        unsigned long long prod = getKeyProduct(searchSid);
        int hvalue = prod % tableSize;
        int step = maxStep - (int)(prod % maxStep);
        int probeCount = 0;
        bool found = false;

        for (int j = 0; j < tableSize; j++) {
            probeCount++;
            int pos = (hvalue + j * step) % tableSize;
            if (hashTable[pos].isEmpty) break;
            if (charArrayToString((char*)hashTable[pos].sid, 10) == searchSid) {
                std::cout << "{ " << charArrayToString((char*)hashTable[pos].sid, 10) << ", " 
                          << charArrayToString((char*)hashTable[pos].sname, 10) << ", " 
                          << formatScore(hashTable[pos].mean) << " } is found after " << probeCount << " probes.\n";
                found = true;
                break;
            }
        }
        if (!found) std::cout << searchSid << " is not found after " << probeCount << " probes.\n";
    }
}

bool executeTask1(std::string fileNumber, std::vector<Student> &students) {
  students.clear();
  std::string binName = "input" + fileNumber + ".bin";
  std::ifstream inFile(binName, std::ios::binary);

  if (!inFile.is_open()) {
    std::cout << "\n### " << binName << " does not exist! ###\n";
    if (!executeTask0(fileNumber)) return false;
    inFile.open(binName, std::ios::binary);
    if (!inFile.is_open()) return false;
  }

  Student temp;
  while (inFile.read((char*)&temp, sizeof(Student))) students.push_back(temp);
  inFile.close();

  if (students.size() == 0) return false;

  int tableSize = getNextPrime((int)(students.size() * 1.15) + 1);
  std::vector<HashNode> hashTable(tableSize);
  int successfulSearches = 0, successfulComparisons = 0;

  for (int i = 0; i < (int)students.size(); i++) {
    std::string sid_str = charArrayToString(students[i].sid, 10);
    int hvalue = hashFunction(sid_str, tableSize);
    bool inserted = false;
    for (int j = 0; j < tableSize; j++) {
      int pos = (hvalue + (int)pow(j, 2)) % tableSize;
      if (hashTable[pos].isEmpty) {
        hashTable[pos].isEmpty = false;
        hashTable[pos].hvalue = hvalue;
        memcpy(hashTable[pos].sid, students[i].sid, 10);
        memcpy(hashTable[pos].sname, students[i].sname, 10);
        hashTable[pos].mean = students[i].average;
        successfulSearches++;
        successfulComparisons += (j + 1);
        inserted = true;
        break;
      }
    }
    if (!inserted) std::cout << "### Failed at [" << i << "]. ###\n";
  }

  int unsuccessfulComparisons = 0;
  for (int i = 0; i < tableSize; i++) {
    for (int j = 0; j < tableSize; j++) {
      int pos = (i + (int)pow(j, 2)) % tableSize;
      if (hashTable[pos].isEmpty) break;
      unsuccessfulComparisons++;
    }
  }

  std::cout << "\nHash table has been successfully created by Quadratic probing\n";
  std::cout << "unsuccessful search: " << std::fixed << std::setprecision(4) << (double)unsuccessfulComparisons / tableSize << " comparisons on average\n";
  std::cout << "successful search: " << std::fixed << std::setprecision(4) << (double)successfulComparisons / successfulSearches << " comparisons on average\n";

  searchTask3(hashTable, tableSize);
  return true;
}

bool executeTask2(std::string fileNumber, std::vector<Student>& students) {
    if (students.empty()) { std::cout << "### Command 1 first. ###\n\n"; return false; }
    int successfulSearches2 = 0, successfulComparisons2 = 0;
    int tableSize = getNextPrime((int)(students.size() * 1.15) + 1);
    int maxStep = getNextPrime((int)students.size() / 5 + 1);
    std::vector<HashNode> hashTable(tableSize);

    for (int i = 0; i < (int)students.size(); i++) {
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
                memcpy(hashTable[pos].sid, students[i].sid, 10);
                memcpy(hashTable[pos].sname, students[i].sname, 10);
                successfulSearches2++;
                successfulComparisons2 += (j + 1);
                inserted = true;
                break;
            }
        }
        if (!inserted) std::cout << "### Failed at [" << i << "]. ###\n";
    }
  std::cout << "\nHash table has been successfully created by Double hashing\n";
  std::cout << "successful search: " << std::fixed << std::setprecision(4) << (double)successfulComparisons2 / successfulSearches2 << " comparisons on average\n";
  searchTask4(hashTable, tableSize, maxStep);
  return true;
}

int main() {
    std::string cmd, fileNum;
    std::vector<Student> students;
    while (true) {
        std::cout << "* Data Structures and Algorithms *\n************ Hash Table **********\n* 0. QUIT                        *\n* 1. Quadratic probing           *\n* 2. Double hashing              *\n**********************************\n";
        std::cout << "Input a choice(0, 1, 2): ";
        std::cin >> cmd;
        if (cmd == "0") break;
        else if (cmd == "1") {
            std::cout << "\nInput a file number ([0] Quit): "; 
            std::cin >> fileNum;
            if (fileNum != "0") executeTask1(fileNum, students);
            std::cout << "\n";
        } else if (cmd == "2") {
            executeTask2(fileNum, students);
            std::cout << "\n";
        } else std::cout << "\nCommand does not exist!\n\n\n";
    }
    return 0;
}
