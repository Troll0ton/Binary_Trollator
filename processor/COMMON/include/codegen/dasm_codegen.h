
PRINT_ARG(2 * O(ARG), MASK_RAM | MASK_REG | MASK_IMM, "[r%cx + %d]\n",
                                                          (int) *(elem_t*)(dasm->code + curr_pos + O(CMD)) + 'a',
                                                          (int) *(elem_t*)(dasm->code + curr_pos + O(CMD) + O(ARG)))

PRINT_ARG(1 * O(ARG), MASK_RAM | MASK_IMM,            "[%d]\n",
                                                          (int) *(elem_t*)(dasm->code + curr_pos + O(CMD)))

PRINT_ARG(1 * O(ARG), MASK_RAM | MASK_REG,            "[r%cx]\n",
                                                          (int) *(elem_t*)(dasm->code + curr_pos + O(CMD)) + 'a')

PRINT_ARG(1 * O(ARG), MASK_REG,                       "r%cx\n",
                                                          (int) *(elem_t*)(dasm->code + curr_pos + O(CMD)) + 'a')

PRINT_ARG(1 * O(ARG), MASK_IMM,                       "%d\n",
                                                          (int) *(elem_t*)(dasm->code + curr_pos + O(CMD)))
