#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <algorithm>
#include <set>
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
//语法分析时记录行列 
vector<int> tokenRows;
vector<int> tokenCols;
int currentTokenIndex = -1;


string keyword[15] = { "procedure","begin","const","var","end","odd","if","else","then","do","while","read","write","program","call" }; 
//词法分析函数返回类型 
struct SYM{
	int kind_code;//属性编码,-1为出错的。 
	string value;//属性值 
}; 

SYM returnSYM; 
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
			
			return ;
			//return returnSYM;
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
				returnSYM.kind_code = n;
				returnSYM.value = "~";
				datafile << returnSYM.kind_code<<endl; 
				tokenRows.push_back(row);
				tokenCols.push_back(index);  // 这里假设strToken是当前token的字符串
				//datafile << strToken << "\t" << n << "\t"<< "~" << endl;
				//datafile << strToken << "\t" << "关键字" << "\t" << n << endl;
			}
			else    //说明该单词为标识符
			{
				vector <string>::iterator iter_str;//建立一个vector <string>迭代器 
				iter_str = find(ID.begin(), ID.end(), strToken);//在标识符表中寻找word 
				if (iter_str != ID.end()){
					//说明当前的strToken为之前已存在的标识符 
					//datafile << strToken << "\t" << "标识符" << "\t" << distance(ID.begin(), iter_str) << endl;
					returnSYM.kind_code = 17;
					returnSYM.value = to_string(distance(ID.begin(), iter_str));
					datafile << returnSYM.kind_code<<endl; 
					tokenRows.push_back(row);
					tokenCols.push_back(index);  // 这里假设strToken是当前token的字符串
				}
				else{
					//说明当前的strToken为新的标识符，将其加入标识表 
					returnSYM.kind_code = 17;
					returnSYM.value = to_string(indexID);
					
					datafile << returnSYM.kind_code<<endl; 
					tokenRows.push_back(row);
					tokenCols.push_back(index);  // 这里假设strToken是当前token的字符串
					//datafile << strToken << "\t" << "标识符" << "\t" << indexID<< endl;
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
				returnSYM.kind_code = -1;
				returnSYM.value =  "error:标识符不规范";
				datafile << returnSYM.kind_code<<endl;
				tokenRows.push_back(row);
				tokenCols.push_back(index);  // 这里假设strToken是当前token的字符串 
				//datafile << "error:标识符不规范" << "    第" << row << "行第" << index << "列,错误标识符为："<<strToken << endl;
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
					returnSYM.kind_code = 16;
					returnSYM.value = to_string(distance(NUM.begin(), iter_digit));
					datafile << returnSYM.kind_code<<endl;
					tokenRows.push_back(row);
					tokenCols.push_back(index);  // 这里假设strToken是当前token的字符串 
					//datafile << strToken << "\t" << "常量" << "\t" << distance(NUM.begin(), iter_digit) << endl;
					
				}
				else
				{
					returnSYM.kind_code = 16;
					returnSYM.value = to_string(indexNUM);
					datafile << returnSYM.kind_code<<endl;
					tokenRows.push_back(row);
					tokenCols.push_back(index);  // 这里假设strToken是当前token的字符串 
					//datafile << strToken << "\t" << "常量" << "\t" << indexNUM << endl;
					indexNUM++;
					NUM.push_back(num);
				}
			}
		}
		//第18-33种 
		//运算符 
		else if(ch == '+')
		{
			returnSYM.kind_code = 18;
			returnSYM.value = "~";
			datafile << returnSYM.kind_code<<endl; 
			tokenRows.push_back(row);
			tokenCols.push_back(index);  // 这里假设strToken是当前token的字符串
		//	datafile << "+" << "\t" << "18" << "\t"<< "~" << endl;
		}
		else if (ch == '-')
		{
			returnSYM.kind_code = 19;
			returnSYM.value = "~";
			datafile << returnSYM.kind_code<<endl; 
			tokenRows.push_back(row);
			tokenCols.push_back(index);  // 这里假设strToken是当前token的字符串
		//	datafile << "-" << "\t" << "19" << "\t" << "~" << endl;
		}
		else if (ch == '*')
		{
			returnSYM.kind_code = 20;
			returnSYM.value = "~";
			datafile << returnSYM.kind_code<<endl; 
			tokenRows.push_back(row);
			tokenCols.push_back(index);  // 这里假设strToken是当前token的字符串
		//	datafile << "*" << "\t" << "20" << "\t" << "~" << endl;
		}
		else if (ch == '/')
		{
			returnSYM.kind_code = 21;
			returnSYM.value = "~";
			datafile << returnSYM.kind_code<<endl; 
			tokenRows.push_back(row);
			tokenCols.push_back(index);  // 这里假设strToken是当前token的字符串
			//datafile << "/" << "\t" << "21" << "\t" << "~" << endl;
		}
		else if (ch == '=')
		{
			returnSYM.kind_code = 22;
			returnSYM.value = "~";
			datafile << returnSYM.kind_code<<endl; 
			tokenRows.push_back(row);
			tokenCols.push_back(index);  // 这里假设strToken是当前token的字符串
			//datafile << "=" << "\t" << "22" << "\t" << "~" << endl;
		}
		else if (ch == '<')
		{
			if (line[index] == '=')  //index始终指向下一个待读字符，相当于超前搜索一个字符了 
			{
				returnSYM.kind_code = 25;
				returnSYM.value = "~";
				datafile << returnSYM.kind_code<<endl;
				tokenRows.push_back(row);
				tokenCols.push_back(index);  // 这里假设strToken是当前token的字符串 
				//datafile << "<=" << "\t" << "25" << "\t" << "~" << endl;
				index++;
			}
			else if (line[index] == '>')
			{
				returnSYM.kind_code = 27;
				returnSYM.value = "~";
				datafile << returnSYM.kind_code<<endl; 
				tokenRows.push_back(row);
				tokenCols.push_back(index);  // 这里假设strToken是当前token的字符串
				//datafile << "<>" << "\t" << "27" << "\t" << "~" << endl;
				index++;
			}
			else
			{
				returnSYM.kind_code = 23;
				returnSYM.value = "~";
				datafile << returnSYM.kind_code<<endl;
				tokenRows.push_back(row);
				tokenCols.push_back(index);  // 这里假设strToken是当前token的字符串 
				//datafile << "<" << "\t" << "23" << "\t" << "~" << endl;
			}
		}
		else if (ch == '>')
		{
			if (line[index] == '=')
			{
				returnSYM.kind_code = 26;
				returnSYM.value = "~";
				datafile << returnSYM.kind_code<<endl;
				tokenRows.push_back(row);
				tokenCols.push_back(index);  // 这里假设strToken是当前token的字符串 
				//datafile << ">=" << "\t" << "26" << "\t" << "~" << endl;
				index++;
			}
			else
			{
				returnSYM.kind_code = 24;
				returnSYM.value = "~";
				datafile << returnSYM.kind_code<<endl;
				tokenRows.push_back(row);
				tokenCols.push_back(index);  // 这里假设strToken是当前token的字符串 
				//datafile << ">" << "\t" << "24" << "\t" << "~" << endl;
			}
		}
		else if (ch == ':')
		{
			if (line[index] == '=')
			{
				returnSYM.kind_code = 28;
				returnSYM.value = "~";
				datafile << returnSYM.kind_code<<endl; 
				tokenRows.push_back(row);
				tokenCols.push_back(index);  // 这里假设strToken是当前token的字符串
				//datafile << ":=" << "\t" << "28" << "\t" << "~" << endl;
				index++;
			}
			else
			{
				returnSYM.kind_code = -1;
				returnSYM.value =  "error:赋值号错误，应为 :=";
				datafile << returnSYM.kind_code<<endl;
				tokenRows.push_back(row);
				tokenCols.push_back(index);  // 这里假设strToken是当前token的字符串 
				//datafile << "error:赋值号错误，应为 :=" << "    第" << row << "行第" << index << "列" << endl;
			}
		}
		//界符 
		else if (ch == ';')
		{
			returnSYM.kind_code = 29;
			returnSYM.value = "~";
			datafile << returnSYM.kind_code<<endl;
			tokenRows.push_back(row);
				tokenCols.push_back(index);  // 这里假设strToken是当前token的字符串 
			//datafile << ";" << "\t" << "29" << "\t" << "~" << endl;
		}
		else if (ch == ',')
		{
			returnSYM.kind_code = 30;
			returnSYM.value = "~";
			datafile << returnSYM.kind_code<<endl;
			tokenRows.push_back(row);
				tokenCols.push_back(index);  // 这里假设strToken是当前token的字符串 
			//datafile << "," << "\t" << "30" << "\t" << "~" << endl;
		}
		else if (ch == '(')
		{
			returnSYM.kind_code = 31; 
			returnSYM.value = "~";
			datafile << returnSYM.kind_code<<endl; 
			tokenRows.push_back(row);
				tokenCols.push_back(index);  // 这里假设strToken是当前token的字符串
			//datafile << "(" << "\t" << "31" << "\t" << "~" << endl;
		}
		else if (ch == ')')
		{
			returnSYM.kind_code = 32;
			returnSYM.value = "~";
			datafile << returnSYM.kind_code<<endl; 
			tokenRows.push_back(row);
				tokenCols.push_back(index);  // 这里假设strToken是当前token的字符串
			//datafile << ")" << "\t" << "32" << "\t" << "~" << endl;
		}
		//结束符 
		else if (ch == '.')
		{
			returnSYM.kind_code = 33;
			returnSYM.value = "~";
			//datafile << "." << "\t" << "33" << "\t" << "~" << endl;
			//datafile << "END" << endl;
			datafile << returnSYM.kind_code<<endl; 
			tokenRows.push_back(row);
				tokenCols.push_back(index);  // 这里假设strToken是当前token的字符串
			return ;
			//return returnSYM;
		}
		else
		{
			returnSYM.kind_code = -1;
			returnSYM.value =  "error:非法字符";
			datafile << returnSYM.kind_code<<endl; 
			tokenRows.push_back(row);
				tokenCols.push_back(index);  // 这里假设strToken是当前token的字符串
			//datafile << "error:非法字符" << "    第" << row << "行第" << index << "列，非法字符为：" <<ch<< endl;
		}
	}
	//return returnSYM;
	//datafile << returnSYM.kind_code<<endl; 
	return ;
}

//词法分析相关全局变量 
int next_token_kind_code;
void getNextToken() {
	//GetSYM(); 
	datafile >> next_token_kind_code;
	currentTokenIndex ++;
	row = tokenRows[currentTokenIndex];
	//cout<<row<<endl; 
	index =  tokenCols[currentTokenIndex];
	//cout<<"token recent:"<<next_token_kind_code<<endl; 
    // 调用GetSYM函数，更新next_token_kind_code和其他相关的全局变量
}

void synchronizeToken(set<int> syncTokens) {
	cout<<"开始跳过"<<endl; 
    while (syncTokens.find(next_token_kind_code) == syncTokens.end()) {
        getNextToken();
    }
}
void id() {
	//cout<<"执行id()函数\n";
	//getNextToken();
	
    if (next_token_kind_code == 17) {  // <id>
        
    } else{
    	std::cerr <<"并不是变量"<<endl; 
        // 输出错误信息，并尝试恢复解析,即假设忽略该错误 
    }
}
//词法分析相关函数定义

void prog();    // <prog> → program <id>；<block>
void block();   // <block> → [<condecl>][<vardecl>][<proc>]<body>
void const_();
void condecl(); // <condecl> → const <const>{,<const>};
void vardecl(); // <vardecl> → var <id>{,<id>};
void proc();    // <proc> → procedure <id>（[<id>{,<id>}]）;<block>{;<proc>}
void body();    // <body> → begin <statement>{;<statement>}end
void statement(); // <statement> ...
void lexp();    // <lexp> → <exp> <lop> <exp>|odd <exp>
void lop();
void exp();     // <exp> → [+|-]<term>{<aop><term>}

void factor();  // <factor>→<id>|<integer>|(<exp>) 
void term();    // <term> → <factor>{<mop><factor>}
//主函数 
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
	row = 1;//从第一行开始
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
	
	cout << "词法分析完毕" << endl;
	read_file.close();
	datafile.close();
	
	datafile.open("lex_out.txt", ios::in);//提前创建好 
    prog();
        
    
    
	cout << "文件处理完毕" << endl;
	//read_file.close();
	datafile.close();
	system("pause"); 
	return 0;
}




void prog() {
	//cout<<"执行prog()函数\n";
	//<prog> → program <id>；<block>
    getNextToken();
    //cout<<"token recent:"<<next_token_kind_code<<endl;
    if (next_token_kind_code != 14) { // Expected `program`
        std::cerr << "Error: 程序开头缺少 'program'" << endl;
        // 假设progrm正确 
    }
    getNextToken();
    id(); 
    if (next_token_kind_code != 17) { // Expected id
        
        std::cerr << "Error:在 'program'.之后缺少id" << endl;
        
    }
    //
    getNextToken();
    if (next_token_kind_code != 29) { // Expected ';'
        cerr << "Error: 缺少 ';' ." << endl;
        // 恢复策略: 假设 ';' 没缺少 
        
    }
    
    else{
    	getNextToken();
	} 
    
    block();
    
}

void block() {
	//cout<<"执行block()函数\n";
	//<block> → [<condecl>][<vardecl>][<proc>]<body>
	int expectedSection = 1; // 预期顺序 1 for condecl, 2 for vardecl, 3 for proc
	
    if (next_token_kind_code == 3) {  // "const"
        getNextToken();
        condecl();
        expectedSection = 2;
    }else if (next_token_kind_code != 4 && next_token_kind_code != 5 && next_token_kind_code != 2) {
        // 肯定缺少const,假设其存在寻找其他可能的错误 
        std::cerr << "Error: 缺少 const, var, procedure, 或者 begin 在  "<<row<<"行"<<index<<" 列." << std::endl;
        // 尝试恢复：跳过输入直到找到下一个同步记号
        synchronizeToken({3, 4, 5, 2});  // const, var, procedure, or begin
    }
    if (next_token_kind_code == 4) {  // "var"
        if (expectedSection > 2) {
            std::cerr << "Error: 顺序错误. var 应该在 procedure 之前. 在 " << row << "行" << index << " 列.\n";
        }
        if (expectedSection < 2) {
            std::cerr << "Error: 顺序错误. var 应该在 const 之后. 在 " << row << "行" << index << " 列.\n";
            vardecl();
            getNextToken(); 
            condecl(); 
        }
        else{
        	vardecl();
		}
		//getNextToken();
        
        expectedSection = 3;
    }
    
    while (next_token_kind_code == 1) {  // "procedure"
        if (expectedSection < 3) {
            std::cerr << "Error: 顺序错误. procedure应该在var和const 之后. 在 " << row << "行" << index << " 列.\n";
        }
		//getNextToken();
        proc();
        expectedSection = 3;
    }
    body();
}

void const_() {
	//cout<<"执行const()函数\n";
	//<const> → <id>:=<integer>
    id();
    getNextToken();
    if (next_token_kind_code == 28) {  // ":="
        getNextToken();
        if (next_token_kind_code == 16) {  // <integer>
            getNextToken();
            
        } else {
            //  输出错误信息，并尝试恢复解析跳过直至下个 ; 
            std::cerr<<"常量赋值语句缺少数字,在"<<row<<"行"<<index<<"列"<<endl;
			synchronizeToken({29});  // ; 
        }
    } else {
        // TODO: 输出错误信息，并尝试恢复解析
        std::cerr<<"常量赋值语句缺少赋值符号:=,在"<<row<<"行"<<index<<"列"<<endl;
        synchronizeToken({29,16});  // id  ; 
    }
}
void condecl() {
	//cout<<"执行condecl()函数\n";
	//<condecl> → const <const>{,<const>};
    //if (next_token_kind_code == 3) { // const
        //getNextToken();
        const_();
        while (next_token_kind_code == 30) { // ,
            getNextToken();
            const_();
        }
        if (next_token_kind_code == 29) { // ;
            getNextToken();
        } else {
            std::cerr << "Error: 缺少 ';' 在"<<row<<"行"<<index<<"列" << std::endl;
            synchronizeToken({29,33});  // ; .
        }
    //} 
	/*else {
        std::cerr << "Error: 缺少 'const'在"<<row<<"行"<<index<<"列" << std::endl;
        synchronizeToken({3});  // const
    }
    */
}
//将参数列表的解析移到了一个单独的函数parseParameterList()中，
//这样proc()函数就不需要处理那么多的嵌套逻辑。
void parseParameterList() {
    getNextToken();

    if (next_token_kind_code != 17) { // ID
        std::cerr << "Error: 缺少变量名  在"<<row<<"行"<<index<<"列" << std::endl;
        synchronizeToken({32, 29, 2});  // ), ;, begin
        return;
    }
    getNextToken();

    while (next_token_kind_code == 30) { // ,
        getNextToken();
        if (next_token_kind_code != 17) { // ID
            std::cerr << "Error: 缺少变量名 在"<<row<<"行"<<index<<"列" << std::endl;
            synchronizeToken({32, 29, 2});  // ), ;, begin
            return;
        }
        getNextToken();
    }

    if (next_token_kind_code != 32) { // )
        std::cerr << "Error: 缺少 ')' 在"<<row<<"行"<<index<<"列" << std::endl;
        synchronizeToken({29, 2});  // ;, begin
    } else {
        getNextToken();
    }
}
void proc() {
	//cout<<"执行proc()函数\n";
	//<proc> → procedure <id>（[<id>{,<id>}]）;<block>{;<proc>}
    if (next_token_kind_code != 1) { // procedure
        std::cerr << "Error: expected 'procedure' 在"<<row<<"行"<<index<<"列" << std::endl;
        
    }
    getNextToken();

    if (next_token_kind_code != 17) { // id 
        std::cerr << "Error: 缺少 变量名 在"<<row<<"行"<<index<<"列" << std::endl;
        
    }
    getNextToken();

    if (next_token_kind_code == 31) { // (
        parseParameterList();
    }

    if (next_token_kind_code != 29) { // ;
        std::cerr << "Error: 缺少 ';' 在"<<row<<"行"<<index<<"列" << std::endl;
        
    }
    getNextToken();

    block();
}



void vardecl() {
	//cout<<"执行vardecl()函数\n";
	//<vardecl> → var <id>{,<id>};
    if (next_token_kind_code == 4) { // var
        getNextToken();
        if (next_token_kind_code == 17) {  // ID
            getNextToken();
            while (next_token_kind_code == 30) { // ,
                getNextToken();
                if (next_token_kind_code == 17) {  // ID
                    getNextToken();
                } else {
                    std::cerr << "Error:变量说明语句缺少id 在"<<row<<"行"<<index<<"列" << std::endl;
                    synchronizeToken({29, 5, 1});  // 跳过直至找到  ;, end, procedure
                }
            }
            if (next_token_kind_code == 29) { // ;
                getNextToken();
            } else {
                std::cerr << "Error: 变量说明语句缺少 ';' 在"<<row<<"行"<<index<<"列" << std::endl;
                synchronizeToken({5, 1});  // end, procedure
            }
        } else {
            std::cerr << "Error: 变量说明语句缺少 ID 在"<<row<<"行"<<index<<"列" << std::endl;
            synchronizeToken({29, 5, 1});  // ;, end, procedure
        }
    }
}




void body() {
	//cout<<"执行body()函数\n";
    if (next_token_kind_code == 2) {  // "begin"
        getNextToken();
        statement();
        while (next_token_kind_code == 29) {  // ";"
            getNextToken();
            statement();
        }
        if (next_token_kind_code == 5) {  // "end"
            getNextToken();
        } else {
            // 输出错误信息，并尝试恢复解析,假设跳过 
            
            std::cerr<<"复合语句缺少begin在"<<row<<"行"<<index<<"列"<<endl; 
            synchronizeToken({29, 5, 1,2});  // ;, end, procedure,begin
        }
    } else {
        // 输出错误信息，并尝试恢复解析,假设跳过 
            std::cerr<<"复合语句缺少begin在"<<row<<"行"<<index<<"列"<<endl; 
            synchronizeToken({29, 5, 1,2});  // ;, end, procedure,begin
    }
}




void statement() {
	//cout<<"执行statement()函数\n";
	//getNextToken();
    if (next_token_kind_code == 17) {  // <id>
        getNextToken();
        if (next_token_kind_code == 28) {  // ":="
            getNextToken();
            exp();
        } else {
            std::cerr << "Error: 缺少 ':=' 在赋值语句 在 " << row << " 行 " << index << " 列. \n";
            getNextToken();
            exp();
        }
    }
    else if (next_token_kind_code == 7) {  // "if"
        getNextToken();
        lexp();
        if (next_token_kind_code == 9) {  // "then"
            getNextToken();
            statement();
            if (next_token_kind_code == 8) {  // "else"
                getNextToken();
                statement();
            }
        } else {
            std::cerr << "Error: 缺少 'then' 在 " << row << " 行 " << index << " 列. \n";
            while (next_token_kind_code != 29 && next_token_kind_code != 5 && next_token_kind_code != 33) { // ";" or "end" or "."
                getNextToken();
            }
        }
    }
    else if (next_token_kind_code == 11) {  // "while"
        getNextToken();
        lexp();
        if (next_token_kind_code == 10) {  // "do"
            getNextToken();
            statement();
        } else {
            std::cerr << "Error: 缺少 'do' 在 " << row << " 行 " << index << "列\n";
            while (next_token_kind_code != 29 && next_token_kind_code != 5 && next_token_kind_code != 33) {
                getNextToken();
            }
        }
    }
    else if (next_token_kind_code == 15) {  // "call"
        getNextToken();
        id();
        getNextToken();
        if (next_token_kind_code == 31) {  // "("
            getNextToken();
            exp();
            while (next_token_kind_code == 30) {  // ","
                getNextToken();
                exp();
            }
            if (next_token_kind_code == 32) {  // ")"
                getNextToken();
            } else {
                std::cerr << "Error: 缺少 ')' 在 " << row << " 行 " << index << " 列\n";
                while (next_token_kind_code != 29 && next_token_kind_code != 5 && next_token_kind_code != 33) {
                    getNextToken();
                }
            }
        }
    }
    else if (next_token_kind_code == 12) {  // "read"
        getNextToken();
        if (next_token_kind_code == 31) {  // "("
            getNextToken();
            id();
            getNextToken();
            while (next_token_kind_code == 30) {  // ","
                getNextToken();
                id();
            }
            if (next_token_kind_code == 32) {  // ")"
                getNextToken();
            } else {
                std::cerr << "Error: 未找到 ')' 在 " << row << " 行 " << index << " 列. \n";
                while (next_token_kind_code != 29 && next_token_kind_code != 5 && next_token_kind_code != 33) {
                    getNextToken();
                }
            }
        }
    }
    else if (next_token_kind_code == 13) {  // "write"
        getNextToken();
        if (next_token_kind_code == 31) {  // "("
            getNextToken();
            exp();
            while (next_token_kind_code == 30) {  // ","
                getNextToken();
                exp();
            }
            if (next_token_kind_code == 32) {  // ")"
                getNextToken();
            } else {
                std::cerr << "缺少 ')' at " << row << " 行 " << index << " 列. \n";
                while (next_token_kind_code != 29 && next_token_kind_code != 5 && next_token_kind_code != 33 ) {
                    getNextToken();
                }
            }
        }
    }
    else if (next_token_kind_code == 2) {  // "begin"
	    getNextToken();
	    statement();
	
	    // 循环直到找到"end"或文件结束
	    while (next_token_kind_code != 5 && next_token_kind_code != 33) {  // "end" or "."
	        if (next_token_kind_code == 29) {  // ";"
	            getNextToken();
	            statement();
	        } else {
	            std::cerr << "Error: 缺少 ';' 在 " << row << " 行 " << index << " 列. \n";
	            synchronizeToken({29, 5});  // ";", "end"
	            if (next_token_kind_code != 5) {
	                getNextToken();
	                statement();
	            }
	        }
	    }
	
	    if (next_token_kind_code == 5) {  // "end"
	        getNextToken();
	        return;
	    }
	}
    else {
        std::cerr << "Error: 未找到 id,if,while,call,begin,read,write在statement开头 在 " << row << " 行 " << index << " 列. \n";
        synchronizeToken({29,5,33}); //; end .
		
    }
}


void exp() {
	//cout<<"执行exp()函数\n";
    // Check if the next token is '+' or '-'
    if (next_token_kind_code == 18 || next_token_kind_code == 19) {  // '+' or '-'
        getNextToken();
    }
    term();
    while (next_token_kind_code == 18 || next_token_kind_code == 19) {  // '+' or '-'
        getNextToken();
        term();
    }
    
    if (next_token_kind_code >= 22 && next_token_kind_code <= 27) {  // if token is a logical operator
        lop();
        exp();
    }
}


void term() {
	//cout<<"执行term()函数\n";
    factor();
    while (next_token_kind_code == 20 || next_token_kind_code == 21) {  // '*' or '/'
        getNextToken();
        factor();
    }
}


void factor() {
	//cout<<"执行factor()函数\n";
    if (next_token_kind_code == 17) {  // <id>
        getNextToken();
    }
    else if (next_token_kind_code == 16) {  // <integer>
        getNextToken();
    }
    else if (next_token_kind_code == 31) {  // '('
        getNextToken();
        exp();
        if (next_token_kind_code == 32) {  // ')'
            getNextToken();
        } else {
            // 输出错误信息
            std::cerr << "Error: 缺少 ')' 在"<<row<<"行"<<index<<"列"<< std::endl;
            // 尝试恢复：跳过输入直到找到一个同步记号
            synchronizeToken({32, 29});  // ')' or ';'
        }
    } else {
        // 输出错误信息
        std::cerr << "Error: 缺少 <id>, <integer>, or '(' 在"<<row<<"行"<<index<<"列" << std::endl;
        // 尝试恢复：跳过输入直到找到一个同步记号
        synchronizeToken({17, 16, 31, 29});  // <id>, <integer>, '(', or ';'
    }
}


void lexp() {
	//cout<<"执行lexp()函数\n";
	if (next_token_kind_code == 6){
		getNextToken();
		exp();
	}
	else{
	    exp();
	    if (next_token_kind_code >= 22 &&next_token_kind_code <= 27) { // 如果是关系运算符
	        getNextToken();
	        exp();
	    }else if(next_token_kind_code == 2||next_token_kind_code == 9||next_token_kind_code == 10||next_token_kind_code == 29) {
	    	
		}
		else {
			cout<<next_token_kind_code;
	        std::cerr << "Error: lexp()中缺少关系运算符 在"<<row<<"行"<<index<<"列" << std::endl;
	        synchronizeToken({9, 10, 29});  // then, do, ;
	    }
	}
}


void lop() {
	//cout<<"执行lop()函数\n";
    // If it's a logical operator, advance to the next token.
    if (next_token_kind_code >= 22 && next_token_kind_code <= 27) {
        getNextToken();
    } else {
        std::cerr << "Error: 缺少逻辑运算符，在 " 
                  << row << " 行 " <<index << " 列. \n";
        // Error Recovery: Skip until a relevant separator or keyword is found, or until the end of the file.
        synchronizeToken({33, 5, 29});//： end . 
        
    }
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
