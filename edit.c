#include "banking.h"

/* Parse ACCOUNT_INFO row from CSV */
static int load_account_csv(const char *fname, struct account *out)
{
    FILE *fp = fopen(fname, "r");
    if (!fp) return 0;

    char line[512];
    int  found = 0;

    while (fgets(line, sizeof(line), fp))
        {
        if (strncmp(line, "ACCOUNT_INFO,", 13) != 0)
        {
            continue;
        }

        char dob_str[12]="", open_str[12]="";
        int parsed = sscanf(line,
            "ACCOUNT_INFO,%d,%19[^,],%39[^,],%39[^,],%11[^,],%d,"
            "\"%79[^\"]\","  "\"%79[^\"]\",%19[^,],%lf,%11[^,],%f,%11[^,]",
            &out->acc_no, out->office_id,
            out->first_name, out->last_name,
            dob_str, &out->age,
            out->curr_address, out->perm_address,
            out->citizenship, &out->phone,
            out->acc_type, &out->amt, open_str);

        if (parsed >= 10)
        {
            sscanf(dob_str, "%d/%d/%d",&out->dob.month, &out->dob.day, &out->dob.year);
            sscanf(open_str, "%d/%d/%d",&out->deposit.month, &out->deposit.day, &out->deposit.year);
            snprintf(out->name, sizeof(out->name), "%s %s",out->first_name, out->last_name);
            strncpy(out->address, out->curr_address, sizeof(out->address)-1);
            found = 1;
        }
        break;
    }
    fclose(fp);
    return found;
}

/* Rewrite only the ACCOUNT_INFO row in-place */
static void rewrite_info_row(const char *fname, const struct account *a)
{
    FILE *fp = fopen(fname, "r");
    if (!fp)
    {
        return;
    }

    /* Load file into buffer */
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

    /* Locate target row */
    char *start = strstr(buf, "ACCOUNT_INFO,");
    if (!start)
    {
        free(buf); return;
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

    /* Build updated row */
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
        a->acc_type, a->amt,
        a->deposit.month, a->deposit.day, a->deposit.year);

    /* Write: before + new row + after */
    fp = fopen(fname, "w");
    if (!fp)
        {
             free(buf); return;
    }
    fwrite(buf, 1, (size_t)(start - buf), fp);
    fputs(new_line, fp);
    fputs(end, fp);
    fclose(fp);
    free(buf);

}

/* Update account info entry point */
void update_account(void)
{
    int  acc_no, choice;
    char fname[40], date_str[12];
    struct account a;
    char old_val[80], new_val[80];

    memset(&a, 0, sizeof(a));

    system("cls");
    system("color 9");
    printf("\n\n\t\t\tCUSTOMER ACCOUNT BANKING MANAGEMENT SYSTEM");
    printf("\n\n\t\t========================================");
    printf("\n\t\t    UPDATE ACCOUNT INFORMATION");
    printf("\n\t\t========================================\n\n");

    printf("\tEnter the account number to update: ");
    scanf("%d", &acc_no);

    csv_filename(acc_no, fname);

    if (!load_account_csv(fname, &a))
        {
        system("cls");
        printf("\nRecord not found!!\a\a\a");
        nav_prompt(1, update_account);
        return;
    }

    /* Block updates on removed accounts */
    {
        FILE *chk = fopen(fname, "r");
        char  ln[512];
        int   removed = 0;
        while (fgets(ln, sizeof(ln), chk))
            if (strncmp(ln, "REMOVED,", 8) == 0)
                {
                     removed = 1; break;
                    }
        fclose(chk);
        if (removed)
            {
            printf("\n\tThis account has been removed and cannot be updated.\n");
            nav_simple();
            return;
        }
    }

    printf("\n\tCurrent information:\n");
    printf("\t  Name            : %s %s\n", a.first_name, a.last_name);
    printf("\t  Current Address : %s\n",    a.curr_address);
    printf("\t  Permanent Addr  : %s\n",    a.perm_address);
    printf("\t  Phone           : %.0lf\n", a.phone);
    printf("\t  Age             : %d\n",    a.age);

    printf("\n\tWhich information do you want to change?\n");
    printf("\t  1. Current Address\n");
    printf("\t  2. Permanent Address\n");
    printf("\t  3. Phone Number\n");
    printf("\t  5. Email Address\n");
    printf("\t  4. Age\n");
    printf("\n\tEnter your choice (1/2/3/4): ");
    scanf("%d", &choice);

    printf("\n\tEnter today's date (mm/dd/yyyy) for the record: ");
    scanf("%11s", date_str);

    while (getchar() != '\n');

    const char *field_name = "";

    switch (choice)
    {
        case 1:
            field_name = "Current Address";
            strncpy(old_val, a.curr_address, sizeof(old_val));
            printf("\tEnter new current address: ");
            fgets(new_val, sizeof(new_val), stdin);
            new_val[strcspn(new_val, "\r\n")] = '\0';
            strncpy(a.curr_address, new_val, sizeof(a.curr_address)-1);
            strncpy(a.address, new_val, sizeof(a.address)-1);
            system("cls");
            printf("Changes saved!");
            break;

        case 2:
            field_name = "Permanent Address";
            strncpy(old_val, a.perm_address, sizeof(old_val));
            printf("\tEnter new permanent address: ");
            fgets(new_val, sizeof(new_val), stdin);
            new_val[strcspn(new_val, "\r\n")] = '\0';
            strncpy(a.perm_address, new_val, sizeof(a.perm_address)-1);
            system("cls");
            printf("Changes saved!");
            break;

        case 3:
            field_name = "Phone Number";
            sprintf(old_val, "%.0lf", a.phone);
            printf("\tEnter new phone number: ");
            scanf("%lf", &a.phone);
            sprintf(new_val, "%.0lf", a.phone);
            system("cls");
            printf("Changes saved!");
            break;

        case 4:
            field_name = "Age";
            sprintf(old_val, "%d", a.age);
            printf("\tEnter new age: ");
            scanf("%d", &a.age);
            sprintf(new_val, "%d", a.age);
            system("cls");
            printf("Changes saved!");
            break;

        default:
            printf("\nInvalid choice!\a\n");
            nav_simple();
            return;
    }

    /* Log old value, rewrite row, refresh master */
    csv_append_update(fname, field_name, old_val, new_val, date_str);
    rewrite_info_row(fname, &a);
    csv_rebuild_master();

    printf("\n\n\tNote: Old information is preserved in %s.\n", fname);
    printf("\tOpen the file manually in Excel to see the full history.\n");

    nav_simple();
}
