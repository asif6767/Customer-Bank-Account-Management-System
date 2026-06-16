#include "banking.h"

/* Remove account entry point */
void remove_account(void)
{
    int  acc_no;
    char fname[40], removed_fname[50];
    char date_str[12], username[60];
    struct account a;

    memset(&a, 0, sizeof(a));

    system("cls");
    system("color 9");
    printf("\n\n\t\t\tCUSTOMER ACCOUNT BANKING MANAGEMENT SYSTEM");
    printf("\n\n\t\t========================================");
    printf("\n\t\t       REMOVE EXISTING ACCOUNT");
    printf("\n\t\t========================================\n\n");

    printf("\tEnter the account number of the customer you want to remove: ");
    scanf("%d", &rem.acc_no);
    acc_no = rem.acc_no;

    csv_filename(acc_no, fname);

    FILE *fp = fopen(fname, "r");
    if (!fp)
        {
        system("cls");
        printf("\nRecord not found!!\a\a\a");
        nav_prompt(1, remove_account);
        return;
    }

    /* Read file: find info row and removed flag */
    char  line[512], info_line[512] = "";
    int   already_removed = 0;

    while (fgets(line, sizeof(line), fp))
        {
        if (strncmp(line, "ACCOUNT_INFO,", 13) == 0)
            strncpy(info_line, line, sizeof(info_line)-1);
        if (strncmp(line, "REMOVED,", 8) == 0)
            already_removed = 1;
    }
    fclose(fp);

    if (already_removed)
        {
        printf("\n\tThis account has already been removed.\n");
        nav_simple();
        return;
    }

    /* Parse name for confirmation prompt */
    char first[40] = "?", last[40] = "?";
    if (info_line[0] != '\0')
        {
        char skip1[20], skip2[12], skip3[12];
        char skip4[80], skip5[80], skip6[20], skip7[12];
        int  i_acc, i_age;
        double i_phone;
        float  i_amt;
        char   i_type[12];

        sscanf(info_line,
            "ACCOUNT_INFO,%d,%19[^,],%39[^,],%39[^,],%11[^,],%d,"
            "\"%79[^\"]\","  "\"%79[^\"]\",%19[^,],%lf,%11[^,],%f,%11[^,]",
            &i_acc, skip1, first, last, skip2, &i_age,
            skip4, skip5, skip6, &i_phone, i_type, &i_amt, skip7);
    }

    printf("\n\tAccount to remove:\n");
    printf("\t  Account No. : %d\n", acc_no);
    printf("\t  Name        : %s %s\n", first, last);
    printf("\n\tAre you sure you want to remove this account?\n");
    printf("\t  1. Yes, remove it\n");
    printf("\t  2. No, go back to main menu\n");
    printf("\n\tEnter your choice: ");

    int confirm;
    scanf("%d", &confirm);

    if (confirm == 2)
        {
        printf("\n\tRemoval cancelled.\n");
        nav_simple();
        return;
    }
    if (confirm != 1)
    {
        printf("\n\tInvalid. Removal cancelled.\n");
        nav_simple();
        return;
    }

    printf("\n\tEnter today's date (mm/dd/yyyy): ");
    scanf("%11s", date_str);

    printf("\tEnter the customer's User ID to confirm removal: ");
    scanf("%59s", username);

    /* Hash username for audit trail */
    char hx[65];
    bx_digest_hex(username, hx);

    /* Append REMOVED row */
    csv_mark_removed(fname, hx, date_str);

    /* Rename file to archive */
    sprintf(removed_fname, "removed_%d.csv", acc_no);
    rename(fname, removed_fname);

    /* Refresh master list */
    csv_rebuild_master();

    system("cls");
    printf("\n\nRecord deleted successfully!\n");
    printf("  Account No. : %d\n", acc_no);
    printf("  Name        : %s %s\n", first, last);
    printf("  Archived in : %s\n", removed_fname);
    printf("\n  (The file is kept on disk but hidden from the system.\n");
    printf("   Open it manually in Excel to view the history.)\n");

    nav_simple();
}
