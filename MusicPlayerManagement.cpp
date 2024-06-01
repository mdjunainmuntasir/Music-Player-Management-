#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stack>
#include <unordered_set>
#include <stdexcept>
#include <limits>
#include <algorithm>

using namespace std;

struct Node {
    string songName;
    string artist;
    Node* prev;
    Node* next;
    vector<Node*> related;  // To store related songs (edges in a graph)
};

Node* createNode(const string& songName, const string& artist) {
    Node* newNode = new Node;
    newNode->songName = songName;
    newNode->artist = artist;
    newNode->prev = nullptr;
    newNode->next = nullptr;
    return newNode;
}

void insertNode(Node** head, const string& songName, const string& artist) {
    Node* newNode = createNode(songName, artist);
    if (*head == nullptr) {
        *head = newNode;
    } else {
        Node* temp = *head;
        while (temp->next != nullptr) {
            temp = temp->next;
        }
        temp->next = newNode;
        newNode->prev = temp;
    }
}

void deleteNode(Node** head, const string& songName) {
    Node* temp = *head;
    while (temp != nullptr) {
        if (temp->songName == songName) {
            if (temp->prev != nullptr) {
                temp->prev->next = temp->next;
            }
            if (temp->next != nullptr) {
                temp->next->prev = temp->prev;
            }
            if (temp == *head) {
                *head = temp->next;
            }
            delete temp;
            cout << "Song '" << songName << "' deleted from the playlist." << endl;
            return;
        }
        temp = temp->next;
    }
    cout << "Song '" << songName << "' not found in the playlist." << endl;
}

void moveSong(Node** head, const string& songName) {
    Node* temp = *head;
    while (temp != nullptr && temp->songName != songName) {
        temp = temp->next;
    }

    if (temp == nullptr) {
        cout << "Song '" << songName << "' not found in the playlist." << endl;
        return;
    }

    if (temp == *head) {
        cout << "Song '" << songName << "' is already at the beginning of the playlist." << endl;
        return;
    }

    if (temp->prev != nullptr) {
        temp->prev->next = temp->next;
    }
    if (temp->next != nullptr) {
        temp->next->prev = temp->prev;
    }
    temp->prev = nullptr;
    temp->next = *head;
    (*head)->prev = temp;
    *head = temp;

    cout << "Song '" << songName << "' moved to the beginning of the playlist." << endl;
}
//this is the node for starting merge sort

Node* merge(Node* left, Node* right) {
    if (left == nullptr) return right;
    if (right == nullptr) return left;

    if (left->songName <= right->songName) {
        left->next = merge(left->next, right);
        left->next->prev = left;
        left->prev = nullptr;
        return left;
    } else {
        right->next = merge(left, right->next);
        right->next->prev = right;
        right->prev = nullptr;
        return right;
    }
}

void split(Node* source, Node** frontRef, Node** backRef) {
    Node* fast = source->next;
    Node* slow = source;

    while (fast != nullptr) {
        fast = fast->next;
        if (fast != nullptr) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    *frontRef = source;
    *backRef = slow->next;
    slow->next = nullptr;
}
//this function is for merge sort implement
void mergeSort(Node** head) {
    if (*head == nullptr || (*head)->next == nullptr) return;

    Node* a;
    Node* b;

    split(*head, &a, &b);

    mergeSort(&a);
    mergeSort(&b);

    *head = merge(a, b);
}
//this is the main function calling for merge sort

void sortPlaylist(Node** head) {
    if (*head == nullptr) {
        cout << "Playlist is empty." << endl;
        return;
    }

    mergeSort(head);
    cout << "Playlist sorted in alphabetical order." << endl;
}

void displayPlaylist(Node* head) {
    if (head == nullptr) {
        cout << "Playlist is empty." << endl;
    } else {
        cout << "Playlist:" << endl;
        Node* temp = head;
        while (temp != nullptr) {
            cout << "Song: " << temp->songName << ", Artist: " << temp->artist << endl;
            temp = temp->next;
        }
    }
}

void loadPlaylistFromFile(Node** head, const string& filename) {
    ifstream file;
    try {
        file.open(filename);
        if (!file) {
            throw ios_base::failure("Error opening file");
        }

        string songName, artist;
        while (getline(file, songName) && getline(file, artist)) {
            insertNode(head, songName, artist);
        }
        file.close();
    } catch (const ios_base::failure& e) {
        cerr << "Exception opening/reading file: " << e.what() << endl;
    }
}

void savePlaylistToFile(Node* head, const string& filename) {
    ofstream file;
    try {
        file.open(filename);
        if (!file) {
            throw ios_base::failure("Error opening file");
        }

        Node* temp = head;
        while (temp != nullptr) {
            file << temp->songName << endl;
            file << temp->artist << endl;
            temp = temp->next;
        }
        file.close();
    } catch (const ios_base::failure& e) {
        cerr << "Exception opening/writing file: " << e.what() << endl;
    }
}

void addRelatedSong(Node* song, Node* relatedSong) {
    song->related.push_back(relatedSong);
}

void dfs(Node* start) {
    if (!start) return;

    unordered_set<Node*> visited;
    stack<Node*> stack;
    stack.push(start);

    while (!stack.empty()) {
        Node* current = stack.top();
        stack.pop();

        if (visited.find(current) == visited.end()) {
            visited.insert(current);
            cout << "Visited Song: " << current->songName << " by " << current->artist << endl;

            for (Node* neighbor : current->related) {
                if (visited.find(neighbor) == visited.end()) {
                    stack.push(neighbor);
                }
            }
        }
    }
}

int main() {
    Node* playlist = nullptr;
    const string filename = "playlist.txt";

    // Load the playlist from the file at the beginning
    loadPlaylistFromFile(&playlist, filename);

    string songName, artist;
    int choice;

    while (true) {
        cout << "\nMusic Playlist Management System" << endl;
        cout << "1. Add Song" << endl;
        cout << "2. Delete Song" << endl;
        cout << "3. Reorder Song" << endl;
        cout << "4. Sort Playlist" << endl;
        cout << "5. Display Playlist" << endl;
        cout << "6. Perform DFS Traversal" << endl;
        cout << "7. Exit" << endl;
        cout << "Enter your choice: ";

        try {
            if (!(cin >> choice)) {
                throw invalid_argument("Invalid input. Please enter a number between 1 and 7.");
            }

            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the newline character left in the buffer

            switch (choice) {
                case 1:
                    cout << "Enter song name: ";
                    getline(cin, songName);
                    cout << "Enter artist name: ";
                    getline(cin, artist);
                    insertNode(&playlist, songName, artist);
                    savePlaylistToFile(playlist, filename);
                    cout << "Song added to the playlist." << endl;
                    break;
                case 2:
                    cout << "Enter song name to delete: ";
                    getline(cin, songName);
                    deleteNode(&playlist, songName);
                    savePlaylistToFile(playlist, filename);
                    break;
                case 3:
                    cout << "Enter song name to move to the beginning: ";
                    getline(cin, songName);
                    moveSong(&playlist, songName);
                    savePlaylistToFile(playlist, filename);
                    break;
                case 4:
                    sortPlaylist(&playlist);
                    savePlaylistToFile(playlist, filename);
                    break;
                case 5:
                    displayPlaylist(playlist);
                    break;
                case 6:
                    cout << "Enter the song name to start DFS traversal: ";
                    getline(cin, songName);
                    {
                        Node* temp = playlist;
                        while (temp != nullptr && temp->songName != songName) {
                            temp = temp->next;
                        }
                        if (temp != nullptr) {
                            dfs(temp);
                        } else {
                            cout << "Song '" << songName << "' not found in the playlist." << endl;
                        }
                    }
                    break;
                case 7:
                    // Free memory - cleanup before exiting
                    while (playlist !=nullptr) {
                        Node* temp = playlist;
                        playlist = playlist->next;
                        delete temp;
                    }
                    return 0;
                default:
                    cout << "Invalid choice. Please enter a number between 1 and 6." << endl;
            }
        } catch (const invalid_argument& e) {
            cin.clear(); // Clear the error flag
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard invalid input
            cout << e.what() << endl;
        }
    }

    return 0;
}
