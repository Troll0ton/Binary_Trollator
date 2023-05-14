
//{----------------------------------------------------------------------------
//!                       OPERATIONS CODEGEN
//}----------------------------------------------------------------------------

OP_DEF(ADD, "+", "add",
{
    curr_node->val.num = LEFT_NUM + RIGHT_NUM;
})

OP_DEF(SUB, "-", "sub",
{
    curr_node->val.num = LEFT_NUM - RIGHT_NUM;
})

OP_DEF(MUL, "*", "mul",
{
    curr_node->val.num = LEFT_NUM * RIGHT_NUM;
})

OP_DEF(DIV, "/", "div",
{
    curr_node->val.num = LEFT_NUM / RIGHT_NUM;
})

OP_DEF(POW, "^", "pow",
{
    curr_node->val.num = pow (LEFT_NUM, RIGHT_NUM);
})

OP_DEF(SIN, "sin", "sin",
{
    curr_node->val.num = sin (RIGHT_NUM);
})

OP_DEF(COS, "cos", "cos",
{
    curr_node->val.num = cos (RIGHT_NUM);
})

OP_DEF(LN, "ln", "ln",
{
    curr_node->val.num = log (RIGHT_NUM);
})

OP_DEF(TG, "tg", "tg",
{
    curr_node->val.num = tan (RIGHT_NUM);
})

OP_DEF(CTG, "ctg", "ctg",
{
    curr_node->val.num = 1 / tan (RIGHT_NUM);
})

OP_DEF(SQRT, "sqrt", "sqrt",
{
    curr_node->val.num = sqrt (RIGHT_NUM);
})

OP_DEF(OUT, "print", "out",
{

})

OP_DEF(SCAN, "scan", "scan",
{

})

OP_DEF(OVER, ">", "OVER",
{

})

OP_DEF(LESS, "<", "LESS",
{

})

OP_DEF(EQL, "~", "EQUALITY",
{

})

OP_DEF(ASG, "", "ASSIGNMENT",
{

})

OP_DEF(ST, "", "STATEMENT",
{

})

OP_DEF(IF, "", "IF",
{

})

OP_DEF(WHILE, "", "WHILE",
{

})

OP_DEF(FUNCT, "", "FUNCTION",
{

})

OP_DEF(BODY, "", "BODY",
{

})

OP_DEF(CALL, "", "CALL",
{

})

//-----------------------------------------------------------------------------


