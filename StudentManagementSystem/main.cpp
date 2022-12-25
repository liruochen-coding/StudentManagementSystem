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

    //�������ݿ�
    sqlite3* db;
    if (sqlite3_open("S_T_U202013982.db", &db)) {
        cout << "�������ݿ�ʧ�ܣ�" << sqlite3_errmsg(db) << endl;
        exit(0);
    }
    else {
        cout << "�������ݿ�ɹ���" << endl;
    }

    //ѭ����ӡѡ�񴰿�
    int choice;
    while (1)
    {
        system("cls");
        cout << "*****************ѧ����Ϣ����ϵͳ*****************" << endl;
        cout << "0.�˳�" << endl;
        cout << "1.����ѧ����Ϣ" << endl;
        cout << "2.�޸�ѧ����Ϣ" << endl;
        cout << "3.�����¿γ�" << endl;
        cout << "4.�޸Ŀγ���Ϣ" << endl;
        cout << "5.ɾ��û��ѡ�εĿγ���Ϣ" << endl;
        cout << "6.¼��ѧ���ɼ�" << endl;
        cout << "7.�޸�ѧ���ɼ�" << endl;
        cout << "8.��ϵͳ��ѧ����ƽ���ɼ�����óɼ������ɼ��������ʡ�����������" << endl;
        cout << "9.��ϵ��ѧ���ɼ�����������ͬʱ��ʾ��ѧ�����γ̺ͳɼ���Ϣ" << endl;
        cout << "10.����ѧ�ţ���ʾ��ѧ���Ļ�����Ϣ��ѡ����Ϣ" << endl;
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
            cout << "���벻�Ϸ���" << endl;
            system("pause");
        }
    }
    //�ر����ݿ�����
    sqlite3_close(db);
    return 0;
}

//�ص�����
int callback(void* not_used, int argc, char** argv, char** azColName)
{
    int i;
    for (i = 0; i < argc; i++) {
        printf("%s = %s\n", U2G(azColName[i]), argv[i] ? U2G(argv[i]) : "NULL");
    }
    printf("\n");
    return 0;
}

//ִ�в���Ҫ���ر��sql���
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
        fprintf(stdout, "�����ɹ���\n");
        return SUCCESS;
    }
}

//ִ����Ҫ���ر��sql��䣬���ز�ѯ�����������print==1��ӡ��������򲻴�ӡ
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
        //δ��ѯ�����������0
        sqlite3_free_table(pResult);
        return nRow;
    }
    else
    {
        //cout << "��ѯ��" << nRow << "�����" << endl << endl;
        if (print == 1)
            print_table(pResult, nRow, nCol);
        sqlite3_free_table(pResult);
        return nRow;
    }
}

//��ӡ���ؽ��
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

//����ѧ����Ϣ
int add_student(sqlite3* db)
{
    using namespace std;

    cout << "����ѧ����Ϣ" << endl << endl;

    string sno;     //ѧ��
    string sname;   //����
    string sdept;   //Ժϵ
    string sage;       //����
    string ssex;    //�Ա�
    string scholarship;  //��ѧ��

    //������Ϣ
    cout << "����ѧ�ţ�9λ����";
    cin >> sno;
    while (sno.length() != 9)
    {
        cout << "ѧ�ų��Ȳ��Ϸ���" << endl;
        cout << "����ѧ�ţ�9λ����";
        cin >> sno;
    }
    cout << "����������";
    cin >> sname;
    sname = "'" + sname + "'";
    cout << "����Ժϵ��";
    cin >> sdept;
    sdept = "'" + sdept + "'";
    cout << "�������䣺";
    cin >> sage;

    int choice;
    cout << "�����Ա�0�����У�1����Ů):";
    cin >> choice;
    while ((choice != 0) && (choice != 1))
    {
        cout << "���벻�Ϸ���" << endl;
        cout << "�����Ա�0�����У�1����Ů):";
        cin >> choice;
    }
    if (choice == 0)
        ssex = "'��'";
    else
        ssex = "'Ů'";

    cout << "�Ƿ��ý�ѧ�𣿣�0����δ��ã�1�����ù�):";
    cin >> choice;
    while ((choice != 0) && (choice != 1))
    {
        cout << "���벻�Ϸ���" << endl;
        cout << "�Ƿ��ý�ѧ�𣿣�0����δ��ã�1�����ù�):";
        cin >> choice;
    }
    if (choice == 0)
        scholarship = "'��'";
    else
        scholarship = "'��'";

    //��������
    std::string sql;
    sql = "INSERT INTO Student (Sno,Sname,Ssex,Sage,Sdept,Scholarship)";
    sql += "VALUES (" + sno + "," + sname + "," + ssex + "," + sage + "," + sdept + "," + scholarship + ");";
    if (sql_notable(db, sql) == FAIL)
        return FAIL;
    else
        return SUCCESS;
}

//�޸�ѧ����Ϣ
int modify_student(sqlite3* db)
{
    using namespace std;
    cout << "�޸�ѧ����Ϣ" << endl << endl;

    string sno;     //ѧ��
    cout << "����ѧ�ţ�";
    cin >> sno;
    sno = "'" + sno + "'";

    //��ѯ
    std::string sql;
    sql = "SELECT * FROM Student WHERE Sno=" + sno + ";";
    int rc = sql_withtable(db, sql);
    if (rc == 0)
    {
        cout << "ѧ�������ڣ�" << endl;
        return FAIL;
    }
    if (rc == FAIL)
    {
        return FAIL;
    }
    
    //�޸�
    cout << "0.�˳�" << endl;
    cout << "1.�޸�����" << endl;
    cout << "2.�޸��Ա�" << endl;
    cout << "3.�޸�����" << endl;
    cout << "4.�޸�Ժϵ" << endl;
    cout << "5.�޸Ľ�ѧ��" << endl;
    int choice;
    cin >> choice;
    sql = "";
    string modify;
    switch (choice)
    {
        case 0:
            return FAIL;
        case 1:
            cout << "�����µ�������";
            cin >> modify;
            modify = "'" + modify + "'";
            sql = "UPDATE Student set Sname=" + modify + " where Sno=" + sno + ";";
            break;
        case 2:
            cout << "�����Ա�0�����У�1����Ů����";
            cin >> modify;
            while ((modify != "0") && (modify != "1"))
            {
                cout << "���벻�Ϸ���" << endl;
                cout << "�����Ա�0�����У�1����Ů����";
                cin >> modify;
            }
            if (modify == "0")
                modify = "'��'";
            else
                modify = "'Ů'";
            sql = "UPDATE Student set Ssex=" + modify + " where Sno=" + sno + ";";
            break;
        case 3:
            cout << "�������䣺";
            cin >> modify;
            sql = "UPDATE Student set Sage=" + modify + " where Sno=" + sno + ";";
            break;
        case 4:
            cout << "����Ժϵ��";
            cin >> modify;
            modify = "'" + modify + "'";
            sql = "UPDATE Student set Sdept=" + modify + " where Sno=" + sno + ";";
            break;
        case 5:
            cout << "���뽱ѧ�������0����δ��ã�1�����ѻ�ã���";
            cin >> modify;
            while ((modify != "0") && (modify != "1"))
            {
                cout << "���벻�Ϸ���" << endl;
                cout << "���뽱ѧ�������0����δ��ã�1�����ѻ�ã���";
                cin >> modify;
            }
            if (modify == "0")
                modify = "'��'";
            else
                modify = "'��'";
            sql = "UPDATE Student set Scholarship=" + modify + " where Sno=" + sno + ";";
            break;
        default:
            cout << "���벻�Ϸ���" << endl;
            return FAIL;
    }
    if (sql_notable(db, sql) == FAIL)
        return FAIL;
    
    //��ӡ�޸ĺ����Ϣ
    sql = "SELECT * FROM Student WHERE Sno=" + sno + ";";
    if (sql_withtable(db, sql) == FAIL)
        return FAIL;
    
    return SUCCESS;
}

//�����¿γ�
int add_course(sqlite3* db)
{
    using namespace std;

    cout << "�����¿γ�" << endl << endl;

    string cno;     //�γ̺�
    string cname;   //�γ�����
    string cpno;    //�ȵ��γ̺�
    string ccredit; //ѧ��

    //������Ϣ
    cout << "����γ̺ţ�";
    cin >> cno;
    cno = "'" + cno + "'";
    cout << "����γ����ƣ�";
    cin >> cname;
    cname = "'" + cname + "'";
    cout << "�����ȵ��γ̺ţ�";
    cin >> cpno;
    cpno = "'" + cpno + "'";
    cout << "����ѧ�֣�";
    cin >> ccredit;

    //��������
    std::string sql;
    sql = "INSERT INTO Course (Cno,Cname,Cpno,Ccredit) ";
    sql += "VALUES (" + cno + "," + cname + "," + cpno + "," + ccredit + ");";
    if (sql_notable(db, sql) == FAIL)
        return FAIL;
    else
        return SUCCESS;
}

//�޸Ŀγ���Ϣ
int modify_course(sqlite3* db)
{
    using namespace std;

    cout << "�޸Ŀγ���Ϣ" << endl << endl;

    string cno;     //�γ̺�
    cout << "����γ̺ţ�";
    cin >> cno;
    cno = "'" + cno + "'";
    string sql = "SELECT * FROM Course WHERE Cno=" + cno + ";";
    int rc = sql_withtable(db, sql);
    if (rc == 0)
    {
        cout << "�γ̲����ڣ�" << endl;
        return FAIL;
    }
    if (rc == FAIL)
    {
        return FAIL;
    }

    //�޸�
    cout << "0.�˳�" << endl;
    cout << "1.�޸Ŀγ�����" << endl;
    cout << "2.�޸��ȵ��γ̺�" << endl;
    cout << "3.�޸�ѧ��" << endl;
    int choice;
    cin >> choice;
    string modify;
    switch (choice)
    {
        case 0:
            return FAIL;
        case 1:
            cout << "����γ����ƣ�";
            cin >> modify;
            modify = "'" + modify + "'";
            sql = "UPDATE Course SET Cname=" + modify + " where Cno=" + cno + ";";
            break;
        case 2:
            cout << "�����ȵ��γ̺ţ�";
            cin >> modify;
            modify = "'" + modify + "'";
            sql = "UPDATE Course SET Cpno=" + modify + " where Cno=" + cno + ";";
            break;
        case 3:
            cout << "����ѧ�֣�";
            cin >> modify;
            sql = "UPDATE Course SET Ccredit=" + modify + " where Cno=" + cno + ";";
            break;
        default:
            cout << "���벻�Ϸ���" << endl;
            return FAIL;
    }
    if (sql_notable(db, sql) == FAIL)
        return FAIL;

    //��ѯ�޸ĺ����Ϣ
    sql = "SELECT * FROM Course WHERE Cno=" + cno + ";";
    if (sql_withtable(db, sql) == FAIL)
        return FAIL;

    return SUCCESS;
}

//ɾ��û��ѡ�εĿγ���Ϣ
int delete_course(sqlite3* db)
{
    using namespace std;

    cout << "ɾ��û��ѡ�εĿγ���Ϣ" << endl << endl;

    string cno;
    cout << "����γ̺ţ�";
    cin >> cno;
    cno = "'" + cno + "'";
    
    //����Course���в�ѯ�Ƿ���ڸÿγ�
    string sql = "SELECT * FROM Course WHERE Cno=" + cno + ";";
    int rc = sql_withtable(db, sql, 0);
    if (rc == FAIL)
        return FAIL;
    if (rc == 0)
    {
        cout << "�γ̲����ڣ�" << endl;
        return FAIL;
    }

    //��SC���в�ѯ�Ƿ���ѡ��
    sql = "SELECT * FROM SC WHERE Cno=" + cno + ";";
    rc = sql_withtable(db, sql, 0);
    if (rc == FAIL)
        return FAIL;
    if (rc > 0)
    {
        cout << "�ÿγ̴���ѡ����Ϣ���޷���ɾ����" << endl;
        return FAIL;
    }

    //ɾ���γ�
    sql = "DELETE FROM Course WHERE Cno=" + cno + ";";
    if (sql_notable(db, sql) == FAIL)
        return FAIL;

    return SUCCESS;
}

//¼��ɼ�
int add_grade(sqlite3* db)
{
    using namespace std;

    cout << "¼��ɼ�" << endl << endl;

    string sno;     //ѧ��
    string cno;     //�γ̺�
    string grade;   //�ɼ�

    //������Ϣ
    cout << "����ѧ�ţ�";
    cin >> sno;
    while (sno.length() != 9)
    {
        cout << "ѧ�ų��Ȳ��Ϸ���" << endl;
        cout << "����ѧ�ţ�";
        cin >> sno;
    }
    sno = "'" + sno + "'";
    cout << "����γ̺ţ�";
    cin >> cno;
    cno = "'" + cno + "'";
    cout << "����ɼ���";
    cin >> grade;

    //��������
    string sql = "INSERT INTO SC (Sno,Cno,Grade) ";
    sql += "VALUES (" + sno + "," + cno + "," + grade + ");";
    if (sql_notable(db, sql) == FAIL)
        return FAIL;
    else
        return SUCCESS;
}

//�޸ĳɼ�
int modify_grade(sqlite3* db)
{
    using namespace std;

    cout << "�޸ĳɼ�" << endl << endl;

    string sno;     //ѧ��
    string cno;     //�γ̺�

    cout << "����ѧ�ţ�";
    cin >> sno;
    sno = "'" + sno + "'";
    cout << "����γ̺ţ�";
    cin >> cno;

    //��ѯ
    string sql = "SELECT * FROM SC WHERE Sno=" + sno + " AND Cno=" + cno + ";";
    int rc = sql_withtable(db, sql);
    if (rc == 0)
    {
        cout << "�ɼ������ڣ�" << endl;
        return FAIL;
    }
    if (rc == FAIL)
    {
        return FAIL;
    }

    string grade;
    cout << "����ɼ���";
    cin >> grade;

    //�޸�
    sql = "UPDATE SC SET Grade=" + grade + " WHERE Sno=" + sno + " AND Cno=" + cno + ";";
    if (sql_notable(db, sql) == FAIL)
        return FAIL;

    //��ѯ�޸ĺ����Ϣ
    sql = "SELECT * FROM SC WHERE Sno=" + sno + " AND Cno=" + cno + ";";
    if (sql_withtable(db, sql) == FAIL)
        return FAIL;

    return SUCCESS;
}

//��ϵͳ��ѧ����ƽ���ɼ�����óɼ������ɼ��������ʡ�����������
int department_stat(sqlite3* db)
{
    using namespace std;

    cout << "��ϵͳ��ѧ����ƽ���ɼ�����óɼ������ɼ��������ʡ�����������" << endl << endl;

    string sdept;       //Ժϵ
    cout << "����Ժϵ��";
    cin >> sdept;
    sdept = "'" + sdept + "'";

    string sql = "SELECT * FROM SC WHERE SC.Sno IN (SELECT Student.Sno FROM Student WHERE Student.Sdept=" + sdept + ");";
    int rc = sql_withtable(db, sql, 0);
    int empty = 0;
    if (rc == 0)
    {
        empty = 1;      //����Ƿ��и�Ժϵ�ĳɼ�����empty��Ϊ��־
    }
    if (rc == FAIL)
    {
        return FAIL;
    }

    //ƽ���ɼ�
    cout << "ƽ���ɼ�" << endl;
    if (empty == 1)
        cout << "������" << endl;
    else
    {
        sql = "SELECT Student.Sdept,AVG(SC.Grade) FROM SC,Student WHERE SC.Sno IN (SELECT Student.Sno FROM Student WHERE Student.Sdept=" + sdept + ");";
        rc = sql_withtable(db, sql);
        if (rc == FAIL)
        {
            return FAIL;
        }
    }

    //��óɼ�
    cout << "��óɼ�" << endl;
    if (empty == 1)
        cout << "������" << endl;
    else
    {
        sql = "SELECT Student.Sdept,MAX(SC.Grade) FROM SC,Student WHERE SC.Sno IN (SELECT Student.Sno FROM Student WHERE Student.Sdept=" + sdept + ");";
        rc = sql_withtable(db, sql);
        if (rc == FAIL)
        {
            return FAIL;
        }
    }

    //���ɼ�
    cout << "���ɼ�" << endl;
    if (empty == 1)
        cout << "������" << endl;
    else
    {
        sql = "SELECT Student.Sdept,MIN(SC.Grade) FROM SC,Student WHERE SC.Sno IN (SELECT Student.Sno FROM Student WHERE Student.Sdept=" + sdept + ");";
        rc = sql_withtable(db, sql);
        if (rc == FAIL)
        {
            return FAIL;
        }
    }

    //������
    cout << "������" << endl;
    sql = "SELECT * FROM SC WHERE SC.Sno IN (SELECT Student.Sno FROM Student WHERE Student.Sdept=" + sdept + ");";
    rc = sql_withtable(db, sql,0);
    int all = rc;
    if (rc == 0)
    {
        cout << "������" << endl;
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

    //����������
    cout << "����������" << endl;
    sql = "SELECT * FROM SC WHERE SC.Grade<60 AND SC.Sno IN (SELECT Student.Sno FROM Student WHERE Student.Sdept=" + sdept + ");";
    rc = sql_withtable(db, sql, 0);
    if (rc == FAIL)
        return FAIL;
    cout << sdept << ":  " << rc << "�˴�" << endl;

    return SUCCESS;
}

//��ϵ��ѧ���ɼ�����������ͬʱ��ʾ��ѧ�����γ̺ͳɼ���Ϣ
int rank_inside_department(sqlite3* db)
{
    using namespace std;

    cout << "��ϵ��ѧ���ɼ�����������ͬʱ��ʾ��ѧ�����γ̺ͳɼ���Ϣ" << endl << endl;

    string sdept;

    cout << "����Ժϵ��";
    cin >> sdept;
    sdept = "'" + sdept + "'";

    string sql = "SELECT SC.Sno,Student.Sname,SC.Cno,Course.Cname,SC.Grade FROM SC,Student,Course WHERE Course.Cno=SC.Cno AND Student.Sno=sc.Sno AND SC.Sno IN (SELECT Sno FROM Student WHERE Sdept=" + sdept + ") ORDER BY Grade DESC;";
    int rc = sql_withtable(db, sql);
    if (rc == FAIL)
        return FAIL;
    if (rc == 0)
        cout << "������" << endl;

    return SUCCESS;
}

//����ѧ�ţ���ʾ��ѧ���Ļ�����Ϣ��ѡ����Ϣ
int print_student_info(sqlite3* db)
{
    using namespace std;

    cout << "����ѧ�ţ���ʾ��ѧ���Ļ�����Ϣ��ѡ����Ϣ" << endl << endl;

    string sno;     //ѧ��
    cout << "����ѧ�ţ�";
    cin >> sno;
    sno = "'" + sno + "'";

    //��ѯ
    std::string sql;
    sql = "SELECT * FROM Student WHERE Sno=" + sno + ";";
    int rc = sql_withtable(db, sql);
    if (rc == 0)
    {
        cout << "ѧ�������ڣ�" << endl;
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
        cout << "��ѡ����Ϣ��" << endl;
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

//UTF-8��GB2312��ת��
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

//GB2312��UTF-8��ת��
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