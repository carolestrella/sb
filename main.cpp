/*
Carolina Estrella Machado
180074792
*/
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <map>
#include <vector>

using namespace std;

map<string,vector<int> > instructions; //map com tabela de instruções
map<string,vector<int> > directives; //map com tabela de directivas

vector<int> values; //valor de cada linha
map<string,int> symbolTable; //map com símbolo e valor da linha correspondente
map<string,int> defTable;
map<string,int> usoTable;
map<string,string> equValue;
vector<int> output;
vector<string> externTable;
vector<string> publicTable;

bool haveSectionText = false;

//retorna número a partir de string
int getNumber(string s){
    if(s.size() == 1) return s[0]-'0';
    return 10*(s[0]-'0')+(s[1]-'0');
}

//retorna número a partir de hexadecimal
unsigned int getHexNumber(string hexadecimal){
    return std::stoul(hexadecimal, nullptr, 16);
}

void secondParsing(string fileName){
    string line;
    ifstream file(fileName);
    if(file.is_open()){
        while(getline(file,line)){
            bool flag = false;
            int beginSymbol;
            int sectionInit = line.find("SECAO TEXTO");
            if(sectionInit != -1) continue;
            int sectionDataInit = line.find("SECAO DADOS");
            if(sectionDataInit != -1) continue;

            int isCONST = line.find("CONST");
            if(isCONST != -1){
                int j = isCONST + 6;
                string num;
                bool isHex = false;
                for(int i = j; i < line.size(); i++){
                    if(line[i] == 'X') isHex = true;
                    num += line[i];
                }
                if(num[0] == '-'){
                    num.erase(num.begin());
                    output.push_back(-getNumber(num));
                }
                else if(isHex) output.push_back(getHexNumber(num));
                else output.push_back(getNumber(num));
                continue;
            }
            int isSPACE = line.find("SPACE");
            if(isSPACE != -1) {output.push_back(0); continue;}

            bool findJMP = false, findJMPN = false, findJMPP = false, findJMPZ = false;
            int l;
            l = line.find("JMP");
            if(l != -1) {findJMP = true; flag = true;}
            l = line.find("JMPN");
            if(l != -1) {findJMP = false; findJMPN = true; flag = true;}
            l = line.find("JMPP");
            if(l != -1) {findJMP = false; findJMPP = true; flag = true;}
            l = line.find("JMPZ");
            if(l != -1) {findJMP = false; findJMPZ = true; flag = true;}
            if(flag){
                if(findJMP){
                    output.push_back(5);

                    beginSymbol = 4;
                    string s;
                    for(int i = beginSymbol; i < line.size(); i++){
                        s += line[i];
                    }

                    for(auto symbol : symbolTable){
                        if(symbol.first == s) output.push_back(symbol.second);
                    }
                }
                else{
                    if(findJMPN) output.push_back(6);
                    else if(findJMPP) output.push_back(7);
                    else if(findJMPZ) output.push_back(8);

                    beginSymbol = 5;
                    string s;
                    for(int i = beginSymbol; i < line.size(); i++){
                        s += line[i];
                    }

                    for(auto symbol : symbolTable){
                        if(symbol.first == s) output.push_back(symbol.second);
                    }
                }
            }

            bool copyInstr = false;
            if(!flag){
                for(auto instruction : instructions){
                    int init = line.find(instruction.first);
                    if(init != -1){
                        output.push_back(instruction.second[1]);
                        beginSymbol = init+instruction.first.size()+1;
                        if(instruction.first == "COPY") copyInstr = true;
                    }
                }

                if(copyInstr){
                    string firstOp, secondOp;
                    int v = line.find(",");
                    for(int i = beginSymbol; i < v; i++){
                        firstOp += line[i];
                    }

                    for(int i = v+2; i < line.size(); i++){
                        secondOp += line[i];
                    }

                    for(auto symbol : symbolTable){
                        if(symbol.first == firstOp) output.push_back(symbol.second);
                    }
                    for(auto symbol : symbolTable){
                        if(symbol.first == secondOp) output.push_back(symbol.second);
                    }
                }
                else{
                    string s;
                    int signalInit = -1;
                    for(int i = beginSymbol; i < line.size(); i++){
                        s += line[i];
                        if(line[i] == '+' or line[i] == '-') signalInit = i;
                    }
                    if(signalInit != -1){
                        int operand = line[signalInit];
                        string num;
                        for(int i = signalInit+1; i < line.size(); i++){
                            num += line[i];
                        }
                        s.erase();
                        for(int i = beginSymbol; i < signalInit; i++){
                            s += line[i];
                        }
                        for(auto symbol : symbolTable){
                            if(symbol.first == s){
                                if(operand == '+') output.push_back(symbol.second+getNumber(num));
                                else if(operand == '-') output.push_back(symbol.second-getNumber(num));
                            }
                        }
                    }
                    else{
                        for(auto symbol : symbolTable){
                            if(symbol.first == s) output.push_back(symbol.second);
                        }
                    }
                }
            }
        }
    }
    file.close();
}

void firstParsing(string fileName){
    string line;
    ifstream file(fileName);
    int ant = 0;
    bool find;
    if(file.is_open()){
        while(getline(file,line)){
            //Lida com secao de texto/dados
            int sectionInit = line.find("SECAO TEXTO");
            if(sectionInit != -1) {haveSectionText = true; continue;}
            int sectionDataInit = line.find("SECAO DADOS");
            if(sectionDataInit != -1) continue;

            find = false;
            values.push_back(ant);
            int l;
            l = line.find("JMP");
            if(l != -1) {ant += 2; continue;}
            l = line.find("JMPN");
            if(l != -1) {ant += 2; continue;}
            l = line.find("JMPP");
            if(l != -1) {ant += 2; continue;}
            l = line.find("JMPZ");
            if(l != -1) {ant += 2; continue;}

            for(auto instruction : instructions){
                int init = line.find(instruction.first);
                if(init != -1){
                    // cout << "INSTRUCAO " << instruction.first << " VALUE: " << instruction.second[2] << endl;
                    ant += instruction.second[2];
                    find = true;
                }
            }
            if(!find) ant += 1;

            int symbolInit = line.find(":");
            if(symbolInit != -1){
                string s;

                for(int i = 0; i < symbolInit; i++) s+=line[i];
                symbolTable.insert({s,values[values.size()-1]});
            }

            // cout << line << endl;
            // for(auto x : values) cout << x << " ";
            // cout << endl;
            // cout << "SIMBOLOS: " << endl;
            // for(auto x : symbolTable) cout << x.first <<  " " << x.second << " | ";
            // cout << endl;
        }
        file.close();
    }
}

void preProcessing(){
    string line;
    ifstream file("tempInput.asm");
    ofstream output_file("PPInput.asm");
    bool previous = false;
    if(file.is_open()){
        while(getline(file,line)){
            if(previous){previous = false; continue;}
            for(auto i : equValue){
                int init = line.find(i.first);
                if(init != -1){
                    line.replace(init,init+i.first.size()-1,i.second); //substituiu label do EQU pelo valor
                }
            }
            int ifInit = line.find("IF");
            if(ifInit != -1){
                if(line[ifInit+3] == '0'){
                    previous = true;
                    continue;
                }
                else continue;
            }
            output_file << line << endl;
        }
        file.close();
        output_file.close();
    }
}

void formatFile(string fileName){
    string line;
    ifstream file (fileName);
    ofstream output_file("tempInput.asm");
    if(file.is_open()){
        while(getline(file,line)){
            int n = line.size();
            if(line.empty()) continue;
            int commentInit = line.find(";");
            if(commentInit != -1){
                for(int i = commentInit-1; i >= 0; i--){
                    if(line[i] != ' ') break;
                    commentInit = i;
                }
                line.erase(commentInit,line.size()-1);
            }
            int twoDotsInit = line.find(":");
            if(twoDotsInit != -1){
                int initErase = 0;
                for(int i = 0; i <= twoDotsInit; i++){
                    if(line[i] == ' ') {initErase = i; break;}
                }
                if(initErase != 0) line.erase(initErase,twoDotsInit-1);
            }
            for(int i = 0 ; i < n; i++){
                if(line[i] == ' ' and line[i-1] == ' ' or line[i] == ' ' and line[i+1] == ','){
                    line.erase(i,1);
                    i = i-1;
                    continue;
                }
                line[i] = toupper(line[i]);
            }
            int init = line.find("EQU");
            if(init != -1){
                string instr, value;
                for(int i = 0; i < init-2; i++){
                    instr+=line[i];
                }
                for(int i = init+4; i < line.size(); i++){
                    value+=line[i];
                }
                equValue.insert({instr,value});
                continue;
            }
            output_file << line << endl;
        }
        file.close();
        output_file.close();
    }
}

void populate(){
    //numero de operandos, código, tamanho
    instructions.insert({"ADD", {1,1,2}});
    instructions.insert({"SUB", {1,2,2}});
    instructions.insert({"MULT", {1,3,2}});
    instructions.insert({"DIV", {1,4,2}});
    instructions.insert({"JMP", {1,5,2}});
    instructions.insert({"JMPN", {1,6,2}});
    instructions.insert({"JMPP", {1,7,2}});
    instructions.insert({"JMPZ", {1,8,2}});
    instructions.insert({"COPY", {2,9,3}});
    instructions.insert({"LOAD", {1,10,2}});
    instructions.insert({"STORE", {1,11,2}});
    instructions.insert({"INPUT", {1,12,2}});
    instructions.insert({"OUTPUT", {1,13,2}});
    instructions.insert({"STOP", {0,14,1}});

    directives.insert({"SPACE", {0,1}});
    directives.insert({"CONST", {1,1}});
    directives.insert({"EQU", {1,0}});
    directives.insert({"IF", {1,0}});
    directives.insert({"BEGIN", {}});
    directives.insert({"END", {}});
    directives.insert({"EXTERN", {}});
    directives.insert({"PUBLIC", {}});
}

int main(int argc, char *argv[]){
    populate();
    string flag = argv[1];
    if(flag == "-p"){
        formatFile(argv[2]); //tira quebra de linha, espaços e salva EQU
        preProcessing();
    }
    else{
        firstParsing(argv[2]);
        if(!haveSectionText) {cout << "ERRO: falta de seção de texto." << endl;return(0);}
        secondParsing(argv[2]);

        cout << "OUTPUT: " << endl;
        for(auto x : output) cout << x << " ";
        cout << endl;


    }

    ofstream output_file(argv[3]);
    for(auto x : output) output_file << x << " ";
    output_file.close();
}