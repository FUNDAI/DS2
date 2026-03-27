// 11327158 謝亞諺

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <iomanip>

// ==========================================
// 資料節點類別：用來儲存每筆學校科系的詳細資料
// ==========================================
class DataNode {
public:
  int id;               // 序號
  std::string school;   // 學校名稱
  std::string dept;     // 科系名稱
  std::string dayNight; // 日夜別
  std::string level;    // 等級別
  int graduates;        // 上學年度畢業生數 (作為 Heap 排序的主要鍵值)
  
  // 預設建構子
  DataNode() {
    id = 0;
    graduates = 0;
  }
  
  // 帶參數的建構子
  DataNode(int _id, std::string _school, std::string _dept, std::string _dayNight, std::string _level, int _graduates) {
    id = _id;
    school = _school;
    dept = _dept;
    dayNight = _dayNight;
    level = _level;
    graduates = _graduates;
  }
};

// ==========================================
// 任務一：最大堆積 (Max Heap)
// ==========================================
class MaxHeap {
private:
  std::vector<DataNode> heap;
  
public:
  MaxHeap() {
    DataNode dummy;
    heap.push_back(dummy); // Index 0 不使用，放入 dummy node 方便計算
  }
  
  void clear() {
    heap.clear();
    DataNode dummy;
    heap.push_back(dummy);
  }
  
  // 插入節點並進行向上重整 (Bubble Up)
  void insert(DataNode node) {
    heap.push_back(node); // 先將新節點加入尾端
    int currentIdx = heap.size() - 1;
    
    // 不斷與父節點比較，若比父節點大則交換
    while (currentIdx > 1) {
      int parentIdx = currentIdx / 2;
      if (heap[currentIdx].graduates > heap[parentIdx].graduates) {
        DataNode temp = heap[currentIdx];
        heap[currentIdx] = heap[parentIdx];
        heap[parentIdx] = temp;
        currentIdx = parentIdx;
      } else {
        break; // 若小於等於父節點，代表重整完成
      }
    }
  }
  
  // 印出 Root, Bottom 以及 Leftmost bottom
  void printResults() {
    if (heap.size() <= 1) {
      std::cout << "<Empty Max Heap!>\n";
      return;
    }
    
    // Root (樹根，即最大值)
    std::cout << "root: [" << heap[1].id << "] " << heap[1].graduates << "\n";
    
    // Bottom (最後一個節點)
    int bottomIdx = heap.size() - 1;
    std::cout << "bottom: [" << heap[bottomIdx].id << "] " << heap[bottomIdx].graduates << "\n";
    
    // Left-most bottom (最左下角的節點，不斷往左子節點尋找)
    int leftMostIdx = 1;
    while (leftMostIdx * 2 <= bottomIdx) {
      leftMostIdx = leftMostIdx * 2;
    }
    std::cout << "leftmost bottom: [" << heap[leftMostIdx].id << "] " << heap[leftMostIdx].graduates << "\n";
  }
};

// ==========================================
// 任務二：雙端堆積 (DEAP)
// ==========================================
class Deap {
private:
  std::vector<DataNode> deap;
  
  // 判斷該 index 是否位於左子樹 (Min Heap)
  bool isLeftTree(int idx) {
    if (idx == 2) return true;
    if (idx == 3) return false;
    int temp = idx;
    while (temp > 3) {
      temp /= 2;
    }
    return temp == 2;
  }
  
  // 取得計算對應節點所需的位移量 (Offset)
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
  
  // 取得 Min Heap 中節點對應到 Max Heap 中的節點位置
  int getCorrespondingInMax(int idx, int lastIdx) {
    int offset = getOffset(idx);
    int corresp = idx + offset;
    // 如果對應的位置超出了目前的節點數，則退回找其父節點
    if (corresp > lastIdx) {
      corresp = corresp / 2;
    }
    return corresp;
  }
  
  // 在 Min Heap 端向上重整
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
  
  // 在 Max Heap 端向上重整
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
    deap.push_back(dummy); // index 0 不使用
    deap.push_back(dummy); // index 1 不使用 (DEAP root 空置)
  }
  
  void clear() {
    deap.clear();
    DataNode dummy;
    deap.push_back(dummy);
    deap.push_back(dummy);
  }
  
  // 插入節點至 DEAP
  void insert(DataNode node) {
    deap.push_back(node);
    int currentIdx = deap.size() - 1;
    
    if (currentIdx == 2) {
      return; // index 2 是 Min Heap 的 root，第一個實際資料直接放這
    }
    
    if (isLeftTree(currentIdx)) {
      // 若新增在左子樹 (Min 端)
      int partnerIdx = getCorrespondingInMax(currentIdx, currentIdx);
      if (deap[currentIdx].graduates > deap[partnerIdx].graduates) {
        // 若大於 Max 端的對應節點，則交換並在 Max 端向上重整
        DataNode temp = deap[currentIdx];
        deap[currentIdx] = deap[partnerIdx];
        deap[partnerIdx] = temp;
        bubbleUpMax(partnerIdx);
      } else {
        // 否則在 Min 端向上重整
        bubbleUpMin(currentIdx);
      }
    } else {
      // 若新增在右子樹 (Max 端)
      int partnerIdx = currentIdx - getOffset(currentIdx);
      if (deap[currentIdx].graduates < deap[partnerIdx].graduates) {
        // 若小於 Min 端的對應節點，則交換並在 Min 端向上重整
        DataNode temp = deap[currentIdx];
        deap[currentIdx] = deap[partnerIdx];
        deap[partnerIdx] = temp;
        bubbleUpMin(partnerIdx);
      } else {
        // 否則在 Max 端向上重整
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
    
    int leftMost = 2; // DEAP 的實際樹根從 2 開始
    while (leftMost * 2 <= bottomIdx) {
      leftMost *= 2;
    }
    std::cout << "leftmost bottom: [" << deap[leftMost].id << "] " << deap[leftMost].graduates << "\n";
  }
};

// ==========================================
// 任務三與四：最小最大堆積 (Min-Max Heap)
// ==========================================
class MinMaxHeap {
private:
  std::vector<DataNode> heap;

  // 判斷目前節點所在的深度是否為 Min 層 (偶數深度：0, 2, 4...)
  bool isMinLevel(int idx) {
    int depth = 0;
    while (idx > 1) {
      idx /= 2;
      depth++;
    }
    return depth % 2 == 0;
  }

  // 往上與「祖父節點」比較並交換 (用於 Min 層)
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

  // 往上與「祖父節點」比較並交換 (用於 Max 層)
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

  // 【核心邏輯：循序向下交換法 (Trickle Down Max)】
  // 取出最大值後，將尾端節點放置於樹根(Max層)，並將其向下過濾以維持 Min-Max 特性
  void trickleDownMax(int idx) {
    int leftChild = 2 * idx;
    int rightChild = 2 * idx + 1;

    // 如果連左小孩都沒有，代表已經是葉節點，直接結束
    if (leftChild >= (int)heap.size()) return;

    // 階段 1：找出最大的小孩
    // 預設最大小孩為左節點。若右節點存在且「嚴格大於」左節點，才更新為右節點 (確保同值取左)
    int maxChildIdx = leftChild;
    if (rightChild < (int)heap.size() && heap[rightChild].graduates > heap[leftChild].graduates) {
      maxChildIdx = rightChild;
    }

    // 階段 2：若最大的小孩比目前的值大，先跟小孩進行交換！
    if (heap[maxChildIdx].graduates > heap[idx].graduates) {
      std::swap(heap[maxChildIdx], heap[idx]);
    }

    // 階段 3：接著找出最大的孫子
    int maxGrandchildIdx = -1;
    int firstGrandchild = 4 * idx;
    for (int i = 0; i < 4; ++i) {
      int gc = firstGrandchild + i;
      if (gc < (int)heap.size()) {
        // 同樣使用「嚴格大於」以確保同值時保留最先遇到(最左邊)的孫子
        if (maxGrandchildIdx == -1 || heap[gc].graduates > heap[maxGrandchildIdx].graduates) {
          maxGrandchildIdx = gc;
        }
      }
    }

    // 階段 4：如果最大的孫子比目前的節點還要大 (注意此時的目前節點可能是剛換上來的小孩值)
    if (maxGrandchildIdx != -1 && heap[maxGrandchildIdx].graduates > heap[idx].graduates) {
      // 進行孫子層級的交換
      std::swap(heap[maxGrandchildIdx], heap[idx]);
      
      // 階段 5：如果跟孫子交換了，必須再跟該孫子的父節點（原本的小孩層，即 Min 層）檢查
      // 因為落下來的值不能比 Min 層的值還要小，若比較小則需再交換一次
      int parentOfGrandchild = maxGrandchildIdx / 2;
      if (heap[maxGrandchildIdx].graduates < heap[parentOfGrandchild].graduates) {
        std::swap(heap[maxGrandchildIdx], heap[parentOfGrandchild]);
      }
      
      // 繼續從孫子的位置往下遞迴重整
      trickleDownMax(maxGrandchildIdx);
    }
    // 若沒有跟孫子交換，則不需要繼續遞迴
  }

public:
  MinMaxHeap() {
    DataNode dummy;
    heap.push_back(dummy); // index 0 不使用
  }

  void clear() {
    heap.clear();
    DataNode dummy;
    heap.push_back(dummy);
  }

  bool isEmpty() const {
    return heap.size() <= 1;
  }

  int getSize() const {
    return heap.size() - 1; 
  }

  // 插入節點至 Min-Max Heap
  void insert(DataNode node) {
    heap.push_back(node);
    int currentIdx = heap.size() - 1;

    if (currentIdx == 1) return; // 只有一個節點(root)時直接結束

    int parentIdx = currentIdx / 2;
    // 根據目前所在層級，判斷該跟父節點做何種比較，並進行對應的 Bubble Up
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

  // 提取出擁有最大值的節點 (Delete Max)
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
    
    // Max Level 是第一層子節點，所以最大值必定在 index 2 或 3
    int maxIdx = 2;
    if (heap.size() > 3) {
      // 同一層有同值先取左邊的，所以右邊(index 3)必須「嚴格大於」左邊(index 2)才當選
      if (heap[3].graduates > heap[2].graduates) {
        maxIdx = 3;
      }
    }

    DataNode maxNode = heap[maxIdx];
    // 將最後一個節點移到被拔除的最大值位子
    heap[maxIdx] = heap.back();
    heap.pop_back();

    // 將該節點往下過濾 (Trickle Down) 以維持屬性
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

// ==========================================
// 輔助函式：根據特定字元 (delimiter) 切割字串
// ==========================================
void splitString(std::string str, char delimiter, std::vector<std::string>& outTokens) {
  outTokens.clear();
  std::string currentToken = "";
  for (int i = 0; i < (int)str.length(); i++) {
    // 忽略 Windows 系統常帶有的換行與回車符號
    if (str[i] == '\r' || str[i] == '\n') continue;
    if (str[i] == delimiter) {
      outTokens.push_back(currentToken);
      currentToken = "";
    } else {
      currentToken += str[i];
    }
  }
  // 推入最後一個 token
  outTokens.push_back(currentToken);
}

// ==========================================
// 主程式
// ==========================================
int main() {
  MaxHeap maxHeap;
  Deap deap;
  MinMaxHeap minMaxHeap;

  std::string command;
  while (true) {
    // 印出主選單
    std::cout << "* Data Structures and Algorithms *\n";
    std::cout << "*** Heap Construction and Use ****\n";
    std::cout << "* 0. QUIT                        *\n";
    std::cout << "* 1. Build a max heap            *\n";
    std::cout << "* 2. Build a DEAP                *\n";
    std::cout << "* 3. Build a min-max heap        *\n";
    std::cout << "* 4: Top-K max from min-max heap *\n";
    std::cout << "**********************************\n";
    std::cout << "Input a choice(0, 1, 2, 3, 4): ";
    
    // 讀取並清理使用者輸入的指令
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
    
    // 檢查輸入是否為合法數字
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
    
    // 處理選擇
    if (cleanedCommand == "0") {
      break; // 結束程式
    } else if (cleanedCommand == "4") {
      // 任務四：從 Min-Max Heap 取出 Top-K 最大值
      if (minMaxHeap.isEmpty()) {
        std::cout << "\n### Execute command 3 first! ###\n\n";
        continue; // 防呆：尚未建立堆積
      }

      int currentSize = minMaxHeap.getSize(); // 取得當前可用資料總筆數
      std::cout << "\nEnter the value of K in [1," << currentSize << "]: ";
      std::string k_str;
      std::getline(std::cin, k_str);
      int k = 0;
      try {
          k = std::stoi(k_str);
      } catch(...) {
          std::cout << "輸入無效！\n\n";
          continue;
      }
      
      // 防呆：檢查 K 是否超出範圍
      if (k > currentSize || k <= 0) {
          std::cout << "\n### The value of K is out of range! ###\n\n";
          continue; 
      }
      
      // 取出 K 筆最大資料並格式化印出
      if (k > 0) {
        for (int i = 0; i < k; ++i) {
          DataNode maxNode = minMaxHeap.extractMax();
          // setw(3) 確保排名數字排版對齊
          std::cout << "Top " << std::setw(3) << (i + 1) << ": [" << maxNode.id << "] "
                    << maxNode.school << maxNode.dept << ", " 
                    << maxNode.dayNight << ", " 
                    << maxNode.level << ", " 
                    << maxNode.graduates << "\n";
        }

        // 提取完畢後，檢查是否為空，若是則重設狀態
        if (minMaxHeap.isEmpty()) {
          minMaxHeap.clear(); 
          std::cout << "\n";
        } else {
          std::cout << "\n";
        }
      }
      continue; // 執行完畢直接回到主選單
    } else if (cleanedCommand == "1" || cleanedCommand == "2" || cleanedCommand == "3") {
      // 任務一、二、三：讀取檔案並建立對應的 Heap
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
          break;
        }

        cleanedFilename = "input" + cleanedFilename + ".txt";
        
        // 嘗試開啟檔案以檢查是否存在
        std::ifstream test_infile(cleanedFilename);
        if (!test_infile) {
          std::cout << "\n### " << cleanedFilename << " does not exist! ###\n";
          continue; 
        }
        test_infile.close();
        break; 
      }

      if (cleanedFilename == "0") {
        continue; 
      }
      
      std::ifstream infile(cleanedFilename);
      
      std::string line;
      // 略過前面 3 行的標題與欄位說明
      for (int i = 0; i < 3; i++) {
        if (!std::getline(infile, line)) {
          break;
        }
      }
      
      // 每次新建時清空舊資料
      if (cleanedCommand == "1") maxHeap.clear();
      else if (cleanedCommand == "2") deap.clear();
      else if (cleanedCommand == "3") minMaxHeap.clear();
      
      int serialNumber = 1; // 記錄原始資料中的流水號
      while (std::getline(infile, line)) {
        if (line.empty() || line[0] == '\r') continue;
        
        std::vector<std::string> tokens;
        splitString(line, '\t', tokens); // 依照 Tab 切割欄位
        
        if (tokens.size() > 8) {
          // 清理畢業生數值的非數字字元
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

          // 抓取各個欄位，供任務四印出使用
          std::string school = (tokens.size() > 1) ? tokens[1] : "";
          std::string dept = (tokens.size() > 3) ? tokens[3] : "";
          std::string dayNight = (tokens.size() > 4) ? tokens[4] : "";
          std::string level = (tokens.size() > 5) ? tokens[5] : "";
          
          DataNode node(serialNumber, school, dept, dayNight, level, graduates);
          
          // 根據指令插入至對應的 Heap 中
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
      
      // 建置完成後印出該結構的驗證資訊 (root, bottom, leftmost bottom)
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