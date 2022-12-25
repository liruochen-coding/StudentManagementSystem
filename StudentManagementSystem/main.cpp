#include <cstdio>
#include <cstdlib>
#include "sqlite3.h"
#include <iostream>
#include <string>
#include <atlstr.h>

#pragma comment(lib,"sqlite3.lib")

const int FAIL = -1;
const int SUCCESS = 0;

void sql_demo(sqlite3* db);
char* U2G(const char* utf8);
char* G2U(const char* gb2312);
int callback(void* not_used, int argc, char** argv, char** azColName);
void print_table(char** pResult, int nrow, int ncol);
int sql_notable(sqlite3* db,std::string & sql);
int sql_withtable(sqlite3* db,std::string & sql,int print);
int add_student(sqlite3* db);
int modify_student(sqlite3* db);
int add_course(sqlite3* db);
int modify_course(sqlite3* db);
int delete_course(sqlite3* db);
int add_grade(sqlite3* db);
int modify_grade(sqlite3* db);
int department_stat(sqlite3* db);
int rank_inside_department(sqlite3* db);
int print_student_info(sqlite3* db);

int main(int argc, char* argv[])
{
    using namespace std;

    //连接数据库
    sqlite3* db;
    if (sqlite3_open("S_T_U202013982.db", &db)) {
        cout << "连接数据库失败！" << sqlite3_errmsg(db) << endl;
        exit(0);
    }
    else {
        cout << "连接数据库成功！" << endl;
    }

    //循坏打印选择窗口
    int choice;
    while (1)
    {
        system("cls");
        cout << "*****************学生信息管理系统*****************" << endl;
        cout << "0.退出" << endl;
        cout << "1.新增学生信息" << endl;
        cout << "2.修改学生信息" << endl;
        cout << "3.增加新课程" << endl;
        cout << "4.修改课程信息" << endl;
        cout << "5.删除没有选课的课程信息" << endl;
        cout << "6.录入学生成绩" << endl;
        cout << "7.修改学生成绩" << endl;
        cout << "8.按系统计学生的平均成绩、最好成绩、最差成绩、优秀率、不及格人数" << endl;
        cout << "9.按系对学生成绩进行排名，同时显示出学生、课程和成绩信息" << endl;
        cout << "10.输入学号，显示该学生的基本信息和选课信息" << endl;
        cout << "**************************************************" << endl;
        cin >> choice;
        switch (choice)
        {
        case 0:
            return 0;
        case 1:
            add_student(db);
            system("pause");
            break;
        case 2:
            modify_student(db);
            system("pause");
            break;
        case 3:
            add_course(db);
            system("pause");
            break;
        case 4:
            modify_course(db);
            system("pause");
            break;
        case 5:
            delete_course(db);
            system("pause");
            break;
        case 6:
            add_grade(db);
            system("pause");
            break;
        case 7:
            modify_grade(db);
            system("pause");
            break;
        case 8:
            department_stat(db);
            system("pause");
            break;
        case 9:
            rank_inside_department(db);
            system("pause");
            break;
        case 10:
            print_student_info(db);
            system("pause");
            break;
        default:
            cout << "输入不合法！" << endl;
            system("pause");
        }
    }
    //关闭数据库连接
    sqlite3_close(db);
    return 0;
}

//回调函数
int callback(void* not_used, int argc, char** argv, char** azColName)
{
    int i;
    for (i = 0; i < argc; i++) {
        printf("%s = %s\n", U2G(azColName[i]), argv[i] ? U2G(argv[i]) : "NULL");
    }
    printf("\n");
    return 0;
}

//执行不需要返回表的sql语句
int sql_notable(sqlite3* db, std::string& sql)
{
    using namespace std;

    char* zErrMsg = 0;
    int  rc;
    rc = sqlite3_exec(db, G2U(sql.c_str()), nullptr, nullptr, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return FAIL;
    }
    else {
        fprintf(stdout, "操作成功！\n");
        return SUCCESS;
    }
}

//执行需要返回表的sql语句，返回查询结果的行数，print==1打印结果，否则不打印
int sql_withtable(sqlite3* db,std::string & sql,int print=1)
{
    using namespace std;

    char** pResult;
    char* zErrMsg = 0;
    int rc;
    int nRow;
    int nCol;
    rc = sqlite3_get_table(db, G2U(sql.c_str()), &pResult, &nRow, &nCol, &zErrMsg);
    if (rc != SQLITE_OK) {
        cout << "SQL error: " << zErrMsg << endl;
        sqlite3_free(zErrMsg);
        return FAIL;
    }
    else if (nRow == 0)
    {
        //未查询到结果，返回0
        sqlite3_free_table(pResult);
        return nRow;
    }
    else
    {
        //cout << "查询到" << nRow << "条结果" << endl << endl;
        if (print == 1)
            print_table(pResult, nRow, nCol);
        sqlite3_free_table(pResult);
        return nRow;
    }
}

//打印返回结果
void print_table(char** pResult, int nrow, int ncol)
{
    using namespace std;
    string strOut;
    int nIndex = ncol;
    for (int i = 0; i < nrow; i++)
    {
        for (int j = 0; j < ncol; j++)
        {
            strOut += U2G(pResult[j]);
            strOut += ":";
            strOut += U2G(pResult[nIndex]);
            strOut += "    ";
            ++nIndex;
        }
        strOut += "\n";
    }
    cout << strOut << endl;
}

//新增学生信息
int add_student(sqlite3* db)
{
    using namespace std;

    cout << "新增学生信息" << endl << endl;

    string sno;     //学号
    string sname;   //姓名
    string sdept;   //院系
    string sage;       //年龄
    string ssex;    //性别
    string scholarship;  //奖学金

    //输入信息
    cout << "输入学号（9位）：";
    cin >> sno;
    while (sno.length() != 9)
    {
        cout << "学号长度不合法！" << endl;
        cout << "输入学号（9位）：";
        cin >> sno;
    }
    cout << "输入姓名：";
    cin >> sname;
    sname = "'" + sname + "'";
    cout << "输入院系：";
    cin >> sdept;
    sdept = "'" + sdept + "'";
    cout << "输入年龄：";
    cin >> sage;

    int choice;
    cout << "输入性别（0代表男，1代表女):";
    cin >> choice;
    while ((choice != 0) && (choice != 1))
    {
        cout << "输入不合法！" << endl;
        cout << "输入性别（0代表男，1代表女):";
        cin >> choice;
    }
    if (choice == 0)
        ssex = "'男'";
    else
        ssex = "'女'";

    cout << "是否获得奖学金？（0代表未获得，1代表获得过):";
    cin >> choice;
    while ((choice != 0) && (choice != 1))
    {
        cout << "输入不合法！" << endl;
        cout << "是否获得奖学金？（0代表未获得，1代表获得过):";
        cin >> choice;
    }
    if (choice == 0)
        scholarship = "'否'";
    else
        scholarship = "'是'";

    //插入数据
    std::string sql;
    sql = "INSERT INTO Student (Sno,Sname,Ssex,Sage,Sdept,Scholarship)";
    sql += "VALUES (" + sno + "," + sname + "," + ssex + "," + sage + "," + sdept + "," + scholarship + ");";
    if (sql_notable(db, sql) == FAIL)
        return FAIL;
    else
        return SUCCESS;
}

//修改学生信息
int modify_student(sqlite3* db)
{
    using namespace std;
    cout << "修改学生信息" << endl << endl;

    string sno;     //学号
    cout << "输入学号：";
    cin >> sno;
    sno = "'" + sno + "'";

    //查询
    std::string sql;
    sql = "SELECT * FROM Student WHERE Sno=" + sno + ";";
    int rc = sql_withtable(db, sql);
    if (rc == 0)
    {
        cout << "学生不存在！" << endl;
        return FAIL;
    }
    if (rc == FAIL)
    {
        return FAIL;
    }
    
    //修改
    cout << "0.退出" << endl;
    cout << "1.修改姓名" << endl;
    cout << "2.修改性别" << endl;
    cout << "3.修改年龄" << endl;
    cout << "4.修改院系" << endl;
    cout << "5.修改奖学金" << endl;
    int choice;
    cin >> choice;
    sql = "";
    string modify;
    switch (choice)
    {
        case 0:
            return FAIL;
        case 1:
            cout << "输入新的姓名：";
            cin >> modify;
            modify = "'" + modify + "'";
            sql = "UPDATE Student set Sname=" + modify + " where Sno=" + sno + ";";
            break;
        case 2:
            cout << "输入性别（0代表男，1代表女）：";
            cin >> modify;
            while ((modify != "0") && (modify != "1"))
            {
                cout << "输入不合法！" << endl;
                cout << "输入性别（0代表男，1代表女）：";
                cin >> modify;
            }
            if (modify == "0")
                modify = "'男'";
            else
                modify = "'女'";
            sql = "UPDATE Student set Ssex=" + modify + " where Sno=" + sno + ";";
            break;
        case 3:
            cout << "输入年龄：";
            cin >> modify;
            sql = "UPDATE Student set Sage=" + modify + " where Sno=" + sno + ";";
            break;
        case 4:
            cout << "输入院系：";
            cin >> modify;
            modify = "'" + modify + "'";
            sql = "UPDATE Student set Sdept=" + modify + " where Sno=" + sno + ";";
            break;
        case 5:
            cout << "输入奖学金情况（0代表未获得，1代表已获得）：";
            cin >> modify;
            while ((modify != "0") && (modify != "1"))
            {
                cout << "输入不合法！" << endl;
                cout << "输入奖学金情况（0代表未获得，1代表已获得）：";
                cin >> modify;
            }
            if (modify == "0")
                modify = "'否'";
            else
                modify = "'是'";
            sql = "UPDATE Student set Scholarship=" + modify + " where Sno=" + sno + ";";
            break;
        default:
            cout << "输入不合法！" << endl;
            return FAIL;
    }
    if (sql_notable(db, sql) == FAIL)
        return FAIL;
    
    //打印修改后的信息
    sql = "SELECT * FROM Student WHERE Sno=" + sno + ";";
    if (sql_withtable(db, sql) == FAIL)
        return FAIL;
    
    return SUCCESS;
}

//增加新课程
int add_course(sqlite3* db)
{
    using namespace std;

    cout << "增加新课程" << endl << endl;

    string cno;     //课程号
    string cname;   //课程名称
    string cpno;    //先导课程号
    string ccredit; //学分

    //输入信息
    cout << "输入课程号：";
    cin >> cno;
    cno = "'" + cno + "'";
    cout << "输入课程名称：";
    cin >> cname;
    cname = "'" + cname + "'";
    cout << "输入先导课程号：";
    cin >> cpno;
    cpno = "'" + cpno + "'";
    cout << "输入学分：";
    cin >> ccredit;

    //插入数据
    std::string sql;
    sql = "INSERT INTO Course (Cno,Cname,Cpno,Ccredit) ";
    sql += "VALUES (" + cno + "," + cname + "," + cpno + "," + ccredit + ");";
    if (sql_notable(db, sql) == FAIL)
        return FAIL;
    else
        return SUCCESS;
}

//修改课程信息
int modify_course(sqlite3* db)
{
    using namespace std;

    cout << "修改课程信息" << endl << endl;

    string cno;     //课程号
    cout << "输入课程号：";
    cin >> cno;
    cno = "'" + cno + "'";
    string sql = "SELECT * FROM Course WHERE Cno=" + cno + ";";
    int rc = sql_withtable(db, sql);
    if (rc == 0)
    {
        cout << "课程不存在！" << endl;
        return FAIL;
    }
    if (rc == FAIL)
    {
        return FAIL;
    }

    //修改
    cout << "0.退出" << endl;
    cout << "1.修改课程名称" << endl;
    cout << "2.修改先导课程号" << endl;
    cout << "3.修改学分" << endl;
    int choice;
    cin >> choice;
    string modify;
    switch (choice)
    {
        case 0:
            return FAIL;
        case 1:
            cout << "输入课程名称：";
            cin >> modify;
            modify = "'" + modify + "'";
            sql = "UPDATE Course SET Cname=" + modify + " where Cno=" + cno + ";";
            break;
        case 2:
            cout << "输入先导课程号：";
            cin >> modify;
            modify = "'" + modify + "'";
            sql = "UPDATE Course SET Cpno=" + modify + " where Cno=" + cno + ";";
            break;
        case 3:
            cout << "输入学分：";
            cin >> modify;
            sql = "UPDATE Course SET Ccredit=" + modify + " where Cno=" + cno + ";";
            break;
        default:
            cout << "输入不合法！" << endl;
            return FAIL;
    }
    if (sql_notable(db, sql) == FAIL)
        return FAIL;

    //查询修改后的信息
    sql = "SELECT * FROM Course WHERE Cno=" + cno + ";";
    if (sql_withtable(db, sql) == FAIL)
        return FAIL;

    return SUCCESS;
}

//删除没有选课的课程信息
int delete_course(sqlite3* db)
{
    using namespace std;

    cout << "删除没有选课的课程信息" << endl << endl;

    string cno;
    cout << "输入课程号：";
    cin >> cno;
    cno = "'" + cno + "'";
    
    //先在Course表中查询是否存在该课程
    string sql = "SELECT * FROM Course WHERE Cno=" + cno + ";";
    int rc = sql_withtable(db, sql, 0);
    if (rc == FAIL)
        return FAIL;
    if (rc == 0)
    {
        cout << "课程不存在！" << endl;
        return FAIL;
    }

    //在SC表中查询是否有选课
    sql = "SELECT * FROM SC WHERE Cno=" + cno + ";";
    rc = sql_withtable(db, sql, 0);
    if (rc == FAIL)
        return FAIL;
    if (rc > 0)
    {
        cout << "该课程存在选课信息，无法被删除！" << endl;
        return FAIL;
    }

    //删除课程
    sql = "DELETE FROM Course WHERE Cno=" + cno + ";";
    if (sql_notable(db, sql) == FAIL)
        return FAIL;

    return SUCCESS;
}

//录入成绩
int add_grade(sqlite3* db)
{
    using namespace std;

    cout << "录入成绩" << endl << endl;

    string sno;     //学号
    string cno;     //课程号
    string grade;   //成绩

    //输入信息
    cout << "输入学号：";
    cin >> sno;
    while (sno.length() != 9)
    {
        cout << "学号长度不合法！" << endl;
        cout << "输入学号：";
        cin >> sno;
    }
    sno = "'" + sno + "'";
    cout << "输入课程号：";
    cin >> cno;
    cno = "'" + cno + "'";
    cout << "输入成绩：";
    cin >> grade;

    //插入数据
    string sql = "INSERT INTO SC (Sno,Cno,Grade) ";
    sql += "VALUES (" + sno + "," + cno + "," + grade + ");";
    if (sql_notable(db, sql) == FAIL)
        return FAIL;
    else
        return SUCCESS;
}

//修改成绩
int modify_grade(sqlite3* db)
{
    using namespace std;

    cout << "修改成绩" << endl << endl;

    string sno;     //学号
    string cno;     //课程号

    cout << "输入学号：";
    cin >> sno;
    sno = "'" + sno + "'";
    cout << "输入课程号：";
    cin >> cno;

    //查询
    string sql = "SELECT * FROM SC WHERE Sno=" + sno + " AND Cno=" + cno + ";";
    int rc = sql_withtable(db, sql);
    if (rc == 0)
    {
        cout << "成绩不存在！" << endl;
        return FAIL;
    }
    if (rc == FAIL)
    {
        return FAIL;
    }

    string grade;
    cout << "输入成绩：";
    cin >> grade;

    //修改
    sql = "UPDATE SC SET Grade=" + grade + " WHERE Sno=" + sno + " AND Cno=" + cno + ";";
    if (sql_notable(db, sql) == FAIL)
        return FAIL;

    //查询修改后的信息
    sql = "SELECT * FROM SC WHERE Sno=" + sno + " AND Cno=" + cno + ";";
    if (sql_withtable(db, sql) == FAIL)
        return FAIL;

    return SUCCESS;
}

//按系统计学生的平均成绩、最好成绩、最差成绩、优秀率、不及格人数
int department_stat(sqlite3* db)
{
    using namespace std;

    cout << "按系统计学生的平均成绩、最好成绩、最差成绩、优秀率、不及格人数" << endl << endl;

    string sdept;       //院系
    cout << "输入院系：";
    cin >> sdept;
    sdept = "'" + sdept + "'";

    string sql = "SELECT * FROM SC WHERE SC.Sno IN (SELECT Student.Sno FROM Student WHERE Student.Sdept=" + sdept + ");";
    int rc = sql_withtable(db, sql, 0);
    int empty = 0;
    if (rc == 0)
    {
        empty = 1;      //检查是否有该院系的成绩，将empty设为标志
    }
    if (rc == FAIL)
    {
        return FAIL;
    }

    //平均成绩
    cout << "平均成绩" << endl;
    if (empty == 1)
        cout << "无数据" << endl;
    else
    {
        sql = "SELECT Student.Sdept,AVG(SC.Grade) FROM SC,Student WHERE SC.Sno IN (SELECT Student.Sno FROM Student WHERE Student.Sdept=" + sdept + ");";
        rc = sql_withtable(db, sql);
        if (rc == FAIL)
        {
            return FAIL;
        }
    }

    //最好成绩
    cout << "最好成绩" << endl;
    if (empty == 1)
        cout << "无数据" << endl;
    else
    {
        sql = "SELECT Student.Sdept,MAX(SC.Grade) FROM SC,Student WHERE SC.Sno IN (SELECT Student.Sno FROM Student WHERE Student.Sdept=" + sdept + ");";
        rc = sql_withtable(db, sql);
        if (rc == FAIL)
        {
            return FAIL;
        }
    }

    //最差成绩
    cout << "最差成绩" << endl;
    if (empty == 1)
        cout << "无数据" << endl;
    else
    {
        sql = "SELECT Student.Sdept,MIN(SC.Grade) FROM SC,Student WHERE SC.Sno IN (SELECT Student.Sno FROM Student WHERE Student.Sdept=" + sdept + ");";
        rc = sql_withtable(db, sql);
        if (rc == FAIL)
        {
            return FAIL;
        }
    }

    //优秀率
    cout << "优秀率" << endl;
    sql = "SELECT * FROM SC WHERE SC.Sno IN (SELECT Student.Sno FROM Student WHERE Student.Sdept=" + sdept + ");";
    rc = sql_withtable(db, sql,0);
    int all = rc;
    if (rc == 0)
    {
        cout << "无数据" << endl;
    }
    else if (rc == FAIL)
    {
        return FAIL;
    }
    else
    {
        sql= "SELECT * FROM SC WHERE SC.Grade>=90 AND SC.Sno IN (SELECT Student.Sno FROM Student WHERE Student.Sdept=" + sdept + ");";
        rc = sql_withtable(db, sql, 0);
        if (rc == FAIL)
            return FAIL;
        cout << sdept << ":  " << double(rc) / all * 100 << "%" << endl << endl;
    }

    //不及格人数
    cout << "不及格人数" << endl;
    sql = "SELECT * FROM SC WHERE SC.Grade<60 AND SC.Sno IN (SELECT Student.Sno FROM Student WHERE Student.Sdept=" + sdept + ");";
    rc = sql_withtable(db, sql, 0);
    if (rc == FAIL)
        return FAIL;
    cout << sdept << ":  " << rc << "人次" << endl;

    return SUCCESS;
}

//按系对学生成绩进行排名，同时显示出学生、课程和成绩信息
int rank_inside_department(sqlite3* db)
{
    using namespace std;

    cout << "按系对学生成绩进行排名，同时显示出学生、课程和成绩信息" << endl << endl;

    string sdept;

    cout << "输入院系：";
    cin >> sdept;
    sdept = "'" + sdept + "'";

    string sql = "SELECT SC.Sno,Student.Sname,SC.Cno,Course.Cname,SC.Grade FROM SC,Student,Course WHERE Course.Cno=SC.Cno AND Student.Sno=sc.Sno AND SC.Sno IN (SELECT Sno FROM Student WHERE Sdept=" + sdept + ") ORDER BY Grade DESC;";
    int rc = sql_withtable(db, sql);
    if (rc == FAIL)
        return FAIL;
    if (rc == 0)
        cout << "无数据" << endl;

    return SUCCESS;
}

//输入学号，显示该学生的基本信息和选课信息
int print_student_info(sqlite3* db)
{
    using namespace std;

    cout << "输入学号，显示该学生的基本信息和选课信息" << endl << endl;

    string sno;     //学号
    cout << "输入学号：";
    cin >> sno;
    sno = "'" + sno + "'";

    //查询
    std::string sql;
    sql = "SELECT * FROM Student WHERE Sno=" + sno + ";";
    int rc = sql_withtable(db, sql);
    if (rc == 0)
    {
        cout << "学生不存在！" << endl;
        return FAIL;
    }
    if (rc == FAIL)
    {
        return FAIL;
    }
    sql = "SELECT * FROM SC WHERE Sno=" + sno + ";";
    rc = sql_withtable(db, sql);
    if (rc == 0)
    {
        cout << "无选课信息！" << endl;
        return FAIL;
    }
    if (rc == FAIL)
    {
        return FAIL;
    }

    return SUCCESS;
}

void sql_demo(sqlite3* db)
{
    //sqlite3* db;
    char* zErrMsg = 0;
    int  rc;
    std::string sql;

    /* Open database */
    /*
    rc = sqlite3_open("S_T_U202013982.db", &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    }
    else {
        fprintf(stdout, "Opened database successfully\n");
    }
    */

    /* Create SQL statement */
    sql = "SELECT * FROM Student;";

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else {
        fprintf(stdout, "Table created successfully\n");
    }
    //sqlite3_close(db);
}

//UTF-8到GB2312的转换
char* U2G(const char* utf8)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
    wchar_t* wstr = new wchar_t[len + 1];
    memset(wstr, 0, len + 1);
    MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
    len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
    char* str = new char[len + 1];
    memset(str, 0, len + 1);
    WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
    if (wstr) delete[] wstr;
    return str;
}

//GB2312到UTF-8的转换
char* G2U(const char* gb2312)
{
    int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
    wchar_t* wstr = new wchar_t[len + 1];
    memset(wstr, 0, len + 1);
    MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);
    len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    char* str = new char[len + 1];
    memset(str, 0, len + 1);
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
    if (wstr) delete[] wstr;
    return str;
}