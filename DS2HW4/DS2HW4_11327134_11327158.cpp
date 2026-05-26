#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
#include <iomanip>
#include <chrono> // 任務四測量執行時間（毫秒）所需

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

    // 輔助靜態函式：用於 std::sort 排序主陣列，告訴他是比ID 且是由小到大
    static bool compareByStudentID(const HeadNode& a, const HeadNode& b) {
        return a.studentID < b.studentID;
    }

    // --- 新增私有輔助函式 (用於任務三與任務四) ---
    
    // 使用二分搜尋法在已排序的 headVector 中快速尋找學號對應的索引 index
    int findStudentIndex(const vector<HeadNode>& headVector, const string& id) {
        int left = 0;
        int right = (int)headVector.size() - 1;
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (headVector[mid].studentID == id) return mid;
            if (headVector[mid].studentID < id) left = mid + 1;
            else right = mid - 1;
        }
        return -1;
    }

    // 任務三指定：深度優先走訪 (DFS) 遞迴尋找滿足權重門檻的相異可達學號
    void dfsInfluence(int curr, float threshold, const vector<HeadNode>& headVector, vector<bool>& visited, vector<string>& reachable) {
        Node* arc = headVector[curr].firstArc;
        while (arc != nullptr) {
            if (arc->weight >= threshold) { // 有效邊判定
                int neighbor = findStudentIndex(headVector, arc->getID);
                if (neighbor != -1 && !visited[neighbor]) {
                    visited[neighbor] = true;
                    reachable.push_back(arc->getID); // 記錄相異的收訊者學號
                    dfsInfluence(neighbor, threshold, headVector, visited, reachable); // 遞迴 DFS
                }
            }
            arc = arc->next;
        }
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

    // =========================
    // 任務三：影響力 influence
    // =========================
    void mission3(string fileNum, vector<HeadNode>& headVector) {
        if (headVector.empty()) {
            cout << "### There is no graph and choose 1 first. ###\n\n";
            return;
        }

        float threshold;
        while (true) {
            std::cout << "\nInput a real number in [0.66,1.0]: ";
            std::cin >> threshold;

            // 一、檢查是否輸入了「非數字」（文字、純符號、文字混符號）導致 cin 壞掉
            if (std::cin.fail()) {
                std::cin.clear(); // 清除錯誤旗標
                
                string garbage;
                std::cin >> garbage; // 吃掉緩衝區的髒資料

                // 檢查這串髒資料裡面，是不是「含有任何英文字母」
                bool hasLetter = false;
                for (int i = 0; i < (int)garbage.length(); ++i) {
                    if (isalpha(garbage[i])) {
                        hasLetter = true;
                        break; 
                    }
                }

                // 如果「一個字母都沒有」，代表它是純符號（如 ###, ...），才印出錯誤訊息
                if (!hasLetter) {
                    std::cout << "\n### It is NOT in [0.66,1.0] ###\n";
                }
                
                continue; // 重新開始迴圈
            }

            // 二、如果是正常數字，進行數值防呆
            if (threshold < 0.0f) {
                // 負數：依要求「不輸出錯誤訊息」，默默重新輸入
                continue;
            }

            if (threshold >= 0.66f && threshold <= 1.0f) {
                // 成功過關，跳出迴圈
                break;
            }

            // 正數但超出 [0.66, 1.0] 範圍（例如 0.5 或 1.5）：印出錯誤訊息
            std::cout << "\n### It is NOT in [0.66,1.0] ###\n";
        }

        struct InfResult {
            string studentID;
            int influence;
            vector<string> reachable;
        };

        vector<InfResult> results;

        for (int i = 0; i < (int)headVector.size(); ++i) {
            vector<bool> visited(headVector.size(), false);
            vector<string> reachable;

            visited[i] = true;
            dfsInfluence(i, threshold, headVector, visited, reachable);

            if (!reachable.empty()) {
                sort(reachable.begin(), reachable.end());
                results.push_back({headVector[i].studentID, (int)reachable.size(), reachable});
            }
        }

        sort(results.begin(), results.end(), [](const InfResult& a, const InfResult& b) {
            if (a.influence != b.influence) return a.influence > b.influence;
            return a.studentID < b.studentID;
        });

        string infName = "pairs" + fileNum + ".inf";
        ofstream outFile(infName);

        outFile << "<<< There are " << (int)results.size() << " IDs in total. >>>\n";
        outFile << "<<< Threshold = " << threshold << " >>>\n";

        for (int i = 0; i < (int)results.size(); ++i) {
            outFile << "[" << setw(3) << right << i + 1 << "] "
                    << results[i].studentID << "(" << results[i].influence << "): \n";

            int countItem = 0;
            for (int j = 0; j < (int)results[i].reachable.size(); ++j) {
                if (countItem >= 12) {
                    outFile << "\n";
                    countItem = 0;
                }
                outFile << "\t(" << setw(2) << right << j + 1 << ") "
                        << results[i].reachable[j];
                countItem++;
            }
            outFile << "\n";
        }

        outFile.close();
        cout << "\n<<< There are " << (int)results.size() << " IDs in total. >>>\n\n";

    }  // --- 新增功能：任務四（以固定門檻找出前 K 名估計影響力並印於螢幕） ---
  void mission4(const vector<HeadNode>& headVector) {
    struct TopKResult {
      string studentID;
      int count;
    };

    vector<TopKResult> results(headVector.size());
    float fixedThreshold = 0.66f;

    // --- 測量執行時間開始 ---
    auto startTime = chrono::high_resolution_clock::now();

    for (int i = 0; i < (int)headVector.size(); ++i) {
        results[i].studentID = headVector[i].studentID;
        vector<bool> visited(headVector.size(), false);
        vector<string> reachable;
        visited[i] = true;
        
        // 走訪獲取估計影響力
        dfsInfluence(i, fixedThreshold, headVector, visited, reachable);
        results[i].count = (int)reachable.size();
    }

    auto endTime = chrono::high_resolution_clock::now();
    // --- 測量執行時間結束 ---
    auto duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count();

    // 收集影響力為正值的索引
    vector<int> idx;
    for (int i = 0; i < (int)results.size(); ++i) {
        if (results[i].count > 0) {
            idx.push_back(i);
        }
    }

    // 排序：估計影響力由大到小，若相同則學號由小到大
    sort(idx.begin(), idx.end(), [&](int a, int b) {
        if (results[a].count != results[b].count) {
            return results[a].count > results[b].count;
        }
        return results[a].studentID < results[b].studentID;
    });

    // 輸出至螢幕
    cout << "\n[Elapsed time] " << duration << " ms\n\n";
    int max = 0;
    for (int i = 0; i < results.size(); ++i) {
        if (results[i].count > 0) {
            max++;
        }
    }
        float k_input; // 改用 float 接收，這樣才能抓到使用者到底有沒有輸入小數點
        int k = 0;     // 最終過關後要使用的整數 K 值

        while (true) {
            std::cout << "Input an integer to show top-K in [1," << max << "]: \n";
            std::cin >> k_input;

            // 一、檢查是否輸入了「非數字」（文字、純符號、文字混符號）導致 cin 壞掉
            if (std::cin.fail()) {
                std::cin.clear(); // 清除錯誤旗標
                
                string garbage;
                std::cin >> garbage; // 吃掉緩衝區的髒資料

                // 檢查這串髒資料裡面，是不是「含有任何英文字母」
                bool hasLetter = false;
                for (int i = 0; i < (int)garbage.length(); ++i) {
                    if (isalpha(garbage[i])) {
                        hasLetter = true;
                        break; 
                    }
                }

                // 如果「一個字母都沒有」，代表它是純符號（如 ###, ...），才印出錯誤訊息
                if (!hasLetter) {
                    std::cout << "### " << garbage << " is NOT in [1," << max << "] ###\n\n";
                }
                
                continue; // 重新開始迴圈
            }

            // 二、檢查是否為負數
            if (k_input < 0.0f) {
                // 負數：依要求默默重新輸入，不印錯誤訊息
                continue; 
            }

            // 三、檢查是否輸入了小數（例如 1.5）
            // 如果比對發現轉成整數後數值不一樣，代表原本有小數點（例如 1.5 != 1）
            if (k_input != (int)k_input) {
                // 小數：不判定為正確，默默重新輸入
                continue; 
            }

            // 四、確認是純整數後，將它轉回 int 並進行範圍判定
            k = (int)k_input;

            if (k >= 1 && k <= max) { // 這裡的 max 依據你的變數，48 的話就用 max
                break; // 成功過關，跳出迴圈
            } else {
                // 超出範圍的正整數（例如 0 或 50）：印出錯誤訊息
                std::cout << "### " << k << " is NOT in [1," << max << "] ###\n\n";
            }
        }
    int printedCount = 0;
    int lastCount = -1;

    for (int i = 0; i < (int)idx.size(); ++i) {
        int realIdx = idx[i];
        
        // 如果印出的數量已達 K，且目前這個人的分數跟上一位不一樣，就截止
        // 這樣可以確保「數值相同太多若超過 K 筆則必須全部輸出」
        if (printedCount >= k && results[realIdx].count != lastCount) {
            break;
        }

        cout << "<" << printedCount + 1 << "> " 
             << results[realIdx].studentID << ": " << results[realIdx].count << "\n";
        
        printedCount++;
        lastCount = results[realIdx].count;
    }
    cout << "\n";
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
                     "\n* 3. Estimate influence values   *"
                     "\n* 4. Find top-k influence values *"
                     "\n**********************************\n"
                     "Input a choice(0, 1, 2, 3, 4): ";      // 更新輸入範圍提示
        std::cin >> cmd;
        if (cmd == "0") {
            break;
        }
        else if (cmd == "1") {
            std::cout << "\nInput a file number ([0] Quit): "; 
            std::cin >> fileNum;
            if (fileNum != "0") {
                graph.mission1(fileNum, students, headVector);
            } else if (fileNum == "0") {
                students.clear();
                headVector.clear();
            }
            std::cout << "\n";
        } else if (cmd == "2") {
            if (headVector.empty()) {
                std::cout << "### There is no graph and choose 1 first. ###\n\n";
            } else {
              graph.mission2(fileNum, headVector);
            }
        } else if (cmd == "3") { // 新增任務三整合與防呆
            if (headVector.empty()) {
                std::cout << "### There is no graph and choose 1 first. ###\n\n";
            } else {
                graph.mission3(fileNum, headVector);
            }
        } else if (cmd == "4") { // 新增任務四整合與防呆
            if (headVector.empty()) {
                std::cout << "### There is no graph and choose 1 first. ###\n\n";
            } else {
                graph.mission4(headVector);
            }
        } else std::cout << "\nCommand does not exist!\n\n";
    }
    return 0;
}
