// 11327158 謝亞諺
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

// 用來儲存讀入的每一筆完整原始資料
class RawData {
 public:
  int id;
  std::string schoolName;
  std::string deptName;
  std::string dayNight;
  std::string level;
  std::string students;
  int graduates;
};

// 2-3樹節點中存放的資料單位 (Key 為畢業生數，ids 為擁有一樣畢業生數的流水號陣列)
// 為了應付同值同節點
class TwoThreeEntry {
 public:
  int key;
  std::vector<int> ids;
};

// 2-3樹的節點類別
class TwoThreeNode {
 public:
  std::vector<TwoThreeEntry> entries;
  std::vector<TwoThreeNode*> children;
  TwoThreeNode* parent;

  TwoThreeNode() {
    parent = nullptr;
  }

  // 判斷是否為樹葉節點
  bool isLeaf() {
    if (children.size() == 0) {
      return true;
    }
    return false;
  }
};

// 任務一：建立 2-3 樹類別
class TwoThreeTree {
 public:
  TwoThreeNode* root;

  TwoThreeTree() {
    root = nullptr;
  }

  // 清除整棵樹釋放記憶體
  void clear(TwoThreeNode* node) {
    if (node != nullptr) {
      for (int i = 0; i < node->children.size(); i++) {
        clear(node->children[i]);
      }
      delete node;
    }
  }

  void clearTree() {
    clear(root);
    root = nullptr;
  }

  // 插入一筆 Entry 到 Node 內，並利用簡單排序維持由小到大
  void insertEntryIntoNode(TwoThreeNode* node, TwoThreeEntry newEntry) {
    node->entries.push_back(newEntry);
    for (int i = 0; i < node->entries.size(); i++) {
      for (int j = i + 1; j < node->entries.size(); j++) {
        if (node->entries[i].key > node->entries[j].key) {
          TwoThreeEntry temp = node->entries[i];
          node->entries[i] = node->entries[j];
          node->entries[j] = temp;
        }
      }
    }
  }

  // 當節點內有3筆 Entry 時，進行節點分裂 (Split)
  void splitNode(TwoThreeNode* node) {
    TwoThreeNode* rightNode = new TwoThreeNode();
    TwoThreeEntry middleEntry = node->entries[1];
    TwoThreeEntry rightEntry = node->entries[2];

    rightNode->entries.push_back(rightEntry);

    if (node->isLeaf() == false) {
      rightNode->children.push_back(node->children[2]);
      rightNode->children.push_back(node->children[3]);
      rightNode->children[0]->parent = rightNode;
      rightNode->children[1]->parent = rightNode;

      node->children.pop_back();
      node->children.pop_back();
    }

    node->entries.pop_back();
    node->entries.pop_back();

    if (node->parent == nullptr) {
      TwoThreeNode* newRoot = new TwoThreeNode();
      newRoot->entries.push_back(middleEntry);
      newRoot->children.push_back(node);
      newRoot->children.push_back(rightNode);
      node->parent = newRoot;
      rightNode->parent = newRoot;
      root = newRoot;
    } else {
      TwoThreeNode* parent = node->parent;
      insertEntryIntoNode(parent, middleEntry);

      int childIndex = 0;
      for (int i = 0; i < parent->children.size(); i++) {
        if (parent->children[i] == node) {
          childIndex = i;
          break;
        }
      }

      // 將 rightNode 插入 parent 的 children 陣列的適當位置
      parent->children.push_back(nullptr);
      for (int i = parent->children.size() - 1; i > childIndex + 1; i--) {
        parent->children[i] = parent->children[i - 1];
      }
      parent->children[childIndex + 1] = rightNode;
      rightNode->parent = parent;

      // 若父節點也滿了，繼續往上分裂
      if (parent->entries.size() == 3) {
        splitNode(parent);
      }
    }
  }

  // 將一筆新資料插入 2-3 樹中
  void insert(int key, int id) {
    if (root == nullptr) {
      root = new TwoThreeNode();
      TwoThreeEntry newEntry;
      newEntry.key = key;
      newEntry.ids.push_back(id);
      root->entries.push_back(newEntry);
      return;
    }

    TwoThreeNode* curr = root;
    while (true) {
      bool found = false;
      // 檢查該畢業生數是否已經存在於此節點
      for (int i = 0; i < curr->entries.size(); i++) {
        if (curr->entries[i].key == key) {
          curr->entries[i].ids.push_back(id);
          found = true;
          break;
        }
      }

      if (found == true) {
        return; // 若找到相同畢業生數，加入 id 後直接結束
      }

      if (curr->isLeaf() == true) {
        break; // 已經走到樹葉，準備插入
      } else {
        // 決定要往哪個子節點走
        int childIndex = 0;
        for (int i = 0; i < curr->entries.size(); i++) {
          if (key < curr->entries[i].key) {
            break;
          }
          childIndex = childIndex + 1;
        }
        curr = curr->children[childIndex];
      }
    }

    TwoThreeEntry newEntry;
    newEntry.key = key;
    newEntry.ids.push_back(id);

    insertEntryIntoNode(curr, newEntry);

    // 如果節點內的 Entry 達到 3 筆，觸發分裂
    if (curr->entries.size() == 3) {
      splitNode(curr);
    }
  }

  // 計算樹高 (從樹根一路往左子節點走到樹葉)
  int getHeight() {
    if (root == nullptr) {
      return 0;
    }
    int height = 1;
    TwoThreeNode* curr = root;
    while (curr->isLeaf() == false) {
      height = height + 1;
      curr = curr->children[0];
    }
    return height;
  }

  // 計算總節點數 (遞迴尋訪)
  int countNodes(TwoThreeNode* node) {
    if (node == nullptr) {
      return 0;
    }
    int count = 1;
    for (int i = 0; i < node->children.size(); i++) {
      count = count + countNodes(node->children[i]);
    }
    return count;
  }

  // 輸出任務要求之結果
  void printRootInfo(std::vector<RawData>& dataList) {
    int height = getHeight();
    int nodes = countNodes(root);
    std::cout << "Tree height: " << height << "\n";
    std::cout << "Number of nodes: " << nodes << "\n";

    if (root != nullptr) {
      int count = 1;
      for (int i = 0; i < root->entries.size(); i++) {
        for (int j = 0; j < root->entries[i].ids.size(); j++) {
          int targetId = root->entries[i].ids[j];
          RawData targetData;
          // 從原始資料陣列找回該筆資料的完整資訊
          for (int k = 0; k < dataList.size(); k++) {
            if (dataList[k].id == targetId) {
              targetData = dataList[k];
              break;
            }
          }

          std::cout << count << ": [" << targetData.id << "] "
                    << targetData.schoolName << ", "
                    << targetData.deptName << ", "
                    << targetData.dayNight << ", "
                    << targetData.level << ", "
                    << targetData.students << ", "
                    << targetData.graduates << "\n";
          count = count + 1;
        }
      }
    }
  }
};

// 任務二預留：建立 AVL 樹類別
class AVLTree {
 public:
  AVLTree() {
    // 為了符合規範 (2)：「必須要分成2-3樹和AVL樹兩個類別」
  }
};

int main() {
  TwoThreeTree tree23;
  std::string command;
  std::vector<RawData> dataList;

  while (true) {
    std::cout << "* Data Structures and Algorithms *\n";
    std::cout << "****** Balanced Search Tree ******\n";
    std::cout << "* 0. QUIT                        *\n";
    std::cout << "* 1. Build 23 tree               *\n";
    std::cout << "* 2. Build AVL tree              *\n";
    std::cout << "**********************************\n";
    std::cout << "Input a choice(0, 1, 2): ";

    // 修正：使用 !std::getline 進行安全讀取並判斷
    if (!std::getline(std::cin, command)) {
      break;
    }

    std::string cleanedCommand = "";
    for (int i = 0; i < command.length(); i++) {
      if (command[i] != ' ') {
        if (command[i] != '\r') {
          if (command[i] != '\n') {
            cleanedCommand = cleanedCommand + command[i];
          }
        }
      }
    }

    if (cleanedCommand == "0") {
      break;
    } else if (cleanedCommand == "1") {
      std::cout << "Input a file number: ";
      std::string fileNum;
      if (!std::getline(std::cin, fileNum)) {
        break;
      }

      std::string cleanedFileNum = "";
      for (int i = 0; i < fileNum.length(); i++) {
        if (fileNum[i] != ' ') {
          if (fileNum[i] != '\r') {
            if (fileNum[i] != '\n') {
              cleanedFileNum = cleanedFileNum + fileNum[i];
            }
          }
        }
      }

      std::string filename = "input" + cleanedFileNum + ".txt";
      std::ifstream infile(filename.c_str());

      if (infile.is_open() == false) {
        std::cout << "### " << filename << " does not exist! ###\n";
      } else {
        tree23.clearTree();
        dataList.clear();

        std::string line;
        // 修正：使用 !std::getline 來略過前三行標題
        for (int i = 0; i < 3; i++) {
          if (!std::getline(infile, line)) {
            break;
          }
        }

        int currentId = 1;
        while (std::getline(infile, line)) {
          if (line.empty() == true) {
            continue;
          }
          if (line[0] == '\r') {
            continue;
          }

          std::stringstream ss(line);
          std::string token;
          std::vector<std::string> tokens;

          while (std::getline(ss, token, '\t')) {
            tokens.push_back(token);
          }

          if (tokens.size() >= 11) {
            RawData data;
            data.id = currentId;
            data.schoolName = tokens[1];
            data.deptName = tokens[3];
            data.dayNight = tokens[4];
            data.level = tokens[5];
            data.students = tokens[6];

            // 處理畢業生人數，過濾掉逗號等非數字字元 (例如 1,000 -> 1000)
            std::string gradStr = tokens[8];
            std::string cleanGrad = "";
            for (int i = 0; i < gradStr.length(); i++) {
              if (gradStr[i] >= '0') {
                if (gradStr[i] <= '9') {
                  cleanGrad = cleanGrad + gradStr[i];
                }
              }
            }

            int graduates = 0;
            if (cleanGrad != "") {
              std::stringstream gradSS(cleanGrad);
              gradSS >> graduates;
            }
            data.graduates = graduates;

            dataList.push_back(data);
            tree23.insert(data.graduates, data.id);
            currentId = currentId + 1;
          }
        }
        infile.close();

        // 列印輸出結果
        tree23.printRootInfo(dataList);
      }
    } else {
      std::cout << "Invalid command!\n";
    }
  }

  // 程式結束前清空記憶體
  tree23.clearTree();
  return 0;
}