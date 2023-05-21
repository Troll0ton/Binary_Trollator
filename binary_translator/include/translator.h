//! @file translator.h 

#ifndef TRANSLATOR_H
#define TRANSLATOR_H

//-----------------------------------------------------------------------------

#include "binary_translator/include/common.h"

//-----------------------------------------------------------------------------

static const uint32_t tmp_size = 20;

//-----------------------------------------------------------------------------

X86_code *translateIrToX86 (IR *ir, int bin_size);
    void translateJmpTargetsX86 (IR *ir, char **jump_table);

void translateCmd (IR_node *curr_node, char **curr_pos);

    void translateHlt (IR_node *curr_node, char **curr_pos);
    void translatePush (IR_node *curr_node, char **curr_pos);
    void translatePop (IR_node *curr_node, char **curr_pos);
    void translateArithmOperations (IR_node *curr_node, char **curr_pos);
    void translateOut (IR_node *curr_node, char **curr_pos);
    void translateDump (IR_node *curr_node, char **curr_pos);
    void translateConditionalJmps (IR_node *curr_node, char **curr_pos);
    void translateJmp (IR_node *curr_node, char **curr_pos);
    void translateCall (IR_node *curr_node, char **curr_pos);
    void translateRet (IR_node *curr_node, char **curr_pos);
    void translateMathFunctions (IR_node *curr_node, char **curr_pos);

void writeCmd (char **code, char *cmd, int size);
void writePtr (char **code, uint32_t addr);
void writeAbsPtr (char **code, uint64_t addr);
void writeNum (char **code, double num);

int double_printf (double *value);

void runCode (char *code, int size);

void CodeX86Dump (char *code, int size);

void jumpTableDump (char **jump_table, int size);

void X86RepresentDtor (X86_code *X86_code);

//-----------------------------------------------------------------------------

#endif //TRANSLATOR_H