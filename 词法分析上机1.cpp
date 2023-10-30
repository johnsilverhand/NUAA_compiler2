#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

//���õ�ȫ�ֱ���
fstream datafile;//�м��ļ� 
string line;   //ÿһ���ַ���
char ch;//��ŵ�ǰ�ַ�����
string strToken;//��ŵ��ʱ���
int len;  //��ǰ�ַ�������
//���ڴ����� 
int row;    //��
int index;  //������ĳһ�е��ַ����
int indexID = 0;//��ʶ��������� 
int indexNUM = 0;//���������� 
//��� 
vector<string>ID;   //��ʶ����
vector<int>NUM;     //������

string keyword[15] = { "procedure","begin","const","var","end","odd","if","else","then","do","while","read","write","program","call" }; 

//���õ�ȫ�ֹ���
int IsKeyword(string strToken){
	//�ж��Ƿ�Ϊ�ؼ��֣����򷵻ظùؼ�����keyword�����λ��+1������0������Ų��� 
	int i;
	for(i=0;i<15;i++){
		if (! strToken.compare(keyword[i])){
			i++ ;//λ��+1 
			return i; 
		}
	} 
	return 0;
}
void GetChar(){
	//������һ�ַ�����������ָʾ��ǰ��
	ch = tolower(line[index]); //���Сд��ĸ
	index++;    //����ָʾ��ǰ��һ�ַ�λ��,���ܷ���index������ǰ�ַ����Ŀ��ܣ���ע�� 
}
 
void Retract(){
	//�ص�����index-1����chΪ �հ��ַ�
	index--;
	ch = ' '; 
} 
void GetBC(){
	//�����ַ�(�ո�tab,���У��س�)
	while((ch==' '||ch=='\t'||ch=='\n'||ch=='\r')&&(index<len)){
		GetChar(); 
	} 
}

bool IsLetter(){
	if (ch>='A' && ch <='Z'){
		return true;
	}
	else if (ch >='a' && ch<='z'){
		return true;
	}
	else{
		return false;
	}
}
bool IsDigit(){
	if (ch>='0' && ch<='9'){
		return true;
	}
	else{
		return false;
	} 
}

//�α��е�Concat������word���ӣ�����strtoken��string����ֱ�������õ�+���������� 
//ʵ�ֵ���ʶ��ʷ������������� 
//����ʶ�� 
void GetSYM()
{
	
	while (index <= len - 1)//index��0��ʼ��lenΪ���ȣ� 
	{
		GetChar();
		GetBC();   //�õ��ǿհ׻����ַ�
		if (index > len){ //��ǰ���� 
			return;
		}
		
		if(IsLetter()) { //��ͷΪ��ĸ����������� ����Ϊ��ʶ��<id> �� l{l|d} ��ؼ���
			
			strToken = "";
			while (IsLetter() || IsDigit()){
				//������Ϊ��ĸ�����֣���һֱ���룬ֱ������
				strToken = strToken + ch;   //��ch�ӵ�wordβ�� 
				GetChar();
			}
			
			Retract();   
			
			int n = IsKeyword(strToken);    //����䷵��ֵ��λ�û�0

			if (n > 0){
				//˵���õ����ǹؼ���
				datafile << strToken << "\t" << n << "\t"<< "~" << endl;
				//datafile << strToken << "\t" << "�ؼ���" << "\t" << n << endl;
			}
			else    //˵���õ���Ϊ��ʶ��
			{
				vector <string>::iterator iter_str;//����һ��vector <string>������ 
				iter_str = find(ID.begin(), ID.end(), strToken);//�ڱ�ʶ������Ѱ��word 
				if (iter_str != ID.end()){
					//˵����ǰ��strTokenΪ֮ǰ�Ѵ��ڵı�ʶ�� 
					datafile << strToken << "\t" << "��ʶ��" << "\t" << distance(ID.begin(), iter_str) << endl;
				}
				else{
					//˵����ǰ��strTokenΪ�µı�ʶ������������ʶ�� 
					datafile << strToken << "\t" << "��ʶ��" << "\t" << indexID<< endl;
					indexID++;
					ID.push_back(strToken);
					//��һ���µ�Ԫ�ؼӵ�vector������棬λ��Ϊ��ǰԪ�ص���һ��Ԫ��
				}
			}
		}
		else if (isdigit(ch))  //��ͷΪ���֣���������Ŀ���Ϊ���ͳ��� 
		{
			strToken = "";
			//while (isdigit(ch))
			while (IsDigit()) 
			{
				strToken = strToken + ch;
				GetChar();//ѭ������ʱ��ǰ���ֵ��ַ���ȫΪ���֣�Ȼ��ǰ����һ���ַ��� 
			}
			
			if (IsLetter())//��ǰ������һ���ַ�Ϊ��ĸ 
			//if (isalpha(ch))
			{
				//���ֺ����ĸ,������Ȼ�������ȡ����������
				while (IsLetter() || IsDigit())
				{
					//index++;
					strToken = strToken + ch;
					GetChar();
				}
				datafile << "error:��ʶ�����淶" << "    ��" << row << "�е�" << index << "��,�����ʶ��Ϊ��"<<strToken << endl;
			}
			else
			{
				Retract();  //�ص�һ���ַ�
				int num = atoi(strToken.c_str());
				//atoi��Ϊ���ֵ��ַ���ת��Ϊ��Ӧ������num 
				//c_str()��������һ��ָ������C�ַ�����ָ�볣�� 
				//Ϊ����c���Լ��ݣ���c������û��string���ͣ��ʱ���ͨ��string�����ĳ�Ա����c_str()��string ����ת����c�е��ַ�����ʽ��
				vector <int>::iterator iter_digit;
				iter_digit = std::find(NUM.begin(), NUM.end(), num);//�鳣����
				if (iter_digit != NUM.end())  //�����ҵ��˳���
				{
					datafile << strToken << "\t" << "����" << "\t" << distance(NUM.begin(), iter_digit) << endl;
					
				}
				else
				{
					datafile << strToken << "\t" << "����" << "\t" << indexNUM << endl;
					indexNUM++;
					NUM.push_back(num);
				}
			}
		}
		//��18-33�� 
		//����� 
		else if(ch == '+')
		{
			
			datafile << "+" << "\t" << "18" << "\t"<< "~" << endl;
		}
		else if (ch == '-')
		{
			datafile << "-" << "\t" << "19" << "\t" << "~" << endl;
		}
		else if (ch == '*')
		{
			datafile << "*" << "\t" << "20" << "\t" << "~" << endl;
		}
		else if (ch == '/')
		{
			datafile << "/" << "\t" << "21" << "\t" << "~" << endl;
		}
		else if (ch == '=')
		{
			datafile << "=" << "\t" << "22" << "\t" << "~" << endl;
		}
		else if (ch == '<')
		{
			if (line[index] == '=')  //indexʼ��ָ����һ�������ַ����൱�ڳ�ǰ����һ���ַ��� 
			{
				datafile << "<=" << "\t" << "25" << "\t" << "~" << endl;
				index++;
			}
			else if (line[index] == '>')
			{
				datafile << "<>" << "\t" << "27" << "\t" << "~" << endl;
				index++;
			}
			else
			{
				datafile << "<" << "\t" << "23" << "\t" << "~" << endl;
			}
		}
		else if (ch == '>')
		{
			if (line[index] == '=')
			{
				datafile << ">=" << "\t" << "26" << "\t" << "~" << endl;
				index++;
			}
			else
			{
				datafile << ">" << "\t" << "24" << "\t" << "~" << endl;
			}
		}
		else if (ch == ':')
		{
			if (line[index] == '=')
			{
				datafile << ":=" << "\t" << "28" << "\t" << "~" << endl;
				index++;
			}
			else
			{
				datafile << "error:��ֵ�Ŵ���ӦΪ :=" << "    ��" << row << "�е�" << index << "��" << endl;
			}
		}
		//��� 
		else if (ch == ';')
		{
			datafile << ";" << "\t" << "29" << "\t" << "~" << endl;
		}
		else if (ch == ',')
		{
			datafile << "," << "\t" << "30" << "\t" << "~" << endl;
		}
		else if (ch == '(')
		{
			datafile << "(" << "\t" << "31" << "\t" << "~" << endl;
		}
		else if (ch == ')')
		{
			datafile << ")" << "\t" << "32" << "\t" << "~" << endl;
		}
		//������ 
		else if (ch == '.')
		{
			datafile << "." << "\t" << "33" << "\t" << "~" << endl;
			datafile << "END" << endl;
			return ;
		}
		else
		{
			
			datafile << "error:�Ƿ��ַ�" << "    ��" << row << "�е�" << index << "�У��Ƿ��ַ�Ϊ��" <<ch<< endl;
		}
	}
}
int main()
{
	fstream read_file;
	char filename[100];   //�ļ���
	cout << "������Ҫ������ļ�����";
	cin >> filename;
	read_file.open(filename, ios::in);
	if (read_file.fail())
	{
		cout << "�������ļ�ʧ��\n";
		return 0;
	}
	datafile.open("lex_out.txt", ios::out);//��ǰ������ 
	if (datafile.fail())
	{
		cout << "����ļ���ʧ��\n";
		return 0;
	}
	row = 1;//�ӵ�һ�п�ʼ
	while (getline(read_file, line))
	{
		len = line.length();  //��ǰ���ַ�������
		index = 0;
		GetSYM();
		row++;
	}
	cout << "�ļ��������" << endl;
	read_file.close();
	datafile.close();
	return 0;
}
/*
���ùؼ���һ��һ�֣������һ��һ�֣���ʶ��ͳ��һ�֣�����ֻ��<integer>һ�� 
������ļ�Ϊ����ȡ���ĵ���   �����ֱ𣨳�������ʶ�������йؼ���,������ͽ��ֱ������ű�ʾ��  ����ֵ����������������ʶ�������������йؼ��ʣ�������ͽ��ֱ����'~'��ʾ�գ� 
�ؼ���:
	1	procedure,
	2	begin,
	3	const,
	4	var,
	5	end,
	6	odd,	
	7	if,
	8	else,
	9	then,
	10	do,
	11	while,
	12	read,
	13	write,
	14  program,
	15  call��


	16	NUM,	//����

	17	ID,	//��ʶ��



	//�����

	18	ADD,	//'+'

	19	SUB,	//'-'

	20	MUL,	//'*'

	21	DIV,	//'/'

	22	EQUAL,	//'='��

	23	LESS,	//'<'

	24	GREAT,	//'>'

	25	LESSEQUAL,	//'<='

	26  GREATEQUAL,//'>='
	
	27  NOTEQUAL,	//'<>������

	
	28  ASSIGN,		//':='   ��ֵ

	



	//���

	29  SEMICOLON //';'

	30  COMMA	//','

	31 	LPAR,	//'('

	32  RPAR,	//')'


	33	.		//������ 
*/
