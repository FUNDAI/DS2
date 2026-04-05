// 11327158 謝亞諺 11327134 曾苡綾
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

// 用來儲存原始資料
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

  bool IsLeaf() {
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

  // 遞迴清除
  void Clear(TwoThreeNode* node) {
    if (node != nullptr) {
      for (int i = 0; i < node->children.size(); i++) {
        Clear(node->children[i]);
      }
      delete node;
    }
  }

  // 清空樹
  void ClearTree() {
    Clear(root);
    root = nullptr;
  }

  // 將新的插入到指定的節點中
  void InsertEntryIntoNode(TwoThreeNode* node, TwoThreeEntry newEntry) {
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

  // 當節點內達到 3 個時，將節點切成兩半，並把中間的往上給父節點
  void SplitNode(TwoThreeNode* node) {
    TwoThreeNode* rightNode = new TwoThreeNode(); // 準備分裂出的右半邊節點
    TwoThreeEntry middleEntry = node->entries[1]; // 中間的資料 
    TwoThreeEntry rightEntry = node->entries[2];  // 右邊的資料 

    rightNode->entries.push_back(rightEntry);

    // 如果該節點不是樹葉，分裂時把子節點分給新的右節點
    if (node->IsLeaf() == false) {
      rightNode->children.push_back(node->children[2]);
      rightNode->children.push_back(node->children[3]);
      rightNode->children[0]->parent = rightNode;
      rightNode->children[1]->parent = rightNode;

      node->children.pop_back(); // 原本的節點保留左邊兩個子節點
      node->children.pop_back();
    }

    node->entries.pop_back(); // 原本的節點只保留左邊一個資料
    node->entries.pop_back();

    // 如果分裂的是樹根
    if (node->parent == nullptr) {
      TwoThreeNode* newRoot = new TwoThreeNode();
      newRoot->entries.push_back(middleEntry);     // 中間的資料成為新的樹根
      newRoot->children.push_back(node);           // 左子樹為原節點
      newRoot->children.push_back(rightNode);      // 右子樹為新分裂的節點
      node->parent = newRoot;
      rightNode->parent = newRoot;
      root = newRoot;
    } 
    // 如果有父節點，將中間的資料與新節點向上合併
    else {
      TwoThreeNode* parent = node->parent;
      InsertEntryIntoNode(parent, middleEntry); // 把中間資料塞給父節點

      // 找出原節點在父節點 children 陣列中的位置
      int childIndex = 0;
      for (int i = 0; i < parent->children.size(); i++) {
        if (parent->children[i] == node) {
          childIndex = i;
          break;
        }
      }

      // 將分裂出來的 rightNode 安插到父節點 children 陣列的適當位置
      parent->children.push_back(nullptr);
      for (int i = parent->children.size() - 1; i > childIndex + 1; i--) {
        parent->children[i] = parent->children[i - 1];
      }
      parent->children[childIndex + 1] = rightNode;
      rightNode->parent = parent;

      // 父節點滿了，繼續遞迴向上分裂
      if (parent->entries.size() == 3) {
        SplitNode(parent);
      }
    }
  }

  // 插入一筆新資料到 2-3 樹
  void Insert(int key, int id) {
    // 若為空樹，直接建立 root
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
      // 檢查目前的節點中是否已經有相同的畢業生數 
      for (int i = 0; i < curr->entries.size(); i++) {
        if (curr->entries[i].key == key) {
          curr->entries[i].ids.push_back(id); // 直接將 id 加入陣列
          found = true;
          break;
        }
      }

      if (found == true) {
        return; 
      }

      // 決定往哪個子樹走
      if (curr->IsLeaf() == true) {
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

    // 建立新的並插入
    TwoThreeEntry newEntry;
    newEntry.key = key;
    newEntry.ids.push_back(id);

    InsertEntryIntoNode(curr, newEntry);

    // 檢查插入後是否達到 3 個
    if (curr->entries.size() == 3) {
      SplitNode(curr);
    }
  }

  // 計算樹高
  int GetHeight() {
    if (root == nullptr) {
      return 0;
    }
    int height = 1;
    TwoThreeNode* curr = root;
    while (curr->IsLeaf() == false) {
      height = height + 1;
      curr = curr->children[0];
    }
    return height;
  }

  // 遞迴計算整棵樹的節點總數
  int CountNodes(TwoThreeNode* node) {
    if (node == nullptr) {
      return 0;
    }
    int count = 1;
    for (int i = 0; i < node->children.size(); i++) {
      count = count + CountNodes(node->children[i]);
    }
    return count;
  }

  void PrintRootInfo(std::vector<RawData>& dataList) {
    int height = GetHeight();
    int nodes = CountNodes(root);
    std::cout << "Tree height = " << height << "\n";
    std::cout << "Number of nodes = " << nodes << "\n";

    if (root != nullptr) {
      int count = 1;
      for (int i = 0; i < root->entries.size(); i++) {
        for (int j = 0; j < root->entries[i].ids.size(); j++) {
          int targetId = root->entries[i].ids[j];
          RawData targetData;
          // 利用 targetId 回去 dataList 中抓取完整資料
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

struct Node {
    Node *right;
    Node *left;
    int height;
    string name;
    vector<int> ids;  // 存同一個學校的
    Node(string n, int idx) {
        name = n;
        left = NULL;
        right = NULL;
        height = 1;
        ids.push_back(idx);
    } 
};

class AVLtree {
 private:
  Node* root;
  int nodecount = 0;
  Node* Insert(Node *node, string name, int idx) {
    if (node == NULL) {
        nodecount++;
        Node *newnode = new Node(name, idx);
        return newnode;
    } 
    if (name < node->name) {  // 找到正確位置並插入
        node->left = Insert(node->left, name, idx);
    } else if (name > node->name) {
        node->right = Insert(node->right, name, idx);
    } else {// 如果學校存在就直接pushback
        node->ids.push_back(idx);
        return node;
    }

    node->height = 1 + max(Getheight(node->left), Getheight(node->right)); // 更新樹高
    int bf = Getbf(node); // 當前節點的平衡係數

    if (bf > 1) {  // 左邊較重
        if (Getbf(node->left) > 0) {
            return LL(node);
        } else {
            return LR(node);
        }

    } else if (bf < -1) { // 右邊較重
        if (Getbf(node->right) < 0) {
            return RR(node);
        } else {
            return RL(node);
        }
    } 
    return node;
  }

 public:
  AVLtree() {
    root = NULL;
    nodecount = 0;
  }

  void Insert(string name, int idx) {
    root = Insert(root, name, idx); 
    // 因為root是private的，為了讓main不能接觸到root，所以private裡面還有一個insert(因為insert一定要傳進root才能做)
  }

  int max(int left, int right) { // 判斷左邊比較高還是右邊比較高
    if (left > right) {
        return left;
    }
    return right;
  }

  int Getheight(Node* node) {  // 取得樹高
    if (node == NULL) {
        return 0;
    } else {
      return node->height;
    }
  }

  int Getbf(Node* node) { // get平衡係數
    return Getheight(node->left) - Getheight(node->right);
  }

  Node* LL(Node* x) { // 旋轉
    Node* y = x->left;
    x->left = y->right;
    y->right = x;
    x->height = max(Getheight(x->left), Getheight(x->right)) + 1;
    y->height = max(Getheight(y->left), Getheight(y->right)) + 1;
    return y;
  }

  Node* RR(Node* x) { // 旋轉
    Node* y = x->right;
    x->right = y->left;
    y->left = x;
    x->height = max(Getheight(x->left), Getheight(x->right)) + 1;
    y->height = max(Getheight(y->left), Getheight(y->right)) + 1;
    return y;
  }

  Node* RL(Node* x) { // 旋轉
    x->right = LL(x->right);
    Node* y = RR(x);
    return y;
  }

  Node* LR(Node* x) { // 旋轉
    x->left = RR(x->left);
    Node* y = LL(x);
    return y;
  }

  void ClearAVL() {
    clear(root);
    root = nullptr; // 不要讓root還指向已經被刪掉的節點
    nodecount = 0;
  }

  void clear(Node* node) {  // 一直遞迴清理
    if (node != nullptr) {
        clear(node->left);
        clear(node->right);
        delete node;
    }
  }

  void printRootInfo(std::vector<RawData>& dataList) {
        if (root == nullptr) {
            std::cout << "Tree is empty.\n";
            return;
        }
        std::cout << "Tree height = " << Getheight(root) << "\n";
        std::cout << "Number of nodes = " << nodecount << "\n";

        for (int i = 0; i < root->ids.size(); i++) {
            int targetId = root->ids[i];

            RawData& d = dataList[targetId - 1]; // 編號從1開始所以要-1
            // call by reference不用浪費空間
            std::cout << (i + 1) << ": [" << d.id << "] "
                      << d.schoolName << ", "
                      << d.deptName << ", "
                      << d.dayNight << ", "
                      << d.level << ", "
                      << d.students << ", "
                      << d.graduates << "\n";
        }
    }
};


int main() {
  TwoThreeTree tree23;
  AVLtree avlTree; 
  std::vector<RawData> dataList;
  bool ismission1done = false; // 檢查是否執行過任務一
  bool ismission2done = false; // 檢查是否執行過任務二
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
    
    // 持續讀取直到非空白
    while (cleanedCommand.empty()) {
      if (!std::getline(std::cin, command)) {
        tree23.ClearTree();
        avlTree.ClearAVL();
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
    } else if (cleanedCommand == "1") { 
      ismission1done = false;
      ismission2done = false;
      std::string cleanedFileNum = "";
      
      while (true) {
        std::cout << "\nInput a file number ([0] Quit): ";
        cleanedFileNum = "";
        
        while (cleanedFileNum.empty()) {
          std::string fileNum;
          if (!std::getline(std::cin, fileNum)) {
            tree23.ClearTree();
            avlTree.ClearAVL();
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
          break; 
        }

        std::string filename = "input" + cleanedFileNum + ".txt";
        std::ifstream infile(filename.c_str());

        if (infile.is_open() == false) {
          std::cout << "\n### " << filename << " does not exist! ###\n";
          continue; 
        } else {
          // 檔案開啟成功，清空指定的樹與資料陣列
          if (cleanedCommand == "1") {
            tree23.ClearTree();
          } else if (cleanedCommand == "2") {
            avlTree.ClearAVL();
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
                tree23.Insert(data.graduates, data.id);
              }     
              currentId = currentId + 1;
            }
          }
          infile.close();

          // 根據指令印出對應的結果
          tree23.PrintRootInfo(dataList);
          std::cout << "\n\n";
          ismission1done = true;
          break; // 處理完畢跳出迴圈
        }
      }
    } else if (cleanedCommand == "2") {
        if (!ismission1done) { 
          cout << "### Choose 1 first. ###\n\n";
          continue;
        }
        if (ismission2done) { // 重複執行任務二
          std::cout << "### AVL tree has been built. ###\n";
        }
        ismission2done = true;
        avlTree.ClearAVL(); // 遞迴清除
        for (int i = 0; i < dataList.size(); i++) {
          avlTree.Insert(dataList[i].schoolName, dataList[i].id);
        }   
        avlTree.printRootInfo(dataList);
        std::cout << "\n\n";
    } else {
      std::cout << "\nCommand does not exist!\n\n";
    }
  }

  tree23.ClearTree();
  avlTree.ClearAVL();
  return 0;
}