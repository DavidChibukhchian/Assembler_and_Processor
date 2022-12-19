//----------------------------------------------------------------------------------------------------------------------

DEF_CMD(hlt, NO_ARGS, {})

//----------------------------------------------------------------------------------------------------------------------

DEF_CMD(push, NUM_REG_RAM,
{
    ip++;
    switch (arg_mask)
    {
        case ARG_NUM:
            number_1 = *((int*) (code + ip));
            stack_Push(stack, number_1);

            ip += sizeof(int);
            break;

        case ARG_REG:
            number_of_register = code[ip];
            number_1 = REG[number_of_register];
            stack_Push(stack, number_1);

            ip++;
            break;

        case (ARG_NUM + ARG_RAM):
            number_1 = *((int*) (code + ip));

            CHECK_RAM_ADDRESS(number_1);

            stack_Push(stack, RAM[number_1]);

            ip += sizeof(int);
            break;

        case (ARG_REG + ARG_RAM):
            number_of_register = code[ip];
            number_1 = REG[number_of_register];

            CHECK_RAM_ADDRESS(number_1);

            stack_Push(stack, RAM[number_1]);

            ip++;
            break;

        case (ARG_NUM + ARG_REG + ARG_RAM):
            number_1 = *((int*) (code + ip));
            ip += sizeof(int);

            number_of_register = code[ip];
            number_2 = REG[number_of_register];

            CHECK_RAM_ADDRESS(number_1 + number_2);

            stack_Push(stack, RAM[number_1 + number_2]);

            ip++;
            break;
    }
})

//----------------------------------------------------------------------------------------------------------------------

DEF_CMD(pop, NUM_REG_RAM,
{
    CHECK_STACK(CMD_pop);
    ip++;

    switch(arg_mask)
    {
        case ARG_REG:
            number_of_register = code[ip];

            stack_Pop(stack, &(REG[number_of_register]));

            ip++;
            break;

        case (ARG_NUM + ARG_RAM):
            number_1 = *((int*) (code + ip));

            CHECK_RAM_ADDRESS(number_1);

            stack_Pop(stack, &(RAM[number_1]));

            ip += sizeof(int);
            break;

        case (ARG_REG + ARG_RAM):
            number_of_register = code[ip];
            number_1 = REG[number_of_register];

            CHECK_RAM_ADDRESS(number_1);

            stack_Pop(stack, &(RAM[number_1]));

            ip++;
            break;

        case (ARG_NUM + ARG_REG + ARG_RAM):
            number_1 = *((int*) (code + ip));
            ip += sizeof(int);

            number_of_register = code[ip];
            number_2 = REG[number_of_register];

            CHECK_RAM_ADDRESS(number_1 + number_2);

            stack_Pop(stack, &(RAM[number_1 + number_2]));

            ip++;
            break;
    }
})

//----------------------------------------------------------------------------------------------------------------------

DEF_CMD(add, NO_ARGS,
{
    CHECK_STACK(CMD_add);
    get_numbers_from_stack(stack, &number_1, &number_2, NO_ARGS);

    stack_Push(stack, number_1 + number_2);
    ip++;
})

//----------------------------------------------------------------------------------------------------------------------

DEF_CMD(sub, NO_ARGS,
{
    CHECK_STACK(CMD_sub);
    get_numbers_from_stack(stack, &number_1, &number_2, NO_ARGS);

    stack_Push(stack, number_2 - number_1);
    ip++;
})

//----------------------------------------------------------------------------------------------------------------------

DEF_CMD(mul, NO_ARGS,
{
    CHECK_STACK(CMD_mul);
    get_numbers_from_stack(stack, &number_1, &number_2, NO_ARGS);

    stack_Push(stack, number_1 * number_2);
    ip++;
})

//----------------------------------------------------------------------------------------------------------------------

DEF_CMD(div, NO_ARGS,
{
    CHECK_STACK(CMD_div);
    get_numbers_from_stack(stack, &number_1, &number_2, NO_ARGS);

    stack_Push(stack, number_2 / number_1);
    ip++;
})

//----------------------------------------------------------------------------------------------------------------------

DEF_CMD(out, NO_ARGS,
{
    printf("\nStack top element: [%d]\n\n", *stack->top_elem);
    ip++;
})

//----------------------------------------------------------------------------------------------------------------------

DEF_CMD(in, NO_ARGS,
{
    err = scanf("%d", &number_1);
    if (!err)
    {
        free(code);
        return Entered_Invalid_Value;
    }
    stack_Push(stack, number_1);
    ip++;
})

//----------------------------------------------------------------------------------------------------------------------

DEF_CMD(dump, NO_ARGS,
{
    err = Dump(code, stack, ip);
    if (err)
    {
        free(code);
        return err;
    }
    ip++;
})




//----------------------------------------------------------------------------------------------------------------------

DEF_CMD(call, LABEL,
{
    ip++;
    new_ip = *((int*)(code + ip));

    stack_Push(&call_stack, ip + sizeof(int));
    ip = new_ip;
})

//----------------------------------------------------------------------------------------------------------------------

DEF_CMD(ret, NO_ARGS,
{
    stack_Pop(&call_stack, &new_ip);

    ip = new_ip;
})
//----------------------------------------------------------------------------------------------------------------------





DEF_CMD(jmp, LABEL,
{
    ip++;
    new_ip = *((int*)(code + ip));
    ip = new_ip;
})

//----------------------------------------------------------------------------------------------------------------------

DEF_CMD(ja, LABEL,
{
    CHECK_STACK(CMD_ja);
    get_numbers_from_stack(stack, &number_1, &number_2, LABEL);

    ip++;
    new_ip = *((int*)(code + ip));

    if (number_1 > number_2)
        ip = new_ip;
    else
        ip += sizeof(int);
})

//----------------------------------------------------------------------------------------------------------------------

DEF_CMD(jae, LABEL,
{
    CHECK_STACK(CMD_jae);
    get_numbers_from_stack(stack, &number_1, &number_2, LABEL);

    ip++;
    new_ip = *((int*)(code + ip));

    if (number_1 >= number_2)
        ip = new_ip;
    else
        ip += sizeof(int);
})

//----------------------------------------------------------------------------------------------------------------------

DEF_CMD(jb, LABEL,
{
    CHECK_STACK(CMD_jb);
    get_numbers_from_stack(stack, &number_1, &number_2, LABEL);

    ip++;
    new_ip = *((int*)(code + ip));

    if (number_1 < number_2)
        ip = new_ip;
    else
        ip += sizeof(int);
})

//----------------------------------------------------------------------------------------------------------------------

DEF_CMD(jbe, LABEL,
{
    CHECK_STACK(CMD_jbe);
    get_numbers_from_stack(stack, &number_1, &number_2, LABEL);

    ip++;
    new_ip = *((int*)(code + ip));

    if (number_1 <= number_2)
        ip = new_ip;
    else
        ip += sizeof(int);
})

//----------------------------------------------------------------------------------------------------------------------

DEF_CMD(je, LABEL,
{
    CHECK_STACK(CMD_je);
    get_numbers_from_stack(stack, &number_1, &number_2, LABEL);

    ip++;
    new_ip = *((int*)(code + ip));

    if (number_1 == number_2)
        ip = new_ip;
    else
        ip += sizeof(int);
})

//----------------------------------------------------------------------------------------------------------------------

DEF_CMD(jne, LABEL,
{
    CHECK_STACK(CMD_jne);
    get_numbers_from_stack(stack, &number_1, &number_2, LABEL);

    ip++;
    new_ip = *((int*)(code + ip));

    if (number_1 != number_2)
        ip = new_ip;
    else
        ip += sizeof(int);
})

//----------------------------------------------------------------------------------------------------------------------