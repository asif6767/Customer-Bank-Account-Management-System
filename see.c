#include "banking.h"
#include <ctype.h>

static int  parse_account_csv(const char *fname, struct account *out);
static int  account_is_removed(const char *fname);
static void print_account_details(const struct account *a);
static char *icase_search(const char *haystack, const char *needle);

/* Parse ACCOUNT_INFO row into struct */
static int parse_account_csv(const char *fname, struct account *out)
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
            sscanf(dob_str, "%d/%d/%d",
                   &out->dob.month, &out->dob.day, &out->dob.year);
            sscanf(open_str, "%d/%d/%d",
                   &out->deposit.month, &out->deposit.day, &out->deposit.year);
            snprintf(out->name, sizeof(out->name), "%s %s",
                     out->first_name, out->last_name);
            strncpy(out->address, out->curr_address, sizeof(out->address)-1);
            found = 1;
        }
        break;
    }
    fclose(fp);
    return found;
}

/* Check REMOVED flag in CSV */
static int account_is_removed(const char *fname)
{
    FILE *fp = fopen(fname, "r");
    if (!fp)
    {
        return 0;
    }

    char line[512];
    int removed = 0;

    while (fgets(line, sizeof(line), fp))
    {
        if (strncmp(line, "REMOVED,", 8) == 0)
        {
            removed = 1;
            break;
        }
    }

    fclose(fp);
    return removed;
}

/* Display full details with interest projection */
static void print_account_details(const struct account *a)
{
    float intrst = 0.0f;
    float time   = 0.0f;
    int   rate   = 0;

    system("cls");
    system("color 9");
    printf("\n\n\t\t\tCUSTOMER ACCOUNT BANKING MANAGEMENT SYSTEM");
    printf("\n\n\t\t\t====== ACCOUNT DETAILS ======\n\n");
    printf("\t\t\tAccount no.       : %d\n",a->acc_no);
    printf("\t\t\tOffice ID         : %s\n",a->office_id);
    printf("\t\t\tFirst name        : %s\n",a->first_name);

    printf("\t\t\tLast name         : %s\n",a->last_name);
    printf("\t\t\tDate of Birth     : %d/%d/%d\n",a->dob.month, a->dob.day, a->dob.year);
    printf("\t\t\tAge               : %d\n",a->age);
    printf("\t\t\tCurrent Address   : %s\n",a->curr_address);
    printf("\t\t\tPermanent Address : %s\n",a->perm_address);
    printf("\t\t\tCitizenship no.   : %s\n",a->citizenship);
    printf("\t\t\tPhone no.         : %.0lf\n",a->phone);
    printf("\t\t\tEmail             : %s\n",    a->email);

    printf("\t\t\tAccount type      : %s\n",a->acc_type);
    printf("\t\t\tDeposited amount  : $%.2f\n",a->amt);
    printf("\t\t\tAccount open date : %d/%d/%d\n", a->deposit.month,a->deposit.day,a->deposit.year);

    /* Interest calculation per account type */
    if (strcmpi(a->acc_type, "fixed1") == 0)
    {
        time   = 1;
        rate   = 9;
        intrst = calc_interest(time, a->amt, rate);
        printf("\n\t\t\tYour interest for 1 year  : $%.2f\n", intrst);
        printf("\t\t\tAfter 1 year the maturity : $%.2f\n", a->amt + intrst);
        printf("\t\t\tYou can withdraw your amount on: %d/%d/%d\n",a->deposit.month, a->deposit.day, a->deposit.year + 1);

    }
    else if (strcmpi(a->acc_type, "fixed2") == 0)
    {
        time   = 2;
        rate   = 11;
        intrst = calc_interest(time, a->amt, rate);
        printf("\n\t\t\tYour interest for 2 years : $%.2f\n", intrst);
        printf("\t\t\tAfter 2 years the maturity: $%.2f\n", a->amt + intrst);
        printf("\t\t\tYou can withdraw your amount on: %d/%d/%d\n",a->deposit.month, a->deposit.day, a->deposit.year + 2);

    }
    else if (strcmpi(a->acc_type, "fixed3") == 0)
    {
        time   = 3;
        rate   = 13;
        intrst = calc_interest(time, a->amt, rate);
        printf("\n\t\t\tYour interest for 3 years : $%.2f\n", intrst);
        printf("\t\t\tAfter 3 years the maturity: $%.2f\n", a->amt + intrst);
        printf("\t\t\tYou can withdraw your amount on: %d/%d/%d\n",a->deposit.month, a->deposit.day, a->deposit.year + 3);

    }
    else if (strcmpi(a->acc_type, "saving") == 0)
    {
        time   = (float)1 / 12;
        rate   = 8;
        intrst = calc_interest(time, a->amt, rate);
        printf("\n\t\t\tYour monthly interest : $%.2f\n", intrst);
        printf("\t\t\tNext month balance    : $%.2f\n", a->amt + intrst);
        printf("\t\t\tYou can withdraw your amount at any time.\n");

    }
    else if (strcmpi(a->acc_type, "current") == 0)
    {
        printf("\n\t\t\tCurrent accounts do not earn interest.\n");
        printf("\t\t\tYou can withdraw your amount at any time.\n");
    }
}

#include <ctype.h>
#include <stddef.h>
/* Case-insensitive substring search */
static char *icase_search(const char *haystack, const char *needle)
{

    if (!*needle)
    {
        return (char *)haystack;
    }

    for (; *haystack; haystack++)
    {

        if (tolower((unsigned char)*haystack) == tolower((unsigned char)*needle))
        {
            const char *h = haystack;
            const char *n = needle;


            while (*h && *n && tolower((unsigned char)*h) == tolower((unsigned char)*n))
            {
                h++;
                n++;
            }


            if (!*n)
            {
                return (char *)haystack;
            }
        }
    }

    return NULL; // No match found
}

/* View account entry point */
void view_account(void)
{
    int choice;
    char fname[40];
    struct account a;

    memset(&a, 0, sizeof(a));

    system("cls");
    system("color 9");
    printf("\n\n\t\t\tCUSTOMER ACCOUNT BANKING MANAGEMENT SYSTEM");
    printf("\n\n\t\t========================================");
    printf("\n\t\t   CHECK ACCOUNT DETAILS");
    printf("\n\t\t========================================\n\n");

    printf("\tDo you want to check by\n");
    printf("\t  1. Account number\n");
    printf("\t  2. Name\n\n");
    printf("\tEnter your choice: ");
    scanf("%d", &choice);

    /* Search by account number */
    if (choice == 1)
        {
        printf("\n\tEnter the account number: ");
        scanf("%d", &check.acc_no);

        csv_filename(check.acc_no, fname);

        if (!parse_account_csv(fname, &a))
        {
            system("cls");
            printf("\n\nRecord not found!!\a\a\a");
            nav_prompt(1, view_account);
            return;
        }

        if (account_is_removed(fname))
        {
            system("cls");
            printf("\n\tThis account has been removed.\n");
            nav_simple();
            return;
        }

        print_account_details(&a);

    /* Search by name */
    }
    else if (choice == 2)
        {
        char search_name[60];
        int  test = 0;

        while (getchar() != '\n');
        printf("\n\tEnter the name (or part of the name): ");
        fgets(search_name, sizeof(search_name), stdin);
        search_name[strcspn(search_name, "\r\n")] = '\0';

        /* Scan all user CSV files */
        WIN32_FIND_DATA fd;
        HANDLE h = FindFirstFile("user_*.csv", &fd);

        if (h != INVALID_HANDLE_VALUE)
            {
            do
            {
                if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                     continue;
                }

                struct account tmp;
                memset(&tmp, 0, sizeof(tmp));

                if (!parse_account_csv(fd.cFileName, &tmp))
                {
                    continue;
                }
                if (account_is_removed(fd.cFileName))
                {
                     continue;
                }

                char full_name[80];
                snprintf(full_name, sizeof(full_name), "%s %s",tmp.first_name, tmp.last_name);

                /* Case-insensitive name match */
                if (icase_search(full_name,      search_name) != NULL ||
                    icase_search(tmp.first_name, search_name) != NULL ||
                    icase_search(tmp.last_name,  search_name) != NULL)
                {
                    print_account_details(&tmp);
                    test = 1;
                    printf("\n\n\t\t(Press Enter to continue...)");
                    while (getchar() != '\n');
                }
            }
            while (FindNextFile(h, &fd));

            FindClose(h);
        }

        if (test == 0)
        {
            system("cls");
            printf("\n\nRecord not found!!\a\a\a");
            nav_prompt(1, view_account);
            return;
        }

    }
    else
    {
        printf("\nInvalid choice!\a\n");
    }

    nav_simple();
}
