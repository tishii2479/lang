#include <iostream>
#include <string>

enum TokenKind {
  TK_RESERVED,
  TK_NUM,
  TK_EOF,
};

struct Token {
  TokenKind kind;
  Token *next;
  int val;
  std::string str;
};

Token *token;

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

bool consume(char op) {
  if (token->kind != TK_RESERVED || token->str[0] != op)
    return false;
  token = token->next;
  return true;
}

void expect(char op) {
  if (token->kind != TK_RESERVED || token->str[0] != op)
    error("Expected '%c', Got '%c'", op, token->str[0]);
  token = token->next;
}

int expect_number() {
  if (token->kind != TK_NUM)
    error("Failed to get number");
  int val = token->val;
  token = token->next;
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

Token *new_token(TokenKind kind, Token *cur, std::string str) {
  Token *tok = (Token *)calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  cur->next = tok;
  return tok;
}

Token *new_token(TokenKind kind, Token *cur, char c) {
  return new_token(kind, cur, std::string{c});
}

Token *tokenize(std::string s) {
  Token head;
  head.next = NULL;
  Token *cur = &head;
  debug("Tokenize: %s, length: %d", s.c_str(), (int)s.length());

  for (int i = 0; i < (int)s.length(); i++) {
    char c = s[i];
    if (isspace(c)) {
      continue;
    }

    if (c == '+' || c == '-') {
      cur = new_token(TK_RESERVED, cur, c);
      continue;
    }

    if (isdigit(c)) {
      cur = new_token(TK_NUM, cur, c);
      cur->val = _strtol(s, i);
      continue;
    }

    error("Failed to tokenize");
  }

  new_token(TK_EOF, cur, "EOF");
  return head.next;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    error("Invalid number of argument");
    return 1;
  }

  std::string s = std::string(argv[1]);

  if (s.size() == 0) {
    error("Failed to read input");
  }

  token = tokenize(s);

  printf(".intel_syntax noprefix\n");
  printf(".globl _main\n");
  printf("_main:\n");
  printf("  mov rax, %d\n", expect_number());

  while (token->kind != TK_EOF) {
    if (consume('+')) {
      printf("  add rax, %d\n", expect_number());
      continue;
    }
    if (consume('-')) {
      printf("  sub rax, %d\n", expect_number());
      continue;
    }
  }

  printf("  ret\n");
  return 0;
}
