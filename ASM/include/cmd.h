DEF_CMD(hlt, NO_ARGS, {})

DEF_CMD(push, ONE_ARG,
{

})

DEF_CMD(pop, ONE_ARG,
{

})

DEF_CMD(add, NO_ARGS,
{
    CHECK_STACK(CMD_ADD);
    stackPop(stk, &num1);
    stackPop(stk, &num2);
    stackPush(stk, num1 + num2);
    ip++;
})

DEF_CMD(sub, NO_ARGS,
{
    CHECK_STACK(CMD_SUB);
    stackPop(stk, &num1);
    stackPop(stk, &num2);
    stackPush(stk, num2 - num1);
    ip++;
})

DEF_CMD(mul, NO_ARGS,
{
    CHECK_STACK(CMD_MUL);
    stackPop(stk, &num1);
    stackPop(stk, &num2);
    stackPush(stk, num1 * num2);
    ip++;
})

DEF_CMD(div, NO_ARGS,
{
    CHECK_STACK(CMD_DIV);
    stackPop(stk, &num1);
    stackPop(stk, &num2);
    stackPush(stk, num2 / num1);
    ip++;
})

DEF_CMD(out, NO_ARGS,
{
    printf("\nStack top element: [%d]\n\n", *stk->top_elem);
    ip++;
})

DEF_CMD(in, NO_ARGS,
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

DEF_CMD(dump, NO_ARGS,
{
    err = Dump(code, stk, ip);
    if (err)
    {
        free(code);
        return err;
    }
    ip++;
})