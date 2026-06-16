#include "banking.h"

void open_account(void)
{
    struct account a;
    char username[60], password[60], conf_pass[60], conf_user[60];
    char fname[40];
    int  acc_no_input;

    memset(&a, 0, sizeof(a));

    system("cls");
    system("color 9");
    printf("\n\n\t\t\tCUSTOMER ACCOUNT BANKING MANAGEMENT SYSTEM");
    printf("\n\n\t\t========================================");
    printf("\n\t\t       CREATE NEW CUSTOMER ACCOUNT");
    printf("\n\t\t========================================\n\n");

    printf("\tEnter today's date (mm/dd/yyyy)         : ");
    scanf("%d/%d/%d", &a.deposit.month, &a.deposit.day, &a.deposit.year);

    printf("\tEnter the Office ID number              : ");
    scanf("%19s", a.office_id);

    /* Unique account number check */
    acc_no_entry:
    printf("\tEnter account number                    : ");
    scanf("%d", &acc_no_input);
    csv_filename(acc_no_input, fname);
    {
        FILE *test = fopen(fname, "r");
        if (test)
        {
            fclose(test);
            printf("\n\tAccount number %d is already in use! Try another.\n\n",acc_no_input);
            goto acc_no_entry;
        }
    }
    a.acc_no = acc_no_input;


    printf("\tEnter first name                        : ");
    scanf("%39s", a.first_name);
    //while (getchar() != '\n');
    //fgets(a.first_name, sizeof(a.first_name), stdin);
    //a.first_name[strcspn(a.first_name, "\r\n")] = '\0';

    printf("\tEnter last name                         : ");
    scanf("%39s", a.last_name);
    fgets(a.last_name, sizeof(a.last_name), stdin);
    a.last_name[strcspn(a.last_name, "\r\n")] = '\0';

    snprintf(a.name, sizeof(a.name), "%s %s", a.first_name, a.last_name);

    printf("\tEnter date of birth (mm/dd/yyyy)        : ");
    scanf("%d/%d/%d", &a.dob.month, &a.dob.day, &a.dob.year);

    printf("\tEnter age                               : ");
    scanf("%d", &a.age);

    while (getchar() != '\n');

    printf("\tEnter current address                   : ");
    fgets(a.curr_address, sizeof(a.curr_address), stdin);
    a.curr_address[strcspn(a.curr_address, "\r\n")] = '\0';
    strncpy(a.address, a.curr_address, sizeof(a.address)-1);

    printf("\tEnter permanent address                 : ");
    fgets(a.perm_address, sizeof(a.perm_address), stdin);
    a.perm_address[strcspn(a.perm_address, "\r\n")] = '\0';

    printf("\tEnter citizenship number                : ");
    scanf("%19s", a.citizenship);

    printf("\tEnter phone number                      : ");
    scanf("%lf", &a.phone);

    printf("\tEnter email number                      : ");
    scanf("%lf", &a.email);

    printf("\tEnter amount to deposit ($)             : ");
    scanf("%f", &a.amt);

    printf("\n\tType of account:\n");
    printf("\t  Saving / Current / Fixed1 / Fixed2 / Fixed3\n");
    printf("\tEnter account type                      : ");
    scanf("%11s", a.acc_type);

    printf("\n\t---- Customer Login Credentials ----\n");

    /* Username uniqueness check */
    username_entry:
    printf("\tEnter new User ID for customer          : ");
    scanf("%59s", username);
    printf("\tConfirm User ID                         : ");
    scanf("%59s", conf_user);

    if (strcmp(username, conf_user) != 0)
    {
        printf("\n\tUser IDs do not match! Try again.\a\n\n");
        goto username_entry;
    }
    if (bx_uid_taken(username))
    {
        printf("\n\tThat User ID is already taken! Choose another.\n\n");
        goto username_entry;
    }

    /* Password match check */
    password_entry:
    printf("\tEnter new Password for customer         : ");
    scanf("%59s", password);
    printf("\tConfirm Password                        : ");
    scanf("%59s", conf_pass);

    if (strcmp(password, conf_pass) != 0)
    {
        printf("\n\tPasswords do not match! Try again.\a\n\n");
        goto password_entry;
    }

    /* Persist account CSV */
    csv_create_account(&a, fname);

    if (!bx_store_cred(username, password, a.acc_no))
    {
        printf("\n[ERROR] Could not save credentials!\n");
        return;
    }

    /* Refresh master list */
    csv_rebuild_master();

    printf("\n\nAccount created successfully!\n");
    printf("  Account Number : %d\n", a.acc_no);
    printf("  Name           : %s %s\n", a.first_name, a.last_name);
    printf("  Data file      : %s\n", fname);

    nav_simple();
}
