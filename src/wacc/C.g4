grammar C;

program: function;

function: 'int' ID '(' ')' '{' statement '}';

statement: 'return' expression ';';

expression: NUM;

ID: [a-zA-Z_][a-zA-Z0-9_]*;
NUM: [0-9]+;
