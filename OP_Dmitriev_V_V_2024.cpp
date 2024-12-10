/* Info:
    Project author: Vladislav Dmitriev
    Group name: ИУК4-12Б
    Input point: OP_Dmitriev_V_V_2024.cpp
    Git repo: OP-Dmitriev-V-V-IUK4-12B-2024-Project
    Last edit: 10/12/2024
    Version: 1.0 (win stable)
*/

#include <iostream>
#include <cstring>
#include <fstream>
#include <windows.h>
#include <filesystem>

using namespace std;

namespace fs = filesystem;

//--------------------------------------------------------------------------------------//

// GLOBAL CONSTANTS:
const int ML_STR = 1000; // MAX_LENGTH_STRING - for strings
const int ML = 150; // MAX_LENGTH - for arrays and other
const int MBL = 512;

// GLOBAL VARS:
// INT:
int programmState {0};
int studentsNum {0};

//BOOL:
bool render {true};

//STRING:
char groups[ML][ML] {""};

//GLOBAL STRUCTURES:
struct Student
    {
        char firstName[ML_STR] = "";
        char secondName[ML_STR] = "";
        char thirdname[ML_STR] = "";
        int joinYear {2024};
        int course {1};
        char group[ML_STR] = "";
        int birthdate {1};
        int birthmounth {1};
        int birthyear {1924};
        int grades[4];
        double avgGrade {0};
};
Student students[ML];

//--------------------------------------------------------------------------------------//

//  START UTIL FUNCTIONS:

size_t utf8_length(const char* str) {
    #ifdef __linux__
    size_t length = 0;
    for (size_t i = 0; str[i] != '\0';) {
        if ((str[i] & 0x80) == 0) {
            i += 1;
        } else if ((str[i] & 0xE0) == 0xC0) {
            i += 2;
        } else if ((str[i] & 0xF0) == 0xE0) {
            i += 3;
        } else if ((str[i] & 0xF8) == 0xF0) {
            i += 4;
        } else {
            i += 1;
        }
        length++;
    }
    return length;
    #elif defined(_WIN32)
    return strlen(str);
    #endif
}
int getStudentBDate(char fullBirth[]) {
    char bDay[3] = "";
    strncpy(bDay, fullBirth, 2);
    int day = atoi(bDay);
    if (day < 1 || day > 31){
        cout << "Incorect date (less then 1 or more then 31 )" << endl;
        return -1;
    }
    return day;
}
int getStudentBMounth(char fullBirth[]) {
    char bMounth[3] = "";
    strncpy(bMounth, fullBirth + 3, 2);
    int mounth = atoi(bMounth);
    if (mounth < 1 || mounth > 12){
        cout << "Incorect mounth (less then 1 or more then 12 )" << endl;
        return -1;
    }
    return mounth;
}
int getStudentBYear(char fullBirth[]) {
    char bYear[5] = "";
    strncpy(bYear, fullBirth + 6, 4);
    int year = atoi(bYear);
    if (year > 2020) {
        cout << "Incorect year, student is too young to study in bmstu" << endl;
        return -1;
    }
    return year;
}
void UserInput(char string[]) {
    bool empti {true};
    char input[ML_STR] = "";
    while (empti) {
        cin.getline(input, ML_STR);
        if (strlen(input) == 0){
            continue;
        }
        else {
            empti = false;
        }
    }
    strcpy(string, input);
}
void CleanCons() {
    cout << "\033[2J\033[1;1H";
}
void dateOutput (int bDay, int bMounth, int bYear) {
    if(bDay < 10) cout << "0" << bDay;
    else cout << bDay;
    cout << ".";
    if(bMounth < 10) cout << "0" << bMounth;
    else cout << bMounth;
    cout << ".";
    cout << bYear;
}
void splitString(const char* input, char result[][26], int& numParts) {
    int inputLength = utf8_length(input);
    numParts = (inputLength + 24) / 25;

    for (int i = 0; i < numParts; ++i) {
        int startIndex = i * 25;
        int length = (i == numParts - 1) ? inputLength - startIndex : 25;

        strncpy(result[i], input + startIndex, length);

        result[i][length] = '\0';
    }
}
void copyStudent(Student& dest, const Student& src) {
    strcpy(dest.firstName, src.firstName);
    strcpy(dest.secondName, src.secondName);
    dest.avgGrade = (src.grades[0] + src.grades[1] + src.grades[2] + src.grades[3]) / 4.;
    strcpy(dest.group, src.group);
}
void CheckGroups() {
    for (int i = 0; i < 30; ++i){
        char studGroup[15] = "";
        strcpy(studGroup, students[i].group);
        for(int j = 0; j < 30; ++j){
            if (strcmp(studGroup, groups[j]) == 0) {
                break;
            }
            else if (strlen(groups[j]) == 0 && strlen(studGroup) != 0){
                strcpy(groups[j], studGroup);
                break;
            }
        }
    }
}
void filterStudentsByGroup(const Student students[], int studentsCount, Student filteredStudents[], int maxFilteredCount, const char* targetGroup) {
    int filteredCount = 0;

    for (int i = 0; i < studentsCount; ++i) {
        if (strcmp(students[i].group, targetGroup) == 0) {
            if (filteredCount < maxFilteredCount) {
                copyStudent(filteredStudents[filteredCount], students[i]);
                ++filteredCount;
            } else {
                cout << "Filtered students array capacity exceeded!" << endl;
                break;
            }
        }
    }
}

//  END UTIL FUNCTIONS

//--------------------------------------------------------------------------------------//

// Functions initialization:

int GetSortMethod(int sortType);
void CloseProgramm ();
void PrintMenu ();
void MenuWork ();
void ShowAllUsers ();
void PrintSortMenu ();
void SortMenuWork ();
void SortStudents(Student* students, int n, int sortType, int sortMethod);
void PrintMonthSelect ();
void CreateUser ();
void TablePrintHead ();
void BestStudent (); 
void TablePrintStudent (char fname[], char sname[], char tname[], char gname[], int jyear, int course, int grades[4], int number, double avg, int birthdate, int birthmounth, int birthyear);
void TablePrintUserStr (char fname[], char sname[], char tname[], char gname[], int jyear, int course, int grades[4], int number, double avg, int birthdate, int birthmounth, int birthyear);
void MounthSort (int month, Student students[], int size);
void BornInMounth ();
void EditStudent ();
void DeleteStudent ();
void SaveListToTxt ();
void SaveListToBinary ();
void LoadStudentsFromFileTXT (const char* filename);
void LoadStudentsFromBinaryFile (const char* filename);
void ListTxtFilesAndLoad ();
void ListBinFilesAndLoad ();

//--------------------------------------------------------------------------------------//

// START MAIN BLOCK
int main() {

    #ifdef __linux__
    //Логика для линукса (настройка локализации)
    #elif defined(_WIN32)
        SetConsoleCP(1251);
        SetConsoleOutputCP(1251);
        setlocale(LC_ALL, "rus");
    #endif

    CleanCons();
    cout << "\n\033[1;36mHello!\033[0m\n" << endl;
    programmState = 1;

    do {
        switch (programmState) {
        case 0:
            CloseProgramm();
            break;
        case 1:
            MenuWork();
            break;
        case 2:
            ShowAllUsers();
            break;
        case 3:
            CreateUser();
            break;
        case 4:
            SortMenuWork();
            break;
        case 5:
            BestStudent();
            break;
        case 6:
            BornInMounth();
            break;
        case 7:
            EditStudent();
            break;
        case 8:
            DeleteStudent();
            break;
        case 9:
            SaveListToTxt();
            break;
        case 10: 
            SaveListToBinary();
            break;
        case 11:
            ListTxtFilesAndLoad();
            break;
        case 12:
            ListBinFilesAndLoad();
            break;
        default:
            break;
        }
    } while (render);
}

// END MAIN BLOCK

//--------------------------------------------------------------------------------------//

// START FUNCTIONS:

int GetSortMethod(int sortType) {
    switch (sortType)
    {
    case 8:
    case 9:
    case 10:
    case 11:
    case 15:
        cout << "\033[1mSort from? ('-t' - from top to bottom, '-b' - from bottom to top):\033[0m " ;
        break;
    case 12: 
        cout << "\033[1mSort from? ('-t' - from newer year to older , '-b' - from older year to newer):\033[0m " ;
        break;
    case 13:
        cout << "\033[1mSort from? ('-t' - from the best to bad, '-b' - from bad to the best):\033[0m " ;
        break;
    case 14:
        cout << "\033[1mSort from? ('-t' - from younger to older, '-b' - from older to younger):\033[0m " ;
        break;
    default:
        break;
    }
    char arrg[ML_STR] = "";
    UserInput(arrg);
    if (strcmp(arrg, "-t") == 0) {
        return 1;
    }
    else if (strcmp(arrg, "-b") == 0){
        return 2;
    }
    else {
        return 1;
    }
}
void CloseProgramm () {
    cout << "\033[2J\033[1;1H";
    cout << "\n\033[1;36mBye <3\033[0m\n" << endl;
    render = false;
}
void PrintMenu () {
    cout << "\e[1;31mMenu: \e[0m" << endl;
    cout << "\033[1m0) /showall\033[0m - Show all students" << endl;
    cout << "\033[1m1) /add\033[0m - Add new student" << endl;
    cout << "\033[1m2) /edit\033[0m - Edit student entry" << endl;
    cout << "\033[1m3) /delete\033[0m - Delete student entry" << endl;
    cout << "\033[1m4) /best\033[0m - Show student's name and surname with best avg. grade" << endl;
    cout << "\033[1m5) /binm\033[0m - Show student's name, surname and group born in select mounth" << endl;
    cout << "\033[1m6) /exit\033[0m - Exit programm" << endl;
    cout << endl;
    cout << "\033[1m18) /lbind\033[0m - \033[1;36mLoad\033[0m binary students list" << endl;
    cout << "\033[1m19) /ltxtd\033[0m - \033[1;36mLoad\033[0m txt students list" << endl;
}
void PrintSortMenu () {
    cout << "\n\033[1;36mSort menu:\033[0m\n" << endl;
    cout << "\033[1m7) /cltab\033[0m - Close all students table" << endl;
    cout << "\033[1m8) /sbfn\033[0m - Sort students by first name" << endl;
    cout << "\033[1m9) /sbsn\033[0m - Sort students by second name" << endl;
    cout << "\033[1m10) /sbtn\033[0m - Sort students by third name" << endl;
    cout << "\033[1m11) /sbgn\033[0m - Sort students by group name" << endl;
    cout << "\033[1m12) /sbjy\033[0m - Sort students by join year" << endl;
    cout << "\033[1m13) /sbgfs\033[0m - Sort students by grades for 4 subjects" << endl;
    cout << "\033[1m14) /sbbd\033[0m - Sort students by birth date" << endl;
    cout << "\033[1m15) /sbc\033[0m - Sort students by course" << endl;
    cout << endl;
    cout << "\033[1m16) /stbin\033[0m - \033[1;36mSave\033[0m list to binary" << endl;
    cout << "\033[1m17) /sttxt\033[0m - \033[1;36mSave\033[0m list to txt" << endl;
}
void PrintMonthSelect () {
    cout << "\e[1;31mSelect mounth: \e[0m" << endl;
    cout << "\033[1m1) January\033[0m" << endl;
    cout << "\033[1m2) February\033[0m" << endl;
    cout << "\033[1m3) March\033[0m" << endl;
    cout << "\033[1m4) April\033[0m" << endl;
    cout << "\033[1m5) May\033[0m" << endl;
    cout << "\033[1m6) June\033[0m" << endl;
    cout << "\033[1m7) July\033[0m" << endl;
    cout << "\033[1m8) August\033[0m" << endl;
    cout << "\033[1m9) September\033[0m" << endl;
    cout << "\033[1m10) October\033[0m" << endl;
    cout << "\033[1m11) November\033[0m" << endl;
    cout << "\033[1m12) December\033[0m" << endl;
}
void MenuWork() {
    PrintMenu();
    char string[ML_STR] = "";
    UserInput(string);
    if (strcmp(string, "/exit") == 0 or strcmp(string, "6") == 0) programmState = 0;
    else if (strcmp(string, "/showall") == 0 or strcmp(string, "0") == 0) programmState = 2;
    else if (strcmp(string, "/add") == 0 or strcmp(string, "1") == 0) programmState = 3;
    else if (strcmp(string, "/edit") == 0 or strcmp(string, "2") == 0) programmState = 7;
    else if (strcmp(string, "/delete") == 0 or strcmp(string, "3") == 0) programmState = 8;
    else if (strcmp(string, "/best") == 0 or strcmp(string, "4") == 0) programmState = 5;
    else if (strcmp(string, "/binm") == 0 or strcmp(string, "5") == 0) programmState = 6;
    else if (strcmp(string, "/lbind") == 0 or strcmp(string, "18") == 0) programmState = 12;
    else if (strcmp(string, "/ltxtd") == 0 or strcmp(string, "19") == 0) programmState = 11;
    else {
        cout << "Command is not found" << endl;
    }
    
}
void ShowAllUsers() {
    CleanCons();
    if (strlen(students[0].firstName) == 0 || strlen(students[0].secondName) == 0 || strlen(students[0].thirdname) == 0  || strlen(students[0].group) == 0){
        cout << "Student's table is empty\n" << endl;
        MenuWork();
        return;
    }
    
    TablePrintHead();
     for (int i = 0; i < 30; ++i) {
        if (strlen(students[i].firstName) == 0 || strlen(students[i].secondName) == 0 || strlen(students[i].thirdname) == 0  || strlen(students[i].group) == 0) break;
        TablePrintStudent(students[i].firstName, students[i].secondName, students[i].thirdname, students[i].group, students[i].joinYear, students[i].course, students[i].grades, i+1, students[i].avgGrade, students[i].birthdate, students[i].birthmounth, students[i].birthyear);
     }
    programmState = 4;
    cout << endl;
    return;
}
void SortMenuWork() {
    PrintSortMenu();
    char string[ML_STR] = "";
    UserInput(string);
    int sortMethod {0};
    if (strcmp(string, "/cltab") == 0 or strcmp(string, "7") == 0) {
        cout << "\033[2J\033[1;1H" << endl;
        programmState = 1;
        return;
    }
    else if (strcmp(string, "/sbfn") == 0 or strcmp(string, "8") == 0) {
        sortMethod = GetSortMethod(8);
        SortStudents(students, 30, 8, sortMethod);
    }
    else if (strcmp(string, "/sbsn") == 0 or strcmp(string, "9") == 0) {
        sortMethod = GetSortMethod(9);
        SortStudents(students, 30, 9, sortMethod);
        // sort students by second name
    }
    else if (strcmp(string, "/sbtn") == 0 or strcmp(string, "10") == 0) {
        sortMethod = GetSortMethod(10);
        SortStudents(students, 30, 10, sortMethod);
        // sort students by third name
    }
    else if (strcmp(string, "/sbgn") == 0 or strcmp(string, "11") == 0) {
        sortMethod = GetSortMethod(11);
        SortStudents(students, 30, 11, sortMethod);
        // sort students by group name
    }
    else if (strcmp(string, "/sbjy") == 0 or strcmp(string, "12") == 0) {
        sortMethod = GetSortMethod(12);
        SortStudents(students, 30, 12, sortMethod);
        // sort students by join year
    }
    else if (strcmp(string, "/sbgfs") == 0 or strcmp(string, "13") == 0) {
        sortMethod = GetSortMethod(13);
        SortStudents(students, 30, 13, sortMethod);
        // sort students by avg grade
    }
    else if (strcmp(string, "/sbbd") == 0 or strcmp(string, "14") == 0) {
        sortMethod = GetSortMethod(14);
        SortStudents(students, 30, 14, sortMethod);
        // sort students by birth date
    }
    else if (strcmp(string, "/sbc") == 0 or strcmp(string, "15") == 0) {
        sortMethod = GetSortMethod(15);
        SortStudents(students, 30, 15, sortMethod);
        // sort students by course
    }

    else if (strcmp(string, "/stbin") == 0 or strcmp(string, "16") == 0) {
        programmState = 10;
        return;
        // save list to bin
    }
    else if (strcmp(string, "/sttxt") == 0 or strcmp(string, "17") == 0) {
        programmState = 9;
        return;
        // save list to txt
    }
    else {
        cout << "Command is not found" << endl;
    }
    programmState = 2;
    return;
}
void SortStudents(Student* students, int n, int sortType, int sortMethod) {
    int stNum {0};
    for(int z = 0; z < n; z++){
        if(strlen(students[z].firstName) != 0) {
            stNum += 1;
        }
    }
    bool ascending {false};
    if (sortMethod == 1 ) {
        ascending = true;
    }
    else {
        ascending = false;
    }
    for (int i = 0; i < stNum - 1; i++) {
        for (int j = 0; j < stNum - i - 1; j++) {
            bool swapNeeded = false;
            switch (sortType) {
                case 8: // Сортировка по Ф
                    swapNeeded = ascending ? strcmp(students[j].firstName, students[j + 1].firstName) > 0 : strcmp(students[j].firstName, students[j + 1].firstName) < 0;
                    break;
                case 9: // Сортировка по И
                    swapNeeded = ascending ? strcmp(students[j].secondName, students[j + 1].secondName) > 0 : strcmp(students[j].secondName, students[j + 1].secondName) < 0;
                    break;
                case 10: // Сортировка по О
                    swapNeeded = ascending ? strcmp(students[j].thirdname, students[j + 1].thirdname) > 0 : strcmp(students[j].thirdname, students[j + 1].thirdname) < 0;
                    break;
                case 11: // Сортировка по группе
                    swapNeeded = ascending ? strcmp(students[j].group, students[j + 1].group) > 0 : strcmp(students[j].group, students[j + 1].group) < 0;
                    break;
                case 12: // Сортировка по году поступления
                    swapNeeded = !ascending ? students[j].joinYear > students[j + 1].joinYear : students[j].joinYear < students[j + 1].joinYear;
                    break;
                case 13: // Сортировка по среднему баллу
                    swapNeeded = !ascending ? students[j].avgGrade > students[j + 1].avgGrade : students[j].avgGrade < students[j + 1].avgGrade;
                    break;
                case 14: // Сортировка по дате рождения
                    swapNeeded = !ascending ? 
                        (students[j].birthyear > students[j + 1].birthyear || 
                        (students[j].birthyear == students[j + 1].birthyear && students[j].birthmounth > students[j + 1].birthmounth) || 
                        (students[j].birthyear == students[j + 1].birthyear && students[j].birthmounth == students[j + 1].birthmounth && students[j].birthdate > students[j + 1].birthdate)) :
                        (students[j].birthyear < students[j + 1].birthyear || 
                        (students[j].birthyear == students[j + 1].birthyear && students[j].birthmounth < students[j + 1].birthmounth) || 
                        (students[j].birthyear == students[j + 1].birthyear && students[j].birthmounth == students[j + 1].birthmounth && students[j].birthdate < students[j + 1].birthdate));
                        break;
                case 15: // Сортировка по курсу
                    swapNeeded = !ascending ? students[j].course > students[j + 1].course : students[j].course < students[j + 1].course;
                    break;
                default:
                    cout << "Неизвестный тип сортировки" << endl;
                    return;
            }
            if (swapNeeded) {
                // Обмен местами двух студентов
                Student temp = students[j];
                students[j] = students[j + 1];
                students[j + 1] = temp;
            }
        }
    }
}
void CreateUser(){
    CleanCons();
    cout << "\n\033[2mClose app: Ctrl + C\033[0m\n" << endl;
    Student student;
    char birthFullDate[ML_STR] = "";

    cout << "\033[1mFirst Name:\033[0m ";
    UserInput(student.firstName);
    
    cout << "\033[1mSecond Name:\033[0m ";
    UserInput(student.secondName);
    
    cout << "\033[1mThird Name:\033[0m ";
    UserInput(student.thirdname);
    
    cout << "\033[1mGroup Name:\033[0m ";
    UserInput(student.group);
    
    cout << "\033[1mJoin Year:\033[0m ";
    cin >> student.joinYear;

    cout << "\033[1mCourse:\033[0m ";
    cin >> student.course;

    cout << "\033[1mBirth (in format DD.MM.YYYY):\033[0m ";

    bool isCorrectData = false;

    while(!isCorrectData) {
        UserInput(birthFullDate);
        if (strlen(birthFullDate) != 10) {
            cout << "Incorect date format, please, write in DD.MM.YYYY format" << endl;
            continue;
        }
        if((getStudentBDate(birthFullDate) == - 1) || (getStudentBMounth(birthFullDate) == - 1) || (getStudentBYear(birthFullDate) == - 1)) continue;
        student.birthdate = getStudentBDate(birthFullDate);
        student.birthmounth = getStudentBMounth(birthFullDate);
        student.birthyear = getStudentBYear(birthFullDate);
        isCorrectData = true;
    }

    cout << "\033[1mGrades for 4 subjects (separated by a space):\033[0m ";
    char studGR[ML_STR] = "";
    double avgG {0};
    cin.getline(studGR, ML_STR);
    if (strlen(studGR) > 0) {
        char* token = strtok(studGR, " ");
        int count = 0;
        while (token != nullptr && count < 4) {
            int grade = atoi(token);
            if (grade >= 2 && grade <= 5) {
                student.grades[count] = grade;
                avgG += grade;
                ++count;
            } else {
                cout << "Incorect grade. Grade must be from 2 to 5." << endl;
                break;
            }
            token = strtok(nullptr, " ");
        }
        student.avgGrade = avgG / 4;
    }

    cout << "\033[2J\033[1;1H";
    cout << "\nIs data right (\033[1;32my\033[0m/\e[1;31mn\e[0m)?\n" << endl;

    TablePrintHead();
    TablePrintStudent(student.firstName, student.secondName, student.thirdname, student.group, student.joinYear, student.course, student.grades, 1, student.avgGrade, student.birthdate, student.birthmounth, student.birthyear);

    char usAnsw[4] = "";
    bool isReady {false};
    while (!isReady) {
        cin >> usAnsw;
        if (strcmp(usAnsw, "y") == 0 || strcmp(usAnsw, "н") == 0){
            cout << "\033[2J\033[1;1H";
            strcpy(students[studentsNum].firstName, student.firstName);
            strcpy(students[studentsNum].secondName, student.secondName);
            strcpy(students[studentsNum].thirdname, student.thirdname);
            strcpy(students[studentsNum].group, student.group);
            students[studentsNum].joinYear = student.joinYear;
            students[studentsNum].course = student.course;
            students[studentsNum].birthdate = student.birthdate;
            students[studentsNum].birthmounth = student.birthmounth;
            students[studentsNum].birthyear = student.birthyear;
            students[studentsNum].grades[0] = student.grades[0];
            students[studentsNum].grades[1] = student.grades[1];
            students[studentsNum].grades[2] = student.grades[2];
            students[studentsNum].grades[3] = student.grades[3];
            students[studentsNum].avgGrade = student.avgGrade;
            studentsNum +=1 ;
            cout << "\n\033[1;32mUser added\033[0m\n" << endl;
            programmState = 1;
            isReady = true;
        }
        else if (strcmp(usAnsw, "n") == 0 || strcmp(usAnsw, "т") == 0) {
            cout << "\033[2J\033[1;1H";
            cout << "\n\033[1;33mData cleared. Try again\033[0m\n" << endl;
            programmState = 1;
            isReady = true;
        }
    }
    
}
void TablePrintHead() {
    for (int i = 0; i < 197; ++i) {
        cout << "\033[2m-\033[0m";
    }
    cout << endl;
    cout << " No \033[2m|\033[0m First Name";
    for (int i = 0; i < 16; ++i) {
        cout << " ";
    }

    cout << "\033[2m|\033[0m Second Name";
    for (int i = 0; i < 15; ++i) {
        cout << " ";
    }

    cout << "\033[2m|\033[0m Third Name";
    for (int i = 0; i < 16; ++i) {
        cout << " ";
    }
    cout << "\033[2m|\033[0m";
    for (int i = 0; i < 5; ++i) {
        cout << " ";
    }
    cout << "Birth Date";
    for (int i = 0; i < 5; ++i) {
        cout << " ";
    }

    cout << "\033[2m|\033[0m Group Name";
    for (int i = 0; i < 16; ++i) {
        cout << " ";
    }
    cout << "\033[2m|\033[0m";
    for (int i = 0; i < 17; ++i) {
        cout << " ";
    }
    cout << "Join Year ";
    cout << "\033[2m|\033[0m";
    for (int i = 0; i < 5; ++i) {
        cout << " ";
    }
    cout << "Course ";
    cout << "\033[2m|\033[0m Grades for 4 subj";
    for (int i = 0; i < 9; ++i) {
        cout << " ";
    }
    cout << endl;
    for (int i = 0; i < 197; ++i) {
        cout << "\033[2m-\033[0m";
    }
    cout << endl;
}
void TablePrintStudent(char fname[], char sname[], char tname[], char gname[], int jyear, int course, int grades[4], int number, double avg, int birthdate, int birthmounth, int birthyear) {
    if (utf8_length(fname) <= 25 &&  utf8_length(sname) <= 25 && utf8_length(tname) <= 25) {
        TablePrintUserStr(fname, sname, tname, gname, jyear, course, grades, number, avg, birthdate, birthmounth, birthyear);
    }
    else {
        char splitedFname[10][26];
        char splitedSname[10][26];
        char splitedTname[10][26];
        int nPFN = 0;
        int nPSN = 0;
        int nPTN = 0;
        int max;

        splitString(fname, splitedFname, nPFN);
        splitString(sname, splitedSname, nPSN);
        splitString(tname, splitedTname, nPTN);

        if (nPFN >= nPSN && nPFN >= nPTN) {
                max = nPFN;
        } else if (nPSN >= nPFN && nPSN >= nPTN) {
                max = nPSN;
        } else {
                max = nPTN;
        }
        TablePrintUserStr(splitedFname[0], splitedSname[0], splitedTname[0], gname, jyear, course, grades, number, avg, birthdate, birthmounth, birthyear);
        for(int i = 1; i < max; ++i) {
            char rowName[ML_STR] = "";
            if (utf8_length(splitedFname[i-1]) < 25) {
                strcpy(rowName, "");
            }
            else strcpy(rowName, splitedFname[i]);
            char rowSName[ML_STR] = "";
            if (utf8_length(splitedSname[i-1]) < 25) {
                strcpy(rowSName, "");
            }
            else strcpy(rowSName, splitedSname[i]);
            char rowTName[ML_STR] = "";
            if (utf8_length(splitedTname[i-1]) < 25) {
                strcpy(rowTName, "");
            }
            else strcpy(rowTName, splitedTname[i]);
            TablePrintUserStr(rowName, rowSName, rowTName, gname, jyear, course, grades, 0, avg, birthdate, birthmounth, birthyear);
        }
    }
}
void TablePrintUserStr (char fname[], char sname[], char tname[], char gname[], int jyear, int course, int grades[4], int number, double avg, int birthdate, int birthmounth, int birthyear) {
    if(number != 0){
        if (number / 10 == 0) {
            cout << " " << number << "  ";
        }
        else {
            cout << " " << number << " ";
        }
        cout << "\033[2m|\033[0m " << fname;
        for (int i = 0; i < 26-utf8_length(fname); ++i) {
            cout << " ";
        }
        cout << "\033[2m|\033[0m " << sname;
        for (int i = 0; i < 26-utf8_length(sname); ++i) {
            cout << " ";
        }
        cout << "\033[2m|\033[0m " << tname;
        for (int i = 0; i < 26-utf8_length(tname); ++i) {
            cout << " ";
        }
        cout << "\033[2m|\033[0m";
        for (int i = 0; i < 5; ++i) {
            cout << " ";
        }
        dateOutput(birthdate, birthmounth, birthyear);
        for (int i = 0; i < 5; ++i) {
            cout << " ";
        }

        cout << "\033[2m|\033[0m " << gname;
        for (int i = 0; i < 26-utf8_length(gname); ++i) {
            cout << " ";
        }
        cout << "\033[2m|\033[0m";
        for (int i = 0; i < 26-4; ++i) {
            cout << " ";
        }
        cout << jyear << " ";
        cout << "\033[2m|\033[0m";
        for (int i = 0; i < 10; ++i) {
            cout << " ";
        }
        cout << course << " ";
        cout << "\033[2m|\033[0m";
        cout << "      ";
        for (int i = 0; i < 4; ++i) {
            cout << grades[i] << " ";
        }
        cout << endl;
    }
    else { // 20 27 27 12 18
        cout << "    ";
        if (fname == "_1 ") {
            for (int i = 0; i < 26; ++i) {
            cout << " ";
        }
        } else {
            cout << "\033[2m|\033[0m " << fname;
        for (int i = 0; i < 26-utf8_length(fname); ++i) {
            cout << " ";
        }
        }
        cout << "\033[2m|\033[0m " << sname;
        for (int i = 0; i < 26-utf8_length(sname); ++i) {
            cout << " ";
        }
        cout << "\033[2m|\033[0m " << tname;
        for (int i = 0; i < 26-utf8_length(tname); ++i) {
            cout << " ";
        }
        cout << "\033[2m|\033[0m";
        for (int i = 0; i < 20; ++i) {
            cout << " ";
        }
        cout << "\033[2m|\033[0m";
        for (int i = 0; i < 27; ++i) {
            cout << " ";
        }
        cout << "\033[2m|\033[0m";
        for (int i = 0; i < 27; ++i) {
            cout << " ";
        }
        cout << "\033[2m|\033[0m";
        for (int i = 0; i < 12; ++i) {
            cout << " ";
        }
        cout << "\033[2m|\033[0m";
        for (int i = 0; i < 18; ++i) {
            cout << " ";
        }

        cout << endl;
    }
    
}
void BestStudent() {
    int bestStNum {0};
    double bestAvg {0.0};
    int acceptGroups {0};
    int selectedGroup {0};
    Student studentsInGroup[30];
    for (int i = 0; i < 30; ++i){
        if (students[i].avgGrade < bestAvg) continue;
        bestAvg = students[i].avgGrade;
        bestStNum = i;
    }
    if (bestAvg == 0.0) {
        cout << "\033[2J\033[1;1H";
        cout << "\nStudent's table is empty\n" << endl;
        programmState = 1;
        return;
    }
    else {
        cout << "\033[2J\033[1;1H";
        cout << "Best student in table: ";
        cout << "\n" << students[bestStNum].firstName << " " << students[bestStNum].secondName << " | " << bestAvg << "\n" <<  endl;
        cout << "Select group:" << endl;
        CheckGroups();
        for (int i = 0; i < 30; ++i) {
            if(strlen(groups[i]) != 0){
                cout << "\033[1m" << i + 1 << ") " << groups[i] << "\033[0m" << endl;
                acceptGroups += 1;
            } 
        }
        cin >> selectedGroup;

        if(selectedGroup > acceptGroups || selectedGroup <= 0) {
            cout << "Group doesn't exist" << endl;
        }
        else {
            filterStudentsByGroup(students, 30, studentsInGroup, 40, groups[selectedGroup-1]);
            bestAvg = 0.0;
            bestStNum = 0;
            for(int i = 0; i < 30; ++i) {
                if(strlen(studentsInGroup[i].firstName) != 0){
                    if (studentsInGroup[i].avgGrade < bestAvg) continue;
                    bestAvg = studentsInGroup[i].avgGrade;
                    bestStNum = i;
                }
            }
            cout << "\033[2J\033[1;1H";
            cout << "\nBest student in a group " << groups[selectedGroup-1] << ": " << studentsInGroup[bestStNum].firstName << " " << studentsInGroup[bestStNum].secondName << " | " << bestAvg << "\n" << endl; 
        }
        programmState = 1;
    }
}
void BornInMounth() {
    cout << "\033[2J\033[1;1H";

    if (strlen(students[0].firstName) == 0 || strlen(students[0].secondName) == 0 || strlen(students[0].thirdname) == 0  || strlen(students[0].group) == 0){
        cout << "\nStudent's table is empty\n" << endl;
        programmState = 1;
        return;
    }

    PrintMonthSelect();
    char string[ML_STR] = "";
    UserInput(string);
    if (strcmp(string, "January") == 0 or strcmp(string, "1") == 0) {
        MounthSort(1, students, 30);
    }
    else if (strcmp(string, "February") == 0 or strcmp(string, "2") == 0) {
        MounthSort(2, students, 30);
    }
    else if (strcmp(string, "March") == 0 or strcmp(string, "3") == 0) {
        MounthSort(3, students, 30);
    }
    else if (strcmp(string, "April") == 0 or strcmp(string, "4") == 0) {
        MounthSort(4, students, 30);
    }
    else if (strcmp(string, "May") == 0 or strcmp(string, "5") == 0) {
        MounthSort(5, students, 30);
    }
    else if (strcmp(string, "June") == 0 or strcmp(string, "6") == 0) {
        MounthSort(6, students, 30);
    }
    else if (strcmp(string, "July") == 0 or strcmp(string, "7") == 0) {
        MounthSort(7, students, 30);
    }
    else if (strcmp(string, "August") == 0 or strcmp(string, "8") == 0) {
        MounthSort(8, students, 30);
    }
    else if (strcmp(string, "September") == 0 or strcmp(string, "9") == 0) {
        MounthSort(9, students, 30);
    }
    else if (strcmp(string, "October") == 0 or strcmp(string, "10") == 0) {
        MounthSort(10, students, 30);
    }
    else if (strcmp(string, "November") == 0 or strcmp(string, "11") == 0) {
        MounthSort(11, students, 30);
    }
    else if (strcmp(string, "December") == 0 or strcmp(string, "12") == 0) {
        MounthSort(12, students, 30);
    }
    else cout << "Incorect mounth" << endl;
}
void BubbleSortByDate(Student students[], int size) {
    for (int i = 0; i < size - 1; ++i) {
        for (int j = 0; j < size - i - 1; ++j) {
            if (students[j].birthdate > students[j + 1].birthdate) {
                Student temp = students[j];
                students[j] = students[j + 1];
                students[j + 1] = temp;
            }
        }
    }
}
void MounthSort(int month, Student students[], int size) {
    Student filteredStudents[30];
    int filteredSize = 0; 
    for (int i = 0; i < size; ++i) {
        if (students[i].birthmounth == month) {
            filteredStudents[filteredSize++] = students[i];
        }
    }
    BubbleSortByDate(filteredStudents, filteredSize);

    if(strlen(filteredStudents[0].firstName) == 0){
        cout << "\033[2J\033[1;1H";
        cout << "\nStudent's born in this month, dsnt exist\n" << endl;
        programmState = 1;
        return;
    }

    cout << "\033[2J\033[1;1H";
    cout << endl;
    for (int i = 0; i < 95; ++i) {
        cout << "\033[2m-\033[0m";
    }
    cout << endl;
    cout << "\033[2m|\033[0m First name";
    for (int i = 0; i < 16; ++i) {
            cout << " ";
        }
    cout << "\033[2m|\033[0m Second name";
    for (int i = 0; i < 15; ++i) {
            cout << " ";
        }
    cout << "\033[2m|\033[0m Group";
    for (int i = 0; i < 10; ++i) {
            cout << " ";
        }
    cout << "\033[2m|\033[0m     Birth date     \033[2m|\033[0m";
    cout << endl;
    for (int i = 0; i < 95; ++i) {
        cout << "\033[2m-\033[0m";
    }
    cout << endl;

    for (int i = 0; i < filteredSize; ++i) {
        int len_name = 26-utf8_length(filteredStudents[i].firstName);
        int len_sname = 26-utf8_length(filteredStudents[i].secondName);
        int len_gr = 15-utf8_length(filteredStudents[i].group);
        // cout << len_name << " " << len_sname << " " << len_gr << endl;
        if(len_name == 26 || len_sname == 26 || len_gr == 15){
           break;
        }

        cout << "\033[2m|\033[0m " << filteredStudents[i].firstName;
        for (int i = 0; i < len_name; ++i) {
            cout << " ";
        }
        cout << "\033[2m|\033[0m " << filteredStudents[i].secondName;
        for (int i = 0; i < len_sname; ++i) {
            cout << " ";
        }
        cout << "\033[2m|\033[0m " << filteredStudents[i].group;
        for (int i = 0; i < len_gr; ++i) {
            cout << " ";
        }
        cout << "\033[2m|\033[0m     ";
        dateOutput(filteredStudents[i].birthdate, filteredStudents[i].birthmounth, filteredStudents[i].birthyear);
        cout << "     \033[2m|\033[0m" << endl;

    }
    for (int i = 0; i < 95; ++i) {
        cout << "\033[2m-\033[0m";
    }
    cout << "\n" << endl;
    programmState = 1;
}
void EditStudent() {
    cout << "\033[2J\033[1;1H";
    cout << endl;

    int numberUserInp {0};
    int studentIndex {0};
    if (strlen(students[0].firstName) == 0 || strlen(students[0].secondName) == 0 || strlen(students[0].thirdname) == 0  || strlen(students[0].group) == 0){
        cout << "Student's table is empty\n" << endl;
        programmState = 1;
        return;
    }

    TablePrintHead();
    for (int i = 0; i < 30; ++i) {
        if (strlen(students[i].firstName) == 0 || strlen(students[i].secondName) == 0 || strlen(students[i].thirdname) == 0  || strlen(students[i].group) == 0) break;
        TablePrintStudent(students[i].firstName, students[i].secondName, students[i].thirdname, students[i].group, students[i].joinYear, students[i].course, students[i].grades, i+1, students[i].avgGrade, students[i].birthdate, students[i].birthmounth, students[i].birthyear);
    }
    cout << "\nSelect number of student to edit: ";
    cin >> numberUserInp;
    cin.ignore();
    cout << endl;

    if(numberUserInp <= 0 || numberUserInp > 30) {
        cout << "Incorect student number\n" << endl;
        programmState = 1;
        return;
    }

    else studentIndex = numberUserInp - 1;

    if (strlen(students[studentIndex].firstName) == 0) {
        cout << "Student with this number dsnt exist\n" << endl;
        programmState = 1;
        return;
    }

    Student& student = students[studentIndex];
    char temp[ML_STR];
    cout << endl;

    cout << "\033[2J\033[1;1H";
    cout << "\n\033[2mEdit value and press Enter, to save new value (press Enter to skip value)\033[0m\n" << endl;
    cout << "First name: " << endl;
    cout << "Old value: " << student.firstName << endl;
    cout << "New value: ";
    cin.getline(temp, ML_STR);
    if (strlen(temp) > 0) {
        strcpy(student.firstName, temp);
    }

    cout << "\033[2J\033[1;1H";
    cout << "\n\033[2mEdit value and press Enter, to save new value (press Enter to skip value)\033[0m\n" << endl;
    cout << "Second name: " << endl;
    cout << "Old value: " << student.secondName << endl;
    cout << "New value: ";
    cin.getline(temp, ML_STR);
    if (strlen(temp) > 0) {
        strcpy(student.secondName, temp);
    }

    cout << "\033[2J\033[1;1H";
    cout << "\n\033[2mEdit value and press Enter, to save new value (press Enter to skip value)\033[0m\n" << endl;
    cout << "Third name: " << endl;
    cout << "Old value: " << student.thirdname << endl;
    cout << "New value: ";
    cin.getline(temp, ML_STR);
    if (strlen(temp) > 0) {
        strcpy(student.thirdname, temp);
    }

    cout << "\033[2J\033[1;1H";
    cout << "\n\033[2mEdit value and press Enter, to save new value (press Enter to skip value)\033[0m\n" << endl;
    cout << "Birth date: " << endl;
    cout << "Old value: ";
    dateOutput (student.birthdate, student.birthmounth, student.birthyear); 
    cout << endl;
    cout << "New value (DD.MM.YYYY format): ";

    bool isCorrectData = false;
    while(!isCorrectData) {
        cin.getline(temp, ML_STR);
        if (strlen(temp) > 0) {
            if (strlen(temp) != 10) {
                cout << "Incorect date format, please, write in DD.MM.YYYY format" << endl;
                continue;
            }
            if((getStudentBDate(temp) == - 1) || (getStudentBMounth(temp) == - 1) || (getStudentBYear(temp) == - 1)) continue;
            student.birthdate = getStudentBDate(temp);
            student.birthmounth = getStudentBMounth(temp);
            student.birthyear = getStudentBYear(temp);
            isCorrectData = true;
        }
        else isCorrectData = true;
    }

    cout << "\033[2J\033[1;1H";
    cout << "\n\033[2mEdit value and press Enter, to save new value (press Enter to skip value)\033[0m\n" << endl;
    cout << "Group: " << endl;
    cout << "Old value: " << student.group << endl;
    cout << "New value: ";
    cin.getline(temp, ML_STR);
    if (strlen(temp) > 0) {
        strncpy(student.group, temp, 25);
        student.group[25] = '\0';
    }

    cout << "\033[2J\033[1;1H";
    cout << "\n\033[2mEdit value and press Enter, to save new value (press Enter to skip value)\033[0m\n" << endl;
    cout << "Join year: " << endl;
    cout << "Old value: " << student.joinYear << endl;
    cout << "New value: ";
    cin.getline(temp, ML_STR);
    if (strlen(temp) > 0) {
        student.joinYear = atoi(temp);
    }

    cout << "\033[2J\033[1;1H";
    cout << "\n\033[2mEdit value and press Enter, to save new value (press Enter to skip value)\033[0m\n" << endl;
    cout << "Course: " << endl;
    cout << "Old value: " << student.course << endl;
    cout << "New value: ";
    cin.getline(temp, ML_STR);
    if (strlen(temp) > 0) {
        student.course = atoi(temp);
    }

    cout << "\033[2J\033[1;1H";
    cout << "\n\033[2mEdit value and press Enter, to save new value (press Enter to skip value)\033[0m\n" << endl;
    cout << "Grades for 4 subj: " << endl;
    cout << "Old value: ";
    for (int i = 0; i < 4; ++i) {
        cout << student.grades[i] << " ";
    }
    cout << endl;
    cout << "New value (with spase): ";
    cin.getline(temp, ML_STR);
    if (strlen(temp) > 0) {
        char* token = strtok(temp, " ");
        int count = 0;
        while (token != nullptr && count < 4) {
            int grade = atoi(token);
            if (grade >= 2 && grade <= 5) {
                student.grades[count] = grade;
                ++count;
            } else {
                cout << "Incorect grade. Grade must be from 2 to 5." << endl;
                break;
            }
            token = strtok(nullptr, " ");
        }
    }

    int sum = 0;
    for (int i = 0; i < 4; ++i) {
        sum += student.grades[i];
    }
    student.avgGrade = static_cast<double>(sum) / 4;

    cout << "\033[2J\033[1;1H";
    cout << "\n\033[1;32mStudent edited\033[0m\n" << endl;
    programmState = 1;
    return;

}
void DeleteStudent() {
    cout << "\033[2J\033[1;1H";
    cout << endl;

    int numberUserInp {0};
    int studentIndex {0};
    if (strlen(students[0].firstName) == 0 || strlen(students[0].secondName) == 0 || strlen(students[0].thirdname) == 0  || strlen(students[0].group) == 0){
        cout << "Student's table is empty\n" << endl;
        programmState = 1;
        return;
    }

    TablePrintHead();
    for (int i = 0; i < 30; ++i) {
        if (strlen(students[i].firstName) == 0 || strlen(students[i].secondName) == 0 || strlen(students[i].thirdname) == 0  || strlen(students[i].group) == 0) break;
        TablePrintStudent(students[i].firstName, students[i].secondName, students[i].thirdname, students[i].group, students[i].joinYear, students[i].course, students[i].grades, i+1, students[i].avgGrade, students[i].birthdate, students[i].birthmounth, students[i].birthyear);
    }
    cout << "\nSelect number of student to delete: ";
    cin >> numberUserInp;
    cin.ignore();
    cout << endl;

    if(numberUserInp <= 0 || numberUserInp > 30) {
        cout << "Incorect student number\n" << endl;
        programmState = 1;
        return;
    }
    else studentIndex = numberUserInp - 1;

    if (strlen(students[studentIndex].firstName) == 0) {
        cout << "Student with this number dsnt exist\n" << endl;
        programmState = 1;
        return;
    }

    for (int i = studentIndex; i < 29; ++i) {
        students[i] = students[i + 1];
    }

    memset(&students[29], 0, sizeof(Student));
    studentsNum -= 1;

    cout << "\033[2J\033[1;1H";
    cout << "\n\033[1;32mStudent deleted\033[0m\n" << endl;
    programmState = 1;
}
void SaveListToTxt() {
    cout << endl;
    cout << "Saving!" << endl;
    cout << endl;

    ofstream file("studentsList.txt");

    if(!file.is_open()) {
        cout << "\033[2J\033[1;1H";
        cout << endl;
        cout << "\e[1;31mSomethings go wrong(\e[0m" << endl;
        cout << endl;
        programmState = 1;
        return;
    }
    for (int i = 0; i < studentsNum; ++i) {
        file << students[i].firstName << " | "
                << students[i].secondName << " | "
                << students[i].thirdname << " | "
                << students[i].joinYear << " | "
                << students[i].course << " | "
                << students[i].group << " | "
                << students[i].birthdate << " | "
                << students[i].birthmounth << " | "
                << students[i].birthyear << " | ";
        for (int j = 0; j < 4; ++j) {
            file << students[i].grades[j] << " ";
        }
        file << "| ";
        file << students[i].avgGrade << endl;
    }

    file.close();
    cout << "\033[2J\033[1;1H";
    cout << "\n\033[1;32mTXT file with students saved!\033[0m\n" << endl;
    programmState = 1;
    return;
}
void SaveListToBinary () {
    cout << endl;
    cout << "Saving!" << endl;
    cout << endl;

    ofstream file("studentsList.bin", ios::binary);
    if(!file.is_open()) {
        cout << "\033[2J\033[1;1H";
        cout << endl;
        cout << "\e[1;31mSomethings go wrong(\e[0m" << endl;
        cout << endl;
        programmState = 1;
        return;
    }

    file.write(reinterpret_cast<const char*>(&studentsNum), sizeof(studentsNum));

    for (int i = 0; i < studentsNum; ++i) {

        const Student& student = students[i];
        file.write(student.firstName, MBL);
        file.write(student.secondName, MBL);
        file.write(student.thirdname, MBL);
        file.write(student.group, MBL);

        file.write(reinterpret_cast<const char*>(&student.joinYear), sizeof(student.joinYear));
        file.write(reinterpret_cast<const char*>(&student.course), sizeof(student.course));
        file.write(reinterpret_cast<const char*>(&student.birthdate), sizeof(student.birthdate));
        file.write(reinterpret_cast<const char*>(&student.birthmounth), sizeof(student.birthmounth));
        file.write(reinterpret_cast<const char*>(&student.birthyear), sizeof(student.birthyear));

        file.write(reinterpret_cast<const char*>(student.grades), sizeof(student.grades));

        file.write(reinterpret_cast<const char*>(&student.avgGrade), sizeof(student.avgGrade));
    }
    file.close();
    cout << "\033[2J\033[1;1H";
    cout << "\n\033[1;32mBINARY file with students saved!\033[0m\n" << endl;
    programmState = 1;
    return;
}
void LoadStudentsFromFileTXT(const char* filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Unc. err then try to open: " << filename << endl;
        programmState = 1;
        return;
    }

    studentsNum = 0;

    char line[ML_STR];
    while (file.getline(line, ML_STR)) {
        if (studentsNum >= ML) {
            cout << "To many students." << endl;
            break;
        }
        Student& student = students[studentsNum++];
        char* token = strtok(line, "| ");
        if (token) {
            strncpy(student.firstName, token, ML_STR);
            student.firstName[ML_STR - 1] = '\0';
        }

        token = strtok(nullptr, "| ");
        if (token) {
            strncpy(student.secondName, token, ML_STR);
            student.secondName[ML_STR - 1] = '\0';
        }

        token = strtok(nullptr, "| ");
        if (token) {
            strncpy(student.thirdname, token, ML_STR);
            student.thirdname[ML_STR - 1] = '\0';
        }

        token = strtok(nullptr, "| ");
        if (token) student.joinYear = atoi(token);

        token = strtok(nullptr, "| ");
        if (token) student.course = atoi(token);

        token = strtok(nullptr, "| ");
        if (token) {
            strncpy(student.group, token, ML_STR);
            student.group[ML_STR - 1] = '\0';
        }

        token = strtok(nullptr, "| ");
        if (token) student.birthdate = atoi(token);

        token = strtok(nullptr, "| ");
        if (token) student.birthmounth = atoi(token);

        token = strtok(nullptr, "| ");
        if (token) student.birthyear = atoi(token);

        token = strtok(nullptr, "|");
        if (token) {
            char* gradeToken = strtok(token, " ");
            for (int i = 0; i < 4 && gradeToken; ++i) {
                student.grades[i] = atoi(gradeToken);
                gradeToken = strtok(nullptr, " ");
            }
        }
        token = strtok(nullptr, "| ");
        if (token) student.avgGrade = atof(token);
    }

    file.close();
}
void ListTxtFilesAndLoad() {
    fs::path currentPath = fs::current_path();
    int fileIndex = 1;
    char filenames[ML][ML_STR]; 
    int fileCount = 0;

    cout << "\033[2J\033[1;1H";
    cout << "\n\033[1;36mTXT files in dir:\033[0m\n" << endl;
    for (const auto& entry : fs::directory_iterator(currentPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            strncpy(filenames[fileCount], entry.path().filename().string().c_str(), ML_STR);
            filenames[fileCount][ML_STR - 1] = '\0';
            fileCount++;
            cout << "\033[1m" << fileIndex << ") " << entry.path().filename().string() << "\033[0m" << endl;
            fileIndex++;
        }
    }

    if (fileCount == 0) {
        cout << "\033[2J\033[1;1H";
        cout << "\nFiles .txt doesn't find.\n" << endl;
        programmState = 1;
        return;
    }
    cout << endl;
    int selectedIndex;
    cout << "Input number of file to load: ";
    cin >> selectedIndex;

    if (selectedIndex < 1 || selectedIndex > fileCount) {
        cout << "\033[2J\033[1;1H";
        cout << "\nIncorect number of file.\n" << endl;
        programmState = 1;
        return;
    }
    LoadStudentsFromFileTXT(filenames[selectedIndex - 1]);
    cout << "\033[2J\033[1;1H";
    cout << "\n\033[1;32mTXT file with students loaded!\033[0m\n" << endl;
    programmState = 1;
}
void LoadStudentsFromBinaryFile(const char* filename) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        cout << "Unc. err then try to open: " << filename << endl;
        return;
    }
    studentsNum = 0;

    file.read(reinterpret_cast<char*>(&studentsNum), sizeof(studentsNum));

    for (int i = 0; i < studentsNum; ++i) {
        Student& student = students[i];

        file.read(student.firstName, MBL);
        file.read(student.secondName, MBL);
        file.read(student.thirdname, MBL);
        file.read(student.group, MBL);

        file.read(reinterpret_cast<char*>(&student.joinYear), sizeof(student.joinYear));
        file.read(reinterpret_cast<char*>(&student.course), sizeof(student.course));
        file.read(reinterpret_cast<char*>(&student.birthdate), sizeof(student.birthdate));
        file.read(reinterpret_cast<char*>(&student.birthmounth), sizeof(student.birthmounth));
        file.read(reinterpret_cast<char*>(&student.birthyear), sizeof(student.birthyear));

        file.read(reinterpret_cast<char*>(student.grades), sizeof(student.grades));

        file.read(reinterpret_cast<char*>(&student.avgGrade), sizeof(student.avgGrade));
    }

    file.close();
}
void ListBinFilesAndLoad() {
    fs::path currentPath = fs::current_path();
    int fileIndex = 1;
    char filenames[ML][ML_STR];
    int fileCount = 0;
    cout << "\033[2J\033[1;1H";
    cout << "\n\033[1;36mBINARY files in dir:\033[0m\n" << endl;
    for (const auto& entry : fs::directory_iterator(currentPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".bin") {
            strncpy(filenames[fileCount], entry.path().filename().string().c_str(), ML_STR);
            filenames[fileCount][ML_STR - 1] = '\0';
            fileCount++;
            cout << "\033[1m" << fileIndex << ") " << entry.path().filename().string() << "\033[0m" << endl;
            fileIndex++;
        }
    }

    if (fileCount == 0) {
        cout << "\033[2J\033[1;1H";
        cout << "\nFiles .bin doesn't find.\n" << endl;
        programmState = 1;
        return;
    }

    cout << endl;
    int selectedIndex;
    cout << "Input number of file to load: ";
    cin >> selectedIndex;

    if (selectedIndex < 1 || selectedIndex > fileCount) {
        cout << "\033[2J\033[1;1H";
        cout << "\nIncorect number of file.\n" << endl;
        programmState = 1;
        return;
    }

    LoadStudentsFromBinaryFile(filenames[selectedIndex - 1]);
    cout << "\033[2J\033[1;1H";
    cout << "\n\033[1;32mBINARY file with students loaded!\033[0m\n" << endl;
    programmState = 1;
}

// END FUNCTIONS

//--------------------------------------------------------------------------------------//

// PROGRAMM STRUCTURE:

/* Util Functions :
  - utf8_length - Type: size_t | Task: get string len | Input args: 1) const char* str
  --------------------------------------------------------------------------------------------
  - getStudentBDate - Type: int | Task: get student birth day | Input args: 
  1) char fullBirth[]
  --------------------------------------------------------------------------------------------
  - getStudentBMounth - Type: int | Task: get student month day | Input args: 
  1) char fullBirth[]
  --------------------------------------------------------------------------------------------
  - getStudentBYear - Type: int | Task: get student year day | Input args: 1) char fullBirth[]
  --------------------------------------------------------------------------------------------
  - UserInput - Type: void | Task: get string inputed by user | Input args: 1) char string[]
  --------------------------------------------------------------------------------------------
  - CleanCons - Type: void | Task: clear console | Input args: none
  --------------------------------------------------------------------------------------------
  - dateOutput - Type: void | Task: Output string with date | Input args: 
  1) int bDay, 2) int bMounth, 3) int bYear
  --------------------------------------------------------------------------------------------
  - splitString - Type: void | Task: Split oversized strings | Input args: 
  1) const char* input, 2) char result[][26], 3) int& numParts
  --------------------------------------------------------------------------------------------
  - copyStudent - Type: void | Task: Copy student entity | Input args: 
  1) Student& dest, 2) const Student& src
  --------------------------------------------------------------------------------------------
  - CheckGroups - Type: void | Task: Get groups name | Input args: none
  --------------------------------------------------------------------------------------------
  - filterStudentsByGroup - Type: void | Task: Sort students by groups | Input args: 
  1) const Student students[], 2) int studentsCount, 3) Student filteredStudents[], 
  4) int maxFilteredCount, 5) const char* targetGroup
*/

/* Functions:
    - GetSortMethod - Type: int | Task: Define sorting method | Input args: 1) int sortType
    --------------------------------------------------------------------------------------------
    - CloseProgramm - Type: void | Task: Close programm | Input args: none
    --------------------------------------------------------------------------------------------
    - PrintMenu - Type: void | Task: print beautiful main menu | Input args: none.
    --------------------------------------------------------------------------------------------
    - PrintSortMenu - Type: void | Task: print beautiful sort menu | Input args: none.
    --------------------------------------------------------------------------------------------
    - PrintMonthSelect - Type: void | Task: print month selecter menu | Input args: none.
    --------------------------------------------------------------------------------------------
    - MenuWork - Type: void | Task: user commands main menu processing | Input args: none
    --------------------------------------------------------------------------------------------
    - ShowAllUsers - Type: void | Task: Output table with all students | Input args: none
    --------------------------------------------------------------------------------------------
    - SortMenuWork - Type: void | Task: user commands sort menu processing | Input args: none
    --------------------------------------------------------------------------------------------
    - SortStudents - Type: void | Task: user commands sort menu processing |  
    Input args: 1) Student* students, 2) int n, 3) int sortType, 4) int sortMethod
    --------------------------------------------------------------------------------------------
    - CreateUser - Type: void | Task: create new student entity | Input args: none
    --------------------------------------------------------------------------------------------
    - TablePrintHead - Type: void | Task: print table head | Input args: none
    --------------------------------------------------------------------------------------------
    - TablePrintStudent - Type: void | Task: print table with students | 
    Input args: 1) char fname[], 2) char sname[], 3) char tname[], 4) char gname[], 
    5) int jyear, 6) int course, 7) int grades[4], 8) int number, 9) double avg, 
    10) int birthdate, 11) int birthmounth, 12) int birthyear
    --------------------------------------------------------------------------------------------
    - TablePrintUserStr - Type: void | Task: print table string with student | 
    Input args: 1) char fname[], 2) char sname[], 3) char tname[], 4) char gname[], 
    5) int jyear, 6) int course, 7) int grades[4], 8) int number, 9) double avg, 
    10) int birthdate, 11) int birthmounth, 12) int birthyear
    --------------------------------------------------------------------------------------------
    - BestStudent - Type: void | Task: Output best student | Input args: none
    --------------------------------------------------------------------------------------------
    - BornInMounth - Type: void | Task: Output students born in select month | Input args: none
    --------------------------------------------------------------------------------------------
    - BubbleSortByDate - Type: void | Task: Sort students by BDay | Input args: 
    1) Student students[], 2) int size
    --------------------------------------------------------------------------------------------
    - MounthSort - Type: void | Task: Sort students by Month | Input args: 
    1) int month, 2) Student students[], 3) int size
    --------------------------------------------------------------------------------------------
    - EditStudent - Type: void | Task: Edit student entity | Input args: none
    --------------------------------------------------------------------------------------------
    - DeleteStudent - Type: void | Task: Delete student entity | Input args: none
    --------------------------------------------------------------------------------------------
    - SaveListToTxt - Type: void | Task: Save students list to txt file | Input args: none
    --------------------------------------------------------------------------------------------
    - SaveListToBinary - Type: void | Task: Save students list to bin file | Input args: none
    --------------------------------------------------------------------------------------------
    - LoadStudentsFromFileTXT - Type: void | Task: Load students list from txt file | 
    Input args: 1) const char* filename
    --------------------------------------------------------------------------------------------
    - ListTxtFilesAndLoad - Type: void | Task: Select txt file to load | Input args: none
    --------------------------------------------------------------------------------------------
    - LoadStudentsFromBinaryFile - Type: void | Task: Load students list from bin file | 
    Input args: 1) const char* filename
    --------------------------------------------------------------------------------------------
    - ListBinFilesAndLoad - Type: void | Task: Select bin file to load | Input args: none
*/