/****************************************************/
/* File: util.c                                     */
/* Utility function implementation                  */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "util.h"

/* Procedure printToken prints a token 
 * and its lexeme to the listing file
 */
void printToken( TokenType token, const char* tokenString )
{switch (token)
  { 
    /*reserved words*/
    case IF: fprintf(listing,"\t%s\t%s\n","IF",tokenString); break;
    case THEN: fprintf(listing,"\t%s\t%s\n","THEN",tokenString); break;
    case ELSE: fprintf(listing,"\t%s\t%s\n","ELSE",tokenString); break;
    case END: fprintf(listing,"\t%s\t%s\n","END",tokenString); break;
    case REPEAT: fprintf(listing,"\t%s\t%s\n","REPEAT",tokenString); break;
    case UNTIL: fprintf(listing,"\t%s\t%s\n","UNTIL",tokenString); break;
    case READ: fprintf(listing,"\t%s\t%s\n","READ",tokenString); break;
    case WRITE: fprintf(listing,"\t%s\t%s\n","WRITE",tokenString); break;
    case VOID: fprintf(listing,"\t%s\t%s\n","VOID",tokenString); break;
    case WHILE: fprintf(listing,"\t%s\t%s\n","WHILE",tokenString); break;
    case INT: fprintf(listing,"\t%s\t%s\n","INT",tokenString); break;
    case RETURN: fprintf(listing,"\t%s\t%s\n","RETURN",tokenString); break;
    
    /*special symbols*/
    case ASSIGN: fprintf(listing,"\t==\t==\n"); break;
    case LT: fprintf(listing,"\t<\t<\n"); break;
    case RT: fprintf(listing,"\t>\t>\n"); break;
    case LEQ: fprintf(listing,"\t<=\t<=\n"); break;
    case REQ: fprintf(listing,"\t>=\t>=\n"); break;
    case EQ: fprintf(listing,"\t=\t=\n"); break;
    case NEQ: fprintf(listing,"\t!=\t!=\n"); break;
    case LPAREN: fprintf(listing,"\t(\t(\n"); break;
    case RPAREN: fprintf(listing,"\t)\t)\n"); break;
    case LSQBRAC: fprintf(listing,"\t[\t[\n"); break;
    case RSQBRAC: fprintf(listing,"\t]\t]\n"); break;
    case LBRAC: fprintf(listing,"\t{\t{\n"); break;
    case RBRAC: fprintf(listing,"\t}\t}\n"); break;
    case COMMA: fprintf(listing,"\t,\t,\n"); break;
    case SEMI: fprintf(listing,"\t;\t;\n"); break;
    case PLUS: fprintf(listing,"\t+\t+\n"); break;
    case MINUS: fprintf(listing,"\t-\t-\n"); break;
    case TIMES: fprintf(listing,"\t*\t*\n"); break;
    case OVER: fprintf(listing,"\t/\t/\n"); break;
    case ENDFILE: fprintf(listing,"\tEOF\n"); break;

    /*pre-declared regex*/
    case CMTERR: 
      fprintf(listing, 
          "\tERROR\tCOMMENT ERROR\n"); break;

    /*lexical error 
      ex. variable mix of number and identifier*/
    case LEXERR: 
      fprintf(listing, 
          "\tERROR\tLEXICAL ERROR(%s)\n", tokenString); break;
    case NUM:
      fprintf(listing,
          "\tNUM\t%s\n",tokenString);
      break;
    case ID:
      fprintf(listing,
          "\tID\t%s\n",tokenString);
      break;
      
    /*other undefined tokens*/
    case ERROR:
      fprintf(listing,
          "\tERROR\t%s\n",tokenString);
      break;
    case TOKENERR:
      fprintf(listing,
          "\t%s\t%s\n",tokenString,tokenString);
      break;
    default: /* should never happen */
      fprintf(listing,"Unknown token: %d\n",token);
  }
}

/* Function newStmtNode creates a new statement
 * node for syntax tree construction
 */
TreeNode * newStmtNode(StmtKind kind)
{ TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else {
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = StmtK;
    t->kind.stmt = kind;
    t->lineno = lineno;
  }
  return t;
}

/* Function newExpNode creates a new expression 
 * node for syntax tree construction
 */
TreeNode * newExpNode(ExpKind kind)
{ TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else {
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = ExpK;
    t->kind.exp = kind;
    t->lineno = lineno;
    t->type = Void;
  }
  return t;
}

/* Function copyString allocates and makes a new
 * copy of an existing string
 */
char * copyString(char * s)
{ int n;
  char * t;
  if (s==NULL) return NULL;
  n = strlen(s)+1;
  t = malloc(n);
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else strcpy(t,s);
  return t;
}

/* Variable indentno is used by printTree to
 * store current number of spaces to indent
 */
static indentno = 0;

/* macros to increase/decrease indentation */
#define INDENT indentno+=4
#define UNINDENT indentno-=4

/* printSpaces indents by printing spaces */
static void printSpaces(void)
{ int i;
  for (i=0;i<indentno;i++)
    fprintf(listing," ");
}

/* procedure printTree prints a syntax tree to the 
 * listing file using indentation to indicate subtrees
 */
void printTree( TreeNode * tree )
{ int i;
  INDENT;
  while (tree != NULL) {
    printSpaces();
    if (tree->nodekind==StmtK)
    { switch (tree->kind.stmt) {
        case IfK:
          fprintf(listing,"If\n");
          break;
        case RepeatK:
          fprintf(listing,"Repeat\n");
          break;
        case AssignK:
          //UNINDENT;
          //printSpaces();
          fprintf(listing,"Assign : =\n");
          INDENT;
          printSpaces();
          fprintf(listing, "Variable : %s\n", tree->attr.name);
          
          UNINDENT;
          break;
        case ReadK:
          fprintf(listing,"Read: %s\n",tree->attr.name);
          break;
        case WriteK:
          fprintf(listing,"Write\n");
          break;
        case CompK:
          fprintf(listing, "Compound statment\n");
          break;
        default:
          fprintf(listing,"Unknown ExpNode kind\n");
          break;
      }
    }
    else if (tree->nodekind==ExpK)
    { switch (tree->kind.exp) {
        case OpK:
          //printSpaces();
          fprintf(listing,"Operator: ");
          if(tree->attr.op == PLUS){
            fprintf(listing,"+\n");
          }
          else if(tree->attr.op == MINUS){
            fprintf(listing,"-\n");
          }
          else if(tree->attr.op == TIMES){
            fprintf(listing,"*\n");
          }
          else if(tree->attr.op == OVER){
            fprintf(listing,"/\n");
          }
          //printToken(tree->attr.op,"\0");
          break;
        case ConstK:
          //printSpaces();
          fprintf(listing,"Constant: %d\n",tree->attr.val);
          break;
        case IdK:
          //printSpaces();
          fprintf(listing,"Variable: %s\n",tree->attr.name);
          break;
        case VarK:
          fprintf(listing, "Variable Declaration: %s\n", tree->attr.name);
          INDENT;
          printSpaces();
          switch(tree->type){
            case Integer: fprintf(listing, "Type: int\n");break;
            case Void: fprintf(listing, "Type: void\n");break;
          }
          UNINDENT;
          break;
        case FuncK:
          fprintf(listing, "Function Declaration: %s\n", tree->attr.name);
          INDENT;
          printSpaces();
          switch(tree->type){
            case Integer: fprintf(listing, "Type: int\n");break;
            case Void: fprintf(listing, "Type: void\n");break;
          }
          UNINDENT;
          break;
        case ArrK:
          fprintf(listing, "Variable Declaration: %s[%d]\n", tree->attr.name, tree->arr_size);
            INDENT;
            printSpaces();
            switch(tree->type){
              case Integer: fprintf(listing, "Type: int\n");break;
              case Void: fprintf(listing, "Type: void\n");break;
            }
            UNINDENT;
            break;
        case ParamK:
          INDENT;
          switch(tree->type){
            case Integer: 
              fprintf(listing, "Parameter: %s\n", tree->attr.name);
              
              printSpaces();
              fprintf(listing, "Type: int\n");
              break;
            }
            UNINDENT;
            break;
        case AddIK:
          //printSpaces();
          fprintf(listing, "Addictive Expression\n");
          INDENT;
          printSpaces();
          fprintf(listing, "Variable: %s\n", tree->attr.name);
          //UNINDENT;
          break;
        case AddCK:
          //printSpaces();
          fprintf(listing, "Addictive Expression\n");
          INDENT;
          printSpaces();
          fprintf(listing, "Constant: %d\n", tree->attr.val);
          //UNINDENT;
          break;
        case MulIK:
          //printSpaces();
          fprintf(listing, "Multiple Expression\n");
          INDENT;
          printSpaces();
          fprintf(listing, "Variable: %s\n", tree->attr.name);
          //UNINDENT;
          break;
        case MulCK:
          //printSpaces();
          fprintf(listing, "Mulpiple Expression\n");
          INDENT;
          printSpaces();
          fprintf(listing, "Constant: %d\n", tree->attr.val);
          //UNINDENT;
          break;
        default:
          fprintf(listing,"Unknown ExpNode kind\n");
          break;
      }
    }
    else fprintf(listing,"Unknown node kind\n");
    for (i=0;i<MAXCHILDREN;i++)
         printTree(tree->child[i]);
    tree = tree->sibling;
  }
  UNINDENT;
}
