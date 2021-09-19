#include <iostream>
#include <list>
#include <string>

std::string s;

void error_at(int pos, std::string fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  fprintf(stderr, "[error] \n");
  fprintf(stderr, "%s\n", s.c_str());
  fprintf(stderr, "%*s", pos, " ");
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt.c_str(), ap);
  fprintf(stderr, "\n");
  exit(1);
}

void error(std::string fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  fprintf(stderr, "[error] ");
  vfprintf(stderr, fmt.c_str(), ap);
  fprintf(stderr, "\n");
  exit(1);
}

void debug(std::string fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  fprintf(stderr, "[debug] ");
  vfprintf(stderr, fmt.c_str(), ap);
  fprintf(stderr, "\n");
}

enum TokenKind {
  TK_RESERVED,
  TK_NUM,
  TK_EOF,
};

struct Token {
  TokenKind kind;
  int val;
  std::string str;

  Token(TokenKind _kind, std::string _str) {
    kind = _kind;
    str = _str;
  }

  Token(TokenKind _kind, char c) {
    kind = _kind;
    str = std::string{c};
  }
};

std::list<Token> token_list;

enum NodeKind {
  ND_ADD,
  ND_SUB,
  ND_MUL,
  ND_DIV,
  ND_NUM,
};

struct Node {
  NodeKind kind;
  Node *lhs;
  Node *rhs;
  int val;
};

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = (Node *)calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val) {
  Node *node = (Node *)calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

bool consume(std::list<Token>::iterator &it, char op) {
  if ((*it).kind != TK_RESERVED || (*it).str[0] != op)
    return false;
  it++;
  return true;
}

void expect(Token tok, char op) {
  if (tok.kind != TK_RESERVED || tok.str[0] != op)
    error("Expected '%c', Got '%c'", op, tok.str[0]);
}

int expect_number(Token tok) {
  if (tok.kind != TK_NUM)
    error("Failed to get number, %s", tok.str.c_str());
  int val = tok.val;
  return val;
}

long _strtol(std::string &s, int &ptr) {
  long cur = 0;
  while (ptr < (int)s.length() and isdigit(s[ptr])) {
    cur *= 10, cur += s[ptr] - '0';
    ptr++;
  }
  ptr--;
  return cur;
}

std::list<Token> tokenize(std::string s) {
  std::list<Token> token_list;
  debug("Tokenize: %s, length: %d", s.c_str(), (int)s.length());

  for (int i = 0; i < (int)s.length(); i++) {
    char c = s[i];
    if (isspace(c)) {
      continue;
    }

    if (isdigit(c)) {
      token_list.push_back(Token(TK_NUM, c));
      token_list.back().val = _strtol(s, i);
      continue;
    }

    if (c == '+' || c == '-' || c == '/' || c == '*') {
      token_list.push_back(Token(TK_RESERVED, c));
      continue;
    }

    error_at(i, "Failed to tokenize");
  }

  token_list.push_back(Token(TK_EOF, "EOF"));
  return token_list;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    error("Invalid number of argument");
    return 1;
  }

  s = std::string(argv[1]);

  if (s.size() == 0) {
    error("Failed to read input");
  }

  token_list = tokenize(s);

  printf(".intel_syntax noprefix\n");
  printf(".globl _main\n");
  printf("_main:\n");
  printf("  mov rax, %d\n", expect_number(token_list.front()));
  token_list.pop_front();

  for (auto it = token_list.begin(); it != token_list.end(); it++) {
    if (consume(it, '+')) {
      printf("  add rax, %d\n", expect_number(*it));
      continue;
    }
    if (consume(it, '-')) {
      printf("  sub rax, %d\n", expect_number(*it));
      continue;
    }
    // if (consume(it, '/')) {
    //   printf("  div rax, %d\n", expect_number(*it));
    //   continue;
    // }
    // if (consume(it, '*')) {
    //   printf("  mul rax, %d\n", expect_number(*it));
    //   continue;
    // }
  }

  printf("  ret\n");
  return 0;
}
