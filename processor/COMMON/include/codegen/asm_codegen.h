
PARSE_ARG(2, MASK_RAM | MASK_REG | MASK_IMM, "%*s [r%cx + %lg]", &arg->reg_sym, &arg->value)

PARSE_ARG(1, MASK_RAM | MASK_IMM,            "%*s [%lg]",        &arg->value)

PARSE_ARG(1, MASK_RAM | MASK_REG,            "%*s [r%cx]",       &arg->reg_sym)

PARSE_ARG(1, MASK_REG,                       "%*s r%cx",         &arg->reg_sym)

PARSE_ARG(1, MASK_IMM,                       "%*s %lg",          &arg->value)


