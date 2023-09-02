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
// Movemos El atributo "STATE" a public para poder actualizarlo en la funcion donde se desarrolla el ciclo del automata
// Para la representacion del automata usamos un while con varios condicionales

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

template<>
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
    {"end", Token::END}
  };



class Scanner {
public:
  Scanner(const char* in_s);
  Token* nextToken();
  ~Scanner();  
  int state;
  
private:
  string input;
  int first, current;
  char nextChar();
  void rollBack();
  void startLexema();
  void incrStartLexema();
  string getLexema();
};


Scanner::Scanner(const char* s):input(s),first(0),current(0) { }

Token* Scanner::nextToken() {
  Token* token;
  char c;
  c = nextChar();
  while (c == ' ') c = nextChar();
  if (c =='\n') return new Token(Token::END)
  this->state = 0;
  startLexema();

  while(true){
    if (this->state == 0){  
      if (isalpha(c)) this->state = 1;
      else if (isdigit(c)) this->state = 4;
      else if (c == '\n') this->state = 6;
      else return new Token(Token::ERR);
      break;
    }
    else if(this->state == 1){
      c = nextChar();
      if (isalpha(c) or isdigit(c) or c == '_') this->state = 1;
      else if (c == ':') this->state = 3;
      else this->state = 2;
      break;
    }
    else if(this->state == 2){
      if (values[this->getLexema()] != Token::ID)
        token =  new Token(values[this->getLexema()]->second);
      else
        token =  new Token(Token::ID, getLexema());
      rollBack();
      return token;
    }
    else if(this->state == 3){
      if (c == ':') return new Token(Token::LABEL, getLexema());
      break;
    }
    else if(this->state == 4){
      c = nextChar();
      if (isdigit(c)) this->state = 4;
      else this->state = 5;
      break;
    }
    else if(this->state == 5){
      rollBack();
      return new Token(Token::NUM, getLexema());
    }
    else if(this->state == 6){
      c = nextChar();
      if (c == '\n') this->state = 6;
      else this->state = 7;
      break;
    }
    else{
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


