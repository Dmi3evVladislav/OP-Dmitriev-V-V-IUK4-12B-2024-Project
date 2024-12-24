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
#include <locale>

using namespace std;

#ifdef __linux__
    #include <sys/stat.h>
    #include <unistd.h>
    #include <dirent.h>
#elif defined(_WIN32)
    #include <windows.h>
    #include <filesystem>
    namespace fs = filesystem;
#endif

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
        cout << "Неправильная дата (меньше 1 или больше 31)" << endl;
        return -1;
    }
    return day;
}
int getStudentBMounth(char fullBirth[]) {
    char bMounth[3] = "";
    strncpy(bMounth, fullBirth + 3, 2);
    int mounth = atoi(bMounth);
    if (mounth < 1 || mounth > 12){
        cout << "Неправильный месяц (меньше 1 или больше 12)" << endl;
        return -1;
    }
    return mounth;
}
int getStudentBYear(char fullBirth[]) {
    char bYear[5] = "";
    strncpy(bYear, fullBirth + 6, 4);
    int year = atoi(bYear);
    if (year > 2006) {
        cout << "Неправильный год, студент слишком молод чтобы поступить в КФ МГТУ" << endl;
        return -1;
    }
    else if (year < 1959) {
        cout << "Неправильный год, КФ МГТУ еще не основали" << endl;
        return -1;
    }
    return year;
}
void UserInput(char string[]) {
    bool empti {true};
    char input[ML_STR] = "";
    while (empti) {
        cin.getline(input, ML_STR);
        if (utf8_length(input) == 0){
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
void splitString(const char* input, char result[][256], int& numParts) {
    int inputLength = utf8_length(input); // Длина строки в символах
    numParts = (inputLength + 24) / 25;   // Количество частей

    int byteIndex = 0; // Текущий байтовый индекс в строке
    for (int i = 0; i < numParts; ++i) {
        int startIndex = byteIndex; // Начальный байтовый индекс текущей части
        int charCount = 0;          // Количество символов в текущей части
        int endIndex = startIndex;  // Конечный байтовый индекс текущей части

        // Определяем границы текущей части
        while (charCount < 25 && input[endIndex] != '\0') {
            if ((input[endIndex] & 0x80) == 0) { // 1-байтовый символ
                endIndex += 1;
            } else if ((input[endIndex] & 0xE0) == 0xC0) { // 2-байтовый символ
                endIndex += 2;
            } else if ((input[endIndex] & 0xF0) == 0xE0) { // 3-байтовый символ
                endIndex += 3;
            } else if ((input[endIndex] & 0xF8) == 0xF0) { // 4-байтовый символ
                endIndex += 4;
            } else {
                // Некорректный байт UTF-8, пропускаем его
                endIndex += 1;
            }
            charCount++;
        }

        // Копируем символы в результат
        
        strncpy(result[i], input + startIndex, endIndex - startIndex);
        result[i][endIndex - startIndex] = '\0'; // Добавляем завершающий нулевой символ

        byteIndex = endIndex; // Переходим к следующей части
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
void ConvertFiles();

//--------------------------------------------------------------------------------------//

// START MAIN BLOCK
int main() {

    #ifdef __linux__
        setlocale(LC_ALL, "ru_RU.UTF-8");
    #elif defined(_WIN32)
        SetConsoleCP(1251);
        SetConsoleOutputCP(1251);
        setlocale(LC_ALL, "rus");
    #endif

    CleanCons();
    cout << "\n\033[1;36mЗдравствуйте!\033[0m\n" << endl;
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
        case 13:
            ConvertFiles();
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
        cout << "\033[1mСортировать откуда? ('-t' - с начала, '-b' - с конца):\033[0m " ;
        break;
    case 12: 
        cout << "\033[1mСортировать откуда? ('-t' - от нового года к старому, '-b' - от старого года к новому):\033[0m " ;
        break;
    case 13:
        cout << "\033[1mСортировать откуда? ('-t' - от лучшего к худшему, '-b' - от худшего к лучшему):\033[0m " ;
        break;
    case 14:
        cout << "\033[1mСортировать откуда? ('-t' - от молодого к взрослому, '-b' - от зрослого к молодому):\033[0m " ;
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
    cout << "\n\033[1;36mДосвидания <3\033[0m\n" << endl;
    render = false;
}
void PrintMenu () {
    cout << "\e[1;31mМеню: \e[0m" << endl;
    cout << "\033[1m0) /showall\033[0m - Показать всех студентов" << endl;
    cout << "\033[1m1) /add\033[0m - Добавить нового студента" << endl;
    cout << "\033[1m2) /edit\033[0m - Редактировать запись студента" << endl;
    cout << "\033[1m3) /delete\033[0m - Удалить запись студента" << endl;
    cout << "\033[1m4) /best\033[0m - Показать лучшего студента" << endl;
    cout << "\033[1m5) /binm\033[0m - Показать студента родившегося в месяце" << endl;
    cout << "\033[1m6) /exit\033[0m - Выйти из программы" << endl;
    cout << endl;
    cout << "\033[1m18) /lbind\033[0m - \033[1;36mЗагрузить\033[0m бинарный лист студентов" << endl;
    cout << "\033[1m19) /ltxtd\033[0m - \033[1;36mЗагрузить\033[0m текстовой лист студентов" << endl;
    cout << "\033[1m20) /convert\033[0m - \033[1;36mКонвертировать\033[0m файл" << endl;
}
void PrintSortMenu () {
    cout << "\n\033[1;36mМеню сортировки:\033[0m\n" << endl;
    cout << "\033[1m7) /cltab\033[0m - Закрыть таблицу студентов" << endl;
    cout << "\033[1m8) /sbfn\033[0m - Сортировать по фамилии" << endl;
    cout << "\033[1m9) /sbsn\033[0m - Сортировать по имени" << endl;
    cout << "\033[1m10) /sbtn\033[0m - Сортировать по отчеству" << endl;
    cout << "\033[1m11) /sbgn\033[0m - Сортировать по группе" << endl;
    cout << "\033[1m12) /sbjy\033[0m - Сортировать по году поступления" << endl;
    cout << "\033[1m13) /sbgfs\033[0m - Сортировать по среднему баллу" << endl;
    cout << "\033[1m14) /sbbd\033[0m - Сортировать по дате рождения" << endl;
    cout << "\033[1m15) /sbc\033[0m - Сортировать по курсу" << endl;
    cout << endl;
    cout << "\033[1m16) /stbin\033[0m - \033[1;36mСохранить\033[0m лист в бинарный файл" << endl;
    cout << "\033[1m17) /sttxt\033[0m - \033[1;36mСохранить\033[0m лист в текстовый файл" << endl;
}
void PrintMonthSelect () {
    cout << "\e[1;31mВыберите месяц: \e[0m" << endl;
    cout << "\033[1m1) Январь\033[0m" << endl;
    cout << "\033[1m2) Февраль\033[0m" << endl;
    cout << "\033[1m3) Март\033[0m" << endl;
    cout << "\033[1m4) Апрель\033[0m" << endl;
    cout << "\033[1m5) Май\033[0m" << endl;
    cout << "\033[1m6) Июнь\033[0m" << endl;
    cout << "\033[1m7) Июль\033[0m" << endl;
    cout << "\033[1m8) Август\033[0m" << endl;
    cout << "\033[1m9) Сентябрь\033[0m" << endl;
    cout << "\033[1m10) Октябрь\033[0m" << endl;
    cout << "\033[1m11) Ноябрь\033[0m" << endl;
    cout << "\033[1m12) Декабрь\033[0m" << endl;
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
    else if (strcmp(string, "/convert") == 0 or strcmp(string, "20") == 0) programmState = 13;
    else {
        cout << "Комманда не найдена" << endl;
    }
    
}
void ShowAllUsers() {
    CleanCons();
    if (strlen(students[0].firstName) == 0 || strlen(students[0].secondName) == 0 || strlen(students[0].thirdname) == 0  || strlen(students[0].group) == 0){
        cout << "Таблица студентов пуста\n" << endl;
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
        cout << "Комманда не найдена" << endl;
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
    cout << "\n\033[2mЗакрыть приложение: Ctrl + C\033[0m\n" << endl;
    Student student;
    char birthFullDate[ML_STR] = "";

    cout << "\033[1mФамилия:\033[0m ";
    UserInput(student.firstName);
    
    cout << "\033[1mИмя:\033[0m ";
    UserInput(student.secondName);
    
    cout << "\033[1mОтчество:\033[0m ";
    UserInput(student.thirdname);
    
    cout << "\033[1mГруппа:\033[0m ";
    UserInput(student.group);
    
    cout << "\033[1mГод поступления:\033[0m ";
    bool isCorrectData = false;
    while(!isCorrectData){
        int joinyear = 0;
        cin >> joinyear;
        if (cin.fail()) {
        cout << "Ошибка: вы ввели не целое число!" << endl;
        cin.clear();
        cin.ignore(1000, '\n');
        }
        if(joinyear < 1959){
            cout << "Некоректная дата поступения" << endl;
            continue;
        }
        else if(joinyear > 2024){
            cout << "Некоректная дата поступения" << endl;
            continue;
        }
        else {
            student.joinYear = joinyear;
            isCorrectData = true;
        }
    }

    cout << "\033[1mКурс:\033[0m ";
    isCorrectData = false;
    while(!isCorrectData){
        int course;
        cin >> course;
        if (cin.fail()) {
        cout << "Ошибка: вы ввели не целое число!" << endl;
        cin.clear();
        cin.ignore(1000, '\n');
        }
        if(course < 1 || course > 6){
            cout << "Неправильный курс" << endl;
            continue;
        }
        else {
            student.course = course;
            isCorrectData = true;
        }
    }

    cout << "\033[1mДата рождения (в формате ДД.ММ.ГГГГ):\033[0m ";

    isCorrectData = false;

    while(!isCorrectData) {
        UserInput(birthFullDate);
        if (strlen(birthFullDate) != 10) {
            cout << "Неправильный формат даты, пожалуйста, пишите в формате ДД.ММ.ГГГГ" << endl;
            continue;
        }
        if((getStudentBDate(birthFullDate) == - 1) || (getStudentBMounth(birthFullDate) == - 1) || (getStudentBYear(birthFullDate) == - 1)) continue;
        student.birthdate = getStudentBDate(birthFullDate);
        student.birthmounth = getStudentBMounth(birthFullDate);
        student.birthyear = getStudentBYear(birthFullDate);
        isCorrectData = true;
    }

    cout << "\033[1mОценки за 4 предмета (разделяя пробелами):\033[0m ";
    char studGR[ML_STR] = "";
    double avgG = 0;
    isCorrectData = false;

    while (!isCorrectData) {
        cin.getline(studGR, ML_STR);
        if (strlen(studGR) > 0) {
            char* token = strtok(studGR, " ");
            int count = 0;
            bool isBreak = false;
            avgG = 0; // Сбрасываем сумму оценок перед каждым новым вводом

            while (token != nullptr && count < 4) {
                int grade = atoi(token);
                if (grade < 2 || grade > 5) {
                    cout << "Неправильная оценка, оценка должна быть от 2 до 5. Попробуйте снова." << endl;
                    isBreak = true;
                    break; // Прерываем текущий цикл, если оценка некорректна
                }
                student.grades[count] = grade;
                avgG += grade;
                ++count;
                token = strtok(nullptr, " ");
            }

            if (isBreak) {
                continue; // Если есть ошибка, начинаем ввод заново
            }

            if (count < 4) {
                cout << "Введено недостаточно оценок. Пожалуйста, введите 4 оценки." << endl;
                continue; // Если введено меньше 4 оценок, начинаем ввод заново
            }

            isCorrectData = true; // Если все оценки корректны и их 4, завершаем цикл
            student.avgGrade = avgG / 4;
        } else {
            cout << "Вы не ввели данные. Попробуйте снова." << endl;
        }
    }

    cout << "\033[2J\033[1;1H";
    cout << "\nВсе введено правильно (\033[1;32my\033[0m/\e[1;31mn\e[0m)?\n" << endl;

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
            cout << "\n\033[1;32mСтудент добавлен\033[0m\n" << endl;
            programmState = 1;
            isReady = true;
        }
        else if (strcmp(usAnsw, "n") == 0 || strcmp(usAnsw, "т") == 0) {
            cout << "\033[2J\033[1;1H";
            cout << "\n\033[1;33mДанные очищены. Попробуйте снова\033[0m\n" << endl;
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
    cout << " No \033[2m|\033[0m Фамилия";  //26
    for (int i = 0; i < 19; ++i) {
        cout << " ";
    }

    cout << "\033[2m|\033[0m Имя";
    for (int i = 0; i < 23; ++i) {
        cout << " ";
    }

    cout << "\033[2m|\033[0m Отчество";
    for (int i = 0; i < 18; ++i) {
        cout << " ";
    }
    cout << "\033[2m|\033[0m";
    for (int i = 0; i < 4; ++i) {
        cout << " ";
    }
    cout << "Дата рожден."; //20
    for (int i = 0; i < 4; ++i) {
        cout << " ";
    }

    cout << "\033[2m|\033[0m Группа";
    for (int i = 0; i < 20; ++i) {
        cout << " ";
    }
    cout << "\033[2m|\033[0m";
    for (int i = 0; i < 12; ++i) {
        cout << " ";
    }
    cout << "Год поступлен. ";
    cout << "\033[2m|\033[0m";
    for (int i = 0; i < 7; ++i) {
        cout << " ";
    }
    cout << "Курс ";
    cout << "\033[2m|\033[0m Оценка за 4 предм";
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
        char splitedFname[10][256];
        char splitedSname[10][256];
        char splitedTname[10][256];
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
        cout << "\nТаблица студентов пуста\n" << endl;
        programmState = 1;
        return;
    }
    else {
        cout << "\033[2J\033[1;1H";
        cout << "Лучший студент: ";
        cout << "\n" << students[bestStNum].firstName << " " << students[bestStNum].secondName << " | " << bestAvg << "\n" <<  endl;
        cout << "Выберете группу:" << endl;
        CheckGroups();
        for (int i = 0; i < 30; ++i) {
            if(strlen(groups[i]) != 0){
                cout << "\033[1m" << i + 1 << ") " << groups[i] << "\033[0m" << endl;
                acceptGroups += 1;
            } 
        }
        cin >> selectedGroup;

        if(selectedGroup > acceptGroups || selectedGroup <= 0) {
            cout << "Группы не существует" << endl;
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
            cout << "\nЛучший студент в группе " << groups[selectedGroup-1] << ": " << studentsInGroup[bestStNum].firstName << " " << studentsInGroup[bestStNum].secondName << " | " << bestAvg << "\n" << endl; 
        }
        programmState = 1;
    }
}
void BornInMounth() {
    cout << "\033[2J\033[1;1H";

    if (strlen(students[0].firstName) == 0 || strlen(students[0].secondName) == 0 || strlen(students[0].thirdname) == 0  || strlen(students[0].group) == 0){
        cout << "\nТаблица студентов пуста\n" << endl;
        programmState = 1;
        return;
    }

    PrintMonthSelect();
    char string[ML_STR] = "";
    UserInput(string);
    if (strcmp(string, "Январь") == 0 or strcmp(string, "1") == 0) {
        MounthSort(1, students, 30);
    }
    else if (strcmp(string, "Февраль") == 0 or strcmp(string, "2") == 0) {
        MounthSort(2, students, 30);
    }
    else if (strcmp(string, "Март") == 0 or strcmp(string, "3") == 0) {
        MounthSort(3, students, 30);
    }
    else if (strcmp(string, "Апрель") == 0 or strcmp(string, "4") == 0) {
        MounthSort(4, students, 30);
    }
    else if (strcmp(string, "Май") == 0 or strcmp(string, "5") == 0) {
        MounthSort(5, students, 30);
    }
    else if (strcmp(string, "Июнь") == 0 or strcmp(string, "6") == 0) {
        MounthSort(6, students, 30);
    }
    else if (strcmp(string, "Июль") == 0 or strcmp(string, "7") == 0) {
        MounthSort(7, students, 30);
    }
    else if (strcmp(string, "Август") == 0 or strcmp(string, "8") == 0) {
        MounthSort(8, students, 30);
    }
    else if (strcmp(string, "Сентябрь") == 0 or strcmp(string, "9") == 0) {
        MounthSort(9, students, 30);
    }
    else if (strcmp(string, "Октябрь") == 0 or strcmp(string, "10") == 0) {
        MounthSort(10, students, 30);
    }
    else if (strcmp(string, "Ноябрь") == 0 or strcmp(string, "11") == 0) {
        MounthSort(11, students, 30);
    }
    else if (strcmp(string, "Декабрь") == 0 or strcmp(string, "12") == 0) {
        MounthSort(12, students, 30);
    }
    else cout << "Некоректный месяц" << endl;
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
        cout << "\nСтуденты, рожденные в этот месяц, отсутствуют\n" << endl;
        programmState = 1;
        return;
    }

    cout << "\033[2J\033[1;1H";

    for (int i = 0; i < filteredSize; ++i) {
        int len_name = 26-utf8_length(filteredStudents[i].firstName);
        int len_sname = 26-utf8_length(filteredStudents[i].secondName);
        int len_gr = 15-utf8_length(filteredStudents[i].group);
        cout << "Фамилия: " << filteredStudents[i].firstName << " Имя: " << filteredStudents[i].secondName << " Группа: " << filteredStudents[i].group << " Родился: ";
        dateOutput(filteredStudents[i].birthdate, filteredStudents[i].birthmounth, filteredStudents[i].birthyear);
        cout << endl;
        

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
        cout << "Таблица студентов пуста\n" << endl;
        programmState = 1;
        return;
    }

    TablePrintHead();
    for (int i = 0; i < 30; ++i) {
        if (strlen(students[i].firstName) == 0 || strlen(students[i].secondName) == 0 || strlen(students[i].thirdname) == 0  || strlen(students[i].group) == 0) break;
        TablePrintStudent(students[i].firstName, students[i].secondName, students[i].thirdname, students[i].group, students[i].joinYear, students[i].course, students[i].grades, i+1, students[i].avgGrade, students[i].birthdate, students[i].birthmounth, students[i].birthyear);
    }
    cout << "\nВыберете номер студента для редактирования: ";
    cin >> numberUserInp;
    cin.ignore();
    cout << endl;

    if(numberUserInp <= 0 || numberUserInp > 30) {
        cout << "Некоректный номер студента\n" << endl;
        programmState = 1;
        return;
    }

    else studentIndex = numberUserInp - 1;

    if (strlen(students[studentIndex].firstName) == 0) {
        cout << "Студент с таким номером не существует\n" << endl;
        programmState = 1;
        return;
    }

    Student& student = students[studentIndex];
    char temp[ML_STR];
    cout << endl;

    cout << "\033[2J\033[1;1H";
    cout << "\n\033[2mИзмените значение и нажмите Enter, чтобы сохранить значение (нажмите Enter чтобы пропустить)\033[0m\n" << endl;
    cout << "Фамилия: " << endl;
    cout << "Старое значение: " << student.firstName << endl;
    cout << "Новое значение: ";
    cin.getline(temp, ML_STR);
    if (strlen(temp) > 0) {
        strcpy(student.firstName, temp);
    }

    cout << "\033[2J\033[1;1H";
    cout << "\n\033[2mИзмените значение и нажмите Enter, чтобы сохранить значение (нажмите Enter чтобы пропустить)\033[0m\n" << endl;
    cout << "Имя: " << endl;
    cout << "Старое значение: " << student.secondName << endl;
    cout << "Новое значение: ";
    cin.getline(temp, ML_STR);
    if (strlen(temp) > 0) {
        strcpy(student.secondName, temp);
    }

    cout << "\033[2J\033[1;1H";
    cout << "\n\033[2mИзмените значение и нажмите Enter, чтобы сохранить значение (нажмите Enter чтобы пропустить)\033[0m\n" << endl;
    cout << "Отчество: " << endl;
    cout << "Старое значение: " << student.thirdname << endl;
    cout << "Новое значение: ";
    cin.getline(temp, ML_STR);
    if (strlen(temp) > 0) {
        strcpy(student.thirdname, temp);
    }

    cout << "\033[2J\033[1;1H";
    cout << "\n\033[2mИзмените значение и нажмите Enter, чтобы сохранить значение (нажмите Enter чтобы пропустить)\033[0m\n" << endl;
    cout << "Дата рождения: " << endl;
    cout << "Старое значение: ";
    dateOutput (student.birthdate, student.birthmounth, student.birthyear); 
    cout << endl;
    cout << "Новое значение (в формате ДД.ММ.ГГГГ): ";

    bool isCorrectData = false;
    while(!isCorrectData) {
        cin.getline(temp, ML_STR);
        if (cin.fail()) {
        cout << "Ошибка: вы ввели не целое число!" << endl;
        cin.clear();
        cin.ignore(1000, '\n');
        }
        if (strlen(temp) > 0) {
            if (strlen(temp) != 10) {
                cout << "Неправильный формат даты, используйте формат ДД.ММ.ГГГГ" << endl;
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
    cout << "\n\033[2mИзмените значение и нажмите Enter, чтобы сохранить значение (нажмите Enter чтобы пропустить)\033[0m\n" << endl;
    cout << "Группа: " << endl;
    cout << "Старое значение: " << student.group << endl;
    cout << "Новое значение: ";
    cin.getline(temp, ML_STR);
    if (strlen(temp) > 0) {
        strncpy(student.group, temp, 25);
        student.group[25] = '\0';
    }

    cout << "\033[2J\033[1;1H";
    cout << "\n\033[2mИзмените значение и нажмите Enter, чтобы сохранить значение (нажмите Enter чтобы пропустить)\033[0m\n" << endl;
    cout << "Год поступления: " << endl;
    cout << "Старое значение: " << student.joinYear << endl;
    cout << "Новое значение: ";
    isCorrectData = false;
    while(!isCorrectData){
        int joinyear;
        cin.getline(temp, ML_STR);
        if (cin.fail()) {
        cout << "Ошибка: вы ввели не целое число!" << endl;
        cin.clear();
        cin.ignore(1000, '\n');
        }
        if (strlen(temp) > 0) {
            joinyear = atoi(temp);
            if(joinyear < 1959){
            cout << "Некоректная дата поступения" << endl;
            continue;
        }
            else if(joinyear > 2024){
                cout << "Некоректная дата поступения" << endl;
                continue;
            }
            else {
                student.joinYear = joinyear;
                isCorrectData = true;
            }
        }
        else {
            isCorrectData = true;
        }
        
    }

    cout << "\033[2J\033[1;1H";
    cout << "\n\033[2mИзмените значение и нажмите Enter, чтобы сохранить значение (нажмите Enter чтобы пропустить)\033[0m\n" << endl;
    cout << "Курс: " << endl;
    cout << "Старое значение: " << student.course << endl;
    cout << "Новое значение: ";
    isCorrectData = false;
    while(!isCorrectData){
        int course;
        cin.getline(temp, ML_STR);
        if (cin.fail()) {
        cout << "Ошибка: вы ввели не целое число!" << endl;
        cin.clear();
        cin.ignore(1000, '\n');
        }
        if (strlen(temp) > 0) {
            course = atoi(temp);
            if(course < 1 || course > 6){
            cout << "Неправильный курс" << endl;
            continue;
        }
        else {
            student.course = course;
            isCorrectData = true;
        }
        }
        else {
            isCorrectData = true;
        }
    }

    cout << "\033[2J\033[1;1H";
    cout << "\n\033[2mИзмените значение и нажмите Enter, чтобы сохранить значение (нажмите Enter чтобы пропустить)\033[0m\n" << endl;
    cout << "Оценки за 4 предмета: " << endl;
    cout << "Старое значение: ";
    for (int i = 0; i < 4; ++i) {
        cout << student.grades[i] << " ";
    }
    cout << endl;
    cout << "Новое значение (через пробелы): ";
    char studGR[ML_STR] = "";
    double avgG = 0;
    isCorrectData = false;

    while (!isCorrectData) {
        cin.getline(studGR, ML_STR);
        if (strlen(studGR) > 0) {
            char* token = strtok(studGR, " ");
            int count = 0;
            bool isBreak = false;
            avgG = 0; // Сбрасываем сумму оценок перед каждым новым вводом

            while (token != nullptr && count < 4) {
                int grade = atoi(token);
                if (grade < 2 || grade > 5) {
                    cout << "Неправильная оценка, оценка должна быть от 2 до 5. Попробуйте снова." << endl;
                    isBreak = true;
                    break; // Прерываем текущий цикл, если оценка некорректна
                }
                student.grades[count] = grade;
                avgG += grade;
                ++count;
                token = strtok(nullptr, " ");
            }

            if (isBreak) {
                continue; // Если есть ошибка, начинаем ввод заново
            }

            if (count < 4) {
                cout << "Введено недостаточно оценок. Пожалуйста, введите 4 оценки." << endl;
                continue; // Если введено меньше 4 оценок, начинаем ввод заново
            }

            isCorrectData = true; // Если все оценки корректны и их 4, завершаем цикл
            student.avgGrade = avgG / 4;
        } else {
            isCorrectData = true;
        }
    }

    cout << "\033[2J\033[1;1H";
    cout << "\n\033[1;32mСтудент изменен\033[0m\n" << endl;
    programmState = 1;
    return;

}
void DeleteStudent() {
    cout << "\033[2J\033[1;1H";
    cout << endl;

    int numberUserInp {0};
    int studentIndex {0};
    if (strlen(students[0].firstName) == 0 || strlen(students[0].secondName) == 0 || strlen(students[0].thirdname) == 0  || strlen(students[0].group) == 0){
        cout << "Таблица студентов пуста\n" << endl;
        programmState = 1;
        return;
    }

    TablePrintHead();
    for (int i = 0; i < 30; ++i) {
        if (strlen(students[i].firstName) == 0 || strlen(students[i].secondName) == 0 || strlen(students[i].thirdname) == 0  || strlen(students[i].group) == 0) break;
        TablePrintStudent(students[i].firstName, students[i].secondName, students[i].thirdname, students[i].group, students[i].joinYear, students[i].course, students[i].grades, i+1, students[i].avgGrade, students[i].birthdate, students[i].birthmounth, students[i].birthyear);
    }
    cout << "\nВыберете номер студента для удаления записи: ";
    cin >> numberUserInp;
    cin.ignore();
    cout << endl;

    if(numberUserInp <= 0 || numberUserInp > 30) {
        cout << "Неправельный номер студента\n" << endl;
        programmState = 1;
        return;
    }
    else studentIndex = numberUserInp - 1;

    if (strlen(students[studentIndex].firstName) == 0) {
        cout << "Студент с таким номером отсутствует\n" << endl;
        programmState = 1;
        return;
    }

    for (int i = studentIndex; i < 29; ++i) {
        students[i] = students[i + 1];
    }

    memset(&students[29], 0, sizeof(Student));
    studentsNum -= 1;

    cout << "\033[2J\033[1;1H";
    cout << "\n\033[1;32mСтудент удален\033[0m\n" << endl;
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
    cout << "\n\033[1;32mTXT файл с студентами сохранен!\033[0m\n" << endl;
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
    cout << "\n\033[1;32mBINARY файл со студентами сохранен!\033[0m\n" << endl;
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
            cout << "Слишком много студентов." << endl;
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
#ifdef __linux__
// Реализация для Linux (без std::filesystem)
void ListTxtFilesAndLoad() {
    char currentPath[ML_STR];
    if (getcwd(currentPath, sizeof(currentPath)) == nullptr) {
        cerr << "Ошибка открытия директории." << endl;
        return;
    }

    int fileIndex = 1;
    char filenames[ML][ML_STR];
    int fileCount = 0;

    cout << "\033[2J\033[1;1H";
    cout << "\n\033[1;36mTXT файлы в директории:\033[0m\n" << endl;

    DIR* dir = opendir(currentPath);
    if (!dir) {
        cerr << "Ошибка при открытии файла: " << currentPath << endl;
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_type == DT_REG) { // Проверяем, является ли это обычным файлом
            string filename = entry->d_name;
            if (filename.size() >= 4 && filename.substr(filename.size() - 4) == ".txt") {
                strncpy(filenames[fileCount], filename.c_str(), ML_STR);
                filenames[fileCount][ML_STR - 1] = '\0'; // Убедимся, что строка завершена
                fileCount++;
                cout << "\033[1m" << fileIndex << ") " << filename << "\033[0m" << endl;
                fileIndex++;
            }
        }
    }

    closedir(dir);

    if (fileCount == 0) {
        cout << "\033[2J\033[1;1H";
        cout << "\nФайлы .txt не найдены.\n" << endl;
        programmState = 1; // Закомментировано, так как неизвестно, что это
        return;
    }

    cout << endl;
    int selectedIndex;
    cout << "Введите номер файла: ";
    cin >> selectedIndex;

    if (selectedIndex < 1 || selectedIndex > fileCount) {
        cout << "\033[2J\033[1;1H";
        cout << "\nНеправильный номер файла.\n" << endl;
        programmState = 1; // Закомментировано, так как неизвестно, что это
        return;
    }

    LoadStudentsFromFileTXT(filenames[selectedIndex - 1]); // Закомментировано, так как неизвестно, что это
    cout << "\033[2J\033[1;1H";
    cout << "\n\033[1;32mTXT файл со студентами загружен!\033[0m\n" << endl;
    programmState = 1; // Закомментировано, так как неизвестно, что это
}
#elif defined(_WIN32)
// Реализация для Windows (с использованием std::filesystem)
void ListTxtFilesAndLoad() {
    namespace fs = std::filesystem;
    fs::path currentPath = fs::current_path();
    int fileIndex = 1;
    char filenames[ML][ML_STR];
    int fileCount = 0;

    cout << "\033[2J\033[1;1H";
    cout << "\n\033[1;36mTXT файлы в дирректории:\033[0m\n" << endl;
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
        cout << "\nФайлы .txt не найдены.\n" << endl;
        programmState = 1; // Закомментировано, так как неизвестно, что это
        return;
    }

    cout << endl;
    int selectedIndex;
    cout << "Input number of file to load: ";
    cin >> selectedIndex;

    if (selectedIndex < 1 || selectedIndex > fileCount) {
        cout << "\033[2J\033[1;1H";
        cout << "\nНекоректный номер файла.\n" << endl;
        programmState = 1; // Закомментировано, так как неизвестно, что это
        return;
    }

    LoadStudentsFromFileTXT(filenames[selectedIndex - 1]); // Закомментировано, так как неизвестно, что это
    cout << "\033[2J\033[1;1H";
    cout << "\n\033[1;32mTXT файл с студентами загружен!\033[0m\n" << endl;
    programmState = 1; // Закомментировано, так как неизвестно, что это
}
#endif
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
#ifdef __linux__
// Реализация для Linux (без std::filesystem)
void ListBinFilesAndLoad() {
    char currentPath[ML_STR];
    if (getcwd(currentPath, sizeof(currentPath)) == nullptr) {
        cerr << "Failed to get current directory." << endl;
        return;
    }

    int fileIndex = 1;
    char filenames[ML][ML_STR];
    int fileCount = 0;

    cout << "\033[2J\033[1;1H";
    cout << "\n\033[1;36mBINARY файлы в дирректории:\033[0m\n" << endl;

    DIR* dir = opendir(currentPath);
    if (!dir) {
        cerr << "Ошибка при открытии директории: " << currentPath << endl;
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_type == DT_REG) { // Проверяем, является ли это обычным файлом
            string filename = entry->d_name;
            if (filename.size() >= 4 && filename.substr(filename.size() - 4) == ".bin") {
                strncpy(filenames[fileCount], filename.c_str(), ML_STR);
                filenames[fileCount][ML_STR - 1] = '\0'; // Убедимся, что строка завершена
                fileCount++;
                cout << "\033[1m" << fileIndex << ") " << filename << "\033[0m" << endl;
                fileIndex++;
            }
        }
    }

    closedir(dir);

    if (fileCount == 0) {
        cout << "\033[2J\033[1;1H";
        cout << "\nФайлы .bin не найдены.\n" << endl;
        programmState = 1; // Закомментировано, так как неизвестно, что это
        return;
    }

    cout << endl;
    int selectedIndex;
    cout << "Введите номер файла: ";
    cin >> selectedIndex;

    if (selectedIndex < 1 || selectedIndex > fileCount) {
        cout << "\033[2J\033[1;1H";
        cout << "\nНеправильный номер файла.\n" << endl;
        programmState = 1; // Закомментировано, так как неизвестно, что это
        return;
    }

    LoadStudentsFromBinaryFile(filenames[selectedIndex - 1]); // Закомментировано, так как неизвестно, что это
    cout << "\033[2J\033[1;1H";
    cout << "\n\033[1;32mBINARY файл со студентами загружен!\033[0m\n" << endl;
    programmState = 1; // Закомментировано, так как неизвестно, что это
}
#elif defined(_WIN32)
void ListBinFilesAndLoad() {
    fs::path currentPath = fs::current_path();
    int fileIndex = 1;
    char filenames[ML][ML_STR];
    int fileCount = 0;
    cout << "\033[2J\033[1;1H";
    cout << "\n\033[1;36mBINARY файлы в дирректории:\033[0m\n" << endl;
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
        cout << "\nФайлы .bin не найдены.\n" << endl;
        programmState = 1;
        return;
    }

    cout << endl;
    int selectedIndex;
    cout << "Ввыедите номер файла: ";
    cin >> selectedIndex;

    if (selectedIndex < 1 || selectedIndex > fileCount) {
        cout << "\033[2J\033[1;1H";
        cout << "\nНекоректный номер файла.\n" << endl;
        programmState = 1;
        return;
    }

    LoadStudentsFromBinaryFile(filenames[selectedIndex - 1]);
    cout << "\033[2J\033[1;1H";
    cout << "\n\033[1;32mBINARY файл со студентами загружен!\033[0m\n" << endl;
    programmState = 1;
}
#endif

void ListFiles(const string& extension, char filenames[ML][ML_STR], int& fileCount) {
#ifdef __linux__
    char currentPath[ML_STR];
    if (getcwd(currentPath, sizeof(currentPath)) == nullptr) {
        cerr << "Ошибка получения текущего репозитория." << endl;
        return;
    }

    DIR* dir = opendir(currentPath);
    if (!dir) {
        cerr << "Ошибка открытия дирректории: " << currentPath << endl;
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_type == DT_REG) { 
            string filename = entry->d_name;
            if (filename.size() >= extension.size() && filename.substr(filename.size() - extension.size()) == extension) {
                strncpy(filenames[fileCount], filename.c_str(), ML_STR);
                filenames[fileCount][ML_STR - 1] = '\0';
                fileCount++;
            }
        }
    }
    closedir(dir);
#elif defined(_WIN32)
    namespace fs = std::filesystem;
    fs::path currentPath = fs::current_path();
    for (const auto& entry : fs::directory_iterator(currentPath)) {
        if (entry.is_regular_file() && entry.path().extension() == extension) {
            strncpy(filenames[fileCount], entry.path().filename().string().c_str(), ML_STR);
            filenames[fileCount][ML_STR - 1] = '\0';
            fileCount++;
        }
    }
#endif
}

// Функция конвертации файлов
void ConvertFiles() {
    cout << "\033[2J\033[1;1H";
    cout << "\n\033[1;36mВыберите тип конвертации:\033[0m\n" << endl;
    cout << "1) TXT в BIN" << endl;
    cout << "2) BIN в TXT" << endl;
    cout << "Введите тип: ";

    int choice;
    cin >> choice;

    if (choice != 1 && choice != 2) {
        cout << "\033[2J\033[1;1H";
        cout << "\n\033[1;31mНеправильный ввод.\033[0m\n" << endl;
        return;
    }

    char filenames[ML][ML_STR];
    int fileCount = 0;
    string extension = (choice == 1) ? ".txt" : ".bin";

    ListFiles(extension, filenames, fileCount);

    if (fileCount == 0) {
        cout << "\033[2J\033[1;1H";
        cout << "\n\033[1;31mNo " << extension << " файл не найден.\033[0m\n" << endl;
        return;
    }

    cout << "\033[2J\033[1;1H";
    cout << "\n\033[1;36mДоступные файлы:\033[0m\n" << endl;
    for (int i = 0; i < fileCount; ++i) {
        cout << i + 1 << ") " << filenames[i] << endl;
    }

    cout << "\nВведите номер файла для конвертации: ";
    int selectedIndex;
    cin >> selectedIndex;

    if (selectedIndex < 1 || selectedIndex > fileCount) {
        cout << "\033[2J\033[1;1H";
        cout << "\n\033[1;31mНеправильный номер файла.\033[0m\n" << endl;
        return;
    }

    string inputFilename = filenames[selectedIndex - 1];
    string outputFilename = inputFilename.substr(0, inputFilename.size() - extension.size()) + ((choice == 1) ? ".bin" : ".txt");

    if (choice == 1) {
        LoadStudentsFromFileTXT(inputFilename.c_str());
        SaveListToBinary();
    } else {
        LoadStudentsFromBinaryFile(inputFilename.c_str());
        SaveListToTxt();
    }

    cout << "\033[2J\033[1;1H";
    cout << "\n\033[1;32mФайл успешно конвертирован в " << outputFilename << "!\033[0m\n" << endl;
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