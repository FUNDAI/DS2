// 11327158 謝亞諺

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>

class DataNode {
public:
  int id;
  std::string school;
  std::string dept;
  std::string dayNight;
  std::string level;
  int graduates;
  
  DataNode() {
    id = 0;
    graduates = 0;
  }
  
  DataNode(int _id, std::string _school, std::string _dept, std::string _dayNight, std::string _level, int _graduates) {
    id = _id;
    school = _school;
    dept = _dept;
    dayNight = _dayNight;
    level = _level;
    graduates = _graduates;
  }
};

class MaxHeap {
private:
  std::vector<DataNode> heap;
  
public:
  MaxHeap() {
    DataNode dummy;
    heap.push_back(dummy); // dummy node at index 0
  }
  
  void clear() {
    heap.clear();
    DataNode dummy;
    heap.push_back(dummy);
  }
  
  void insert(DataNode node) {
    heap.push_back(node);
    int currentIdx = heap.size() - 1;
    
    while (currentIdx > 1) {
      int parentIdx = currentIdx / 2;
      if (heap[currentIdx].graduates > heap[parentIdx].graduates) {
        DataNode temp = heap[currentIdx];
        heap[currentIdx] = heap[parentIdx];
        heap[parentIdx] = temp;
        currentIdx = parentIdx;
      } else {
        break;
      }
    }
  }
  
  void printResults() {
    if (heap.size() <= 1) {
      std::cout << "<Empty Max Heap!>\n";
      return;
    }
    
    // Root
    std::cout << "root: [" << heap[1].id << "] " << heap[1].graduates << "\n";
    
    // Bottom
    int bottomIdx = heap.size() - 1;
    std::cout << "bottom: [" << heap[bottomIdx].id << "] " << heap[bottomIdx].graduates << "\n";
    
    // Left-most bottom
    int leftMostIdx = 1;
    while (leftMostIdx * 2 <= bottomIdx) {
      leftMostIdx = leftMostIdx * 2;
    }
    std::cout << "leftmost bottom: [" << heap[leftMostIdx].id << "] " << heap[leftMostIdx].graduates << "\n";
  }
};

class Deap {
private:
  std::vector<DataNode> deap;
  
  bool isLeftTree(int idx) {
    if (idx == 2) return true;
    if (idx == 3) return false;
    int temp = idx;
    while (temp > 3) {
      temp /= 2;
    }
    return temp == 2;
  }
  
  int getOffset(int idx) {
    int temp = idx;
    int depth = 0;
    while (temp > 1) {
      temp = temp / 2;
      depth++;
    }
    int offset = 1;
    for (int i = 0; i < depth - 1; i++) {
        offset *= 2;
    }
    return offset;
  }
  
  int getCorrespondingInMax(int idx, int lastIdx) {
    int offset = getOffset(idx);
    int corresp = idx + offset;
    if (corresp > lastIdx) {
      corresp = corresp / 2;
    }
    return corresp;
  }
  
  void bubbleUpMin(int idx) {
    int parentIdx = idx / 2;
    while (parentIdx >= 2) {
      if (deap[idx].graduates < deap[parentIdx].graduates) {
        DataNode temp = deap[idx];
        deap[idx] = deap[parentIdx];
        deap[parentIdx] = temp;
        idx = parentIdx;
        parentIdx = idx / 2;
      } else {
        break;
      }
    }
  }
  
  void bubbleUpMax(int idx) {
    int parentIdx = idx / 2;
    while (parentIdx >= 3) {
      if (deap[idx].graduates > deap[parentIdx].graduates) {
        DataNode temp = deap[idx];
        deap[idx] = deap[parentIdx];
        deap[parentIdx] = temp;
        idx = parentIdx;
        parentIdx = idx / 2;
      } else {
        break;
      }
    }
  }

public:
  Deap() {
    DataNode dummy;
    deap.push_back(dummy); // index 0 (unused)
    deap.push_back(dummy); // index 1 (unused/empty)
  }
  
  void clear() {
    deap.clear();
    DataNode dummy;
    deap.push_back(dummy);
    deap.push_back(dummy);
  }
  
  void insert(DataNode node) {
    deap.push_back(node);
    int currentIdx = deap.size() - 1;
    
    if (currentIdx == 2) {
      return; // 2 is the min heap root
    }
    
    if (isLeftTree(currentIdx)) {
      int partnerIdx = getCorrespondingInMax(currentIdx, currentIdx);
      if (deap[currentIdx].graduates > deap[partnerIdx].graduates) {
        DataNode temp = deap[currentIdx];
        deap[currentIdx] = deap[partnerIdx];
        deap[partnerIdx] = temp;
        bubbleUpMax(partnerIdx);
      } else {
        bubbleUpMin(currentIdx);
      }
    } else {
      int partnerIdx = currentIdx - getOffset(currentIdx);
      if (deap[currentIdx].graduates < deap[partnerIdx].graduates) {
        DataNode temp = deap[currentIdx];
        deap[currentIdx] = deap[partnerIdx];
        deap[partnerIdx] = temp;
        bubbleUpMin(partnerIdx);
      } else {
        bubbleUpMax(currentIdx);
      }
    }
  }
  
  void printResults() {
    if (deap.size() <= 2) {
      std::cout << "<Empty Deap!>\n";
      return;
    }
    
    int bottomIdx = deap.size() - 1;
    std::cout << "bottom: [" << deap[bottomIdx].id << "] " << deap[bottomIdx].graduates << "\n";
    
    int leftMost = 2;
    while (leftMost * 2 <= bottomIdx) {
      leftMost *= 2;
    }
    std::cout << "leftmost bottom: [" << deap[leftMost].id << "] " << deap[leftMost].graduates << "\n";
  }
};

class MinMaxHeap {
private:
  std::vector<DataNode> heap;

  bool isMinLevel(int idx) {
    int depth = 0;
    while (idx > 1) {
      idx /= 2;
      depth++;
    }
    return depth % 2 == 0;
  }

  void bubbleUpMin(int idx) {
    int grandparentIdx = idx / 4;
    while (grandparentIdx >= 1) {
      if (heap[idx].graduates < heap[grandparentIdx].graduates) {
        std::swap(heap[idx], heap[grandparentIdx]);
        idx = grandparentIdx;
        grandparentIdx = idx / 4;
      } else {
        break;
      }
    }
  }

  void bubbleUpMax(int idx) {
    int grandparentIdx = idx / 4;
    while (grandparentIdx >= 1) {
      if (heap[idx].graduates > heap[grandparentIdx].graduates) {
        std::swap(heap[idx], heap[grandparentIdx]);
        idx = grandparentIdx;
        grandparentIdx = idx / 4;
      } else {
        break;
      }
    }
  }

  int getMaxChildOrGrandchild(int idx) {
    int maxIdx = -1;
    // 子節點與孫節點 (由左至右，確保遇到同值時可以取左側)
    int indices[] = {2*idx, 2*idx+1, 4*idx, 4*idx+1, 4*idx+2, 4*idx+3};
    for (int c : indices) {
      if (c < (int)heap.size()) {
        if (maxIdx == -1 || heap[c].graduates > heap[maxIdx].graduates) {
          maxIdx = c;
        }
      }
    }
    return maxIdx;
  }

  void trickleDownMax(int idx) {
    int m = getMaxChildOrGrandchild(idx);
    if (m == -1) return; // 沒有子節點

    bool isGrandchild = (m >= 4 * idx);
    if (isGrandchild) {
      if (heap[m].graduates > heap[idx].graduates) {
        std::swap(heap[m], heap[idx]);
        int parent = m / 2;
        if (heap[m].graduates < heap[parent].graduates) {
          std::swap(heap[m], heap[parent]);
        }
        trickleDownMax(m);
      }
    } else { // 只是子節點
      if (heap[m].graduates > heap[idx].graduates) {
        std::swap(heap[m], heap[idx]);
      }
    }
  }

public:
  MinMaxHeap() {
    DataNode dummy;
    heap.push_back(dummy); // dummy node at index 0
  }

  void clear() {
    heap.clear();
    DataNode dummy;
    heap.push_back(dummy);
  }

  bool isEmpty() const {
    return heap.size() <= 1;
  }

  void insert(DataNode node) {
    heap.push_back(node);
    int currentIdx = heap.size() - 1;

    if (currentIdx == 1) return;

    int parentIdx = currentIdx / 2;
    if (isMinLevel(currentIdx)) {
      if (heap[currentIdx].graduates > heap[parentIdx].graduates) {
        std::swap(heap[currentIdx], heap[parentIdx]);
        bubbleUpMax(parentIdx);
      } else {
        bubbleUpMin(currentIdx);
      }
    } else {
      if (heap[currentIdx].graduates < heap[parentIdx].graduates) {
        std::swap(heap[currentIdx], heap[parentIdx]);
        bubbleUpMin(parentIdx);
      } else {
        bubbleUpMax(currentIdx);
      }
    }
  }

  DataNode extractMax() {
    if (heap.size() == 2) {
      DataNode maxNode = heap[1];
      heap.pop_back();
      return maxNode;
    }
    if (heap.size() == 3) {
      DataNode maxNode = heap[2];
      heap.pop_back();
      return maxNode;
    }
    
    // 最大值會在 index 2 或 3
    int maxIdx = 2;
    if (heap.size() > 3) {
      // 依題目要求，同一層有同值先取左邊的，因此必須加上 > 的判斷而非 >=
      if (heap[3].graduates > heap[2].graduates) {
        maxIdx = 3;
      }
    }

    DataNode maxNode = heap[maxIdx];
    heap[maxIdx] = heap.back();
    heap.pop_back();

    if (maxIdx < (int)heap.size()) {
      trickleDownMax(maxIdx);
    }

    return maxNode;
  }

  void printResults() {
    if (heap.size() <= 1) {
      std::cout << "<Empty Min-Max Heap!>\n";
      return;
    }

    std::cout << "root: [" << heap[1].id << "] " << heap[1].graduates << "\n";

    int bottomIdx = heap.size() - 1;
    std::cout << "bottom: [" << heap[bottomIdx].id << "] " << heap[bottomIdx].graduates << "\n";

    int leftMostIdx = 1;
    while (leftMostIdx * 2 <= bottomIdx) {
      leftMostIdx = leftMostIdx * 2;
    }
    std::cout << "leftmost bottom: [" << heap[leftMostIdx].id << "] " << heap[leftMostIdx].graduates << "\n";
  }
};

void splitString(std::string str, char delimiter, std::vector<std::string>& outTokens) {
  outTokens.clear();
  std::string currentToken = "";
  for (int i = 0; i < (int)str.length(); i++) {
    if (str[i] == '\r' || str[i] == '\n') continue;
    if (str[i] == delimiter) {
      outTokens.push_back(currentToken);
      currentToken = "";
    } else {
      currentToken += str[i];
    }
  }
  outTokens.push_back(currentToken);
}

int main() {
  MaxHeap maxHeap;
  Deap deap;
  MinMaxHeap minMaxHeap;

  std::string command;
  while (true) {
    std::cout << "* Data Structures and Algorithms *\n";
    std::cout << "*** Heap Construction and Use ****\n";
    std::cout << "* 0. QUIT                        *\n";
    std::cout << "* 1. Build a max heap            *\n";
    std::cout << "* 2. Build a DEAP                *\n";
    std::cout << "* 3. Build a min-max heap        *\n";
    std::cout << "* 4. Extract max from min-max    *\n";
    std::cout << "***************************** ****\n";
    std::cout << "Input a choice(0, 1, 2, 3, 4): ";
    std::string cleanedCommand = "";
    while (cleanedCommand.empty()) {
        std::getline(std::cin, command);
        
        cleanedCommand = "";
        for (int i = 0; i < (int)command.length(); i++) {
            if (command[i] != ' ' && command[i] != '\r' && command[i] != '\n') {
                cleanedCommand += command[i];
            }
        }
    }
    
    bool isString = false;
    for (int i = 0; i < (int)cleanedCommand.length(); i++) {
        if (i == 0 && cleanedCommand[i] == '-' && cleanedCommand.length() > 1) {
            continue;
        }
        if (cleanedCommand[i] < '0' || cleanedCommand[i] > '9') {
            isString = true;
            break;
        }
    }

    if (isString) {
        break;
    }
    
    if (cleanedCommand == "0") {
      break;
    } else if (cleanedCommand == "4") {
      // 任務四：提取資料
      if (minMaxHeap.isEmpty()) {
        std::cout << "\n<Error> Min-Max Heap 尚未建立或已經為空！請先執行任務三(選項3)。\n\n";
        continue;
      }

      while (!minMaxHeap.isEmpty()) {
        std::cout << "\n請輸入要取出擁有最大值的前 K 筆紀錄 (輸入 0 回主選單): ";
        std::string k_str;
        std::getline(std::cin, k_str);
        int k = 0;
        try {
            k = std::stoi(k_str);
        } catch(...) {
            continue; // 防呆
        }
        
        if (k <= 0) break;
        
        std::cout << "\n前 " << k << " 筆畢業生數最多的紀錄：\n";
        for (int i = 0; i < k; ++i) {
          if (minMaxHeap.isEmpty()) {
            std::cout << "--- 堆積結構中已無資料！ ---\n";
            break;
          }
          DataNode maxNode = minMaxHeap.extractMax();
          std::cout << "[" << maxNode.id << "] "
                    << maxNode.school << ", " 
                    << maxNode.dept << ", " 
                    << maxNode.dayNight << ", " 
                    << maxNode.level << ", " 
                    << maxNode.graduates << "\n";
        }

        if (minMaxHeap.isEmpty()) {
          std::cout << "\n堆積結構已空！將重設為初始狀態，請重新執行任務三。\n\n";
          minMaxHeap.clear();
          break;
        }
      }
      continue;
    } else if (cleanedCommand == "1" || cleanedCommand == "2" || cleanedCommand == "3") {
      std::string cleanedFilename = "";
      while (true) {
        std::cout << "\nInput a file number ([0] Quit): ";
        cleanedFilename = "";
        while (cleanedFilename.empty()) {
          std::string filename;
          std::getline(std::cin, filename);
          
          cleanedFilename = "";
          for(int i = 0; i < (int)filename.length(); i++) {
            if (filename[i] != '\r' && filename[i] != '\n' && filename[i] != ' ') {
              cleanedFilename += filename[i];
            }
          }
        }

        if (cleanedFilename == "0") {
          std::cout << "\n";
          break; // leave the while loop
        }

        cleanedFilename = "input" + cleanedFilename + ".txt";
        
        std::ifstream test_infile(cleanedFilename);
        if (!test_infile) {
          std::cout << "\n### " << cleanedFilename << " does not exist! ###\n";
          continue; // ask for filename again
        }
        test_infile.close();
        break; // file is good, leave the loop
      }

      if (cleanedFilename == "0") {
        continue; // continue to the next iteration of the main menu while loop
      }
      
      std::ifstream infile(cleanedFilename);
      
      std::string line;
      for (int i = 0; i < 3; i++) {
        if (!std::getline(infile, line)) {
          break;
        }
      }
      
      if (cleanedCommand == "1") maxHeap.clear();
      else if (cleanedCommand == "2") deap.clear();
      else if (cleanedCommand == "3") minMaxHeap.clear();
      
      int serialNumber = 1;
      while (std::getline(infile, line)) {
        if (line.empty() || line[0] == '\r') continue;
        
        std::vector<std::string> tokens;
        splitString(line, '\t', tokens);
        
        if (tokens.size() > 8) {
          std::string gradStr = tokens[8];
          std::string cleanGrad = "";
          for(int i = 0; i < (int)gradStr.length(); i++) {
            if (gradStr[i] >= '0' && gradStr[i] <= '9') {
              cleanGrad += gradStr[i];
            }
          }
          
          int graduates = 0;
          if (cleanGrad != "") {
            std::stringstream ss(cleanGrad);
            ss >> graduates;
          }

          // 取出任務四所需輸出的欄位 (依據政府公開資料預估格式，可視需求調整 token 的 index)
          std::string school = (tokens.size() > 1) ? tokens[1] : "";
          std::string dept = (tokens.size() > 3) ? tokens[3] : "";
          std::string dayNight = (tokens.size() > 4) ? tokens[4] : "";
          std::string level = (tokens.size() > 5) ? tokens[5] : "";
          
          DataNode node(serialNumber, school, dept, dayNight, level, graduates);
          
          if (cleanedCommand == "1") {
            maxHeap.insert(node);
          } else if (cleanedCommand == "2") {
            deap.insert(node);
          } else if (cleanedCommand == "3") {
            minMaxHeap.insert(node);
          }
          
          serialNumber++;
        }
      }
      infile.close();
      
      if (cleanedCommand == "1") {
        std::cout << "<max heap>\n";
        maxHeap.printResults();
      } else if (cleanedCommand == "2") {
        std::cout << "<DEAP>\n";
        deap.printResults();
      } else if (cleanedCommand == "3") {
        std::cout << "<min-max heap>\n";
        minMaxHeap.printResults();
      }
      
      std::cout << "\n";
      
    } else {
      std::cout << "\nCommand does not exist!\n\n";
    }
  }
  return 0;
}