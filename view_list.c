#include "banking.h"

/* List all active accounts */
void list_accounts(void)
{
    system("cls");
    system("color 9");
    printf("\n\n\t\t\tCUSTOMER ACCOUNT BANKING MANAGEMENT SYSTEM");
    printf("\n\n\t\t========================================");
    printf("\n\t\t       ALL ACTIVE CUSTOMER ACCOUNTS");
    printf("\n\t\t========================================\n\n");

    /* Scan per-user CSV files */
    WIN32_FIND_DATA fd;
    HANDLE h = FindFirstFile("user_*.csv", &fd);

    int count = 0;

    if (h == INVALID_HANDLE_VALUE)
    {
        printf("\n\t  No customer records found.\n");
        nav_simple();
        return;
    }

    printf("\t%-6s  %-25s  %-12s  %-10s  %s\n",
           "Acc No", "Name", "Account Type", "Balance", "Open Date");
    printf("\t---------------------------------------------------------------\n");

    do
        {
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            continue;
        }

        FILE *fp = fopen(fd.cFileName, "r");
        if (!fp)
        {
            continue;
        }

        char line[512];
        char info[512] = "";
        int  removed   = 0;

        while (fgets(line, sizeof(line), fp))
            {
            if (strncmp(line, "ACCOUNT_INFO,", 13) == 0)
                {
                    strncpy(info, line, sizeof(info)-1);
                }
            if (strncmp(line, "REMOVED,", 8) == 0)
                {
                    removed = 1;
                }
        }
        fclose(fp);

        if (removed || !info[0])
        {
            continue;
        }

        /* Parse row fields */
        int    acc_no, age;
        double phone;
        float  amt;
        char   office_id[20], first[40], last[40];
        char   dob_str[12], open_str[12];
        char   curr_addr[80], perm_addr[80];
        char   citizen[20], acc_type[12];

        int parsed = sscanf(info,
            "ACCOUNT_INFO,%d,%19[^,],%39[^,],%39[^,],%11[^,],%d,"
            "\"%79[^\"]\","  "\"%79[^\"]\",%19[^,],%lf,%11[^,],%f,%11[^,]",
            &acc_no, office_id, first, last,
            dob_str, &age,
            curr_addr, perm_addr,
            citizen, &phone,
            acc_type, &amt, open_str);

        if (parsed < 10)
        {
            continue;
        }

        char full_name[65];
        snprintf(full_name, sizeof(full_name), "%s %s", first, last);

        /* Formatted account row */
        printf("\t%-6d  %-25s  %-12s  $%-9.2f  %s\n",acc_no, full_name, acc_type, amt, open_str[0] ? open_str : "N/A");
        count++;

    }
     while (FindNextFile(h, &fd));

    FindClose(h);

    if (count == 0)
        {
            printf("\n\t  No active accounts found.\n");
        }
    else
    {
        printf("\n\t  Total active accounts: %d\n", count);

    }
    nav_simple();
}
