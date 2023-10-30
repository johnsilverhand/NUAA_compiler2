#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <algorithm>
#include <set>
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
//�﷨����ʱ��¼���� 
vector<int> tokenRows;
vector<int> tokenCols;
int currentTokenIndex = -1;


string keyword[15] = { "procedure","begin","const","var","end","odd","if","else","then","do","while","read","write","program","call" }; 
//�ʷ����������������� 
struct SYM{
	int kind_code;//���Ա���,-1Ϊ����ġ� 
	string value;//����ֵ 
}; 

SYM returnSYM; 
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
			
			return ;
			//return returnSYM;
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
				returnSYM.kind_code = n;
				returnSYM.value = "~";
				datafile << returnSYM.kind_code<<endl; 
				tokenRows.push_back(row);
				tokenCols.push_back(index);  // �������strToken�ǵ�ǰtoken���ַ���
				//datafile << strToken << "\t" << n << "\t"<< "~" << endl;
				//datafile << strToken << "\t" << "�ؼ���" << "\t" << n << endl;
			}
			else    //˵���õ���Ϊ��ʶ��
			{
				vector <string>::iterator iter_str;//����һ��vector <string>������ 
				iter_str = find(ID.begin(), ID.end(), strToken);//�ڱ�ʶ������Ѱ��word 
				if (iter_str != ID.end()){
					//˵����ǰ��strTokenΪ֮ǰ�Ѵ��ڵı�ʶ�� 
					//datafile << strToken << "\t" << "��ʶ��" << "\t" << distance(ID.begin(), iter_str) << endl;
					returnSYM.kind_code = 17;
					returnSYM.value = to_string(distance(ID.begin(), iter_str));
					datafile << returnSYM.kind_code<<endl; 
					tokenRows.push_back(row);
					tokenCols.push_back(index);  // �������strToken�ǵ�ǰtoken���ַ���
				}
				else{
					//˵����ǰ��strTokenΪ�µı�ʶ������������ʶ�� 
					returnSYM.kind_code = 17;
					returnSYM.value = to_string(indexID);
					
					datafile << returnSYM.kind_code<<endl; 
					tokenRows.push_back(row);
					tokenCols.push_back(index);  // �������strToken�ǵ�ǰtoken���ַ���
					//datafile << strToken << "\t" << "��ʶ��" << "\t" << indexID<< endl;
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
				returnSYM.kind_code = -1;
				returnSYM.value =  "error:��ʶ�����淶";
				datafile << returnSYM.kind_code<<endl;
				tokenRows.push_back(row);
				tokenCols.push_back(index);  // �������strToken�ǵ�ǰtoken���ַ��� 
				//datafile << "error:��ʶ�����淶" << "    ��" << row << "�е�" << index << "��,�����ʶ��Ϊ��"<<strToken << endl;
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
					returnSYM.kind_code = 16;
					returnSYM.value = to_string(distance(NUM.begin(), iter_digit));
					datafile << returnSYM.kind_code<<endl;
					tokenRows.push_back(row);
					tokenCols.push_back(index);  // �������strToken�ǵ�ǰtoken���ַ��� 
					//datafile << strToken << "\t" << "����" << "\t" << distance(NUM.begin(), iter_digit) << endl;
					
				}
				else
				{
					returnSYM.kind_code = 16;
					returnSYM.value = to_string(indexNUM);
					datafile << returnSYM.kind_code<<endl;
					tokenRows.push_back(row);
					tokenCols.push_back(index);  // �������strToken�ǵ�ǰtoken���ַ��� 
					//datafile << strToken << "\t" << "����" << "\t" << indexNUM << endl;
					indexNUM++;
					NUM.push_back(num);
				}
			}
		}
		//��18-33�� 
		//����� 
		else if(ch == '+')
		{
			returnSYM.kind_code = 18;
			returnSYM.value = "~";
			datafile << returnSYM.kind_code<<endl; 
			tokenRows.push_back(row);
			tokenCols.push_back(index);  // �������strToken�ǵ�ǰtoken���ַ���
		//	datafile << "+" << "\t" << "18" << "\t"<< "~" << endl;
		}
		else if (ch == '-')
		{
			returnSYM.kind_code = 19;
			returnSYM.value = "~";
			datafile << returnSYM.kind_code<<endl; 
			tokenRows.push_back(row);
			tokenCols.push_back(index);  // �������strToken�ǵ�ǰtoken���ַ���
		//	datafile << "-" << "\t" << "19" << "\t" << "~" << endl;
		}
		else if (ch == '*')
		{
			returnSYM.kind_code = 20;
			returnSYM.value = "~";
			datafile << returnSYM.kind_code<<endl; 
			tokenRows.push_back(row);
			tokenCols.push_back(index);  // �������strToken�ǵ�ǰtoken���ַ���
		//	datafile << "*" << "\t" << "20" << "\t" << "~" << endl;
		}
		else if (ch == '/')
		{
			returnSYM.kind_code = 21;
			returnSYM.value = "~";
			datafile << returnSYM.kind_code<<endl; 
			tokenRows.push_back(row);
			tokenCols.push_back(index);  // �������strToken�ǵ�ǰtoken���ַ���
			//datafile << "/" << "\t" << "21" << "\t" << "~" << endl;
		}
		else if (ch == '=')
		{
			returnSYM.kind_code = 22;
			returnSYM.value = "~";
			datafile << returnSYM.kind_code<<endl; 
			tokenRows.push_back(row);
			tokenCols.push_back(index);  // �������strToken�ǵ�ǰtoken���ַ���
			//datafile << "=" << "\t" << "22" << "\t" << "~" << endl;
		}
		else if (ch == '<')
		{
			if (line[index] == '=')  //indexʼ��ָ����һ�������ַ����൱�ڳ�ǰ����һ���ַ��� 
			{
				returnSYM.kind_code = 25;
				returnSYM.value = "~";
				datafile << returnSYM.kind_code<<endl;
				tokenRows.push_back(row);
				tokenCols.push_back(index);  // �������strToken�ǵ�ǰtoken���ַ��� 
				//datafile << "<=" << "\t" << "25" << "\t" << "~" << endl;
				index++;
			}
			else if (line[index] == '>')
			{
				returnSYM.kind_code = 27;
				returnSYM.value = "~";
				datafile << returnSYM.kind_code<<endl; 
				tokenRows.push_back(row);
				tokenCols.push_back(index);  // �������strToken�ǵ�ǰtoken���ַ���
				//datafile << "<>" << "\t" << "27" << "\t" << "~" << endl;
				index++;
			}
			else
			{
				returnSYM.kind_code = 23;
				returnSYM.value = "~";
				datafile << returnSYM.kind_code<<endl;
				tokenRows.push_back(row);
				tokenCols.push_back(index);  // �������strToken�ǵ�ǰtoken���ַ��� 
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
				tokenCols.push_back(index);  // �������strToken�ǵ�ǰtoken���ַ��� 
				//datafile << ">=" << "\t" << "26" << "\t" << "~" << endl;
				index++;
			}
			else
			{
				returnSYM.kind_code = 24;
				returnSYM.value = "~";
				datafile << returnSYM.kind_code<<endl;
				tokenRows.push_back(row);
				tokenCols.push_back(index);  // �������strToken�ǵ�ǰtoken���ַ��� 
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
				tokenCols.push_back(index);  // �������strToken�ǵ�ǰtoken���ַ���
				//datafile << ":=" << "\t" << "28" << "\t" << "~" << endl;
				index++;
			}
			else
			{
				returnSYM.kind_code = -1;
				returnSYM.value =  "error:��ֵ�Ŵ���ӦΪ :=";
				datafile << returnSYM.kind_code<<endl;
				tokenRows.push_back(row);
				tokenCols.push_back(index);  // �������strToken�ǵ�ǰtoken���ַ��� 
				//datafile << "error:��ֵ�Ŵ���ӦΪ :=" << "    ��" << row << "�е�" << index << "��" << endl;
			}
		}
		//��� 
		else if (ch == ';')
		{
			returnSYM.kind_code = 29;
			returnSYM.value = "~";
			datafile << returnSYM.kind_code<<endl;
			tokenRows.push_back(row);
				tokenCols.push_back(index);  // �������strToken�ǵ�ǰtoken���ַ��� 
			//datafile << ";" << "\t" << "29" << "\t" << "~" << endl;
		}
		else if (ch == ',')
		{
			returnSYM.kind_code = 30;
			returnSYM.value = "~";
			datafile << returnSYM.kind_code<<endl;
			tokenRows.push_back(row);
				tokenCols.push_back(index);  // �������strToken�ǵ�ǰtoken���ַ��� 
			//datafile << "," << "\t" << "30" << "\t" << "~" << endl;
		}
		else if (ch == '(')
		{
			returnSYM.kind_code = 31; 
			returnSYM.value = "~";
			datafile << returnSYM.kind_code<<endl; 
			tokenRows.push_back(row);
				tokenCols.push_back(index);  // �������strToken�ǵ�ǰtoken���ַ���
			//datafile << "(" << "\t" << "31" << "\t" << "~" << endl;
		}
		else if (ch == ')')
		{
			returnSYM.kind_code = 32;
			returnSYM.value = "~";
			datafile << returnSYM.kind_code<<endl; 
			tokenRows.push_back(row);
				tokenCols.push_back(index);  // �������strToken�ǵ�ǰtoken���ַ���
			//datafile << ")" << "\t" << "32" << "\t" << "~" << endl;
		}
		//������ 
		else if (ch == '.')
		{
			returnSYM.kind_code = 33;
			returnSYM.value = "~";
			//datafile << "." << "\t" << "33" << "\t" << "~" << endl;
			//datafile << "END" << endl;
			datafile << returnSYM.kind_code<<endl; 
			tokenRows.push_back(row);
				tokenCols.push_back(index);  // �������strToken�ǵ�ǰtoken���ַ���
			return ;
			//return returnSYM;
		}
		else
		{
			returnSYM.kind_code = -1;
			returnSYM.value =  "error:�Ƿ��ַ�";
			datafile << returnSYM.kind_code<<endl; 
			tokenRows.push_back(row);
				tokenCols.push_back(index);  // �������strToken�ǵ�ǰtoken���ַ���
			//datafile << "error:�Ƿ��ַ�" << "    ��" << row << "�е�" << index << "�У��Ƿ��ַ�Ϊ��" <<ch<< endl;
		}
	}
	//return returnSYM;
	//datafile << returnSYM.kind_code<<endl; 
	return ;
}

//�ʷ��������ȫ�ֱ��� 
int next_token_kind_code;
void getNextToken() {
	//GetSYM(); 
	datafile >> next_token_kind_code;
	currentTokenIndex ++;
	row = tokenRows[currentTokenIndex];
	//cout<<row<<endl; 
	index =  tokenCols[currentTokenIndex];
	//cout<<"token recent:"<<next_token_kind_code<<endl; 
    // ����GetSYM����������next_token_kind_code��������ص�ȫ�ֱ���
}

void synchronizeToken(set<int> syncTokens) {
	cout<<"��ʼ����"<<endl; 
    while (syncTokens.find(next_token_kind_code) == syncTokens.end()) {
        getNextToken();
    }
}
void id() {
	//cout<<"ִ��id()����\n";
	//getNextToken();
	
    if (next_token_kind_code == 17) {  // <id>
        
    } else{
    	std::cerr <<"�����Ǳ���"<<endl; 
        // ���������Ϣ�������Իָ�����,��������Ըô��� 
    }
}
//�ʷ�������غ�������

void prog();    // <prog> �� program <id>��<block>
void block();   // <block> �� [<condecl>][<vardecl>][<proc>]<body>
void const_();
void condecl(); // <condecl> �� const <const>{,<const>};
void vardecl(); // <vardecl> �� var <id>{,<id>};
void proc();    // <proc> �� procedure <id>��[<id>{,<id>}]��;<block>{;<proc>}
void body();    // <body> �� begin <statement>{;<statement>}end
void statement(); // <statement> ...
void lexp();    // <lexp> �� <exp> <lop> <exp>|odd <exp>
void lop();
void exp();     // <exp> �� [+|-]<term>{<aop><term>}

void factor();  // <factor>��<id>|<integer>|(<exp>) 
void term();    // <term> �� <factor>{<mop><factor>}
//������ 
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
	row = 1;//�ӵ�һ�п�ʼ
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
	
	cout << "�ʷ��������" << endl;
	read_file.close();
	datafile.close();
	
	datafile.open("lex_out.txt", ios::in);//��ǰ������ 
    prog();
        
    
    
	cout << "�ļ��������" << endl;
	//read_file.close();
	datafile.close();
	system("pause"); 
	return 0;
}




void prog() {
	//cout<<"ִ��prog()����\n";
	//<prog> �� program <id>��<block>
    getNextToken();
    //cout<<"token recent:"<<next_token_kind_code<<endl;
    if (next_token_kind_code != 14) { // Expected `program`
        std::cerr << "Error: ����ͷȱ�� 'program'" << endl;
        // ����progrm��ȷ 
    }
    getNextToken();
    id(); 
    if (next_token_kind_code != 17) { // Expected id
        
        std::cerr << "Error:�� 'program'.֮��ȱ��id" << endl;
        
    }
    //
    getNextToken();
    if (next_token_kind_code != 29) { // Expected ';'
        cerr << "Error: ȱ�� ';' ." << endl;
        // �ָ�����: ���� ';' ûȱ�� 
        
    }
    
    else{
    	getNextToken();
	} 
    
    block();
    
}

void block() {
	//cout<<"ִ��block()����\n";
	//<block> �� [<condecl>][<vardecl>][<proc>]<body>
	int expectedSection = 1; // Ԥ��˳�� 1 for condecl, 2 for vardecl, 3 for proc
	
    if (next_token_kind_code == 3) {  // "const"
        getNextToken();
        condecl();
        expectedSection = 2;
    }else if (next_token_kind_code != 4 && next_token_kind_code != 5 && next_token_kind_code != 2) {
        // �϶�ȱ��const,���������Ѱ���������ܵĴ��� 
        std::cerr << "Error: ȱ�� const, var, procedure, ���� begin ��  "<<row<<"��"<<index<<" ��." << std::endl;
        // ���Իָ�����������ֱ���ҵ���һ��ͬ���Ǻ�
        synchronizeToken({3, 4, 5, 2});  // const, var, procedure, or begin
    }
    if (next_token_kind_code == 4) {  // "var"
        if (expectedSection > 2) {
            std::cerr << "Error: ˳�����. var Ӧ���� procedure ֮ǰ. �� " << row << "��" << index << " ��.\n";
        }
        if (expectedSection < 2) {
            std::cerr << "Error: ˳�����. var Ӧ���� const ֮��. �� " << row << "��" << index << " ��.\n";
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
            std::cerr << "Error: ˳�����. procedureӦ����var��const ֮��. �� " << row << "��" << index << " ��.\n";
        }
		//getNextToken();
        proc();
        expectedSection = 3;
    }
    body();
}

void const_() {
	//cout<<"ִ��const()����\n";
	//<const> �� <id>:=<integer>
    id();
    getNextToken();
    if (next_token_kind_code == 28) {  // ":="
        getNextToken();
        if (next_token_kind_code == 16) {  // <integer>
            getNextToken();
            
        } else {
            //  ���������Ϣ�������Իָ���������ֱ���¸� ; 
            std::cerr<<"������ֵ���ȱ������,��"<<row<<"��"<<index<<"��"<<endl;
			synchronizeToken({29});  // ; 
        }
    } else {
        // TODO: ���������Ϣ�������Իָ�����
        std::cerr<<"������ֵ���ȱ�ٸ�ֵ����:=,��"<<row<<"��"<<index<<"��"<<endl;
        synchronizeToken({29,16});  // id  ; 
    }
}
void condecl() {
	//cout<<"ִ��condecl()����\n";
	//<condecl> �� const <const>{,<const>};
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
            std::cerr << "Error: ȱ�� ';' ��"<<row<<"��"<<index<<"��" << std::endl;
            synchronizeToken({29,33});  // ; .
        }
    //} 
	/*else {
        std::cerr << "Error: ȱ�� 'const'��"<<row<<"��"<<index<<"��" << std::endl;
        synchronizeToken({3});  // const
    }
    */
}
//�������б�Ľ����Ƶ���һ�������ĺ���parseParameterList()�У�
//����proc()�����Ͳ���Ҫ������ô���Ƕ���߼���
void parseParameterList() {
    getNextToken();

    if (next_token_kind_code != 17) { // ID
        std::cerr << "Error: ȱ�ٱ�����  ��"<<row<<"��"<<index<<"��" << std::endl;
        synchronizeToken({32, 29, 2});  // ), ;, begin
        return;
    }
    getNextToken();

    while (next_token_kind_code == 30) { // ,
        getNextToken();
        if (next_token_kind_code != 17) { // ID
            std::cerr << "Error: ȱ�ٱ����� ��"<<row<<"��"<<index<<"��" << std::endl;
            synchronizeToken({32, 29, 2});  // ), ;, begin
            return;
        }
        getNextToken();
    }

    if (next_token_kind_code != 32) { // )
        std::cerr << "Error: ȱ�� ')' ��"<<row<<"��"<<index<<"��" << std::endl;
        synchronizeToken({29, 2});  // ;, begin
    } else {
        getNextToken();
    }
}
void proc() {
	//cout<<"ִ��proc()����\n";
	//<proc> �� procedure <id>��[<id>{,<id>}]��;<block>{;<proc>}
    if (next_token_kind_code != 1) { // procedure
        std::cerr << "Error: expected 'procedure' ��"<<row<<"��"<<index<<"��" << std::endl;
        
    }
    getNextToken();

    if (next_token_kind_code != 17) { // id 
        std::cerr << "Error: ȱ�� ������ ��"<<row<<"��"<<index<<"��" << std::endl;
        
    }
    getNextToken();

    if (next_token_kind_code == 31) { // (
        parseParameterList();
    }

    if (next_token_kind_code != 29) { // ;
        std::cerr << "Error: ȱ�� ';' ��"<<row<<"��"<<index<<"��" << std::endl;
        
    }
    getNextToken();

    block();
}



void vardecl() {
	//cout<<"ִ��vardecl()����\n";
	//<vardecl> �� var <id>{,<id>};
    if (next_token_kind_code == 4) { // var
        getNextToken();
        if (next_token_kind_code == 17) {  // ID
            getNextToken();
            while (next_token_kind_code == 30) { // ,
                getNextToken();
                if (next_token_kind_code == 17) {  // ID
                    getNextToken();
                } else {
                    std::cerr << "Error:����˵�����ȱ��id ��"<<row<<"��"<<index<<"��" << std::endl;
                    synchronizeToken({29, 5, 1});  // ����ֱ���ҵ�  ;, end, procedure
                }
            }
            if (next_token_kind_code == 29) { // ;
                getNextToken();
            } else {
                std::cerr << "Error: ����˵�����ȱ�� ';' ��"<<row<<"��"<<index<<"��" << std::endl;
                synchronizeToken({5, 1});  // end, procedure
            }
        } else {
            std::cerr << "Error: ����˵�����ȱ�� ID ��"<<row<<"��"<<index<<"��" << std::endl;
            synchronizeToken({29, 5, 1});  // ;, end, procedure
        }
    }
}




void body() {
	//cout<<"ִ��body()����\n";
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
            // ���������Ϣ�������Իָ�����,�������� 
            
            std::cerr<<"�������ȱ��begin��"<<row<<"��"<<index<<"��"<<endl; 
            synchronizeToken({29, 5, 1,2});  // ;, end, procedure,begin
        }
    } else {
        // ���������Ϣ�������Իָ�����,�������� 
            std::cerr<<"�������ȱ��begin��"<<row<<"��"<<index<<"��"<<endl; 
            synchronizeToken({29, 5, 1,2});  // ;, end, procedure,begin
    }
}




void statement() {
	//cout<<"ִ��statement()����\n";
	//getNextToken();
    if (next_token_kind_code == 17) {  // <id>
        getNextToken();
        if (next_token_kind_code == 28) {  // ":="
            getNextToken();
            exp();
        } else {
            std::cerr << "Error: ȱ�� ':=' �ڸ�ֵ��� �� " << row << " �� " << index << " ��. \n";
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
            std::cerr << "Error: ȱ�� 'then' �� " << row << " �� " << index << " ��. \n";
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
            std::cerr << "Error: ȱ�� 'do' �� " << row << " �� " << index << "��\n";
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
                std::cerr << "Error: ȱ�� ')' �� " << row << " �� " << index << " ��\n";
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
                std::cerr << "Error: δ�ҵ� ')' �� " << row << " �� " << index << " ��. \n";
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
                std::cerr << "ȱ�� ')' at " << row << " �� " << index << " ��. \n";
                while (next_token_kind_code != 29 && next_token_kind_code != 5 && next_token_kind_code != 33 ) {
                    getNextToken();
                }
            }
        }
    }
    else if (next_token_kind_code == 2) {  // "begin"
	    getNextToken();
	    statement();
	
	    // ѭ��ֱ���ҵ�"end"���ļ�����
	    while (next_token_kind_code != 5 && next_token_kind_code != 33) {  // "end" or "."
	        if (next_token_kind_code == 29) {  // ";"
	            getNextToken();
	            statement();
	        } else {
	            std::cerr << "Error: ȱ�� ';' �� " << row << " �� " << index << " ��. \n";
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
        std::cerr << "Error: δ�ҵ� id,if,while,call,begin,read,write��statement��ͷ �� " << row << " �� " << index << " ��. \n";
        synchronizeToken({29,5,33}); //; end .
		
    }
}


void exp() {
	//cout<<"ִ��exp()����\n";
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
	//cout<<"ִ��term()����\n";
    factor();
    while (next_token_kind_code == 20 || next_token_kind_code == 21) {  // '*' or '/'
        getNextToken();
        factor();
    }
}


void factor() {
	//cout<<"ִ��factor()����\n";
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
            // ���������Ϣ
            std::cerr << "Error: ȱ�� ')' ��"<<row<<"��"<<index<<"��"<< std::endl;
            // ���Իָ�����������ֱ���ҵ�һ��ͬ���Ǻ�
            synchronizeToken({32, 29});  // ')' or ';'
        }
    } else {
        // ���������Ϣ
        std::cerr << "Error: ȱ�� <id>, <integer>, or '(' ��"<<row<<"��"<<index<<"��" << std::endl;
        // ���Իָ�����������ֱ���ҵ�һ��ͬ���Ǻ�
        synchronizeToken({17, 16, 31, 29});  // <id>, <integer>, '(', or ';'
    }
}


void lexp() {
	//cout<<"ִ��lexp()����\n";
	if (next_token_kind_code == 6){
		getNextToken();
		exp();
	}
	else{
	    exp();
	    if (next_token_kind_code >= 22 &&next_token_kind_code <= 27) { // ����ǹ�ϵ�����
	        getNextToken();
	        exp();
	    }else if(next_token_kind_code == 2||next_token_kind_code == 9||next_token_kind_code == 10||next_token_kind_code == 29) {
	    	
		}
		else {
			cout<<next_token_kind_code;
	        std::cerr << "Error: lexp()��ȱ�ٹ�ϵ����� ��"<<row<<"��"<<index<<"��" << std::endl;
	        synchronizeToken({9, 10, 29});  // then, do, ;
	    }
	}
}


void lop() {
	//cout<<"ִ��lop()����\n";
    // If it's a logical operator, advance to the next token.
    if (next_token_kind_code >= 22 && next_token_kind_code <= 27) {
        getNextToken();
    } else {
        std::cerr << "Error: ȱ���߼���������� " 
                  << row << " �� " <<index << " ��. \n";
        // Error Recovery: Skip until a relevant separator or keyword is found, or until the end of the file.
        synchronizeToken({33, 5, 29});//�� end . 
        
    }
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
