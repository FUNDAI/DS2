#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
#include <iomanip>

using namespace std;

struct Student {
    char putID[12]; 
    char getID[12]; 
    float weight;   
};

// 1. 串列節點：存放【收訊者學號】和【量化權重】
struct Node {
    string getID;       // 收訊者學號
    float weight;       // 量化權重
    Node* next = nullptr;
};

// 2. 主陣列元素：存放【發訊者學號】及指向其串列的指標
struct HeadNode {
    string studentID;   // 學生學號
    Node* firstArc = nullptr; // 串列起點（若從未發訊則為 nullptr）
};


class Graph {
 private:
    void insertSorted(Node*& head, const string& getID, float weight) {
        Node* newNode = new Node{getID, weight, nullptr};
        if (!head || getID < head->getID) {
            newNode->next = head;
            head = newNode;
        } else {
            Node* curr = head;
            while (curr->next && curr->next->getID < getID) {
                curr = curr->next;
            }
            newNode->next = curr->next;
            curr->next = newNode;
        }
    }


    int getOrAddStudent(vector<HeadNode>& headVector, const string& id) {
        for (int i = 0; i < (int)headVector.size(); ++i) {
            if (headVector[i].studentID == id) {
                return i;
            }
        }
        headVector.push_back({id, nullptr});
        return (int)headVector.size() - 1;
    }

    // 輔助靜態函式：用於 std::sort 排序主陣列
    static bool compareByStudentID(const HeadNode& a, const HeadNode& b) {
        return a.studentID < b.studentID;
    }

 public:

    void mission1(string fileNum, vector<Student> &students, vector<HeadNode> &headVector) {
        students.clear();
        headVector.clear();
        std::string binName = "pairs" + fileNum + ".bin";
        std::ifstream inFile(binName, std::ios::binary);
        if (!inFile.is_open()) {
            cout << "\n### " << binName << " does not exist! ###\n";
            return;
        }
        Student temp;
        while (inFile.read((char*)&temp, sizeof(Student))) students.push_back(temp);
        inFile.close();
        if (students.empty()) return;
            // --- 將讀進來的資料轉換為「相鄰串列」 ---
            // 使用 int 作為計數器
        for (int i = 0; i < (int)students.size(); ++i) {
            const Student& record = students[i];
                    
            char cleanPut[13] = {0}; // 開 13 格，初始化全為 \0
            char cleanGet[13] = {0};

            // 只精準複製 12 個字元，因為陣列第 13 格是 \0，字串絕對會在這裡乖乖切斷
            strncpy(cleanPut, record.putID, 12);
            strncpy(cleanGet, record.getID, 12);

            // 這時候轉出來的字串就非常乾淨，絕對不會帶有後面的 float 權重亂碼
            string putID(cleanPut);
            string getID(cleanGet);
            
            // 把發訊者與收訊者加入主陣列
            int putIdx = getOrAddStudent(headVector, putID);
            getOrAddStudent(headVector, getID);

                // 將收訊者排序插入對應的串列
            insertSorted(headVector[putIdx].firstArc, getID, record.weight);
        }

            // 將相鄰串列的主陣列依照【學號字串】由小到大排序
        sort(headVector.begin(), headVector.end(), compareByStudentID);

            // --- 輸出到 .adj 文字檔 ---
        std::string adjName = "pairs" + fileNum + ".adj";
        ofstream outFile(adjName);
        outFile << "\n<<< There are " << (int)headVector.size() << " IDs in total. >>>\n";
        for (int i = 0; i < (int)headVector.size(); ++i) {
            int count = 0;
            const HeadNode& head = headVector[i];
            outFile << "[" << setw(3) << right << i + 1 << "] " << head.studentID << ": \n";
                
            Node* curr = head.firstArc;
            int nodeIdx = 1; // 串列節點的計數器
                
            while (curr != nullptr) {
                if (count >= 12) { // 如果已經印了 12 個節點，換行並重置計數器
                    count = 0;
                }
                outFile << "\t(" << setw(2) << right << nodeIdx << ") " << curr->getID << ",";
                outFile << "   " << setw(4) << right;
                if (curr->weight == (int)curr->weight) {
                    outFile << (int)curr->weight; // 如果是 1.00，就直接印出 1
                } else {
                    outFile << curr->weight; 
                }
                curr = curr->next;
                nodeIdx++;
                    
                count++;
                if (count == 12) {
                    outFile << "\n";
                }
            }
            outFile << "\n"; // 這個學生印完了，換行
        }   

        // --- 新增：計算總節點數 (邊數) ---
        int totalNodes = 0;
        for (int i = 0; i < (int)headVector.size(); ++i) {
            Node* curr = headVector[i].firstArc;
            while (curr != nullptr) {
                totalNodes++;
                curr = curr->next;
            }
        }
        outFile << "<<< There are " << totalNodes << " nodes in total. >>>\n";
        outFile.close();
        cout << "\n<<< There are " << (int)headVector.size() << " IDs in total. >>>\n\n";
        cout << "<<< There are " << totalNodes << " nodes in total. >>>\n";
    }

  void mission2(string fileNum, vector<HeadNode>& headVector) {
    struct CntResult {
      string studentID;
      int count;
      vector<string> reachable;
    };
    
    vector<CntResult> results(headVector.size());
    for (int i = 0; i < (int)headVector.size(); ++i) {
      results[i].studentID = headVector[i].studentID;
      results[i].count = 0;
      
      vector<bool> visited(headVector.size(), false);
      vector<int> q;
      q.push_back(i);
      visited[i] = true;
      
      int head = 0;
      while (head < (int)q.size()) {
        int curr = q[head++];
        Node* arc = headVector[curr].firstArc;
        
        while (arc != nullptr) {
          int left = 0;
          int right = (int)headVector.size() - 1;
          int neighbor = -1;
          
          while (left <= right) {
            int mid = left + (right - left) / 2;
            if (headVector[mid].studentID == arc->getID) {
              neighbor = mid;
              break;
            }
            if (headVector[mid].studentID < arc->getID) {
              left = mid + 1;
            } else {
              right = mid - 1;
            }
          }
          
          if (neighbor != -1 && !visited[neighbor]) {
            visited[neighbor] = true;
            results[i].reachable.push_back(arc->getID);
            q.push_back(neighbor);
          }
          arc = arc->next;
        }
      }
      results[i].count = (int)results[i].reachable.size();
      sort(results[i].reachable.begin(), results[i].reachable.end());
    }
    
    vector<int> idx(results.size());
    for (int i = 0; i < (int)idx.size(); ++i) {
      idx[i] = i;
    }
    
    sort(idx.begin(), idx.end(), [&](int a, int b) {
      if (results[a].count != results[b].count) {
        return results[a].count > results[b].count;
      }
      return results[a].studentID < results[b].studentID;
    });
    
    string cntName = "pairs" + fileNum + ".cnt";
    ofstream outFile(cntName);
    
    outFile << "<<< There are " << (int)headVector.size() << " IDs in total. >>>\n";
    for (int i = 0; i < (int)idx.size(); ++i) {
      int realIdx = idx[i];
      outFile << "[" << setw(3) << right << i + 1 << "] " 
              << results[realIdx].studentID << "(" << results[realIdx].count << "): \n";
              
      int countItem = 0;
      int nodeIdx = 1;
      for (int j = 0; j < results[realIdx].count; ++j) {
        if (countItem >= 12) {
          countItem = 0;
        }
        outFile << "\t(" << setw(2) << right << nodeIdx << ") " << results[realIdx].reachable[j];
        nodeIdx++;
        countItem++;
        
        if (countItem == 12) {
          outFile << "\n";
        }
      }
      outFile << "\n";
    }
    outFile.close();
    cout << "\n<<< There are " << (int)headVector.size() << " IDs in total. >>>\n\n";
  }
};



int main() {
    std::string cmd, fileNum;
    vector<Student> students; 
    vector<HeadNode> headVector; 
    Graph graph;
    bool beenm1 = false;
    
    while (true) {
        std::cout << "* Data Structures and Algorithms *"
                     "\n**** Graph data manipulation *****"
                     "\n* 0. QUIT                        *"
                     "\n* 1. Build adjacency lists       *"
                     "\n* 2. Compute connection counts   *"
                     "\n**********************************\n"
                     "Input a choice(0, 1, 2): ";
        std::cin >> cmd;
        if (cmd == "0") {
            break;
        }
        else if (cmd == "1") {
            std::cout << "\nInput a file number ([0] Quit): "; 
            std::cin >> fileNum;
            if (fileNum != "0") {
                graph.mission1(fileNum, students, headVector);
            }
            std::cout << "\n";
        } else if (cmd == "2") {
            if (headVector.empty()) {
                std::cout << "### There is no graph and choose 1 first. ###\n\n";
            } else {
              graph.mission2(fileNum, headVector);
            }
        } else std::cout << "\nCommand does not exist!\n\n";
    }
    return 0;
}