#ifndef _CMD_H
#define _CMD_H

DEF_CMD(HLT,  NO_ARGS, {})


//DEF_CMD(PUSH, NUM, {} )
//
//DEF_CMD(PUSH, REG, {})

DEF_CMD(PUSH, arg, {})


DEF_CMD(POP,  arg, {})

DEF_CMD(ADD,  NO_ARGS,
{
    CHECK_STACK(CMD_ADD);
    stackPop(stk, &num1);
    stackPop(stk, &num2);
    stackPush(stk, num1 + num2);
    ip++;
})

DEF_CMD(SUB,  NO_ARGS,
{
    CHECK_STACK(CMD_SUB);
    stackPop(stk, &num1);
    stackPop(stk, &num2);
    stackPush(stk, num2 - num1);
    ip++;
})

DEF_CMD(MUL,  NO_ARGS,
{
    CHECK_STACK(CMD_MUL);
    stackPop(stk, &num1);
    stackPop(stk, &num2);
    stackPush(stk, num1 * num2);
    ip++;
})

DEF_CMD(DIV,  NO_ARGS,
{
    CHECK_STACK(CMD_DIV);
    stackPop(stk, &num1);
    stackPop(stk, &num2);
    stackPush(stk, num2 / num1);
    ip++;
})

DEF_CMD(OUT,  NO_ARGS,
{
    printf("\nStack top element: [%d]\n\n", *stk->top_elem);
    ip++;
})

DEF_CMD(IN,   NO_ARGS,
{
    err = scanf("%d", &num1);
    if (!err)
    {
        free(code);
        return Entered_Invalid_Value;
    }
    stackPush(stk, num1);
    ip++;
})

DEF_CMD(DUMP, NO_ARGS,
{
    err = Dump(code, stk, ip);
    if (err)
    {
        free(code);
        return err;
    }
    ip++;
})

#endif