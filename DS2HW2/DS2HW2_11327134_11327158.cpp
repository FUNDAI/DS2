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

// ==========================================
// 任務一：2-3 樹相關類別
// ==========================================
class TwoThreeEntry {
 public:
  int key;
  std::vector<int> ids;
};

class TwoThreeNode {
 public:
  std::vector<TwoThreeEntry> entries;
  std::vector<TwoThreeNode*> children;
  TwoThreeNode* parent;

  TwoThreeNode() {
    parent = nullptr;
  }

  bool isLeaf() {
    if (children.size() == 0) {
      return true;
    }
    return false;
  }
};

class TwoThreeTree {
 public:
  TwoThreeNode* root;

  TwoThreeTree() {
    root = nullptr;
  }

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

      parent->children.push_back(nullptr);
      for (int i = parent->children.size() - 1; i > childIndex + 1; i--) {
        parent->children[i] = parent->children[i - 1];
      }
      parent->children[childIndex + 1] = rightNode;
      rightNode->parent = parent;

      if (parent->entries.size() == 3) {
        splitNode(parent);
      }
    }
  }

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
      for (int i = 0; i < curr->entries.size(); i++) {
        if (curr->entries[i].key == key) {
          curr->entries[i].ids.push_back(id);
          found = true;
          break;
        }
      }

      if (found == true) {
        return; 
      }

      if (curr->isLeaf() == true) {
        break; 
      } else {
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

    if (curr->entries.size() == 3) {
      splitNode(curr);
    }
  }

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

  void printRootInfo(std::vector<RawData>& dataList) {
    int height = getHeight();
    int nodes = countNodes(root);
    std::cout << "Tree height = " << height << "\n";
    std::cout << "Number of nodes = " << nodes << "\n";

    if (root != nullptr) {
      int count = 1;
      for (int i = 0; i < root->entries.size(); i++) {
        for (int j = 0; j < root->entries[i].ids.size(); j++) {
          int targetId = root->entries[i].ids[j];
          RawData targetData;
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

// ==========================================
// 任務二：AVL 樹相關類別 (空間預留)
// ==========================================
// 依據講義，AVL 樹的 Key 是「學校名稱 (字串)」，同名稱的也要放一起
class AVLEntry {
 public:
  std::string key; // 學校名稱
  std::vector<int> ids;
};

class AVLNode {
 public:
  AVLEntry entry;
  AVLNode* left;
  AVLNode* right;
  int height; // AVL 樹需要紀錄高度來計算平衡因子

  AVLNode() {
    left = nullptr;
    right = nullptr;
    height = 1;
  }
};

class AVLTree {
 public:
  AVLNode* root;

  AVLTree() {
    root = nullptr;
  }

  void clear(AVLNode* node) {
    if (node != nullptr) {
      clear(node->left);
      clear(node->right);
      delete node;
    }
  }

  void clearTree() {
    clear(root);
    root = nullptr;
  }

  // 預留插入方法 (傳入學校名稱與序號)
  void insert(std::string key, int id) {
    // TODO: 這裡之後要實作 AVL Tree 的插入與平衡旋轉 (LL, RR, LR, RL)
  }

  // 預留印出方法
  void printRootInfo(std::vector<RawData>& dataList) {
    // TODO: 計算樹高、節點數，並印出樹根內的資料
    std::cout << "<AVL Tree preview - Function not implemented yet>\n";
  }
};

int main() {
  TwoThreeTree tree23;
  AVLTree avlTree; // 實例化 AVL Tree
  std::vector<RawData> dataList;

  while (true) {
    std::cout << "* Data Structures and Algorithms *\n";
    std::cout << "****** Balanced Search Tree ******\n";
    std::cout << "* 0. QUIT                        *\n";
    std::cout << "* 1. Build 23 tree               *\n";
    std::cout << "* 2. Build AVL tree              *\n";
    std::cout << "**********************************\n";
    std::cout << "Input a choice(0, 1, 2): ";

    std::string command;
    std::string cleanedCommand = "";
    
    // 防呆：持續讀取直到有非空白的內容
    while (cleanedCommand.empty()) {
      if (!std::getline(std::cin, command)) {
        tree23.clearTree();
        avlTree.clearTree();
        return 0;
      }
      cleanedCommand = "";
      for (int i = 0; i < command.length(); i++) {
        if (command[i] != ' ') {
          if (command[i] != '\r') {
            if (command[i] != '\n') {
              cleanedCommand = cleanedCommand + command[i];
            }
          }
        }
      }
    }

    if (cleanedCommand == "0") {
      break;
    } else if (cleanedCommand == "1" || cleanedCommand == "2") {
      // 合併選項 1 與選項 2 的檔案讀取邏輯
      std::string cleanedFileNum = "";
      
      while (true) {
        std::cout << "\nInput a file number ([0] Quit): ";
        cleanedFileNum = "";
        
        while (cleanedFileNum.empty()) {
          std::string fileNum;
          if (!std::getline(std::cin, fileNum)) {
            tree23.clearTree();
            avlTree.clearTree();
            return 0;
          }

          cleanedFileNum = "";
          for (int i = 0; i < fileNum.length(); i++) {
            if (fileNum[i] != ' ') {
              if (fileNum[i] != '\r') {
                if (fileNum[i] != '\n') {
                  cleanedFileNum = cleanedFileNum + fileNum[i];
                }
              }
            }
          }
        }

        if (cleanedFileNum == "0") {
          std::cout << "\n";
          break; // 回主選單
        }

        std::string filename = "input" + cleanedFileNum + ".txt";
        std::ifstream infile(filename.c_str());

        if (infile.is_open() == false) {
          std::cout << "\n### " << filename << " does not exist! ###\n";
          continue; 
        } else {
          // 檔案開啟成功，清空指定的樹與資料陣列
          if (cleanedCommand == "1") {
            tree23.clearTree();
          } else if (cleanedCommand == "2") {
            avlTree.clearTree();
          }
          dataList.clear();

          std::string line;
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
              
              // 根據指令，將資料送到對應的樹結構中
              if (cleanedCommand == "1") {
                tree23.insert(data.graduates, data.id);
              } else if (cleanedCommand == "2") {
                avlTree.insert(data.schoolName, data.id);
              }
              
              currentId = currentId + 1;
            }
          }
          infile.close();

          // 根據指令印出對應的結果
          if (cleanedCommand == "1") {
            tree23.printRootInfo(dataList);
          } else if (cleanedCommand == "2") {
            avlTree.printRootInfo(dataList);
          }
          std::cout << "\n";
          
          break; // 處理完畢跳出迴圈
        }
      }
    } else {
      std::cout << "\nCommand does not exist!\n\n";
    }
  }

  tree23.clearTree();
  avlTree.clearTree();
  return 0;
}