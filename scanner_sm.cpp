#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <fstream>
#include <unordered_map>

using namespace std;

/*
  Autores:
  Rodo Arturo Vilcarromero Moscoso
  Ronaldo Dylan Flores Soto
*/

// Incluir estructura de datos que permita la busqueda de las palabras reservadas

// Utilizamos un unordered_map para poder almacenar todas las palabras reservadas
// 

class Token {
public:
  enum Type {NUM, ID, LABEL, EOL, PUSH, JMPEQ, JMPGT, JMPGE, JMPLT, JMPLE, GOTO, SKIP, POP, DUP, SWAP, ADD, SUB, MUL, DIV, STORE, LOAD, ERR, END};
  static const char* token_names[25]; 
  Type type;
  string lexema;
  Token(Type);
  Token(Type, char c);
  Token(Type, const string source);
};

const char* Token::token_names[25] = { "NUM", "ID", "LABEL", "EOL", "PUSH", "JMPEQ", "JMPGT", "JMPGE", "JMPLT", "JMPLE", "GOTO", "SKIP", "POP", "DUP", "SWAP", "ADD", "SUB", "MUL", "DIV", "STORE", "LOAD", "ERR", "END" };

Token::Token(Type type):type(type) { lexema = ""; }

Token::Token(Type type, char c): type(type) { lexema = c; }

Token::Token(Type type, const string source):type(type) {
  lexema = source;
}

// Modificar
std::ostream& operator << ( std::ostream& outs, const Token & tok )
{
  if (tok.lexema.empty())
    return outs << Token::token_names[tok.type];
  else
    return outs << Token::token_names[tok.type] << "(" << tok.lexema << ")";
}

std::ostream& operator << ( std::ostream& outs, const Token* tok ) {
  return outs << *tok;
}

struct hash<Token>
{
  size_t operator()(const Token &token) const
  {
    return static_cast<size_t>(token.type) ^ hash<string>()(token.lexema);
  }
};


std::unordered_map<string, Token::Type> values = {
    {"push", Token::PUSH},
    {"jmpeq", Token::JMPEQ},
    {"jmpgt", Token::JMPGT},
    {"jmpge", Token::JMPGE},
    {"jmplt", Token::JMPLT},
    {"jmple", Token::JMPLE},
    {"goto", Token::GOTO},
    {"skip", Token::SKIP},
    {"pop", Token::POP},
    {"dup", Token::DUP},
    {"swap", Token::SWAP},
    {"add", Token::ADD},
    {"sub", Token::SUB},
    {"mul", Token::MUL},
    {"div", Token::DIV},
    {"store", Token::STORE},
    {"load", Token::LOAD},
    {"END", Token::END}
  };



class Scanner {
public:
  Scanner(const char* in_s);
  Token* nextToken();
  ~Scanner();  

  
private:
  string input;
  int first, current;
  int state;
  char nextChar();
  void rollBack();
  void startLexema();
  void incrStartLexema();
  string getLexema();
  int SiteState();
  void JumpState(int state);
};


Scanner::Scanner(const char* s):input(s),first(0),current(0) { }

Token* Scanner::nextToken() {
  Token* token;
  char c;
  c = nextChar();
  while (c == ' ') c = nextChar();
  if (c =='\n') return new Token(Token::END)
  JumpState(0);
  startLexema();
  
  while(true){
    switch (this->SiteState())
    {
    case 0:
      if (isalpha(c)) setState(1);
      else if (isdigit(c)) setState(4);
      else if (c == '\n') setState(6);
      else return new Token(Token::ERR);
      break;
    
    case 1:
      c = nextChar();
      if (isalpha(c) || isdigit(c) || c == '_') setState(1);
      else if (c == ':') setState(3);
      else setState(2);
      break;
    case 2:
      if (keywords.find(getLexema().substr(0,getLexema().size()-1)) != keywords.end())
          token = &(keywords.find(getLexema().substr(0, getLexema().size() - 1))->second);
      else
        token =  new Token(Token::ID, getLexema());
      rollBack();
      return token;
    case 3:
      return new Token(Token::LABEL, getLexema().substr(0, getLexema().size() - 1));
    case 4:
      c = nextChar();
      if (isdigit(c)) setState(4);
      else setState(5);
      break;
    case 5:
      rollBack();
      return new Token(Token::NUM, getLexema());
    case 6:
      c = nextChar();
      if (c == '\n') setState(6);
      else setState(7);
      break;
    case 7:
      rollBack();
      return new Token(Token::EOL);

    }
  }
}

Scanner::~Scanner() { }

char Scanner::nextChar() {
  int c = input[current];
  if (c != '\0') current++;
  return c;
}

void Scanner::rollBack() { // retract
  if (input[current] != '\0')
    current--;
}

void Scanner::startLexema() {
  first = current-1;  
  return;
}

void Scanner::incrStartLexema() {
  first++;
}


string Scanner::getLexema() {
  return input.substr(first,current-first);
}

int Scanner::SiteState(){
  return state;
}

void Scanner::JumpState(int state){
  this->state = state;
}





// ---------------------------------------------------

int main(int argc, const char* argv[]) {
  
  if (argc != 2) {
    cout << "Incorrect number of arguments" << endl;
    exit(1);
  }

  // Cambiar:
  // input es ahora el nombre de archivo con las instruccion de la SM
  

  Scanner scanner(argv[1]);
  Token* tk = scanner.nextToken();
  while (tk->type != Token::END) {
    cout << "next token " << tk << endl;
    delete tk;
    tk =  scanner.nextToken();
  }
  cout << "last token " << tk << endl;
  delete tk;

}


