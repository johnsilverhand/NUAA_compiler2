#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

//引用的全局变量
fstream datafile;//中间文件 
string line;   //每一行字符串
char ch;//存放当前字符变量
string strToken;//存放单词变量
int len;  //当前字符串长度
//用于错误处理 
int row;    //排
int index;  //索引，某一行的字符序号
int indexID = 0;//标识符表的索引 
int indexNUM = 0;//常量表索引 
//存放 
vector<string>ID;   //标识符表
vector<int>NUM;     //常数表

string keyword[15] = { "procedure","begin","const","var","end","odd","if","else","then","do","while","read","write","program","call" }; 

//引用的全局过程
int IsKeyword(string strToken){
	//判断是否为关键字，是则返回该关键字在keyword数组的位置+1，返回0则代表着不是 
	int i;
	for(i=0;i<15;i++){
		if (! strToken.compare(keyword[i])){
			i++ ;//位置+1 
			return i; 
		}
	} 
	return 0;
}
void GetChar(){
	//读入下一字符，并将搜索指示器前移
	ch = tolower(line[index]); //获得小写字母
	index++;    //搜索指示器前移一字符位置,可能发生index超出当前字符串的可能，需注意 
}
 
void Retract(){
	//回调，将index-1，置ch为 空白字符
	index--;
	ch = ' '; 
} 
void GetBC(){
	//检查空字符(空格，tab,换行，回车)
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

//课本中的Concat函数将word连接，由于strtoken是string类型直接用内置的+操作符即可 
//实现单词识别词法分析的主过程 
//单词识别 
void GetSYM()
{
	
	while (index <= len - 1)//index从0开始，len为长度； 
	{
		GetChar();
		GetBC();   //得到非空白或换行字符
		if (index > len){ //提前跳出 
			return;
		}
		
		if(IsLetter()) { //开头为字母，除错误外的 可能为标识符<id> → l{l|d} 或关键字
			
			strToken = "";
			while (IsLetter() || IsDigit()){
				//后续若为字母或数字，则一直读入，直至结束
				strToken = strToken + ch;   //将ch加到word尾部 
				GetChar();
			}
			
			Retract();   
			
			int n = IsKeyword(strToken);    //获得其返回值，位置或0

			if (n > 0){
				//说明该单词是关键字
				datafile << strToken << "\t" << n << "\t"<< "~" << endl;
				//datafile << strToken << "\t" << "关键字" << "\t" << n << endl;
			}
			else    //说明该单词为标识符
			{
				vector <string>::iterator iter_str;//建立一个vector <string>迭代器 
				iter_str = find(ID.begin(), ID.end(), strToken);//在标识符表中寻找word 
				if (iter_str != ID.end()){
					//说明当前的strToken为之前已存在的标识符 
					datafile << strToken << "\t" << "标识符" << "\t" << distance(ID.begin(), iter_str) << endl;
				}
				else{
					//说明当前的strToken为新的标识符，将其加入标识表 
					datafile << strToken << "\t" << "标识符" << "\t" << indexID<< endl;
					indexID++;
					ID.push_back(strToken);
					//将一个新的元素加到vector的最后面，位置为当前元素的下一个元素
				}
			}
		}
		else if (isdigit(ch))  //开头为数字，除错误外的可能为整型常量 
		{
			strToken = "";
			//while (isdigit(ch))
			while (IsDigit()) 
			{
				strToken = strToken + ch;
				GetChar();//循环结束时当前部分的字符串全为数字，然后超前搜索一个字符。 
			}
			
			if (IsLetter())//超前搜索的一个字符为字母 
			//if (isalpha(ch))
			{
				//数字后跟字母,错误处理，然后继续读取完整个单词
				while (IsLetter() || IsDigit())
				{
					//index++;
					strToken = strToken + ch;
					GetChar();
				}
				datafile << "error:标识符不规范" << "    第" << row << "行第" << index << "列,错误标识符为："<<strToken << endl;
			}
			else
			{
				Retract();  //回调一个字符
				int num = atoi(strToken.c_str());
				//atoi将为数字的字符串转化为对应整型数num 
				//c_str()函数返回一个指向正规C字符串的指针常量 
				//为了与c语言兼容，在c语言中没有string类型，故必须通过string类对象的成员函数c_str()把string 对象转换成c中的字符串样式。
				vector <int>::iterator iter_digit;
				iter_digit = std::find(NUM.begin(), NUM.end(), num);//查常数表
				if (iter_digit != NUM.end())  //表中找到此常量
				{
					datafile << strToken << "\t" << "常量" << "\t" << distance(NUM.begin(), iter_digit) << endl;
					
				}
				else
				{
					datafile << strToken << "\t" << "常量" << "\t" << indexNUM << endl;
					indexNUM++;
					NUM.push_back(num);
				}
			}
		}
		//第18-33种 
		//运算符 
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
			if (line[index] == '=')  //index始终指向下一个待读字符，相当于超前搜索一个字符了 
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
				datafile << "error:赋值号错误，应为 :=" << "    第" << row << "行第" << index << "列" << endl;
			}
		}
		//界符 
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
		//结束符 
		else if (ch == '.')
		{
			datafile << "." << "\t" << "33" << "\t" << "~" << endl;
			datafile << "END" << endl;
			return ;
		}
		else
		{
			
			datafile << "error:非法字符" << "    第" << row << "行第" << index << "列，非法字符为：" <<ch<< endl;
		}
	}
}
int main()
{
	fstream read_file;
	char filename[100];   //文件名
	cout << "请输入要处理的文件名：";
	cin >> filename;
	read_file.open(filename, ios::in);
	if (read_file.fail())
	{
		cout << "打开输入文件失败\n";
		return 0;
	}
	datafile.open("lex_out.txt", ios::out);//提前创建好 
	if (datafile.fail())
	{
		cout << "输出文件打开失败\n";
		return 0;
	}
	row = 1;//从第一行开始
	while (getline(read_file, line))
	{
		len = line.length();  //当前行字符串长度
		index = 0;
		GetSYM();
		row++;
	}
	cout << "文件处理完毕" << endl;
	read_file.close();
	datafile.close();
	return 0;
}
/*
采用关键字一符一种，运算符一符一种，标识符统归一种，常数只有<integer>一种 
输出到文件为：读取到的单词   单词种别（常量，标识符，其中关键词,运算符和界符直接用序号表示）  属性值（常量表索引，标识符表索引，其中关键词，运算符和界符直接用'~'表示空） 
关键词:
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
	15  call，


	16	NUM,	//常量

	17	ID,	//标识符



	//运算符

	18	ADD,	//'+'

	19	SUB,	//'-'

	20	MUL,	//'*'

	21	DIV,	//'/'

	22	EQUAL,	//'='，

	23	LESS,	//'<'

	24	GREAT,	//'>'

	25	LESSEQUAL,	//'<='

	26  GREATEQUAL,//'>='
	
	27  NOTEQUAL,	//'<>不等于

	
	28  ASSIGN,		//':='   赋值

	



	//界符

	29  SEMICOLON //';'

	30  COMMA	//','

	31 	LPAR,	//'('

	32  RPAR,	//')'


	33	.		//结束符 
*/
