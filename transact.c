#include "banking.h"

/* Read balance and account info from CSV */
static int load_for_txn(const char *fname,float  *out_balance,char   *out_type,char   *out_first,char   *out_last,struct account *full_a)
{
    FILE *fp = fopen(fname, "r");
    if (!fp)
    {
        return 0;

    }
    char line[512];
    int  found = 0;

    while (fgets(line, sizeof(line), fp))
        {
        if (strncmp(line, "ACCOUNT_INFO,", 13) != 0)
        {
            continue;
        }

        char dob_str[12]="", open_str[12]="";
        sscanf(line,
            "ACCOUNT_INFO,%d,%19[^,],%39[^,],%39[^,],%11[^,],%d,"
            "\"%79[^\"]\","  "\"%79[^\"]\",%19[^,],%lf,%11[^,],%f,%11[^,]",
            &full_a->acc_no, full_a->office_id,
            out_first, out_last,
            dob_str, &full_a->age,
            full_a->curr_address, full_a->perm_address,
            full_a->citizenship, &full_a->phone,
            out_type, out_balance, open_str);

        sscanf(dob_str, "%d/%d/%d",
               &full_a->dob.month, &full_a->dob.day, &full_a->dob.year);
        sscanf(open_str, "%d/%d/%d",
               &full_a->deposit.month, &full_a->deposit.day, &full_a->deposit.year);

        strncpy(full_a->first_name, out_first, sizeof(full_a->first_name)-1);
        strncpy(full_a->last_name,  out_last,  sizeof(full_a->last_name)-1);
        strncpy(full_a->acc_type,   out_type,  sizeof(full_a->acc_type)-1);
        full_a->amt = *out_balance;
        found = 1;
        break;
    }
    fclose(fp);
    return found;
}

/* Rewrite balance in ACCOUNT_INFO row */
static void persist_balance(const char *fname,const struct account *a,float new_balance)
{
    FILE *fp = fopen(fname, "r");
    if (!fp)
    {
        return;
    }

    /* Load into buffer */
    char  *buf = (char *)malloc(65536);
    size_t total = 0;
    char   line[512];

    while (fgets(line, sizeof(line), fp))
    {
        size_t len = strlen(line);
        memcpy(buf + total, line, len);
        total += len;
    }
    buf[total] = '\0';
    fclose(fp);

    char *start = strstr(buf, "ACCOUNT_INFO,");
    if (!start)
        {
             free(buf);
    return;
    }
    char *end = strchr(start, '\n');
    if (end)
    {
        end++;
    }
    else
    {
        end = start + strlen(start);
    }

    char new_line[512];
    sprintf(new_line,
        "ACCOUNT_INFO,%d,%s,%s,%s,%02d/%02d/%04d,%d,"
        "\"%s\",\"%s\",%s,%.0lf,%s,%.2f,%02d/%02d/%04d,ACTIVE\n",
        a->acc_no, a->office_id,
        a->first_name, a->last_name,
        a->dob.month, a->dob.day, a->dob.year,
        a->age,
        a->curr_address, a->perm_address,
        a->citizenship, a->phone,
        a->acc_type, new_balance,
        a->deposit.month, a->deposit.day, a->deposit.year);

    /* Write updated file */
    fp = fopen(fname, "w");
    if (!fp)
        {
             free(buf);
    return;
    }
    fwrite(buf, 1, (size_t)(start - buf), fp);
    fputs(new_line, fp);
    fputs(end, fp);
    fclose(fp);
    free(buf);
}

/* Transaction entry point */
void do_transaction(void)
{
    int  choice;
    char fname[40], date_str[12];
    float balance = 0;
    char  acc_type[12]="", first[40]="", last[40]="";
    struct account a;

    memset(&a, 0, sizeof(a));

    system("cls");
    system("color 9");
    printf("\n\n\t\t\tCUSTOMER ACCOUNT BANKING MANAGEMENT SYSTEM");
    printf("\n\n\t\t========================================");
    printf("\n\t\t            TRANSACTIONS");
    printf("\n\t\t========================================\n\n");

    printf("\tEnter the account number of the customer: ");
    scanf("%d", &transaction.acc_no);

    csv_filename(transaction.acc_no, fname);

    if (!load_for_txn(fname, &balance, acc_type, first, last, &a))
        {
        printf("\n\nRecord not found!!\a\a\a");
        nav_prompt(1, do_transaction);
        return;
    }

    /* Fixed accounts block transactions */
    if (strcmpi(acc_type, "fixed1")==0 ||strcmpi(acc_type, "fixed2")==0 ||strcmpi(acc_type, "fixed3")==0)
    {
        printf("\a\a\a\n\nYOU CANNOT DEPOSIT OR WITHDRAW CASH IN FIXED ACCOUNTS!!!!!");
        cpu_pause(1000000000);
        system("cls");
        menu();
        return;
    }

    printf("\n\tAccount holder  : %s %s\n", first, last);
    printf("\tCurrent balance : $%.2f\n", balance);

    printf("\n\tDo you want to\n");
    printf("\t  1. Deposit\n");
    printf("\t  2. Withdraw\n\n");
    printf("\tEnter your choice (1 for deposit and 2 for withdraw): ");
    scanf("%d", &choice);

    printf("\n\tEnter today's date (mm/dd/yyyy): ");
    scanf("%11s", date_str);

    if (choice == 1)
        {
        printf("\tEnter the amount you want to deposit: $ ");
        scanf("%f", &transaction.amt);
        float new_balance = balance + transaction.amt;
        persist_balance(fname, &a, new_balance);
        /* Log deposit */
        csv_append_txn(fname, "DEPOSIT", transaction.amt, new_balance, date_str);
        printf("\n\nDeposited successfully!\n");
        printf("  Deposited : $%.2f\n", transaction.amt);
        printf("  Balance   : $%.2f\n", new_balance);

    }
    else
        {
        printf("\tEnter the amount you want to withdraw: $ ");
        scanf("%f", &transaction.amt);

        /* Insufficient funds check */
        if (transaction.amt > balance)
            {
            printf("\n\nInsufficient funds! Balance is only $%.2f\a\n", balance);
            nav_simple();
            return;
        }
        float new_balance = balance - transaction.amt;
        persist_balance(fname, &a, new_balance);
        /* Log withdrawal */
        csv_append_txn(fname, "WITHDRAWAL", transaction.amt, new_balance, date_str);
        printf("\n\nWithdrawn successfully!\n");
        printf("  Withdrawn : $%.2f\n", transaction.amt);
        printf("  Balance   : $%.2f\n", new_balance);
    }

    nav_simple();
}
