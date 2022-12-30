/****************************************************/
/* File: parse.c                                    */
/* The parser implementation for the TINY compiler  */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

static TokenType token; /* holds current token */

/* function prototypes for recursive calls */
static TreeNode *stmt_sequence(void);
static TreeNode *statement(void);
static TreeNode *if_stmt(void);
static TreeNode *repeat_stmt(void);
static TreeNode *assign_stmt(void);
static TreeNode *read_stmt(void);
static TreeNode *write_stmt(void);
static TreeNode *exp(void);
static TreeNode *add_exp(TreeNode *g);
static TreeNode *term(TreeNode *g);
static TreeNode *factor(TreeNode *g);
static TreeNode *param(void);
static ExpType get_type(void);
static TreeNode *compound(void);
static TreeNode *local_declare(void);
static TreeNode *stmt_declare(void);
static TreeNode *call(void);
static TreeNode *args(void);
static TreeNode *simple_exp(void);
static TreeNode *ret_stmt(void);
static TreeNode *exp_list(void);
static TreeNode *stmt_list(void);
static TreeNode *var_declare(void);
static TreeNode *tempa(void);
static TreeNode *add_oper(void);
static TreeNode *add_op(void);
static TreeNode *mul_op(void);
static TreeNode *sim_op(void);
static TreeNode *callparam(void);
TreeNode *arr_op(void);
int flag = 0;
int add_mul_flag = 0;

static void syntaxError(char *message)
{ // fprintf(listing,"\n>>> ");
  fprintf(listing, "Syntax error at line %d: %s", lineno, message);
  fprintf(listing, "Current token: \t");
  printToken(token, tokenString);
  fprintf(listing, "\nSyntax tree:\n");
  exit(-1);
  Error = TRUE;
}

static void match(TokenType expected)
{
  if (token == expected)
  {
    token = getToken();
    while (token == NLSP)
    {
      token = getToken();
    }
  }
  else
  {
    syntaxError("!!unexpected token -> ");
    printToken(token, tokenString);

    fprintf(listing, "      ");
  }
}

// type 반환
ExpType get_type(void)
{

  if (token == INT)
  {
    match(INT);
    return Integer;
  }
  else if (token == VOID)
  {
    match(VOID);
    return Void;
  }
  else
  {
    syntaxError("syntax error\n");
    fprintf(listing, "get -Current token: \t");
    printToken(TOKENERR, tokenString);
    token = getToken();
  }
}
TreeNode *stmt_sequence(void)
{
  TreeNode *t = statement();

  TreeNode *p = t;
  while ((token != ENDFILE) && (token != END) &&
         (token != ELSE) && (token != UNTIL))
  {
    TreeNode *q;
    // match(SEMI);
    q = statement();
    // if(ERROR) return;
    if (q != NULL)
    {
      if (t == NULL)
        t = p = q;
      else /* now p cannot be NULL either */
      {
        p->sibling = q;
        p = q;
      }
    }
  }
  return t;
}

TreeNode *statement(void)
{
  TreeNode *t = NULL;
  ExpType type = get_type();
  char *name = copyString(tokenString);
  match(ID);
  // if(ERROR) return;
  /* 프로그램 젤 앞의 d선언문 scan완료*/

  if (token == SEMI)
  {

    /* 변수 선언*/
    match(SEMI);
    t = newExpNode(VarK);
    t->attr.name = name;
    t->type = type;
    //
  }
  else if (token == LPAREN)
  {

    /*함수 선언*/
    t = newExpNode(FuncK);
    t->attr.name = name;
    t->type = type;
    match(LPAREN);
    t->child[0] = param();
    match(RPAREN);
    t->child[1] = compound();
  }
  else if (token == LSQBRAC)
  {

    /*배열 선언*/
    t = newExpNode(ArrK);
    t->attr.name = name;
    t->type = type;
    match(LSQBRAC);
    t->arr_size = atoi(tokenString);
    match(NUM);
    match(RSQBRAC);
    match(SEMI);
  }
  else
  {
    syntaxError("syntax error\n");
    fprintf(listing, "stmt - Current token: \t");
    printToken(TOKENERR, tokenString);
    token = getToken();
  }
  return t;
}

TreeNode *param(void)
{
  TreeNode *t = newExpNode(ParamK);
  TreeNode *q;
  switch (token)
  {
  case INT:
    match(INT);
    t->type = Integer;
    t->attr.name = copyString(tokenString);
    match(ID);
    if (token == LSQBRAC)
    {
      match(LSQBRAC);
      match(RSQBRAC);
    }
    if (token == COMMA)
    {
      match(COMMA);
      q = param();
      t->sibling = q;
    }
    break;
  case VOID:
    match(VOID);
    t->type = Void;
    t->attr.name = copyString(tokenString);

    break;
  }

  return t;
}
TreeNode *callparam(void)
{
  TreeNode *t = NULL;
  TreeNode *q;
  // match(token);
  switch (token)
  {
  case ID:
    t = newExpNode(IdK);
    t->attr.name = copyString(tokenString);
    match(ID);
    if (token == COMMA)
    {
      match(COMMA);
      q = callparam();
      t->sibling = q;
    }
    break;
  case NUM:
    t = newExpNode(ConstK);
    t->attr.val = atoi(tokenString);
    match(NUM);
    if (token == COMMA)
    {
      match(COMMA);
      q = callparam();
      t->sibling = q;
    }
    break;
  default:
    syntaxError("syntax error\n");
    fprintf(listing, "Current token: \t");
    printToken(TOKENERR, tokenString);
  }
  return t;
}
TreeNode *compound(void)
{
  TreeNode *t = newStmtNode(CompK);
  match(LBRAC);
  t->child[0] = local_declare();
  t->child[1] = stmt_list();
  // match(SEMI);
  match(RBRAC);
  return t;
}
TreeNode *tempa(void)
{

  TreeNode *t = NULL;
  char *name = copyString(tokenString);

  if (token == ID)
  {
    match(ID);
    // printToken(token, tokenString);
    if (token == EQ)
    {
      t = newStmtNode(AssignK);
      t->attr.name = name;
      match(EQ);
      t->child[0] = add_oper();
    }
    else if ((token == LT) || (token == ASSIGN) || (token == RT) || (token == REQ) || (token == LEQ) || (token == NEQ))
    {
      t = newExpNode(SimpIK);
      t->simp_name = name;
      t->attr.op = token;
      match(token);
      t->child[0] = add_oper();
    }
    else if ((token == PLUS) || (token == MINUS))
    {
      add_mul_flag = 0;
      t = newExpNode(AddSI);
      t->simp_name = name;
      t->attr.op = token;
      match(token);
      t->child[0] = add_oper();
    }
    else if ((token == TIMES) || (token == OVER))
    {
      add_mul_flag = 1;
      t = newExpNode(MulSI);
      t->simp_name = name;
      t->attr.op = token;
      match(token);
      t->child[0] = add_oper();
    }
    else if (token == LPAREN)
    {

      match(LPAREN);
      t = newStmtNode(CallK);
      t->child[0] = callparam();
      match(RPAREN);
    }
    else if (token == LSQBRAC)
    {
      t = newStmtNode(AssignKarr);
      t->attr.name = name;
      match(LSQBRAC);
      t->arr_size = atoi(tokenString);
      match(NUM);
      match(RSQBRAC);
      match(EQ);
      t->child[0] = add_oper();
    }
  }
  return t;
}
TreeNode *add_oper(void)
{
  TreeNode *t;
  TreeNode *q;
  if (token == LPAREN)
    match(LPAREN);
  char *name = copyString(tokenString);

  if (token == ID)
  {
    match(ID);
    if (token == LPAREN)
      match(LPAREN);
    if (token == RPAREN)
      match(RPAREN);
    if (token == SEMI)
    {
      t = newExpNode(IdK);
      t->attr.name = name;
    }
    else if ((token == PLUS) || (token == MINUS))
    {
      add_mul_flag = 0;
      t = newExpNode(AddIK);
      t->attr.name = name;
      q = add_op();
      t->sibling = q;
    }
    else if ((token == TIMES) || (token == OVER))
    {
      add_mul_flag = 1;
      t = newExpNode(MulIK);
      t->attr.name = name;
      q = mul_op();
      t->sibling = q;
    }
    else if ((token == LT) || (token == ASSIGN) || (token == RT) || (token == REQ) || (token == LEQ) || (token == NEQ))
    {
      t = newExpNode(SimpIK);
      t->simp_name = name;
      t->attr.op = token;
      q = sim_op();
      t->sibling = q;
    }
    else if (token == LSQBRAC)
    {
      t = newExpNode(ArrexpK);
      t->attr.name = name;
      match(LSQBRAC);
      q = add_oper();
      t->child[0] = q;
      match(RSQBRAC);
      if ((token == PLUS) || (token == MINUS))
      {
        q = add_op();
        t->sibling = q;
      }
      if ((token == TIMES) || (token == OVER))
      {

        q = mul_op();
        t->sibling = q;
      }
    }
  }
  else if (token == NUM)
  {
    int temp = atoi(tokenString);
    match(NUM);
    if (token == LPAREN)
      match(LPAREN);
    if (token == RPAREN)
      match(RPAREN);
    if (token == SEMI)
    {
      t = newExpNode(ConstK);
      t->attr.val = temp;
    }
    else if ((token == PLUS) || (token == MINUS))
    {
      // match(token);
      t = newExpNode(AddCK);
      t->attr.val = temp;
      q = add_op();

      t->sibling = q;
    }
    else if ((token == TIMES) || (token == OVER))
    {
      // match(token);
      t = newExpNode(MulCK);
      t->attr.val = temp;
      q = mul_op();
      t->sibling = q;
    }
    else if ((token == LT) || (token == ASSIGN) || (token == RT) || (token == REQ) || (token == LEQ) || (token == NEQ))
    {
      t = newExpNode(SimpNK);
      t->simp_val = temp;
      t->attr.op = token;
      q = sim_op();
      t->sibling = q;
    }
  }
  else
  {
    syntaxError("syntax error\n");
    fprintf(listing, "add- Current token: \t");
    printToken(TOKENERR, tokenString);
  }
  return t;
}
TreeNode *sim_op(void)
{
  TreeNode *t = NULL;
  match(token);
  if (token == NUM)
  {
    t = newExpNode(ConstK);
    t->attr.val = atoi(tokenString);
    match(token);
    if (token != SEMI)
    {
      add_mul_flag = 2;
      if ((token == TIMES) || (token == OVER))
        t->sibling = mul_op();
      else
        t->sibling = add_op();
    }
  }
  else if (token == ID)
  {
    t = newExpNode(IdK);
    t->attr.name = copyString(tokenString);
    match(token);
    if (token != SEMI)
    {
      add_mul_flag = 2;
      if ((token == TIMES) || (token == OVER))
        t->sibling = mul_op();
      else if (token == LSQBRAC)
        t->sibling = arr_op();
      else
        t->sibling = add_op();
    }
  }

  return t;
}
TreeNode *add_op(void)
{
  TreeNode *t = NULL;
  TreeNode *q;
  if (token == LPAREN)
    match(LPAREN);
  if (token == RPAREN)
    match(RPAREN);
  if ((flag == 0) && ((token == PLUS) || (token == MINUS)))
  {

    t = newExpNode(OpK);
    t->attr.op = token;
    if (add_mul_flag == 2 || add_mul_flag)
      t->type = MtA;
    add_mul_flag = 0;
    match(token);
    flag = 1;
    if (token != SEMI)
    {
      flag = 1;
      if ((token == TIMES) || (token == OVER))
        t->sibling = mul_op();
      else
        t->sibling = add_op();
    }
  }
  else if (token == ID && flag == 1)
  {
    t = newExpNode(IdK);
    t->attr.name = copyString(tokenString);
    match(token);
    if (token == RPAREN)
      match(RPAREN);
    flag = 0;
    if (token != SEMI)
    {

      flag = 0;
      if ((token == TIMES) || (token == OVER))
        t->sibling = mul_op();
      else if (token == LSQBRAC)
      {
        t->sibling = arr_op();
      }
      else
        t->sibling = add_op();
    }
  }
  else if (token == NUM && flag == 1)
  {
    t = newExpNode(ConstK);
    t->attr.val = atoi(tokenString);
    match(token);
    if (token == RPAREN)
      match(RPAREN);
    flag = 0;
    if (token != SEMI)
    {
      flag = 0;
      if ((token == TIMES) || (token == OVER))
        t->sibling = mul_op();
      else
        t->sibling = add_op();
    }
  }
  return t;
}

TreeNode *arr_op(void)
{
  TreeNode *t = NULL;
  TreeNode *q;

  if (token == LSQBRAC)
  {
    match(LSQBRAC);
    t = newExpNode(ArrexpK);
    t->attr.name = copyString(tokenString);

    q = add_oper();
    t->child[0] = q;
    match(RSQBRAC);
    if ((token == PLUS) || (token == MINUS))
    {
      q = add_op();
      t->sibling = q;
    }
    if ((token == TIMES) || (token == OVER))
    {

      q = mul_op();
      t->sibling = q;
    }
  }
  return t;
}
TreeNode *mul_op(void)
{
  TreeNode *t;
  TreeNode *q;
  if (token == LPAREN)
    match(LPAREN);
  if (token == RPAREN)
    match(RPAREN);
  if ((flag == 0) && ((token == TIMES) || (token == OVER)))
  {

    t = newExpNode(OpK);
    t->attr.op = token;
    if (add_mul_flag == 2 || !add_mul_flag)
      t->type = AtM;
    add_mul_flag = 1;
    match(token);
    if (token != SEMI)
    {
      flag = 1;
      t->sibling = add_op();
    }
  }
  else if (token == ID && flag == 1)
  {
    t = newExpNode(IdK);
    t->attr.name = copyString(tokenString);
    match(token);
    if (token == RPAREN)
      match(RPAREN);
    if (token != SEMI)
    {
      flag = 0;
      if ((token == TIMES) || (token == OVER))
        t->sibling = mul_op();
      else if (token == LSQBRAC)
        t->sibling = arr_op();
      else
        t->sibling = add_op();
    }
  }
  else if (token == NUM && flag == 1)
  {
    t = newExpNode(ConstK);
    t->attr.val = atoi(tokenString);
    match(token);
    if (token == RPAREN)
      match(RPAREN);
    if (token != SEMI)
    {
      flag = 0;
      if ((token == TIMES) || (token == OVER))
        t->sibling = mul_op();
      else
        t->sibling = add_op();
    }
  }
  else
  {
    syntaxError("syntax error\n");
    fprintf(listing, "mul- Current token: \t");
    printToken(TOKENERR, tokenString);
  }
  return t;
}
TreeNode *var_declare(void)
{
  TreeNode *t;
  ExpType type = get_type();
  char *name = copyString(tokenString);
  match(ID);

  if (token == SEMI)
  {

    /* 변수 선언*/
    match(SEMI);
    // return NULL;
    t = newExpNode(VarK);
    t->attr.name = name;
    t->type = type;
    //
  }
  else if (token == LSQBRAC)
  {

    /*배열 선언*/
    t = newExpNode(ArrK);
    t->attr.name = name;
    t->type = type;
    match(LSQBRAC);
    t->arr_size = atoi(tokenString);
    match(NUM);
    match(RSQBRAC);
    match(SEMI);
  }
  else
  {
    syntaxError("syntax error\n");
    fprintf(listing, "var -Current token: \t");
    printToken(TOKENERR, tokenString);
    token = getToken();
  }
  return t;
}
TreeNode *local_declare(void)
{
  TreeNode *t = NULL;
  if (token == INT || token == VOID)
  {
    t = var_declare();
  }
  TreeNode *p = t;
  if (t != NULL)
  {
    while ((token == INT || token == VOID))
    {
      TreeNode *q;
      // match(SEMI);
      q = var_declare();
      // if(ERROR) return;
      if (q != NULL)
      {
        if (t == NULL)
          t = p = q;
        else /* now p cannot be NULL either */
        {
          p->sibling = q;
          p = q;
        }
      }
    }
  }
  return t;
}
TreeNode *stmt_list(void)
{
  TreeNode *t = NULL;

  if (token == RBRAC)
    return NULL;

  t = stmt_declare();
  TreeNode *p = t;
  while (token != RBRAC)
  {
    TreeNode *q;
    // match(SEMI);
    q = stmt_declare();
    // if(ERROR) return;
    if (q != NULL)
    {
      if (t == NULL)
        t = p = q;
      else /* now p cannot be NULL either */
      {
        p->sibling = q;
        p = q;
      }
    }
  }
  return t;
}
TreeNode *stmt_declare(void)
{
  TreeNode *t = NULL;
  switch (token)
  {
  case WHILE:
    t = repeat_stmt();
    break;
  case IF:
    t = if_stmt();
    break;
  case RETURN:
    t = ret_stmt();
    break;
  case LBRAC:
    t = compound();

    break;
  case ID:
    t = tempa();
    match(SEMI);
    break;
  default:
    syntaxError("syntax error\n");
    fprintf(listing, "stCurrent token: \t");
    printToken(TOKENERR, tokenString);
    token = getToken();
    break;
  }

  return t;
}
TreeNode *ret_stmt(void)
{
  TreeNode *t = newStmtNode(ReturnK);

  match(RETURN);
  if (token != SEMI)
  {
    t->child[0] = add_oper();
  }
  match(SEMI);
  return t;
}
TreeNode *if_stmt(void)
{
  TreeNode *t = newStmtNode(IfK);
  match(IF);
  match(LPAREN);
  if (t != NULL)
    t->child[0] = add_oper();

  if (t != NULL)
    t->child[1] = stmt_declare();

  if (token == ELSE)
  {
    match(ELSE);
    if (t != NULL)
      t->child[2] = stmt_declare();
  }
  return t;
}

TreeNode *repeat_stmt(void)
{
  TreeNode *t = newStmtNode(RepeatK);
  match(WHILE);
  match(LPAREN);
  if (t != NULL)
    t->child[0] = add_oper();

  if (t != NULL)
    t->child[1] = stmt_declare();
  return t;
}

TreeNode *assign_stmt(void)
{
  TreeNode *t = newStmtNode(AssignK);
  if ((t != NULL) && (token == ID))
    t->attr.name = copyString(tokenString);
  match(ID);
  match(ASSIGN);
  if (t != NULL)
    t->child[0] = tempa();
  return t;
}

TreeNode *read_stmt(void)
{
  TreeNode *t = newStmtNode(ReadK);
  match(READ);
  if ((t != NULL) && (token == ID))
    t->attr.name = copyString(tokenString);
  match(ID);
  return t;
}

TreeNode *write_stmt(void)
{
  TreeNode *t = newStmtNode(WriteK);
  match(WRITE);
  if (t != NULL)
    t->child[0] = tempa();
  return t;
}

TreeNode *add_exp(TreeNode *g)
{
  TreeNode *t = term(g);
  while ((token == PLUS) || (token == MINUS))
  {
    TreeNode *p = newExpNode(OpK);
    if (p != NULL)
    {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
      match(token);
      t->child[1] = term(NULL);
    }
  }
  return t;
}

TreeNode *term(TreeNode *g)
{
  TreeNode *t = factor(g);
  while ((token == TIMES) || (token == OVER))
  {
    TreeNode *p = newExpNode(OpK);
    if (p != NULL)
    {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
      match(token);
      p->child[1] = factor(NULL);
    }
  }
  return t;
}

TreeNode *factor(TreeNode *g)
{
  TreeNode *t = NULL;
  if (g != NULL)
    return g;
  switch (token)
  {
  case NUM:
    t = newExpNode(ConstK);
    if ((t != NULL) && (token == NUM))
    {
      t->attr.val = atoi(tokenString);
      t->type = Integer;
    }
    match(NUM);
    break;
  case ID:
    t = newExpNode(IdK);
    if ((t != NULL) && (token == ID))
      match(ID);
    break;
  case LPAREN:
    match(LPAREN);
    t = tempa();
    match(RPAREN);
    break;
  default:
    syntaxError("unexpected token -> ");
    printToken(token, tokenString);
    token = getToken();
    break;
  }
  return t;
}

/****************************************/
/* the primary function of the parser   */
/****************************************/
/* Function parse returns the newly
 * constructed syntax tree
 */
TreeNode *parse(void)
{
  TreeNode *t;
  token = getToken();
  t = stmt_sequence();
  // if(ERROR) return NULL;
  if (token != ENDFILE)
    syntaxError("Code ends before file\n");
  return t;
}
